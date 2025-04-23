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
void run_server(const char *username); 
void run_client(const char *username); 
void comms_loop(int sock_fd, const char *useername, const char *contact_name); 


int perform_dh_exchange_server(int client_socket, unsigned char *session_key);
int perform_dh_exchange_client(int server_socket, unsigned char *session_key);


extern char *IP_ADDRESS;
extern bool chat_running;

#endif