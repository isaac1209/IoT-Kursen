//
// Created by noahj on 2025-11-15.
//

#include "CoapMessage.h"

#include <algorithm>
#include <bitset>
#include <cstring>
#include <iostream>

CoapMessage::CoapMessage(CoapMessageType type, CoapMessageMethod method) {
    // 0-1 = version, 2-3 = type, 4-7 = token length
    uint8_t tmpUint = this->default_version | static_cast<uint8_t>(type) | this->no_token;
    char tmpChar = static_cast<char>(tmpUint);
    this->message.push_back(tmpChar);
    this->message.push_back(static_cast<char>(static_cast<uint8_t>(method)));

    // Push back a "random" msg id
    this->message.push_back(static_cast<uint8_t>(0b10101010));
    this->message.push_back(static_cast<uint8_t>(0b10101010));


    // For testing purposes
    std::cout << "Created message: ";
    printMessage();
}

CoapMessage::CoapMessage(const char *bytes, const size_t length) {
    for (size_t i = 0; i < length; i++) {
        this->message.push_back(bytes[i]);
    }
}

void CoapMessage::printMessage() {
    std::ranges::for_each(this->message,
                      [](const char& byte) {
                          std::cout << std::bitset<8>(byte) << + " ";
                      });
    std::cout << std::endl;
}

void CoapMessage::printMessagePayload() {
    // Finds delimiter for payload
    auto payloadStart = std::ranges::find(this->message, static_cast<char>(0b11111111));
    if (payloadStart != this->message.end()) {
        std::cout << "Payload: ";
        std::ranges::for_each(payloadStart, this->message.end(),
            [](const char& byte) {
                std::cout << byte;
            });
        std::cout << std::endl;
    }
}

char* CoapMessage::getMessage() {
    return message.data();
}

size_t CoapMessage::getMessageSize() {
    return this->message.size();
}




