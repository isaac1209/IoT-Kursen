#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h> 
#include <netdb.h>

class MQTTClient {
private:
    int client_fd;
    const int port = 1883;
    const std::string server_addr = "broker.mqttdashboard.com"; // Public MQTT broker
public:
    MQTTClient();
    ~MQTTClient();
    bool connectToServer(); 
    void sendMessage(const std::string& topic, const std::string& payload);
    void receiveMessage();
};

MQTTClient::MQTTClient() {
    // 1. Create a TCP socket (SOCK_STREAM)
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        std::cerr << "Socket creation failed!" << std::endl;
    }
}
MQTTClient::~MQTTClient() {
    if (client_fd >= 0) {
        close(client_fd);
    }
}
bool MQTTClient::connectToServer() {
    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);  
    // Use getaddrinfo to resolve the server DNS name to an IPv4 address.
    addrinfo hints{};
    addrinfo* res = nullptr;
    hints.ai_family = AF_INET;        // IPv4
    hints.ai_socktype = SOCK_STREAM;   // TCP
    int ret = getaddrinfo(server_addr.c_str(), nullptr, &hints, &res);
    if (ret != 0 || res == nullptr) {
        std::cerr << "getaddrinfo failed: " << gai_strerror(ret) << std::endl;
        return false;
    }
    // Extract IPv4 address from the result
    sockaddr_in* addr_in = reinterpret_cast<struct sockaddr_in*>(res->ai_addr);
    server_address.sin_addr = addr_in->sin_addr;
    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &server_address.sin_addr, ipstr, sizeof(ipstr));
    std::cout << "Connecting to " << ipstr << " on port " << port << std::endl;
    freeaddrinfo(res); // Free the address info structure
    // 2. Connect to the server
    if (connect(client_fd, reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address)) < 0) {
        std::cerr << "Connection to server failed!" << std::endl;
        return false;
    }
    std::cout << "Connected to the MQTT server successfully." << std::endl;
    return true;
}



#endif //MQTT_CLIENT_H