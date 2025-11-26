#ifndef COAP_COAPMESSAGE_H
#define COAP_COAPMESSAGE_H

#include "Header.h"
#include <iostream>
#include <bitset>


class CoapMessage
{
private:
    std::vector<uint8_t> messages;
public:
    CoapMessage();
    void clearMessage();
    CoapMessage( MessageType TPY, Method method, const std::vector<std::string>& uriPath, const std::vector<uint8_t>& payload = {});
    int findPayloadMarker(const std::vector<uint8_t>& packet);
    std::string bytesToString(const std::vector<uint8_t>& bytes);
    void addOption(OptionNumber number, const std::vector<uint8_t>& value);
    void printMessage();
    std::vector<uint8_t> getMessage();
    bool deserialize(const uint8_t* data, size_t length);
    ~CoapMessage();

};
CoapMessage::CoapMessage(){}

CoapMessage::CoapMessage( MessageType TPY, Method method, const std::vector<std::string>& uriPath, const std::vector<uint8_t>& payload)
{


    // ---- Construct Header ----
    uint8_t header = static_cast<uint8_t>(Version::V1) | static_cast<uint8_t>(TPY) | static_cast<uint8_t>(TokenLength::ZERO);
    this->messages.push_back(static_cast<char>(header));
    this->messages.push_back(static_cast<uint8_t>(method));
    this->messages.push_back(static_cast<uint16_t>(MessageID::ID) >> 8 & 0xFF);
    this->messages.push_back(static_cast<uint16_t>(MessageID::ID) & 0xFF);

    // ---- Add URI_PATH options ----
    for (const auto& pathSegment : uriPath) {
        std::vector<uint8_t> value(pathSegment.begin(), pathSegment.end());
        addOption(OptionNumber::URI_PATH, value);
    }

     // ---- Only add payload marker if payload exists ----
    if (!payload.empty()) {
        messages.push_back(static_cast<uint8_t>(PayloadMarker::END));   // Payload marker
        messages.insert(messages.end(), payload.begin(), payload.end());
    }
}
void CoapMessage::printMessage()
{
    for (const auto& byte : this->messages) {
        std::cout << std::bitset<8>(byte) << " ";
    }
    std::cout << std::endl;
}
std::vector<uint8_t> CoapMessage::getMessage()
{
    return this->messages;
}

bool CoapMessage::deserialize(const uint8_t* data, size_t length) {
    if (length < 4) {
        std::cerr << "Error: Data too short to be a valid CoAP message." << std::endl;
        return false;
    }

    // Clear existing message data and load new data
    messages.clear();
    for (size_t i = 0; i < length; ++i) {
        messages.push_back(data[i]);
    }
    // Find payload marker
    int payloadIndex = findPayloadMarker(messages);
    if (payloadIndex == -1) {
        std::cout << "No Payload found: " << payloadIndex << std::endl;
        // If no payload marker, treat entire rest as header and set payloadIndex to end-1 so slicing below is safe
        payloadIndex = static_cast<int>(messages.size());
    }
    
    // split options and payload if needed
    std::vector<uint8_t> header(messages.begin(), messages.begin() + payloadIndex);
    std::vector<uint8_t> payload;
    if (payloadIndex < static_cast<int>(messages.size())) {
        // create payload slice if payload marker is present
        payload.assign(messages.begin() + payloadIndex + 1, messages.end());
    }

    // Print payload in binary
    std::cout << "Payload:\n";
    std::cout << bytesToString(payload) << std::endl;

    payload.clear(); // Clear payload after processing
    messages.clear(); // Clear messages after processing
    return true;
}

int CoapMessage::findPayloadMarker(const std::vector<uint8_t>& packet)
{
    for (size_t i = 0; i < packet.size(); i++)
    {
        if (packet[i] == 0xFF)   // payload marker
            return i;
    }
    return -1; // no payload
}


void CoapMessage::addOption(OptionNumber number, const std::vector<uint8_t>& value)
{
    // reset last option number for simplicity in this example
    uint8_t lastOptionNumber = 0;
   
    // Calculate delta and length
    uint8_t optNum = static_cast<uint8_t>(number);
    uint8_t delta  = optNum - lastOptionNumber;
    lastOptionNumber = optNum;

    uint8_t len = value.size();

    // minimal encoding: delta < 15 and len < 15
    uint8_t optionHeader = (delta << 4) | (len & 0x0F);

    messages.push_back(optionHeader);

    for (uint8_t byte : value)
        messages.push_back(byte);
        
}

std::string CoapMessage::bytesToString(const std::vector<uint8_t>& bytes)
{
    return std::string(bytes.begin(), bytes.end());
}

void CoapMessage::clearMessage()
{
    messages.clear();
}

CoapMessage::~CoapMessage()
{
}

#endif //COAP_COAPMESSAGE_H