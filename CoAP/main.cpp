#include <iostream>
#include "Coap_client.h"
#include "CoapMessage.h"
std::string toUpper(std::string s);
Method parseMethod(std::string method);

int main() {
    CoapClient client;
    if (!client.connectToServer()) {
        std::cerr << "Failed to connect to CoAP server." << std::endl;
        return -1;
    }
    std::vector<uint8_t> payload = {};
    std::vector<std::string> uriPath = {};
    std::string input_path;
    std::string input_payload;
    std::string method;

    while (true)
    {
    
    std::cout << "Enter Method (GET, POST, PUT, DELETE): ";
    std::getline(std::cin, method);
    Method coapMethod = parseMethod(method);

    if (method == "clear") {
        system("clear");   // or "cls" on Windows
        continue;          // restart loop immediately
    }
    
    if(method == "post" || method == "put")
    {

        std::cout << "Enter Payload (e.g., 'Hello CoAP'): ";
        std::getline(std::cin, input_payload);

        for (char c : input_payload) {
            payload.push_back(static_cast<uint8_t>(c));
        }
       
        if (input_payload == "clear") {
            system("clear");   // or "cls" on Windows
            continue;          // restart loop immediately
        }

    }

    // Get user input for URI path and payload
    std::cout << "\nEnter URI path (e.g., 'test'): ";
    std::getline(std::cin, input_path);
    uriPath.push_back(input_path);
    if (input_path == "clear") {
        system("clear");   // or "cls" on Windows
        continue;          // restart loop immediately
    }


   

    CoapMessage message(MessageType::NON, coapMethod, uriPath, payload);
    client.sendMessage(&message);
    client.receiveMessage();
    uriPath.clear();
    payload.clear();
    input_path.clear();
    input_payload.clear();
    }
    
  
    return 0;
}

// Helper functions to parse method to uppercase and convert to Method enum
std::string toUpper(std::string s)
{
    for (auto &c : s)
        c = std::toupper(c);
    return s;
}

Method parseMethod(std::string method)
{
    method = toUpper(method);

    if (method == "GET") return Method::GET;
    if (method == "POST") return Method::POST;
    if (method == "PUT") return Method::PUT;
    if (method == "DELETE") return Method::DELETE;

    return Method::EMPTY;
}