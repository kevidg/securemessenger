#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* !!!!!!!!!!!!!!!!!!!!!!!!! */
/* !!!! DEFINE stuff here !!!!! 
Maybe move to a header file??
/*****************************/
#define server_port 31337
#define client_port 31339

/* ~~~~~ Function Prototypes ~~~~~~
/////////////////////////////////////*/
int validate_ip(const char *in_addr);


/* ~~~~~~ Globals ~~~~~*/
int IP_ADDRESS;


/*****************/
/* Start Here */
/*vvvvvvvvvvvvvvvv*/
int main(int argc, char *argv[]){
    // Arg Parser
    int opt;
    char *user_in_addr = NULL;
    while((opt = getopt(argc, argv, "i:")) != -1){
        switch (opt){
            case 'i':
                user_in_addr = optarg;
                break;
            case '?':
            default:
               fprintf(stderr, "Usage: %s -i <ip_address>\n", argv[0]);
               return -1;
        }
    } // End Arg Parser While loop

    //IP check

    if(user_in_addr == NULL){
        fprintf(stderr, "!! Error: Please provide an IP Address.\n");
        fprintf(stderr, "Usage: %s -i <ip_address>\n", argv[0]);
        return 1;
    }

    if (validate_ip(user_in_addr)){
        printf("## Connecting to IP Address: %s\n", user_in_addr);
    }
    else{
        fprintf(stderr, "!! Error: Invalid IP address");
    }

    /************************/
    /* Server Listener code */

    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(server_port);

    if(bind( server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

} // ---- End Main ----

/***************** */
/*Input Validation*/
/**************** */
int validate_ip(const char *in_addr){
    int segments = 0;
    int num;
    char *token;
    char *cp_in_addr = strdup(in_addr);

    if (!cp_in_addr){
        return 0;
    }

    token =strtok(cp_in_addr, ".");
    while(token){
        if(isdigit(token[0]) == 0)
        {
            free(cp_in_addr);
            return 0;
        }
        
        num = atoi(token);
        if(num < 0 || num > 255){
            free(cp_in_addr);
            return 0;
        }

            segments++;
            token = strtok(NULL, ".");
    }
    free(cp_in_addr);

    return (segments == 4);
} // End validate_ip()