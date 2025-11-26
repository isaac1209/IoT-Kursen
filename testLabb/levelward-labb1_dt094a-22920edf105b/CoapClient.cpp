//
// Created by noahj on 2025-11-14.
//

#include "CoapClient.h"
#include <vector>
#include <cstdint>
#include <iostream>
#include <winsock2.h>
#include <windows.h>

#include <ws2tcpip.h>

#include <stdio.h>

void CoapClient::startConnTest() {
    printf("Starting connection test to coap.me on port 5683 with empty GET...\n");

    WSADATA wsaData;
    int iResult;

    SOCKET UdpSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL;
    struct addrinfo hints;

    char recvbuf[2048];
    int recvbuflen = 2048;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return;
    }

    // Resolve coap.me address
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;           // IPv4
    hints.ai_socktype = SOCK_DGRAM;      // UDP
    hints.ai_protocol = IPPROTO_UDP;

    iResult = getaddrinfo("coap.me", "5683", &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return;
    }

    // Create UDP socket
    UdpSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (UdpSocket == INVALID_SOCKET) {
        printf("socket failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return;
    }

    // Prepare CoAP message using proper byte representation
    uint8_t message[4] = {0x40, 0x01, 0x00, 0x00};

    // Send message to coap.me
    int sent = sendto(
        UdpSocket,
        reinterpret_cast<const char *>(&message),
        sizeof(message),
        0,
        result->ai_addr,
        (int)result->ai_addrlen
    );

    if (sent == SOCKET_ERROR) {
        printf("sendto failed: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(UdpSocket);
        WSACleanup();
        return;
    }

    printf("Sent %d bytes to coap.me\n", sent);

    // Receive response
    struct sockaddr_storage senderAddr;
    int senderAddrSize = sizeof(senderAddr);

    iResult = recvfrom(
        UdpSocket,
        recvbuf,
        this->bufferByteSize,
        0,
        reinterpret_cast<sockaddr *>(&senderAddr),
        &senderAddrSize
    );

    if (iResult == SOCKET_ERROR) {
        printf("recvfrom failed: %d\n", WSAGetLastError());
    } else if (iResult > 0) {
        printf("Received %d bytes\n", iResult);

        // Print bytes in hex
        printf("Response (hex): ");
        for (int i = 0; i < iResult; i++) {
            printf("%02X ", static_cast<unsigned char>(recvbuf[i]));
        }
        printf("\n");
    }

    // Cleanup
    printf("Closing socket and cleaning up...\n");
    freeaddrinfo(result);
    closesocket(UdpSocket);
    WSACleanup();

}

bool CoapClient::sendMessage(CoapMessage msg) {
    WSADATA wsaData;
    int iResult;

    SOCKET UdpSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL;
    struct addrinfo hints;

    char recvbuf[2048];

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return false;
    }

    // Resolve coap.me address
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;           // IPv4
    hints.ai_socktype = SOCK_DGRAM;      // UDP
    hints.ai_protocol = IPPROTO_UDP;

    iResult = getaddrinfo("coap.me", "5683", &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return false;
    }

    // Create UDP socket
    UdpSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (UdpSocket == INVALID_SOCKET) {
        printf("socket failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return false;
    }

    // Send message to coap.me
    int sent = sendto(
        UdpSocket,
        msg.getMessage(),
        msg.getMessageSize(),
        0,
        result->ai_addr,
        static_cast<int>(result->ai_addrlen)
    );

    if (sent == SOCKET_ERROR) {
        printf("sendto failed: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(UdpSocket);
        WSACleanup();
        return false;
    }

    printf("Sent %d bytes to coap.me\n", sent);

    // Receive response
    struct sockaddr_storage senderAddr;
    int senderAddrSize = sizeof(senderAddr);

    iResult = recvfrom(
        UdpSocket,
        recvbuf,
        this->bufferByteSize,
        0,
        reinterpret_cast<sockaddr *>(&senderAddr),
        &senderAddrSize
    );

    if (iResult == SOCKET_ERROR) {
        printf("recvfrom failed: %d\n", WSAGetLastError());
    } else if (iResult > 0) {
        // Create a message object to easier work with response
        CoapMessage response(recvbuf, iResult);
        response.printMessagePayload();
    }

    // Cleanup
    printf("Closing socket and cleaning up...\n");
    freeaddrinfo(result);
    closesocket(UdpSocket);
    WSACleanup();

    return true;
}
