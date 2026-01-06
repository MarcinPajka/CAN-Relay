#ifndef ETHERNET_COMMUNICATION_HANDLER_H
#define ETHERNET_COMMUNICATION_HANDLER_H

// Initialize ethernet server, returns server socket fd
int ethernet_init();

// Handle incoming connections and messages (non-blocking)
void ethernet_handle(int server_sock);

#endif // ETHERNET_COMMUNICATION_HANDLER_H