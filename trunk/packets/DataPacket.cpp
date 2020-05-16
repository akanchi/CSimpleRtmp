#include "DataPacket.h"

namespace akanchi {
    DataPacket::DataPacket()
            : VariableBodyRtmpPacket() {

    }

    DataPacket::~DataPacket() {

    }

    void DataPacket::read(SimpleBuffer *sb) {
        uint8_t marker = sb->read_1byte();
        uint16_t length = sb->read_2bytes();
        type = sb->read_string(length);

        readVariableData(sb, 3 + length);
    }

    void DataPacket::write(SimpleBuffer *sb) {
        Amf0String commandNameString(type);
        commandNameString.write(sb);

        writeVariableData(sb);
    }
}
