#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include "can_relay.h"
#include "ethernet_communication_handler.h"

volatile bool running = true;

void signal_handler(int sig) {
    (void)sig;
    running = false;
}

int main() {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

  
    // Initialize ethernet server
    int server_sock = ethernet_init();
    if (server_sock < 0) {
        fprintf(stderr, "Failed to initialize ethernet server\n");
        return 1;
    }

    printf("Relay server started. Listening on port %d\n", 5000);

    while (running) {
        ethernet_handle(server_sock);
        usleep(10000);  // 10ms delay
    }

    close(server_sock);
    printf("Relay server stopped\n");
    return 0;
}
