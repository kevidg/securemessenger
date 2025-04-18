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

    int server_socket, client_socket, opt;
    struct sockaddr_in server_address, client_address;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    if(server_socket < 0){
        perror("!! Socket Creation Failed");
        exit(EXIT_FAILURE);
    }

    
    server_address.sin_family = AF_INET; 
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); //reuse address after exiting

    if(bind( server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0){
        perror("!! Bind Failed");
        exit(EXIT_FAILURE);
    }

    //Listen
    if(listen(server_socket, 1) < 0){
        perror("!! Listener Failure");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Listening on port ---> %d\n", SERVER_PORT);
    socklen_t clientLen = sizeof(client_address);
    client_socket = accept(server_socket, (struct sockaddr *)&client_address ,&clientLen);
    if(client_socket < 0){
        perror("!! Client Accept Failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    } 

    
    
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

    comms_loop(cl_sock);
    close(cl_sock);
}
/* End run_client()*/
/*************************/

/********************/
/* Begin comms_loop()*/
/********************/
void comms_loop(int ne_socket){
    fd_set readfds; 
    char buffer[BUFFER_SIZE];
    unsigned char ciphertxt[BUFFER_SIZE];
    unsigned char plaintext[BUFFER_SIZE];
    int max_fd;

    if(ne_socket > STDIN_FILENO){
        max_fd = ne_socket;
    }else{
        max_fd = STDIN_FILENO;
    }

    printf(":: Connected to [name] :: \n");
    printf(":: Type message || Type '/quit' to exit ::\n");

    while(chat_running){
        FD_ZERO(&readfds);
        FD_SET(ne_socket, &readfds); // monitors network
        FD_SET(STDIN_FILENO, &readfds); // monitors keyboard, stdin

        if (select(max_fd + 1, &readfds, NULL, NULL, NULL) < 0){
            perror("!! Select Error");
            break;
        }

        //Check for user input
        if(FD_ISSET(STDIN_FILENO, &readfds)){
            memset(buffer, 0, BUFFER_SIZE); // Clears the buffer
            if(fgets(buffer, sizeof(buffer), stdin) == NULL){
                printf("!! Input Stream Closed");
                break;
            }

            // strip the '/n' from the end
            buffer[strcspn(buffer, "\n")] = '\0';

            // Check for a clear screen cmd
            if(strcasecmp(buffer, "/clear") == 0){
                printf("\033[2J\033[H"); // ANSI escape codes to clear the screen 
                fflush(stdout);
                continue;
            } 
            
            // Encrypt and Send
            int ciphertext_len = aes_encrypt((unsigned char*)buffer, strlen(buffer), ciphertxt);
            send(ne_socket, ciphertxt, ciphertext_len, 0);

            if(strcasecmp(buffer, "/quit") == 0){
                shutdown(ne_socket, SHUT_RDWR);
                break;
            }
        }

        //Check for network data
        if(FD_ISSET(ne_socket, &readfds)){
            int bytes_received = recv(ne_socket, buffer, BUFFER_SIZE, 0); // reads the size of the incoming message
       
            if(bytes_received <= 0){ // if no bytes come in the connection is closed
                printf("!! Connection Ended.\n");
                break;
            }

            // Decryption funtion for received data    
            int plaintext_len = aes_decrypt((unsigned char*)buffer, bytes_received, plaintext);
            plaintext[plaintext_len] = '\0'; // Null-terminate

            // Check if the quit() cmd was sent
            if(strcasecmp((char*)plaintext, "/quit") == 0){
                    printf(":: Chat ended by [name].\n");
                    break;
            }
            printf("[name] >> %s\n", plaintext);
        }
    }
    printf(":: Chat Session Ended ::\n");
}
/* End comms_loop() */
/***************************************************************/


/********************/
/* Begin generate_aes_key_iv() */
/*********************/
void generate_aes_key_iv(){
    RAND_bytes(aes_key, sizeof(aes_key));
    RAND_bytes(aes_iv, sizeof(aes_iv));
}
/* End generate_aes_key_iv() */
/*****************************************************************/

/**********************/
/* Begin aes_encrypt()*/
/**********************/
int aes_encrypt(const unsigned char *plaintxt, int plaintxt_len, unsigned char *ciphertxt){
    EVP_CIPHER_CTX *ctx;
    int len, ciphertxt_len;

    // intialize the cipher context
    if(!(ctx = EVP_CIPHER_CTX_new())){
        perror("!! CTX not initialized");
    }
    // initialize the encryptiong operation
    if(EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, aes_key, aes_iv) != 1){
        perror("!! Encrypt Op not Initialized");
    }
    // Provide message to be encrypted
    if(EVP_EncryptUpdate(ctx, ciphertxt, &len, plaintxt, plaintxt_len) != 1){
        perror("!! Error encrypting message");
    }
    ciphertxt_len = len;
    // Finalize encryption
    if(EVP_EncryptFinal_ex(ctx, ciphertxt + len, &len) != 1){
        perror("!!Error Finalizing encryption");
    }
    ciphertxt_len += len;

    // Clean up
    EVP_CIPHER_CTX_free(ctx);
    return ciphertxt_len;

}
/* End aes_encrypt()*/
/*****************************************************************************/

/**********************/
/* Begin aes_decrypt()*/
/**********************/
int aes_decrypt(const unsigned char *ciphertxt, int ciphertxt_len, unsigned char *plaintxt){
    EVP_CIPHER_CTX *ctx;
    int len, plaintxt_len;

    // intialize the cipher context
    if(!(ctx = EVP_CIPHER_CTX_new())){
        perror("!! CTX not initialized");
    }
    // initialize the decryption operation
    if(EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, aes_key, aes_iv) != 1){
        perror("!! Decrypt Op not Initialized");
    }
    // Provide message to be decrypted
    if(EVP_DecryptUpdate(ctx, plaintxt, &len, ciphertxt, ciphertxt_len) != 1){
        perror("!! Error decrypting message");
    }
    plaintxt_len = len;

    // Finalize encryption
    if(EVP_DecryptFinal_ex(ctx, plaintxt + len, &len) != 1){
        perror("!! Error Finalizing decryption");
    }
    plaintxt_len += len;

    // Clean up
    EVP_CIPHER_CTX_free(ctx);
    return plaintxt_len;

}
/* End aes_decrypt()*/
/*****************************************************************************/

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
