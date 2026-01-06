#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include "can_relay.h"

#define PORT 5000U
#define BUFFER_SIZE 1024U
#define SIGNAL_NAME_MAX_LEN 50U
#define BACKLOG 5

static bool extract_signal(const char * const json, char * const signal)
{
    bool success = false;
    const char *signal_start = strstr(json, "\"signal\"");
    if (signal_start != NULL)
    {
        signal_start = strstr(signal_start, "\"");
        if (signal_start != NULL)
        {
            signal_start++;
            const char *signal_end = strstr(signal_start, "\"");
            if (signal_end != NULL)
            {
                size_t len = (size_t)(signal_end - signal_start);
                if (len < SIGNAL_NAME_MAX_LEN)
                {
                    (void)strncpy(signal, signal_start, len);
                    signal[len] = '\0';
                    success = true;
                }
            }
        }
    }
    return success;
}

static bool extract_value(const char * const json, const char * const signal, void * const value, int * const type)
{
    bool success = false;
    const char *value_start = strstr(json, "\"value\"");
    if (value_start != NULL)
    {
        value_start = strstr(value_start, ":");
        if (value_start != NULL)
        {
            value_start++;
            while (*value_start == ' ')
            {
                value_start++;
            }
            if (strcmp(signal, "battery_level") == 0)
            {
                *type = 0;
                *(int *)value = atoi(value_start);
                success = true;
            }
            else if (strcmp(signal, "velocity") == 0)
            {
                *type = 1;
                *(float *)value = (float)atof(value_start);
                success = true;
            }
            else if ((strcmp(signal, "charging_active") == 0) || (strcmp(signal, "charge_request") == 0))
            {
                *type = 2;
                if ((strstr(value_start, "true") != NULL) || (strstr(value_start, "1") != NULL))
                {
                    *(bool *)value = true;
                }
                else
                {
                    *(bool *)value = false;
                }
                success = true;
            }
            else
            {
                /* Unknown signal */
            }
        }
    }
    return success;
}

/* Simple JSON parser for {"signal": "name", "value": value} */
static int parse_json(const char * const json, char * const signal, void * const value, int * const type)
{
    int result = -1;
    bool parsing_success = false;

    if (json != NULL)
    {
        if (extract_signal(json, signal))
        {
            if (extract_value(json, signal, value, type))
            {
                parsing_success = true;
            }
        }
    }

    if (parsing_success)
    {
        result = 0;
    }

    return result;
}

static void handle_client(int client_sock)
{
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read = read(client_sock, buffer, BUFFER_SIZE - 1U);
    if (bytes_read > 0)
    {
        buffer[bytes_read] = '\0';

        char signal[SIGNAL_NAME_MAX_LEN];
        union
        {
            int i;
            float f;
            bool b;
        } val;
        int type;
        if (parse_json(buffer, signal, &val, &type) == 0)
        {
            if ((strcmp(signal, "battery_level") == 0) && (type == 0))
            {
                (void)send_battery_level((uint8_t)val.i);
            }
            else if ((strcmp(signal, "velocity") == 0) && (type == 1))
            {
                (void)send_velocity(val.f);
            }
            else if ((strcmp(signal, "charging_active") == 0) && (type == 2))
            {
                (void)send_charging_active(val.b);
            }
            else if ((strcmp(signal, "charge_request") == 0) && (type == 2))
            {
                (void)send_charge_request(val.b);
            }
            else
            {
                /* Invalid signal or type */
            }
        }
    }
    (void)close(client_sock);
}

int ethernet_init(void)
{
    int server_sock = -1;
    bool initialization_success = false;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock >= 0)
    {
        struct sockaddr_in server_addr;
        (void)memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(PORT);

        if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) >= 0)
        {
            if (listen(sock, BACKLOG) >= 0)
            {
                /* Set non-blocking */
                if (fcntl(sock, F_SETFL, O_NONBLOCK) >= 0)
                {
                    server_sock = sock;
                    initialization_success = true;
                }
            }
        }

        if (!initialization_success)
        {
            (void)close(sock);
        }
    }

    return server_sock;
}

void ethernet_handle(int server_sock)
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
    if (client_sock >= 0)
    {
        handle_client(client_sock);
    }
}
