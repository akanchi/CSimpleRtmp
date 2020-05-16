#ifndef TRUNK_SETCHUNKSIZEPACKET_H
#define TRUNK_SETCHUNKSIZEPACKET_H


#include "RtmpPacket.h"

namespace akanchi {
    class SetChunkSizePacket : public RtmpPacket {
    public:
        SetChunkSizePacket();

        virtual ~SetChunkSizePacket();

    public:
        void read(SimpleBuffer *sb);

        void write(SimpleBuffer *sb);

    public:
        uint32_t chunk_size;
    };
}

#endif //TRUNK_SETCHUNKSIZEPACKET_H
