#include "AudioPacket.h"

namespace akanchi {
    AudioPacket::AudioPacket()
            : ContentData() {
        header->csid = 0x04;
        header->message_type_id = ChunkMessageType::AUDIO_MESSAGE;
    }

    AudioPacket::~AudioPacket() {

    }
}
