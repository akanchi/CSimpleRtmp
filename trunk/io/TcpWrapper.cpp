#include "TcpWrapper.h"
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

#include "../amf/amf0/simple_buffer.h"

namespace akanchi {
    static const int MAX_BUFFER_SIZE = 4096;

    int TcpWrapper::socket() {
        int fd = ::socket(AF_INET, SOCK_STREAM, 0);
        return fd;
    }

    int TcpWrapper::connect(int fd, const std::string &ip, int port) {
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip.c_str());

        if (::connect(fd, (const struct sockaddr *) &addr, sizeof(sockaddr_in)) < 0) {
            return -1;
        }

        return 0;
    }

    int TcpWrapper::read(int fd, SimpleBuffer *data) {
        std::vector<char> read_buffer(MAX_BUFFER_SIZE, 0);
        int nb_read = ::recv(fd, (char *) (&read_buffer[0]), MAX_BUFFER_SIZE, 0);
        if (nb_read <= 0) {
            return -1;
        }

        data->append(&read_buffer[0], nb_read);

        return nb_read;
    }

    int TcpWrapper::read(int fd, SimpleBuffer *data, int size) {
        int ret = 0;
        std::vector<char> read_buffer(size, 0);

        int left = size;
        while (left > 0) {
            int nb_read = ::recv(fd, (char *) (&read_buffer[0]) + size - left, size, 0);
            if (nb_read <= 0) {
                ret = -1;
                break;
            }

            left -= nb_read;
        }


        data->append(&read_buffer[0], size);

        return ret;
    }

    int TcpWrapper::write(int fd, SimpleBuffer *data) {
        int nb_write = ::send(fd, data->data(), data->size(), 0);

        return nb_write;
    }

    int TcpWrapper::close(int fd) {
        if (fd > 0) {
#ifdef _WIN32
            ::closesocket(fd);
#else
            ::close(fd);
#endif
        }

        return 0;
    }
}