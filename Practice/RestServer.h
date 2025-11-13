#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <csignal>      

class RestServer
{
private:
   
    bool runServer = true;
    size_t server_fd = socket(AF_INET, SOCK_STREAM,0);
    int sensor = 42;
    int port = 80;
public:
    RestServer(/* args */);
    ~RestServer();
    void startServer();
    void set_ServerRun(bool run);
};

RestServer::RestServer(/* args */)
{
    if (server_fd == -1) {
        std::cerr << "Socket creation failed!" << std::endl;
        return;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 3);
    std::cout << "Server running on port " << port << "..." << std::endl;

}
void RestServer::startServer()
{
    while (runServer) {
        int new_socket = accept(server_fd, nullptr, nullptr);
        if (new_socket < 0) {
            perror("Accept failed");
            continue;
        }

        std::cout << "Client connected" << std::endl;

        char buffer[4096] = {0};
        int bytesRead = read(new_socket, buffer, sizeof(buffer));
        if (bytesRead <= 0) {
            close(new_socket);
            continue;
        }

        std::string request(buffer);
        std::cout << "Request:\n" << request << std::endl;

        // Parse first line (method, path, protocol)
        std::string method, path, protocol;
        size_t firstLineEnd = request.find("\r\n"); // Index of the end of the first line
        if (firstLineEnd != std::string::npos) {
            std::string firstLine = request.substr(0, firstLineEnd);
            std::istringstream iss(firstLine);
            iss >> method >> path >> protocol;
        }

        std::string responseHeader;
        std::string responseBody;

        if (method == "GET") {
            if (path == "/sensor") {
                responseBody = "<h1>Sensor Value</h1> sensor = " + std::to_string(sensor);
                responseHeader = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
            } else {
                responseBody = "<h1>File not found</h1>";
                responseHeader = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: ";
            }
        } else if (method == "POST") {
            if (path == "/sensor") {
                size_t bodyStart = request.find("\r\n\r\n");
                if (bodyStart != std::string::npos) {
                    std::string body = request.substr(bodyStart + 4);
                    try {
                        sensor = std::stoi(body);
                    } catch (...) {
                        sensor = -1;
                    }
                }
                responseBody = "<h1>Sensor Updated! </h1> sensor = " + std::to_string(sensor);
                responseHeader = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
            } else {
                responseBody = "<h1>File not found</h1>";
                responseHeader = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: ";
            }
        } else {
            responseBody = "<h1>Unsupported Method</h1>";
            responseHeader = "HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/html\r\nContent-Length: ";
        }

        responseHeader += std::to_string(responseBody.size()) + "\r\n\r\n";
        std::string fullResponse = responseHeader + responseBody;

        send(new_socket, fullResponse.c_str(), fullResponse.size(), 0);
        close(new_socket);
        std::cout << "Client disconnected\n" << std::endl;
    }

    close(server_fd);
}

void RestServer::set_ServerRun(bool run)
{
    runServer = run;
}

RestServer::~RestServer()
{
}

