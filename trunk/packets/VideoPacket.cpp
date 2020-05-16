#include "VideoPacket.h"

namespace akanchi {
    VideoPacket::VideoPacket()
            : ContentData() {
        header->csid = 0x05;
        header->message_type_id = ChunkMessageType::VIDEO_MESSAGE;
    }

    VideoPacket::~VideoPacket() {

    }
}
