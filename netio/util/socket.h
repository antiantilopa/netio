#ifndef SOCKET_H
#define SOCKET_H

#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <string>

namespace netio {

class socket {
private:
    SOCKET sock;
public:
    
    socket();
    socket(SOCKET s);
    ~socket();
    
    void bind(const char* addr, int port);
    void bind(std::string addr, int port);
    void listen(int maxconn);
    void connect(const char* addr, int port);
    socket accept();
    void send(const char* data, int len);
    std::string recv_string(int buffersize);
    int recv(char* buffer, int buffersize);
    void close();
};

} // namespace netio
#endif