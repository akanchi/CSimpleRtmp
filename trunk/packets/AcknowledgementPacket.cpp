#include "AcknowledgementPacket.h"

namespace akanchi {
    AcknowledgementPacket::AcknowledgementPacket()
            : RtmpPacket() {
        header->csid = RtmpChannel::RTMP_CONTROL_CHANNEL;
        header->message_type_id = ChunkMessageType::ACKNOWLEDGEMENT;
    }

    AcknowledgementPacket::~AcknowledgementPacket() {

    }

    void AcknowledgementPacket::read(SimpleBuffer *sb) {
        sequence_number = sb->read_4bytes();
    }

    void AcknowledgementPacket::write(SimpleBuffer *sb) {
        sb->write_4bytes(sequence_number);
    }
}
