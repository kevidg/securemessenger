/**************************************/
/* The net.c file provides the server and client socket code and 
    the communications loop for exchanging text between the users. 
    The networking code follows the examples from websites such as
    https://www.geeksforgeeks.org/socket-programming-cc/
    Attr: Jay Patel, Michael Guerette (MDG)*/

#include "net.h"
#include "crypto.h"
#include "secmsg.h"
#include "dh.h"  // Add this include



char *IP_ADDRESS = "127.0.0.1";
bool chat_running = true; // Global variable to control the chat loop


/************************/
/* Begin run_server() 
    This function starts a server listening on port 31337 using the loopback address
    After receiving a connection the DH key exchange is performed 
    then the usernames are exchanged in clear text for demonstration purposes
    The username is the one provided from main() and get_default_user()
    The contact name is provided from the client and exchanged over the TCP connection
    It then calls the communication loop function and passes the socket info, username, and contact name
    Attr: (MDG)*/
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
    /*The socklen_t type is used in socket programming to represent the size of a socket address structure. 
    It is typically used in functions that deal with socket addresses, such as accept(), getsockname(), and
    getpeername().*/
    client_socket = accept(server_socket, (struct sockaddr *)&client_address ,&clientLen);
    if(client_socket < 0){
        perror("!! Client Accept Failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    } 

    // Perform DH key exchange before username exchange
    printf("Server: Starting DH key exchange...\n");
    unsigned char session_key[32];  // Increased size for stronger key
    if (perform_dh_exchange_server(client_socket, session_key) != 0) {
        printf("Server: DH exchange failed\n");
        close(client_socket);
        close(server_socket);
        return;
    }
    printf("Server: DH exchange completed successfully\n");

    // Set up encryption with new session key
    if (set_crypto_key(session_key, 32) != 0) {
        printf("Server: Failed to set session key\n");
        close(client_socket);
        close(server_socket);
        return;
    }

    // Clear session key from memory
    memset(session_key, 0, sizeof(session_key));

    //Exchange names in clear before calling comms_loop
    //For demonstration purposes
    send(client_socket, username, strlen(username), 0);
    //Receive contact username;
    char contact_name[64]={0};
    recv(client_socket, contact_name, sizeof(contact_name), 0);
    //debug
    //printf("[DEBUG]: CONTACT NAME: %s\n", contact_name);
    //printf("[DEBUG]: USER NAME: %s\n", username);
    
    comms_loop(client_socket, username, contact_name);

    close(server_socket);
     
    return;

}
/* End run_server() */
/*****************************************************************/

/********************/
/* Begin run_client()
    Similar to the run_server() function, but the client-side of the TCP connection
    It also exchanges DH key info and then the usernames
    Then calls the communication loop function and passes the socket info, username, and contact name
    Attr: (MDG) */
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
    // The following function takes a string(IP_ADDRESS) and converts to an IPv4 binary and stores it 
    // in the sockaddr_in struct (server_address)
    inet_pton(AF_INET, IP_ADDRESS, &server_address.sin_addr);
    // Try to connect to the server
    if(connect(cl_sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0){
        perror("!! Connection failure");
        close(cl_sock);
        exit(EXIT_FAILURE);
    }

    // Perform DH key exchange before username exchange
    printf("Client: Starting DH key exchange...\n");
    unsigned char session_key[32];  // Increased size for stronger key
    if (perform_dh_exchange_client(cl_sock, session_key) != 0) {
        printf("Client: DH exchange failed\n");
        close(cl_sock);
        return;
    }
    printf("Client: DH exchange completed successfully\n");

    // Set up encryption with new session key
    if (set_crypto_key(session_key, 32) != 0) {
        printf("Client: Failed to set session key\n");
        close(cl_sock);
        return;
    }

    // Clear session key from memory
    memset(session_key, 0, sizeof(session_key));

//Receive Server username;
    char contact_name[64]={0};
    recv(cl_sock, contact_name, sizeof(contact_name), 0);
    send(cl_sock, username, sizeof(username), 0);
    /*~~~~~~~~~*/
    //debug
    //printf("[DEBUG]: CONTACT NAME: %s\n", contact_name);
    //printf("[DEBUG]: USER NAME: %s\n", username);

    comms_loop(cl_sock, username, contact_name);
    close(cl_sock);
    return;
}
/* End run_client()*/
/***************************************************************/

/********************/
/* Begin comms_loop()*/
/* The purpose of the comms_loop is to keep the TCP connection alive in a loop
    allowing the users to exchange text back and forth. It calls functions from 
    crypto.c to encrypt and decrypt the messages. It uses the select() function so
    the app can switch between reading data from the user's cli input and the 
    contacts incoming network data. Examples from 
    https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/
    The encrption methods follow the implementatiopn examples from:
    https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption

    Attr: (MDG)
    */
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
        FD_SET(ne_socket, &readfds); // monitors network
        FD_SET(STDIN_FILENO, &readfds); // monitors keyboard, stdin

        if (select(max_fd + 1, &readfds, NULL, NULL, NULL) < 0){
            perror("!! Select Error");
            break;
        }

        //Check for user input
        if(FD_ISSET(STDIN_FILENO, &readfds)){
            memset(buffer, 0, BUFFER_SIZE); // Clears the buffer

            /* ~~~~~~ Mitigate Buffer Overflow */
            // Using fgets here prevents users from buffer overflow since fgets will only read the 
            // number of bytes that can fit in the array 'buffer'. Plus the buffer size is set in the 
            // #define directive in secmsg.h
            if(fgets(buffer, sizeof(buffer), stdin) == NULL){
                printf("!! Input Stream Error");
                break;
            }

            // strip the '\n' from the end, creates a NULL terminated buffer
            buffer[strcspn(buffer, "\n")] = '\0';

            // Check for a clear screen cmd
            if(strcasecmp(buffer, "/clear") == 0){
                printf("\033[2J\033[H"); // ANSI escape codes to clear the screen 
                fflush(stdout);
                continue;
            } 
            // Write to log
            log_msg(msgLog, username, buffer);
            
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

            //Write to log
            log_msg(msgLog, contact_name, (char *)plaintext);

            // Check if the quit() cmd was sent
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
/* Begin perform_dh_exchange_server() 
    Attr: Jay Patel*/
/********************/
int perform_dh_exchange_server(int client_socket, unsigned char *session_key) {
    dh_keys_t keys = {0};
    unsigned char received_pubkey[384];  // Increased buffer size to match dh_keys_t
    int ret = -1;

    // Generate our DH keypair
    if (generate_dh_keys(&keys) != 0) {
        printf("Server: Failed to generate DH keys\n");
        return -1;
    }

    // Send our public key to client
    if (send(client_socket, keys.public_key, sizeof(keys.public_key), 0) != sizeof(keys.public_key)) {
        printf("Server: Failed to send public key\n");
        goto cleanup;
    }

    // Receive client's public key
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

    // Use first 32 bytes of shared secret as session key
    memcpy(session_key, keys.shared_secret, 32);
    ret = 0;

cleanup:
    // Securely wipe sensitive data
    secure_zero(&keys, sizeof(keys));
    return ret;
}
/* End perform_dh_exchange_server() */
/***************************************************************/

/********************/
/* Begin perform_dh_exchange_client() 
    Attr: Jay Patel*/
/********************/
int perform_dh_exchange_client(int server_socket, unsigned char *session_key) {
    dh_keys_t keys = {0};
    unsigned char received_pubkey[384];  // Increased buffer size to match dh_keys_t
    int ret = -1;

    // Generate our DH keypair
    if (generate_dh_keys(&keys) != 0) {
        printf("Client: Failed to generate DH keys\n");
        return -1;
    }

    // Receive server's public key
    ssize_t received = recv(server_socket, received_pubkey, sizeof(received_pubkey), 0);
    if (received != sizeof(received_pubkey)) {
        printf("Client: Failed to receive server's public key\n");
        goto cleanup;
    }

    // Send our public key to server
    if (send(server_socket, keys.public_key, sizeof(keys.public_key), 0) != sizeof(keys.public_key)) {
        printf("Client: Failed to send public key\n");
        goto cleanup;
    }

    // Compute shared secret
    if (compute_shared_secret(&keys, received_pubkey, sizeof(received_pubkey)) != 0) {
        printf("Client: Failed to compute shared secret\n");
        goto cleanup;
    }

    // Use first 32 bytes of shared secret as session key
    memcpy(session_key, keys.shared_secret, 32);
    ret = 0;

cleanup:
    // Securely wipe sensitive data
    secure_zero(&keys, sizeof(keys));
    return ret;
}
/* End perform_dh_exchange_client() */
/***************************************************************/