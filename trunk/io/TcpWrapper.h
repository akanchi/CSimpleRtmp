#ifndef TRUNK_TCPWRAPPER_H
#define TRUNK_TCPWRAPPER_H

#include <string>

class SimpleBuffer;

namespace akanchi {
    class TcpWrapper {
    public:
        static int socket();

        static int connect(int fd, const std::string &ip, int port);

        static int read(int fd, SimpleBuffer *data);

        static int read(int fd, SimpleBuffer *data, int size);

        static int write(int fd, SimpleBuffer *data);

        static int close(int fd);
    };
}

#endif //TRUNK_TCPWRAPPER_H
