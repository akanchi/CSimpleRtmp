#ifndef TRUNK_ACKNOWLEDGEMENTPACKET_H
#define TRUNK_ACKNOWLEDGEMENTPACKET_H

#include "RtmpPacket.h"

namespace akanchi {
    class AcknowledgementPacket : public RtmpPacket {
    public:
        AcknowledgementPacket();

        virtual ~AcknowledgementPacket();

    public:
        void read(SimpleBuffer *sb);

        void write(SimpleBuffer *sb);

    public:
        uint32_t sequence_number;
    };
}


#endif //TRUNK_ACKNOWLEDGEMENTPACKET_H
