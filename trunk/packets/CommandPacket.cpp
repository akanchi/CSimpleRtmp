#include "CommandPacket.h"
#include "../amf/amf0/simple_buffer.h"

namespace akanchi {
    CommandPacket::CommandPacket()
            : VariableBodyRtmpPacket() {
        header->message_type_id = ChunkMessageType::COMMAND_MESSAGE_AMF0;
    }

    CommandPacket::~CommandPacket() {

    }

    void CommandPacket::read(SimpleBuffer *sb) {
        // command_name
        uint8_t marker = sb->read_1byte();
        uint16_t length = sb->read_2bytes();
        command_name = sb->read_string(length);

        // transaction_id
        marker = sb->read_1byte();
        int64_t temp = sb->read_8bytes();
        memcpy(&transaction_id, &temp, 8);

        readVariableData(sb, 3 + length + 9);
    }

    void CommandPacket::write(SimpleBuffer *sb) {
        Amf0String commandNameString(command_name);
        commandNameString.write(sb);

        Amf0Number transactionIdNumber(transaction_id);
        transactionIdNumber.write(sb);

        writeVariableData(sb);
    }
}
