#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <getopt.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

/* !!!!!!!!!!!!!!!!!!!!!!!!! */
/*    DEFINE stuff here  
Maybe move to a header file??
/*****************************/
#define SERVER_PORT 31337
#define BUFFER_SIZE 1024


/* ~~~~~ Function Prototypes ~~~~~~
/////////////////////////////////////*/
int validate_ip(const char *in_addr);
void run_server(); // The server side of network connnection, waits for client to connect
void run_client(); // The client side of the network connection
void comms_loop(int sock_fd); // The function accepts a socket file descriptor and maintains a communication loop


/* ~~~~~~ Globals ~~~~~*/
char *IP_ADDRESS = "127.0.0.1";
//static char default_msg[] = "Establishing Connection ...";


/*****************/
/* Start Here */
/*vvvvvvvvvvvvvvvv*/
int main(int argc, char *argv[]){
    // Arg Parser
    int opt;
    char *user_in_addr = NULL;
    while((opt = getopt(argc, argv, "cs")) != -1){
        switch (opt){
            case 'c':
                run_client();
                break;
            case 's':
                run_server();
                break;
            case '?':
            default:
               fprintf(stderr, "Usage: %s -s <ip_address>\n", argv[0]);
               return -1;
        }
    } // End Arg Parser While loop

    //IP check
    /*
    if(user_in_addr == NULL){
        fprintf(stderr, "!! Error: Please provide an IP Address.\n");
        fprintf(stderr, "Usage: %s -i <ip_address>\n", argv[0]);
        return 1;
    }

    if (validate_ip(user_in_addr)){
        printf("## Connecting to IP Address: %s\n", user_in_addr);
        run_client();
    }
    else{
        fprintf(stderr, "!! Error: Invalid IP address");
    }

    */

} 

/* ---- End Main ----*/
/********************************************************************/



/************************/
/* Begin run_server code*/
/************************/
void run_server(){
    /************************/
    /* Server Listener code */

    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    
    

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket < 0){
        perror("!! Socket creation failed");
        exit(EXIT_FAILURE);
    }

    
    server_address.sin_family = AF_INET; 
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if(bind( server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0){
        perror("!! bind failed");
        exit(EXIT_FAILURE);
    }

    //Listen
    if(listen(server_socket, 1) < 0){
        perror("!! Listener failure");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Listening on port ---> %d\n", SERVER_PORT);
    socklen_t clientLen = sizeof(client_address);
    client_socket = accept(server_socket, (struct sockaddr *)&client_address ,&clientLen);
    if(client_socket < 0){
        perror("Client Accept Failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    } 

    printf(":: Connection established - %s\n", inet_ntoa(client_address.sin_addr));

    char buffer[BUFFER_SIZE];
    char msg[] = "What's the BLUF?";
    printf("!! Testing clear text\nSending ---> '%s'\n", msg);
    
    int bytes_read = read(client_socket, buffer, BUFFER_SIZE);
    buffer[bytes_read] = '\0';
    printf("[%s]: %s\n", IP_ADDRESS, buffer);

    send(client_socket, msg, strlen(msg), 0);

    close(client_socket);
    close(server_socket);

}
/* End run_server() */
/*****************************************************************/

/********************/
/* Begin run_client()*/
/********************/
void run_client(){
    int cl_sock;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE] = "It's all chicken feed.";

    
    if((cl_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("!! Socket Failure");
        exit(EXIT_FAILURE);
    }
    
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, IP_ADDRESS, &server_address.sin_addr);

    if(connect(cl_sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0){
        perror("!! Connection failure");
        close(cl_sock);
        exit(EXIT_FAILURE);
    }
    send(cl_sock, buffer, strlen(buffer), 0);
    printf("## Message to server ---> %s\n", buffer);

    int bytes_read = read(cl_sock, buffer, BUFFER_SIZE);
    buffer[bytes_read] = '\0';
    printf("[%s]: %s\n", IP_ADDRESS, buffer);
    close(cl_sock);
}
/* End run_client()*/
/*************************/

/********************/
/* Begin comms_loop()*/
/********************/
void comms_loop(int ne_socket){
    char buffer[BUFFER_SIZE];
    fd_set readfds; 
    int max_fd;

    if(ne_socket > STDIN_FILENO){
        max_fd = ne_socket;
    }else{
        max_fd = STDIN_FILENO;
    }

    while(1){
        FD_ZERO(&readfds);
        FD_SET()
    }
}

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
} 
/* End validate_ip() ***/
/***************************************** */
