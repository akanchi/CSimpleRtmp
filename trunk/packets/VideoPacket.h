#ifndef TRUNK_VIDEOPACKET_H
#define TRUNK_VIDEOPACKET_H

#include "ContentData.h"

namespace akanchi {
    class VideoPacket : public ContentData {
    public:
        VideoPacket();

        virtual ~VideoPacket();
    };
}

#endif //TRUNK_VIDEOPACKET_H
