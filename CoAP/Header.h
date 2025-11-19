#include <vector>
#include <cstdint>

    enum class Version : uint8_t {
        V1 = 0b01000000
    };

    enum class MessageType : uint8_t {
        CON = 0b00000000, 
        NON = 0b00010000, 
        ACK = 0b00100000, 
        RST = 0b00110000 
    };

    enum class TokenLength : uint8_t {
        ZERO = 0b0000
    };

    enum class Method : uint8_t {
        EMPTY = 0b00000000,
        GET = 0b00000001,
        POST = 0b00000010,
        PUT = 0b00000011,
        DELETE = 0b00000100
    };

    enum class MessageID : uint16_t {
        ID = 0b1010101010101010
    };

    enum class Token : uint8_t {
        TOKEN = 0b00000000
        // Placeholder for Token representation
    };

    enum class OptionNumber : uint8_t {
    // Option Numbers (The integer value itself is the number)
    
    
    // 7 (00000111)
    URI_PORT = 0b00000111,          
    
    // 8 (00001000)
    LOCATION_PATH = 0b00001000,     
    
    // 11 (00001011)
    URI_PATH = 0b00001011,        
    
    // 12 (00001100)
    CONTENT_FORMAT = 0b00001100,   

    
    // 15 (00001111)
    URI_QUERY = 0b00001111,         
    
    // 17 (00010001)
    ACCEPT = 0b00010001,           
    
    // 20 (00010100)
    LOCATION_QUERY = 0b00010100,  
    
  
    };
   
    enum class PayloadMarker: uint8_t {
        END = 0b11111111
        // Placeholder for End of Options representation
    };


    enum class ResponseCode : uint8_t {

    OK = 0b01000000,        // 2.00 (Reserved) - Not used in responses
    CREATED = 0b01000001,   // 2.01 - POST/PUT success (for creation)
    DELETED = 0b01000010,   // 2.02 - DELETE success
    VALID = 0b01000011,     // 2.03 - GET success (used with ETag)
    CHANGED = 0b01000100,   // 2.04 - PUT/POST success (for updates)
    CONTENT = 0b01000101,   // 2.05 - GET success (returning content)

    // 4.xx Client Error
    BAD_REQUEST = 0b10000000, // 4.00
    UNAUTHORIZED = 0b10000001, // 4.01
    BAD_OPTION = 0b10000010,    // 4.02
    FORBIDDEN = 0b10000011,   // 4.03
    NOT_FOUND = 0b10000100,   // 4.04
    METHOD_NOT_ALLOWED = 0b10000101, // 4.05

    // 5.xx Server Error
    INTERNAL_SERVER_ERROR = 0b10100000, // 5.00
    NOT_IMPLEMENTED = 0b10100001
    };


