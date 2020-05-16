#ifndef TRUNK_ABORTPACKET_H
#define TRUNK_ABORTPACKET_H

#include "RtmpPacket.h"

namespace akanchi {
    class AbortPacket : public RtmpPacket {
    public:
        AbortPacket();

        virtual ~AbortPacket();

    public:
        void read(SimpleBuffer *sb);

        void write(SimpleBuffer *sb);

    public:
        uint32_t chunk_stream_id;
    };
}

#endif //TRUNK_ABORTPACKET_H
