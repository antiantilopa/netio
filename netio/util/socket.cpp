#include <iostream>
#include "socket.h"
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <string>

namespace netio {

socket::socket() {
    WSADATA wsaData;
    WORD DLL_VERSION = MAKEWORD(2, 2);
    if (WSAStartup(DLL_VERSION, &wsaData) != 0){
        std::cerr << "Failed to initialize Winsock" << std::endl;
        exit(1);
    }
    sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        exit(1);
    }
}

socket::socket(SOCKET s) {
    sock = s;
}

socket::~socket() {
        WSACleanup();
    }

void socket::bind(const char* addr, int port) {
    SOCKADDR_IN serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(addr);

    if (::bind(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind socket" << std::endl;
        exit(1);
    }
}
void socket::bind(std::string addr, int port) {
    SOCKADDR_IN serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(addr.c_str());

    if (::bind(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind socket" << std::endl;
        exit(1);
    }
}

void socket::listen(int maxconn) {
    if (::listen(sock, maxconn) == SOCKET_ERROR) {
        std::cerr << "Failed to listen on socket" << std::endl;
        exit(1);
    }
}

void socket::connect(const char* addr, int port) {
    SOCKADDR_IN serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(addr);

    if (::connect(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to server" << std::endl;
        exit(1);
    }
}

socket socket::accept() {
    SOCKET clientSock = ::accept(sock, NULL, NULL);
    if (clientSock == INVALID_SOCKET) {
        std::cerr << "Failed to accept client connection" << std::endl;
        exit(1);
    }
    return socket(clientSock);
}

void socket::send(const char* data, int len) {
    if (::send(sock, data, len, 0) == SOCKET_ERROR) {
        std::cerr << "Failed to send data" << std::endl;
        exit(1);
    }
}

std::string socket::recv_string(int buffersize) {
    char* buffer = new char[buffersize];
    int bytesReceived = ::recv(sock, buffer, buffersize, 0);
    if (bytesReceived == SOCKET_ERROR) {
        int error = WSAGetLastError();
        std::cerr << "recv failed with error: " << error << std::endl;
        WSACleanup();
        exit(1);
    }
    
    std::string result = std::string((const char*)buffer, bytesReceived);
    delete[] buffer;
    return result;
}

int socket::recv(char* buffer, int buffersize) {
    int bytesReceived = ::recv(sock, buffer, buffersize, 0);
    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "Failed to receive data" << std::endl;
        exit(1);
    }
    return bytesReceived;
}

void socket::close() {
    closesocket(sock);
}
}// namespace netio