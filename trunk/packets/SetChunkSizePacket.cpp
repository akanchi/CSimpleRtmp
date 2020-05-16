#include "SetChunkSizePacket.h"

namespace akanchi {
    SetChunkSizePacket::SetChunkSizePacket()
            : RtmpPacket(), chunk_size(128) {
        header->csid = RtmpChannel::RTMP_CONTROL_CHANNEL;
        header->message_type_id = ChunkMessageType::SET_CHUNK_SIZE;
    }

    SetChunkSizePacket::~SetChunkSizePacket() {

    }

    void SetChunkSizePacket::read(SimpleBuffer *sb) {
        chunk_size = sb->read_4bytes();
    }

    void SetChunkSizePacket::write(SimpleBuffer *sb) {
        sb->write_4bytes(chunk_size);
    }
}
