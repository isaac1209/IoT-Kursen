#ifndef MQTT_BROKER_H
#define MQTT_BROKER_H
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h> 
#include <thread>
#include <mutex>
#include <algorithm>
#include "Message_header.h"

class MQTTBroker {
private:
    int broker_fd;
    const int port = 1883;
    std::map<std::string, std::vector<int>> topic_subscribers;
    std::map<std::string, std::string> retained_messages;
    std::mutex mtx; 
public:
     MQTTBroker();
    ~MQTTBroker();
    void start();
    void handleClient(int client_fd);
    void parseMessage(const uint8_t* buffer, int length, int client_fd);
    void parseConnect(const uint8_t* buffer, int length, int client_fd);
    void parseSubscribe(const uint8_t* buffer, int length, int client_fd);
    void parsePublish(const uint8_t* buffer, int length, int client_fd);
    void parseUnsubscribe(const uint8_t* buffer, int length, int client_fd);
    void connectClient(int client_fd);
    void mqttPing(int client_fd);
    void disconnectClient(int client_fd);
    void publishMessage(const std::string& topic, const std::string& payload);
    void sendRetainedMessages(int client_fd, const std::string& topic);

};

#endif // MQTT_BROKER_H