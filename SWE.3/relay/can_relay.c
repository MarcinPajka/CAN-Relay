/*
 * @file can_relay.c
 * @brief Generic CAN-controlled relay interface implementation.
 *
 * This module provides a SocketCAN backend for MCP2515 (can0) on Raspberry Pi.
 * It implements CAN-based relay control with the following features:
 * - can_hw_send() using PF_CAN (SocketCAN)
 * - Weak functions for relay hardware initialization and control
 * - Helper functions to open/close CAN socket
 * - Support for up to 8 relays with CAN command/status interface
 * - Additional signal sending functions for battery, velocity, charging status
 *
 * @author [Your Name]
 * @date 2025
 * @version 1.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

/** @brief Maximum number of relays supported */
#define MAX_RELAYS         8u
/** @brief CAN ID for relay commands */
#define CAN_CMD_ID         0x400u
/** @brief CAN ID for relay status replies */
#define CAN_STATUS_ID      0x401u

/** @brief CAN ID for battery level signal */
#define CAN_BATTERY_ID     0x100u
/** @brief CAN ID for velocity signal */
#define CAN_VELOCITY_ID    0x101u
/** @brief CAN ID for charging active signal */
#define CAN_CHARGING_ACTIVE_ID 0x102u
/** @brief CAN ID for charge request signal */
#define CAN_CHARGE_REQUEST_ID  0x103u

/** @brief Opcode for setting relay state */
#define OPCODE_SET         0x01u
/** @brief Opcode for toggling relay state */
#define OPCODE_TOGGLE      0x02u
/** @brief Opcode for querying single relay state */
#define OPCODE_QUERY       0x03u
/** @brief Opcode for querying all relay states */
#define OPCODE_QUERY_ALL   0x04u

/** @brief Status reply for all relays */
#define STATUS_ALL         0x10u
/** @brief Status reply for single relay */
#define STATUS_SINGLE      0x11u

/** @brief Relay state bitmask (up to 16 relays supported) */
static uint16_t relay_state_mask = 0u;

/** @brief Logging levels */
typedef enum {
    LOG_ERROR,  /**< Error level */
    LOG_INFO,   /**< Info level */
    LOG_DEBUG   /**< Debug level */
} log_level_t;

/** @brief Error codes for CAN relay operations */
typedef enum {
    CAN_RELAY_SUCCESS = 0,               /**< Operation successful */
    CAN_RELAY_ERROR_CAN_NOT_OPEN = -1,   /**< Failed to open CAN socket */
    CAN_RELAY_ERROR_INVALID_INDEX = -2,  /**< Invalid relay index */
    CAN_RELAY_ERROR_NULL_POINTER = -3,   /**< Null pointer passed */
    CAN_RELAY_ERROR_CAN_SEND_FAILED = -4 /**< Failed to send CAN frame */
} can_relay_error_t;

/**
 * @brief Log a message with specified level.
 * @param level The logging level.
 * @param msg The message to log.
 */
static void log_message(log_level_t level, const char *msg)
{
    const char *level_str[] = {"ERROR", "INFO", "DEBUG"};
    fprintf(stderr, "[%s] %s\n", level_str[level], msg);
}

/* ---------- Platform CAN backend (SocketCAN) ---------- */

/** @brief Default CAN interface name */
static char can_ifname[IF_NAMESIZE] = "can0";
/** @brief CAN socket file descriptor */
static int can_sock = -1;

/**
 * @brief Open CAN socket on the given interface name.
 * @param ifname The CAN interface name (e.g., "can0"). If NULL or empty, uses default.
 * @return CAN_RELAY_SUCCESS on success, error code on failure.
 */
can_relay_error_t can_platform_open(const char *ifname)
{
    if (ifname != NULL && ifname[0] != '\0') {
        strncpy(can_ifname, ifname, IF_NAMESIZE - 1u);
        can_ifname[IF_NAMESIZE - 1u] = '\0';
    }

    if (can_sock >= 0) {
        log_message(LOG_DEBUG, "CAN socket already open");
        return CAN_RELAY_SUCCESS;
    }

    struct ifreq ifr;
    struct sockaddr_can addr;

    can_sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (can_sock < 0) {
        log_message(LOG_ERROR, "Failed to create CAN socket");
        return CAN_RELAY_ERROR_CAN_NOT_OPEN;
    }

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, can_ifname, IF_NAMESIZE - 1u);
    if (ioctl(can_sock, SIOCGIFINDEX, &ifr) < 0) {
        log_message(LOG_ERROR, "Failed to get CAN interface index");
        close(can_sock);
        can_sock = -1;
        return CAN_RELAY_ERROR_CAN_NOT_OPEN;
    }

    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(can_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        log_message(LOG_ERROR, "Failed to bind CAN socket");
        close(can_sock);
        can_sock = -1;
        return CAN_RELAY_ERROR_CAN_NOT_OPEN;
    }

    log_message(LOG_INFO, "CAN socket opened successfully");
    return CAN_RELAY_SUCCESS;
}

/**
 * @brief Close the CAN socket.
 */
void can_platform_close(void)
{
    if (can_sock >= 0) {
        close(can_sock);
        can_sock = -1;
        log_message(LOG_INFO, "CAN socket closed");
    }
}

/* ---------- Hardware abstraction -- weak, override in application if needed ---------- */

/**
 * @brief Initialize relay hardware.
 * @note This is a weak function; override in application for actual GPIO initialization.
 */
__attribute__((weak)) void relay_hw_init(void)
{
    /* user should initialize GPIOs in their override */
    log_message(LOG_DEBUG, "relay_hw_init called (weak implementation)");
}

/**
 * @brief Set relay state.
 * @param idx Relay index.
 * @param on True to turn on, false to turn off.
 * @note This is a weak function; override in application for actual relay control.
 */
__attribute__((weak)) void relay_hw_set(uint8_t idx, bool on)
{
    (void)idx; (void)on;
    /* user should implement actual relay drive in their override */
    log_message(LOG_DEBUG, "relay_hw_set called (weak implementation)");
}

/**
 * @brief Send a CAN frame.
 * @param id CAN identifier.
 * @param data Pointer to data payload (can be NULL if len is 0).
 * @param len Length of data (0-8).
 * @return CAN_RELAY_SUCCESS on success, error code on failure.
 */
can_relay_error_t can_hw_send(uint32_t id, const uint8_t *data, uint8_t len)
{
    if (can_sock < 0) {
        log_message(LOG_ERROR, "CAN socket not open");
        return CAN_RELAY_ERROR_CAN_SEND_FAILED;
    }
    struct can_frame frame;
    memset(&frame, 0, sizeof(frame));

    /* ensure we don't exceed 8 bytes (classic CAN) */
    if (len > 8u) {
        len = 8u;
    }
    frame.can_id = id & CAN_SFF_MASK;
    frame.can_dlc = len;
    if (len > 0u && data != NULL) {
        memcpy(frame.data, data, len);
    }

    ssize_t n = write(can_sock, &frame, sizeof(frame));
    if (n != (ssize_t)sizeof(frame)) {
        log_message(LOG_ERROR, "Failed to write CAN frame");
        return CAN_RELAY_ERROR_CAN_SEND_FAILED;
    }
    log_message(LOG_DEBUG, "CAN frame sent");
    return CAN_RELAY_SUCCESS;
}

/* ---------- Helpers ---------- */

/**
 * @brief Check if relay index is valid.
 * @param idx Relay index.
 * @return True if valid, false otherwise.
 */
static inline bool valid_index(uint8_t idx)
{
    return idx < MAX_RELAYS;
}

/**
 * @brief Apply hardware relay state.
 * @param idx Relay index.
 * @param on True to turn on, false to turn off.
 */
static inline void hw_apply(uint8_t idx, bool on)
{
    relay_hw_set(idx, on);
}

/* ---------- Public API ---------- */

/**
 * @brief Initialize the relay module and hardware.
 * @param can_iface CAN interface name (e.g., "can0"). If NULL, uses default.
 * @return 0 on success, -1 on CAN open failure.
 */
int can_relay_init_ex(const char *can_iface)
{
    relay_state_mask = 0u;
    relay_hw_init();
    /* try to open CAN interface; if it fails, still return -1 so caller can handle */
    if (can_platform_open(can_iface) != CAN_RELAY_SUCCESS) {
        log_message(LOG_ERROR, "Failed to initialize CAN relay");
        return -1;
    }
    log_message(LOG_INFO, "CAN relay initialized");
    return 0;
}

/**
 * @brief Initialize the relay module with default CAN interface ("can0").
 * @note Backwards-compatible wrapper.
 */
void can_relay_init(void)
{
    relay_state_mask = 0u;
    relay_hw_init();
    /* best-effort open; ignore return for legacy callers */
    (void)can_platform_open(NULL);
    log_message(LOG_INFO, "CAN relay initialized (legacy)");
}

/**
 * @brief Close the relay module and CAN socket.
 */
void can_relay_close(void)
{
    can_platform_close();
}

/**
 * @brief Set a relay to on/off.
 * @param idx Relay index.
 * @param on True to turn on, false to turn off.
 * @return True if index valid and operation applied, false otherwise.
 */
bool can_relay_set(uint8_t idx, bool on)
{
    if (!valid_index(idx)) {
        log_message(LOG_ERROR, "Invalid relay index");
        return false;
    }
    if (on) {
        relay_state_mask |= (1u << idx);
    } else {
        relay_state_mask &= ~(1u << idx);
    }
    hw_apply(idx, on);
    log_message(LOG_DEBUG, "Relay set");
    return true;
}

/**
 * @brief Toggle a relay.
 * @param idx Relay index.
 * @return True if index valid, false otherwise.
 */
bool can_relay_toggle(uint8_t idx)
{
    if (!valid_index(idx)) {
        log_message(LOG_ERROR, "Invalid relay index");
        return false;
    }
    bool new_state = !((relay_state_mask >> idx) & 1u);
    return can_relay_set(idx, new_state);
}

/**
 * @brief Get current relay state.
 * @param idx Relay index.
 * @return True if on, false if off or index invalid.
 */
bool can_relay_get(uint8_t idx)
{
    if (!valid_index(idx)) {
        log_message(LOG_ERROR, "Invalid relay index");
        return false;
    }
    return (relay_state_mask >> idx) & 1u;
}

/* ---------- Status sending ---------- */

/**
 * @brief Send status of all relays.
 */
static void send_status_all(void)
{
    uint8_t data[3u];
    data[0u] = STATUS_ALL;
    data[1u] = (uint8_t)(relay_state_mask & 0xFFu);
    data[2u] = (uint8_t)((relay_state_mask >> 8u) & 0xFFu);
    (void)can_hw_send(CAN_STATUS_ID, data, 3u);
    log_message(LOG_DEBUG, "Sent all relay status");
}

/**
 * @brief Send status of a single relay.
 * @param idx Relay index.
 */
static void send_status_single(uint8_t idx)
{
    uint8_t data[3u];
    data[0u] = STATUS_SINGLE;
    data[1u] = idx;
    data[2u] = (uint8_t)((relay_state_mask >> idx) & 1u);
    (void)can_hw_send(CAN_STATUS_ID, data, 3u);
    log_message(LOG_DEBUG, "Sent single relay status");
}

/**
 * @brief Handle incoming CAN message for relay control.
 * @param can_id CAN identifier.
 * @param data Pointer to message payload.
 * @param len Length of payload.
 * @return True if message was consumed by this module, false otherwise.
 */
bool can_relay_handle_can_msg(uint32_t can_id, const uint8_t *data, uint8_t len)
{
    if (can_id != CAN_CMD_ID) {
        return false;
    }
    if (len == 0u || data == NULL) {
        log_message(LOG_DEBUG, "Malformed CAN message");
        return true; /* malformed but consumed */
    }

    uint8_t op = data[0u];

    switch (op) {
    case OPCODE_SET:
        if (len < 3u) {
            log_message(LOG_ERROR, "Invalid SET command length");
            break;
        }
        if (valid_index(data[1u])) {
            can_relay_set(data[1u], data[2u] != 0u);
            send_status_single(data[1u]);
        } else {
            log_message(LOG_ERROR, "Invalid relay index in SET");
        }
        break;

    case OPCODE_TOGGLE:
        if (len < 2u) {
            log_message(LOG_ERROR, "Invalid TOGGLE command length");
            break;
        }
        if (valid_index(data[1u])) {
            can_relay_toggle(data[1u]);
            send_status_single(data[1u]);
        } else {
            log_message(LOG_ERROR, "Invalid relay index in TOGGLE");
        }
        break;

    case OPCODE_QUERY:
        if (len < 2u) {
            log_message(LOG_ERROR, "Invalid QUERY command length");
            break;
        }
        if (valid_index(data[1u])) {
            send_status_single(data[1u]);
        } else {
            log_message(LOG_ERROR, "Invalid relay index in QUERY");
        }
        break;

    case OPCODE_QUERY_ALL:
        send_status_all();
        break;

    default:
        /* unknown opcode: ignore */
        log_message(LOG_DEBUG, "Unknown opcode received");
        break;
    }

    return true;
}

/**
 * @brief Set all relays by bitmask.
 * @param mask Bitmask for relay states.
 * @return True on success.
 */
bool can_relay_set_mask(uint16_t mask)
{
    for (uint8_t i = 0u; i < MAX_RELAYS; ++i) {
        bool on = (mask >> i) & 1u;
        hw_apply(i, on);
    }
    relay_state_mask = mask & ((1u << MAX_RELAYS) - 1u);
    log_message(LOG_DEBUG, "Relay mask set");
    return true;
}

/* ---------- Signal sending functions ---------- */

/**
 * @brief Send battery level signal.
 * @param level Battery level (0-255).
 * @return CAN_RELAY_SUCCESS on success, error code on failure.
 */
can_relay_error_t send_battery_level(uint8_t level)
{
    uint8_t data[1u] = {level};
    can_relay_error_t ret = can_hw_send(CAN_BATTERY_ID, data, 1u);
    if (ret == CAN_RELAY_SUCCESS) {
        log_message(LOG_DEBUG, "Battery level sent");
    }
    return ret;
}

/**
 * @brief Send velocity signal.
 * @param velocity Velocity value.
 * @return 0 on success, -1 on failure.
 */
int send_velocity(float velocity)
{
    uint8_t data[4u];
    memcpy(data, &velocity, sizeof(float));
    int ret = can_hw_send(CAN_VELOCITY_ID, data, 4u);
    if (ret == 0) {
        log_message(LOG_DEBUG, "Velocity sent");
    }
    return ret;
}

/**
 * @brief Send charging active signal.
 * @param active True if charging is active.
 * @return 0 on success, -1 on failure.
 */
int send_charging_active(bool active)
{
    uint8_t data[1u] = {active ? 1u : 0u};
    int ret = can_hw_send(CAN_CHARGING_ACTIVE_ID, data, 1u);
    if (ret == 0) {
        log_message(LOG_DEBUG, "Charging active sent");
    }
    return ret;
}

/**
 * @brief Send charge request signal.
 * @param request True if requesting charge.
 * @return 0 on success, -1 on failure.
 */
int send_charge_request(bool request)
{
    uint8_t data[1u] = {request ? 1u : 0u};
    int ret = can_hw_send(CAN_CHARGE_REQUEST_ID, data, 1u);
    if (ret == 0) {
        log_message(LOG_DEBUG, "Charge request sent");
    }
    return ret;
}