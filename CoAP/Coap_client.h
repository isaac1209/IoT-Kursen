#ifndef COAP_ClIENT_H
#define COAP_ClIENT_H

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h> 
#include <netdb.h>
#include "CoapMessage.h" 

class CoapClient {
private:
    int client_fd;
    const int port = 5683;
    const std::string server_addr = "coap.me"; // DNS name for the server

public:
    CoapClient();
    ~CoapClient();
    bool connectToServer(); 
    void sendMessage(CoapMessage* message);
    void receiveMessage();
};


CoapClient::CoapClient() {
    // 1. Create a UDP socket (SOCK_DGRAM)
    client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_fd < 0) {
        std::cerr << "Socket creation failed!" << std::endl;
    }
}

CoapClient::~CoapClient() {
    if (client_fd >= 0) {
        close(client_fd);
    }
}
bool CoapClient::connectToServer() {
    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    // Use getaddrinfo properly to resolve the server DNS name to an IPv4 address.
    addrinfo hints{};
    addrinfo* res = nullptr;
    hints.ai_family = AF_INET;        // IPv4
    hints.ai_socktype = SOCK_DGRAM;   // UDP

    int ret = getaddrinfo(server_addr.c_str(), nullptr, &hints, &res);
   
    if (ret != 0 || res == nullptr) {
        std::cerr << "getaddrinfo failed: " << gai_strerror(ret) << std::endl;
        return false;
    }

    // Extract IPv4 address from the result
    sockaddr_in* addr_in = reinterpret_cast<struct sockaddr_in*>(res->ai_addr);
    server_address.sin_addr = addr_in->sin_addr;

    // Convert address to string for logging
    char ipstr[INET_ADDRSTRLEN] = {0};
    inet_ntop(AF_INET, &server_address.sin_addr, ipstr, sizeof(ipstr));

    freeaddrinfo(res);

    // 2. Connect the UDP socket to the remote server's address. 
    // This makes the client default to sending data to the resolved address.
    if (connect(client_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Connection failed!" << std::endl;
        return false;
    }

    std::cout << "CoAP Client connected to " << ipstr << ":" << port << std::endl;
    return true;
}


void CoapClient::sendMessage(CoapMessage* message) {
    if (client_fd < 0) {
        std::cerr << "Invalid socket!" << std::endl;
        return;
    }

    // Now we get the serialized message from CoapMessage
    std::vector<uint8_t> msgBuffer = message->getMessage();

    ssize_t bytesSent = send(client_fd, msgBuffer.data(), msgBuffer.size(), 0);
    message->clearMessage(); // Clear message after sending

    if (bytesSent < 0) {
        std::cerr << "Failed to send message!" << std::endl;
        return;
    }
}

void CoapClient::receiveMessage() {

    
    // CoAP messages are typically small (max payload is around 1024 bytes)
    const size_t MAX_COAP_SIZE = 1280; 
    uint8_t buffer[MAX_COAP_SIZE];
    
    memset(buffer, 0, MAX_COAP_SIZE);
    ssize_t bytes_received = recv(client_fd, buffer, MAX_COAP_SIZE, 0);

    if (bytes_received < 0) {
        std::cerr << "Error receiving response: " << strerror(errno) << std::endl;
        return;
    }
    
    std::cout << "Received CoAP response (" << bytes_received << " bytes)." << std::endl;

    // **NOTE:** You must implement the CoapMessage::deserialize() method.
    CoapMessage response;
    bool success = response.deserialize(buffer, bytes_received);
   
    if (!success) {
     std::cerr << "Error: Failed to deserialize received CoAP message." << std::endl;
    }

}
#endif //COAP_ClIENT_H