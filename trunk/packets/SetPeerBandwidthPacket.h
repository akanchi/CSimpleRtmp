#ifndef TRUNK_SETPEERBANDWIDTHPACKET_H
#define TRUNK_SETPEERBANDWIDTHPACKET_H

#include "RtmpPacket.h"

namespace akanchi {
    class SetPeerBandwidthPacket : public RtmpPacket {
    public:
        SetPeerBandwidthPacket();

        virtual ~SetPeerBandwidthPacket();

    public:
        void read(SimpleBuffer *sb);

        void write(SimpleBuffer *sb);

    public:
        uint8_t limit_type;
        uint32_t bandwidth;
    };
}

#endif //TRUNK_SETPEERBANDWIDTHPACKET_H
