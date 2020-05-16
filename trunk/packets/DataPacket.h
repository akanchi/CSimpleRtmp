#ifndef TRUNK_DATAPACKET_H
#define TRUNK_DATAPACKET_H

#include "VariableBodyRtmpPacket.h"

namespace akanchi {
    class DataPacket : public VariableBodyRtmpPacket {
    public:
        DataPacket();

        virtual ~DataPacket();

    public:
        void read(SimpleBuffer *sb);

        void write(SimpleBuffer *sb);

    public:
        std::string type;
    };
}

#endif //TRUNK_DATAPACKET_H
