#include "secmsg.h"

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
    /*The socklen_t type is used in socket programming to represent the size of a socket address structure. 
    It is typically used in functions that deal with socket addresses, such as accept(), getsockname(), and
    getpeername().*/
    client_socket = accept(server_socket, (struct sockaddr *)&client_address ,&clientLen);
    if(client_socket < 0){
        perror("Client Accept Failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    } 

    printf(":: Connected to [name] :: Type message || Type 'quit()' to exit.\n");
    
    comms_loop(client_socket);

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
    //char buffer[BUFFER_SIZE];

    
    if((cl_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("!! Socket Failure");
        exit(EXIT_FAILURE);
    }
    
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    // The following function takes a string(IP_ADDRESS) and converts to an IPv4 binary and stores it 
    // in the sockaddr_in struct (server_address)
    inet_pton(AF_INET, IP_ADDRESS, &server_address.sin_addr);

    if(connect(cl_sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0){
        perror("!! Connection failure");
        close(cl_sock);
        exit(EXIT_FAILURE);
    }

    printf(":: Connected to [name] :: Type message || Type 'quit()' to exit.\n");
    comms_loop(cl_sock);
    close(cl_sock);
}
/* End run_client()*/
/*************************/

/********************/
/* Begin comms_loop()*/
/********************/
void comms_loop(int ne_socket){
    char buffer[BUFFER_SIZE];

    //Create new thread for receiving messages
    pthread_t receiver_thread;
    pthread_create(&receiver_thread, NULL, msg_receiver, &ne_socket );

    while(1){
        memset(buffer, 0, BUFFER_SIZE); // clear buffer
        fgets(buffer, BUFFER_SIZE, stdin); //Get input form user
        if(strncmp(buffer, "quit()", 6) == 0){
            break;
        }
        send(ne_socket, buffer, strlen(buffer), 0);
    }
    pthread_join(receiver_thread, NULL);
    
}
/***** End comms_loop() ******/

/***************************/
/*  Begin msg_receiver()  */
/************************ */
//When a connection is made a thread is created to receive the incoming messages, 
//this function is called by the new thread. 
//
void *msg_receiver(void *arg){
    int sock = *(int *)arg; //the incoming socket from the function argument
    char buffer[BUFFER_SIZE];

    while(1){
        memset(buffer, 0, BUFFER_SIZE); // Clears the buffer
        int bytes_received = recv(sock, buffer, BUFFER_SIZE, 0); // reads the size of the incoming message
        if(bytes_received <= 0){ // if no bytes come in the connection is closed
            printf("!! Connection Lost / Ended\n");
            break;
        }
        printf("[Name]: %s\n", buffer);
    }
    return NULL;
}

/******  End msg_receiver() ******/

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
