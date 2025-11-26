//
// Created by noahj on 2025-11-15.
//


#ifndef LABB1_DT094A_COAPMESSAGE_H
#define LABB1_DT094A_COAPMESSAGE_H

#include <cstdint>
#include <vector>

enum class CoapMessageType : uint8_t {
    CON =       0b00000000,
    NON =       0b00010000,
    ACK =       0b00100000,
    RST =       0b00110000,
};

enum class CoapMessageMethod : uint8_t {
    EMPTY =     0b00000000,
    GET =       0b00000001,
    POST =      0b00000010,
    PUT =       0b00000011,
    DELETE =    0b00000100,
};

enum class CoapMessageResponseCode : uint8_t { // More can be found in RFC
    OK =        0b01000000,
    CONTENT =   0b01000101,
    NOT_FOUND = 0b10000100,
};

//TODO: ADD options, TOKENS and MESSAGEID

class CoapMessage {
private:
    uint8_t default_version = 0b01000000; // => version 1
    uint8_t no_token = 0b00000000; // => 0 length

    std::vector<char> message;
public:
    // Constructor will default to using version 1 and Token length 0
    explicit CoapMessage(CoapMessageType type, CoapMessageMethod method); // WILL NEED ADDING
    CoapMessage(const char* bytes, const size_t length);
    ~CoapMessage() = default;

    void printMessage();
    void printMessagePayload();
    char* getMessage();
    size_t getMessageSize();
};


#endif //LABB1_DT094A_COAPMESSAGE_H