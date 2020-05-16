#ifndef TRUNK_COMMANDPACKET_H
#define TRUNK_COMMANDPACKET_H

#include <string>
#include "VariableBodyRtmpPacket.h"

namespace akanchi {
    class CommandPacket : public VariableBodyRtmpPacket {
    public:
        CommandPacket();

        virtual ~CommandPacket();

    public:
        void read(SimpleBuffer *sb);

        void write(SimpleBuffer *sb);

    public:
        std::string command_name;
        double transaction_id;
    };
}

#endif //TRUNK_COMMANDPACKET_H
