#ifndef TRUNK_HANDSHAKE_H
#define TRUNK_HANDSHAKE_H

#include "amf/amf0/simple_buffer.h"

namespace akanchi {
    class TcpClient;
    class Handshake {
    public:
        Handshake(TcpClient *tcp);

        virtual ~Handshake();

    public:
        int write_c0();

        int write_c1();

        int write_c2();

        int read_s0();

        int read_s1();

        int read_s2();

    private:
        TcpClient *_tcp;
        SimpleBuffer _s1;
    };
}


#endif //TRUNK_HANDSHAKE_H
