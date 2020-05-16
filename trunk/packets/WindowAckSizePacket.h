#ifndef TRUNK_WINDOWACKSIZEPACKET_H
#define TRUNK_WINDOWACKSIZEPACKET_H

#include "RtmpPacket.h"

namespace akanchi {
    class WindowAckSizePacket : public RtmpPacket {
    public:
        WindowAckSizePacket();

        virtual ~WindowAckSizePacket();

    public:
        void read(SimpleBuffer *sb);

        void write(SimpleBuffer *sb);

    public:
        uint32_t acknowledgement_window_size;
    };
}

#endif //TRUNK_WINDOWACKSIZEPACKET_H
