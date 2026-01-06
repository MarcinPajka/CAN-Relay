#ifndef CAN_RELAY_H
#define CAN_RELAY_H

#include <stdint.h>
#include <stdbool.h>

// CAN IDs for signals
#define CAN_BATTERY_ID     0x100u
#define CAN_VELOCITY_ID    0x101u
#define CAN_CHARGING_ACTIVE_ID 0x102u
#define CAN_CHARGE_REQUEST_ID  0x103u

// Initialization
int can_relay_init_ex(const char *can_iface);
void can_relay_init(void);
void can_relay_close(void);

// Relay control (existing)
bool can_relay_set(uint8_t idx, bool on);
bool can_relay_toggle(uint8_t idx);
bool can_relay_get(uint8_t idx);
bool can_relay_set_mask(uint16_t mask);

// CAN message handling
bool can_relay_handle_can_msg(uint32_t can_id, const uint8_t *data, uint8_t len);

// Signal sending
int send_battery_level(uint8_t level);
int send_velocity(float velocity);
int send_charging_active(bool active);
int send_charge_request(bool request);

#endif // CAN_RELAY_H