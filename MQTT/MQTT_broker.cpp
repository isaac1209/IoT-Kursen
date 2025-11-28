#include "MQTT_broker.h"

MQTTBroker::MQTTBroker() {
    broker_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (broker_fd < 0) {
        std::cerr << "Socket creation failed!" << std::endl;
    }
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    bind(broker_fd, (struct sockaddr*)&address, sizeof(address));
    listen(broker_fd, 10);
    std::cout << "MQTT Broker running on port " << port << "..." << std::endl;
}
MQTTBroker::~MQTTBroker() {
    if (broker_fd >= 0) {
        close(broker_fd);
    }
}
void MQTTBroker::start() {
    while (true) {
        int client_fd = accept(broker_fd, nullptr, nullptr);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }
        // Handle each client in a separate thread
        std::thread(&MQTTBroker::handleClient, this, client_fd).detach();
    }
}
void MQTTBroker::handleClient(int client_fd) {
    uint8_t buffer[4096];

    while (true) {
        int bytesRead = read(client_fd, buffer, sizeof(buffer));
        if (bytesRead <= 0) {
            disconnectClient(client_fd);
            break;
        }

        parseMessage(buffer, bytesRead, client_fd);
    }
}

void MQTTBroker::parseMessage(const uint8_t* buffer, int length, int client_fd) {

    MessageType packetType = static_cast<MessageType>(buffer[0] & 0xF0);

    switch (packetType) {

        case MessageType::CONNECT:
            parseConnect(buffer, length, client_fd);
            break;

        case MessageType::PINGREQ:
            mqttPing(client_fd);
            break;

        case MessageType::SUBSCRIBE:
            parseSubscribe(buffer, length, client_fd);
            break;
        case MessageType::UNSUBSCRIBE:
            parseUnsubscribe(buffer, length, client_fd);
            break;

        case MessageType::PUBLISH:
            parsePublish(buffer, length, client_fd);
            break;

        case MessageType::DISCONNECT:
            disconnectClient(client_fd);
            break;

        default:
            std::cout << "Unknown MQTT packet type: 0x"
                      << std::hex << (int)(buffer[0] & 0xF0)
                      << std::dec << "\n";
            break;
    }
}

void MQTTBroker::parseConnect(const uint8_t* buffer, int length, int client_fd) 
{
    int index = 0;

    // ---- FIXED HEADER ----
    MessageType type = static_cast<MessageType>(buffer[index] & 0xF0);
    index++;

    // Remaining length
    uint8_t remainingLength = buffer[index++];
    

    // ---- VARIABLE HEADER ----
    uint16_t protocolNameLength = (buffer[index] << 8) | buffer[index + 1];
    index += 2;

    // Protocol Name
    std::string protocolName(reinterpret_cast<const char*>(&buffer[index]), protocolNameLength);
    index += protocolNameLength;

    if (protocolName != "MQTT") {
        std::cerr << "[ERROR] Unsupported protocol name: " << protocolName << "\n";
        disconnectClient(client_fd);
        return;
    }

    // Protocol Level
    uint8_t protocolLevel = buffer[index++];
    if (protocolLevel != 5 && protocolLevel != 4) {
        std::cout << "[ERROR] Unsupported MQTT version: " << (int)protocolLevel << std::endl;
        disconnectClient(client_fd);
        return;
    }

    // Connect Flags
    uint8_t connectFlags = buffer[index++];

    // Keep Alive
    uint16_t keepAlive = (buffer[index] << 8) | buffer[index + 1];
    index += 2;

    // Properties length
    uint8_t propertiesLength = buffer[index++];
    index += propertiesLength; // skip properties

    // ---- PAYLOAD ----
    uint16_t clientIdLength = (buffer[index] << 8) | buffer[index + 1];
    index += 2;

    std::string clientId(reinterpret_cast<const char*>(&buffer[index]), clientIdLength);
    index += clientIdLength;

    std::cout << "[CONNECT] Client ID: " << clientId 
              << " | KeepAlive: " << keepAlive << "\n";

    // ---- SEND ACK ----
    connectClient(client_fd);
}

void MQTTBroker::parseSubscribe(const uint8_t* buffer, int length, int client_fd) {
    int index = 0;

    // --- FIXED HEADER ---
    uint8_t packetType = buffer[index++];
    uint8_t remainingLength = buffer[index++];

    // --- VARIABLE HEADER ---
    uint16_t packetId = (buffer[index] << 8) | buffer[index + 1];
    index += 2;

    // Properties Length
    uint8_t propertiesLength = buffer[index++];
    index += propertiesLength;

    // --- PAYLOAD ---
    uint16_t topicLength = (buffer[index] << 8) | buffer[index + 1];
    index += 2;

    // Topic Name
    std::string topic(reinterpret_cast<const char*>(&buffer[index]), topicLength);
    index += topicLength;

    // QoS byte (we will ignore)
    uint8_t qos = buffer[index++];

    std::cout << "[SUBSCRIBE] Client " << client_fd 
              << " subscribed to topic: " << topic
              << " (PacketID=" << packetId << ")\n";

    // Prevents race conditions if multiple clients subscribe simultaneously
    {
        std::lock_guard<std::mutex> lock(mtx);
        topic_subscribers[topic].push_back(client_fd);
    }

    // Send SUBACK
    uint8_t suback[6] = {
        static_cast<uint8_t>(MessageType::SUBACK), // 0x90
        0x04, // remaining length
        (uint8_t)(packetId >> 8),   // MSB
        (uint8_t)(packetId),        // LSB
        0x00, // properties length
        0x00  // reason code = granted QoS 0
    };

    write(client_fd, suback, sizeof(suback));

    std::cout << "[SUBACK] Sent to client fd " << client_fd << "\n";
}

void MQTTBroker::parsePublish(const uint8_t* buffer, int length, int client_fd) {
    int index = 0;

    // --- FIXED HEADER ---
    uint8_t header = buffer[index++];
    uint8_t remainingLength = buffer[index++];

    // QoS level
    uint8_t qos = (header & 0x06) >> 1;

    // --- VARIABLE HEADER ---

    // Topic length (2 bytes)
    uint16_t topicLength = (buffer[index] << 8) | buffer[index + 1];
    index += 2;

    // Topic name
    std::string topic(reinterpret_cast<const char*>(&buffer[index]), topicLength);
    index += topicLength;

    // Properties Length
    uint8_t propsLen = buffer[index++];
    index += propsLen; // skip properties

    // --- PAYLOAD ---
    int payloadLen = length - index;
    // start at current index to the end of the buffer by payloadLen
    std::string payload(reinterpret_cast<const char*>(&buffer[index]), payloadLen);

    std::cout << "[PUBLISH] Topic: " << topic 
              << " | Payload: " << payload
              << " | From fd: " << client_fd << "\n";

    // Forward the message to all subscribers
    publishMessage(topic, payload);
}

void MQTTBroker::parseUnsubscribe(const uint8_t* buffer, int length, int client_fd) {
    int index = 0;

    // --- FIXED HEADER ---
    uint8_t header = buffer[index++];
    uint8_t remainingLength = buffer[index++];

    // --- VARIABLE HEADER ---
    uint16_t packetId = (buffer[index] << 8) | buffer[index + 1];
    index += 2;

    // MQTT v5 properties length
    uint8_t propsLen = buffer[index++];
    index += propsLen;

    // --- PAYLOAD (topic to unsubscribe) ---

    uint16_t topicLength = (buffer[index] << 8) | buffer[index + 1];
    index += 2;

    std::string topic(reinterpret_cast<const char*>(&buffer[index]), topicLength);
    index += topicLength;

    // Remove from map
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto& subs = topic_subscribers[topic];

        // Remove client_fd
        subs.erase(std::remove(subs.begin(), subs.end(), client_fd), subs.end());
    }

    std::cout << "[UNSUBSCRIBE] Client " << client_fd
              << " unsubscribed from: " << topic << "\n";

    // Send UNSUBACK
    uint8_t unsuback[5] = {
        static_cast<uint8_t>(MessageType::UNSUBACK), // 0xB0
        0x03,      // remaining length
        (uint8_t)(packetId >> 8),
        (uint8_t)(packetId),
        0x00       // reason code: success
    };

    write(client_fd, unsuback, sizeof(unsuback));

    std::cout << "[UNSUBACK] Sent to client fd " << client_fd << "\n";
}

void MQTTBroker::publishMessage(const std::string& topic, const std::string& payload) {
    std::lock_guard<std::mutex> lock(mtx);

    if (topic_subscribers.count(topic) == 0) {
        std::cout << "[BROKER] No subscribers for topic: " << topic << "\n";
        return;
    }

    for (int subscriber_fd : topic_subscribers[topic]) {

        std::vector<uint8_t> packet;

        // Fixed Header
        packet.push_back(static_cast<uint8_t>(MessageType::PUBLISH)); // 0x30

        // Remaining Length = topic length (2) + topic + properties len (1) + payload size
        uint8_t remainingLength =
            2 + topic.size() + 1 + payload.size();
        packet.push_back(remainingLength);

        // Topic length
        packet.push_back((topic.size() >> 8) & 0xFF);
        packet.push_back(topic.size() & 0xFF);

        // Topic name
        packet.insert(packet.end(), topic.begin(), topic.end());

        // Properties length (0)
        packet.push_back(0x00);

        // Payload
        packet.insert(packet.end(), payload.begin(), payload.end());

        // Send to subscriber
        write(subscriber_fd, packet.data(), packet.size());

        std::cout << "[BROKER] Forwarded PUBLISH to fd " << subscriber_fd << "\n";
    }
}



void MQTTBroker::connectClient(int client_fd) {
    uint8_t connack[5] = {
        static_cast<uint8_t>(MessageType::CONNACK), // 0x20
        0x03, // remaining length = 3 bytes follow
        0x00, // session present = 0
        0x00, // reason code = success
        0x00  // properties length = 0
    };

    write(client_fd, connack, sizeof(connack));
    std::cout << "[CONNACK] Sent to client fd " << client_fd << "\n";
}

void MQTTBroker::mqttPing(int client_fd) {

    // ---- SEND PINGRESP ----
    uint8_t pingresp[2] = {
        static_cast<uint8_t>(MessageType::PINGRESP),
        0x00 // remaining length
    };

    write(client_fd, pingresp, sizeof(pingresp));
    std::cout << "[PINGRESP] Sent to client fd " << client_fd << "\n";
}

void MQTTBroker::disconnectClient(int client_fd) {
    std::lock_guard<std::mutex> lock(mtx);

    // Remove client_fd from all topic subscriber lists
    for (auto &entry : topic_subscribers) {
        auto &subs = entry.second;
        subs.erase(
            std::remove(subs.begin(), subs.end(), client_fd),
            subs.end()
        );
    }

    std::cout << "[DISCONNECT] Client " << client_fd << " disconnected\n";
    close(client_fd);
}