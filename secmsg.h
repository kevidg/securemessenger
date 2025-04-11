// secmsg.h
#ifndef SECMSG_HEADERS_H
#define SECMSG_HEADERS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <getopt.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

/* !!!!!!!!!!!!!!!!!!!!!!!!! */
/*    DEFINE stuff here  
Maybe move to a header file??
/*****************************/
#define SERVER_PORT 31337
#define BUFFER_SIZE 1024


/* ~~~~~ Function Prototypes ~~~~~~
/////////////////////////////////////*/
// User Input Functions
int validate_ip(const char *in_addr);

// Network Functions
void run_server(); // The server side of network connnection, waits for client to connect
void run_client(); // The client side of the network connection
void comms_loop(int sock_fd); // The function accepts a socket file descriptor and maintains a communication loop
void *msg_receiver(void *arg); // A function called by the thread for receiving a message

//Encrption Functions
void generate_aes_key_iv();
int aes_encrypt(const unsigned char *plaintxt, int plaintxt_len, unsigned char *ciphertxt);
int aes_decrypt(const unsigned char *ciphertxt, int ciphertxt_len, unsigned char *plaintxt);

/* ~~~~~~ Globals ~~~~~*/
volatile int chat_running = true;
char *IP_ADDRESS = "127.0.0.1";

//Cipher variables
// For testing these are hard coded keys and IVs
unsigned char aes_key[16] = "myonlypasswordis";
unsigned char aes_iv[16] = "iforgotwhatisaid";

#endif // SECMSG_HEADERS_H