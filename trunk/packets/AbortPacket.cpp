#include "AbortPacket.h"

namespace akanchi {
    AbortPacket::AbortPacket()
            : RtmpPacket() {

    }

    AbortPacket::~AbortPacket() {

    }

    void AbortPacket::read(SimpleBuffer *sb) {
        chunk_stream_id = sb->read_4bytes();
    }

    void AbortPacket::write(SimpleBuffer *sb) {
        sb->write_4bytes(chunk_stream_id);
    }
}
