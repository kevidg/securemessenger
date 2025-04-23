
#include "net.h"
#include "crypto.h"
#include "secmsg.h"
#include "dh.h"  



char *IP_ADDRESS = "127.0.0.1";
bool chat_running = true; 


/************************/
/* Begin run_server() 
/************************/
void run_server(const char *username){
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
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); 

    if(bind( server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0){
        perror("!! Bind Failed");
        exit(EXIT_FAILURE);
    }


    if(listen(server_socket, 1) < 0){
        perror("!! Listener Failure");
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
        perror("!! Client Accept Failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    } 

   
    printf("Server: Starting DH key exchange...\n");
    unsigned char session_key[32];  
    if (perform_dh_exchange_server(client_socket, session_key) != 0) {
        printf("Server: DH exchange failed\n");
        close(client_socket);
        close(server_socket);
        return;
    }
    printf("Server: DH exchange completed successfully\n");

   
    if (set_crypto_key(session_key, 32) != 0) {
        printf("Server: Failed to set session key\n");
        close(client_socket);
        close(server_socket);
        return;
    }


    memset(session_key, 0, sizeof(session_key));

   
    
    send(client_socket, username, strlen(username), 0);
    
    char contact_name[64]={0};
    recv(client_socket, contact_name, sizeof(contact_name), 0);
    
    
    comms_loop(client_socket, username, contact_name);

    close(server_socket);
     
    return;

}
/* End run_server() */
/*****************************************************************/

/********************/
/* Begin run_client()
/********************/
void run_client(const char *username){
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

 
    printf("Client: Starting DH key exchange...\n");
    unsigned char session_key[32];  
    if (perform_dh_exchange_client(cl_sock, session_key) != 0) {
        printf("Client: DH exchange failed\n");
        close(cl_sock);
        return;
    }
    printf("Client: DH exchange completed successfully\n");


    if (set_crypto_key(session_key, 32) != 0) {
        printf("Client: Failed to set session key\n");
        close(cl_sock);
        return;
    }

    
    memset(session_key, 0, sizeof(session_key));

//Receive Server username;
    char contact_name[64]={0};
    recv(cl_sock, contact_name, sizeof(contact_name), 0);
    send(cl_sock, username, sizeof(username), 0);
    

    comms_loop(cl_sock, username, contact_name);
    close(cl_sock);
    return;
}
/* End run_client()*/
/***************************************************************/

/********************/
/* Begin comms_loop()*/
/*     */
/********************/
void comms_loop(int ne_socket, const char *username, const char *contact_name){
    fd_set readfds; 
    char buffer[BUFFER_SIZE];
    unsigned char ciphertxt[BUFFER_SIZE];
    unsigned char plaintext[BUFFER_SIZE];
    int max_fd;
    // Text file for log
    const char *msgLog = "chat_log.txt";

    if(ne_socket > STDIN_FILENO){
        max_fd = ne_socket;
    }else{
        max_fd = STDIN_FILENO;
    }

    printf(":: Connected to [%s] :: \n", contact_name);
    printf(":: Type '/quit' to exit ::\n");

    while(chat_running){
        FD_ZERO(&readfds);
        FD_SET(ne_socket, &readfds); 
        FD_SET(STDIN_FILENO, &readfds); 

        if (select(max_fd + 1, &readfds, NULL, NULL, NULL) < 0){
            perror("!! Select Error");
            break;
        }

        
        if(FD_ISSET(STDIN_FILENO, &readfds)){
            memset(buffer, 0, BUFFER_SIZE); 

            if(fgets(buffer, sizeof(buffer), stdin) == NULL){
                printf("!! Input Stream Error");
                break;
            }

            
            buffer[strcspn(buffer, "\n")] = '\0';

            
            if(strcasecmp(buffer, "/clear") == 0){
                printf("\033[2J\033[H"); // ANSI escape codes to clear the screen 
                fflush(stdout);
                continue;
            }
            log_msg(msgLog, username, buffer);
            
          
            int ciphertext_len = aes_encrypt((unsigned char*)buffer, strlen(buffer), ciphertxt);
            send(ne_socket, ciphertxt, ciphertext_len, 0);

            if(strcasecmp(buffer, "/quit") == 0){
                shutdown(ne_socket, SHUT_RDWR);
                break;
            }
        }

        if(FD_ISSET(ne_socket, &readfds)){
            int bytes_received = recv(ne_socket, buffer, BUFFER_SIZE, 0); 
       
            if(bytes_received <= 0){ 
                printf("!! Connection Ended.\n");
                break;
            }

              
            int plaintext_len = aes_decrypt((unsigned char*)buffer, bytes_received, plaintext);
            plaintext[plaintext_len] = '\0'; 
            
            log_msg(msgLog, contact_name, (char *)plaintext);

            
            if(strcasecmp((char*)plaintext, "/quit") == 0){
                    printf(":: Chat ended by [%s].\n", contact_name);
                    break;
            }
            printf("[%s] >> %s\n", contact_name, plaintext);
        }
    }
    printf(":: Chat Session Ended ::\n");
}
/* End comms_loop() */
/***************************************************************/

/********************/
/* Begin perform_dh_exchange_server() */
/********************/
int perform_dh_exchange_server(int client_socket, unsigned char *session_key) {
    dh_keys_t keys = {0};
    unsigned char received_pubkey[384];  
    int ret = -1;

 
    if (generate_dh_keys(&keys) != 0) {
        printf("Server: Failed to generate DH keys\n");
        return -1;
    }

   
    if (send(client_socket, keys.public_key, sizeof(keys.public_key), 0) != sizeof(keys.public_key)) {
        printf("Server: Failed to send public key\n");
        goto cleanup;
    }

   
    ssize_t received = recv(client_socket, received_pubkey, sizeof(received_pubkey), 0);
    if (received != sizeof(received_pubkey)) {
        printf("Server: Failed to receive client's public key\n");
        goto cleanup;
    }

    // Compute shared secret
    if (compute_shared_secret(&keys, received_pubkey, sizeof(received_pubkey)) != 0) {
        printf("Server: Failed to compute shared secret\n");
        goto cleanup;
    }

    
    memcpy(session_key, keys.shared_secret, 32);
    ret = 0;

cleanup:
    
    secure_zero(&keys, sizeof(keys));
    return ret;
}
/* End perform_dh_exchange_server() */
/***************************************************************/

/********************/
/* Begin perform_dh_exchange_client() */
/********************/
int perform_dh_exchange_client(int server_socket, unsigned char *session_key) {
    dh_keys_t keys = {0};
    unsigned char received_pubkey[384];  
    int ret = -1;

    
    if (generate_dh_keys(&keys) != 0) {
        printf("Client: Failed to generate DH keys\n");
        return -1;
    }

 
    ssize_t received = recv(server_socket, received_pubkey, sizeof(received_pubkey), 0);
    if (received != sizeof(received_pubkey)) {
        printf("Client: Failed to receive server's public key\n");
        goto cleanup;
    }

    
    if (send(server_socket, keys.public_key, sizeof(keys.public_key), 0) != sizeof(keys.public_key)) {
        printf("Client: Failed to send public key\n");
        goto cleanup;
    }

    
    if (compute_shared_secret(&keys, received_pubkey, sizeof(received_pubkey)) != 0) {
        printf("Client: Failed to compute shared secret\n");
        goto cleanup;
    }

    
    memcpy(session_key, keys.shared_secret, 32);
    ret = 0;

cleanup:
    
    secure_zero(&keys, sizeof(keys));
    return ret;
}
/* End perform_dh_exchange_client() */
/***************************************************************/