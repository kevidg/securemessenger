#include "secmsg.h"

// Modular exponentiation function (a^b mod p)
unsigned long power_mod(unsigned long base, unsigned long exp, unsigned long mod) {
    unsigned long result = 1;
    base = base % mod;
    
    while (exp > 0) {
        if (exp & 1)
            result = (result * base) % mod;
        base = (base * base) % mod;
        exp = exp >> 1;
    }
    return result;
}

// Server-side DH exchange
unsigned long perform_dh_server(int client_socket) {
    unsigned long private_key, public_key, client_public, shared_secret;
    ssize_t bytes_received;

    // Generate private key (random number)
    private_key = rand() % (DH_P - 1) + 1;
    
    // Calculate public key: g^private_key mod p
    public_key = power_mod(DH_G, private_key, DH_P);
    
    // Send public key to client
    send(client_socket, &public_key, sizeof(public_key), 0);
    
    // Receive client's public key
    bytes_received = recv(client_socket, &client_public, sizeof(client_public), 0);
    if (bytes_received <= 0) {
        perror("Failed to receive client public key");
        return 0;
    }
    
    // Calculate shared secret: (client_public)^private_key mod p
    shared_secret = power_mod(client_public, private_key, DH_P);
    printf("DH Exchange Complete (Server)\n");
    
    return shared_secret;
}

// Client-side DH exchange
unsigned long perform_dh_client(int server_socket) {
    unsigned long private_key, public_key, server_public, shared_secret;
    ssize_t bytes_received;

    // Generate private key (random number)
    private_key = rand() % (DH_P - 1) + 1;
    
    // Calculate public key: g^private_key mod p
    public_key = power_mod(DH_G, private_key, DH_P);
    
    // Receive server's public key
    bytes_received = recv(server_socket, &server_public, sizeof(server_public), 0);
    if (bytes_received <= 0) {
        perror("Failed to receive server public key");
        return 0;
    }
    
    // Send public key to server
    send(server_socket, &public_key, sizeof(public_key), 0);
    
    // Calculate shared secret: (server_public)^private_key mod p
    shared_secret = power_mod(server_public, private_key, DH_P);
    printf("DH Exchange Complete (Client)\n");
    
    return shared_secret;
}