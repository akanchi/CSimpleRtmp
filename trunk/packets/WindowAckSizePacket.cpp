#include "WindowAckSizePacket.h"

namespace akanchi {
    WindowAckSizePacket::WindowAckSizePacket()
            : RtmpPacket() {
        header->csid = RtmpChannel::RTMP_CONTROL_CHANNEL;
        header->message_type_id = ChunkMessageType::WINDOW_ACKNOWLEDGEMENT_SIZE;
    }

    WindowAckSizePacket::~WindowAckSizePacket() {

    }

    void WindowAckSizePacket::read(SimpleBuffer *sb) {
        acknowledgement_window_size = sb->read_4bytes();
    }

    void WindowAckSizePacket::write(SimpleBuffer *sb) {
        sb->write_4bytes(acknowledgement_window_size);
    }
}
