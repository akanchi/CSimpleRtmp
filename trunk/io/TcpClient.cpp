#include "TcpClient.h"

#ifdef _WIN32
#include <winsock2.h>
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <vector>

#include <iostream>

#include "../amf/amf0/simple_buffer.h"
#include "TcpWrapper.h"

namespace akanchi {
    TcpClient::TcpClient()
            : _fd(-1) {
//    socket();
    }

    TcpClient::~TcpClient() {

    }

    int TcpClient::socket() {
        if (_fd != -1) {
            return _fd;
        }

        _fd = TcpWrapper::socket();
        if (_fd < 0) {
            std::cerr << "socket error" << std::endl;
        }

        return _fd;
    }

    int TcpClient::connect(const std::string &ip, int port) {
        return TcpWrapper::connect(_fd, ip, port);
    }

    int TcpClient::read(SimpleBuffer *data) {
        return TcpWrapper::read(_fd, data);
    }

    int TcpClient::read(SimpleBuffer *data, int size) {
        return TcpWrapper::read(_fd, data, size);
    }

    int TcpClient::write(SimpleBuffer *data) {
        return TcpWrapper::write(_fd, data);
    }

    int TcpClient::close() {
        TcpWrapper::close(_fd);
        _fd = -1;

        return 0;
    }

    int TcpClient::fd() {
        return _fd;
    }
}
