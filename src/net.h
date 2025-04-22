#ifndef NET_H
#define NET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
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

int perform_dh_exchange_server(int client_socket, unsigned char *session_key);
int perform_dh_exchange_client(int server_socket, unsigned char *session_key);

//volatile int chat_running = true;
extern char *IP_ADDRESS;
extern bool chat_running;

#endif