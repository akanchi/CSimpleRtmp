#ifndef TRUNK_USERCONTROLPACKET_H
#define TRUNK_USERCONTROLPACKET_H

#include <map>

#include "RtmpPacket.h"

namespace akanchi {
    class UserControlEventType {
    public:
        static const uint16_t STREAM_BEGIN = 0;
        static const uint16_t STREAM_EOF = 1;
        static const uint16_t STREAM_DRY = 2;
        static const uint16_t SET_BUFFER_LENGTH = 3;
        static const uint16_t PING_REQUEST = 6;
        static const uint16_t PONG_REPLY = 7;
        static const uint16_t BUFFER_EMPTY = 31;
        static const uint16_t BUFFER_READY = 32;
    };

    class UserControlPacket : public RtmpPacket {
    public:
        UserControlPacket();

        virtual ~UserControlPacket();

    public:
        void read(SimpleBuffer *sb);

        void write(SimpleBuffer *sb);

    public:
        uint16_t event_type;
        std::map<int, int> event_data;
    };
}

#endif //TRUNK_USERCONTROLPACKET_H
