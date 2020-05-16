#include "UserControlPacket.h"

namespace akanchi {
    UserControlPacket::UserControlPacket()
            : RtmpPacket(), event_type(0) {
        header->csid = RtmpChannel::RTMP_CONTROL_CHANNEL;
        header->message_type_id = ChunkMessageType::USER_CONTROL_MESSAGE;
    }

    UserControlPacket::~UserControlPacket() {

    }

    void UserControlPacket::read(SimpleBuffer *sb) {
        event_type = sb->read_2bytes();
        if (event_type == UserControlEventType::SET_BUFFER_LENGTH) {
            event_data[0] = sb->read_4bytes();
            event_data[1] = sb->read_4bytes();
        } else {
            event_data[0] = sb->read_4bytes();
        }
    }

    void UserControlPacket::write(SimpleBuffer *sb) {
        sb->write_2bytes(event_type);
        sb->write_4bytes(event_data[0]);
        if (event_type == UserControlEventType::SET_BUFFER_LENGTH) {
            sb->write_4bytes(event_data[1]);
        }
    }
}
