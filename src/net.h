#ifndef NET_H
#define NET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "crypto.h"
#include "logger.h"
#include "util.h"

// Network Functions
void run_server(const char *username); // The server side of network connnection, waits for client to connect
void run_client(const char *username); // The client side of the network connection
void comms_loop(int sock_fd, const char *useername, const char *contact_name); // The function accepts a socket file descriptor and maintains a communication loop
//void *msg_receiver(void *arg); // A function called by the thread for receiving a message

#endif