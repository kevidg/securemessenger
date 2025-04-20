// secmsg.h
#ifndef SECMSG_H
#define SECMSG_H


#include <stdbool.h>

#include <pwd.h>
#include <time.h>

#include <sys/select.h>
#include <getopt.h>
#include <ctype.h>

#include <netinet/in.h>




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
const char *get_default_name();




//File & Logging Functions
void log_msg(const char *filename, const char *sender, const char *message);

/* ~~~~~~ Globals ~~~~~*/
//volatile int chat_running = true;
char *IP_ADDRESS = "127.0.0.1";
bool chat_running = true;

//Cipher variables
// For testing these are hard coded keys and IVs
unsigned char aes_key[16] = "myonlypasswordis";
unsigned char aes_iv[16] = "iforgotwhatisaid";

#endif // SECMSG_HEADERS_H