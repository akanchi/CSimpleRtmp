#include "SetPeerBandwidthPacket.h"

namespace akanchi {
    SetPeerBandwidthPacket::SetPeerBandwidthPacket()
            : RtmpPacket() {
        header->csid = RtmpChannel::RTMP_CONTROL_CHANNEL;
        header->message_type_id = ChunkMessageType::SET_PEER_BANDWIDTH;
    }

    SetPeerBandwidthPacket::~SetPeerBandwidthPacket() {

    }

    void SetPeerBandwidthPacket::read(SimpleBuffer *sb) {
        bandwidth = sb->read_4bytes();
        limit_type = sb->read_1byte();
    }

    void SetPeerBandwidthPacket::write(SimpleBuffer *sb) {
        sb->write_4bytes(bandwidth);
        sb->write_1byte(limit_type);
    }
}
