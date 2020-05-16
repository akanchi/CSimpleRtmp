#ifndef TRUNK_TCPCLIENT_H
#define TRUNK_TCPCLIENT_H

#include <string>

class SimpleBuffer;

namespace akanchi {
    class TcpClient {
    public:
        TcpClient();

        virtual ~TcpClient();

    public:
        int socket();

        int connect(const std::string &ip, int port);

        int read(SimpleBuffer *data);

        int read(SimpleBuffer *data, int size);

        int write(SimpleBuffer *data);

        int close();

        int fd();

    private:
        int _fd;
    };
}

#endif //TRUNK_TCPCLIENT_H
