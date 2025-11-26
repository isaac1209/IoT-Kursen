#ifndef MESSAGE_HEADER_H
#define MESSAGE_HEADER_H
#include <cstdint>

enum class MessageType : uint8_t {
    CONNECT     = 0x10,
    CONNACK     = 0x20,
    PUBLISH     = 0x30,
    PUBACK      = 0x40,
    PUBREC      = 0x50,
    PUBREL      = 0x60,
    PUBCOMP     = 0x70,
    SUBSCRIBE   = 0x80,
    SUBACK      = 0x90,
    UNSUBSCRIBE = 0xA0,
    UNSUBACK    = 0xB0,
    PINGREQ     = 0xC0,
    PINGRESP    = 0xD0,
    DISCONNECT  = 0xE0
};

enum DUP_FLAG : uint8_t {
    DUP_0 = 0x00,
    DUP_1 = 0x08
};

enum class QoS : uint8_t {
    AT_MOST_ONCE  = 0x00,
    AT_LEAST_ONCE = 0x01,
    EXACTLY_ONCE  = 0x02
};

enum RETAIN_FLAG : uint8_t {
    RETAIN_0 = 0x00,
    RETAIN_1 = 0x01
};

enum class REMAINING_LENGTH : uint8_t {
    LENGTH_0   = 0x00,
    LENGTH_127 = 0x7F
};

enum class HEADER_LENGTH : uint8_t {
    LENGTH_2 = 2,
    LENGTH_4 = 4
};

enum class PAYLOAD_LENGTH : uint8_t {
    LENGTH_0   = 0x00,
    LENGTH_256 = 0xFF
};

#endif //MESSAGE_HEADER_H