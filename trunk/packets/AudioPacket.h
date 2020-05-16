#ifndef TRUNK_AUDIOPACKET_H
#define TRUNK_AUDIOPACKET_H

#include "ContentData.h"

namespace akanchi {
    class AudioPacket : public ContentData {
    public:
        AudioPacket();

        virtual ~AudioPacket();
    };
}

#endif //TRUNK_AUDIOPACKET_H
