#include "RtmpPacket.h"
#include "../amf/amf0/simple_buffer.h"

namespace akanchi {
    RtmpPacket::RtmpPacket() {
        header = std::make_shared<RtmpHeader>();
    }

    RtmpPacket::~RtmpPacket() {

    }

    void RtmpPacket::read(SimpleBuffer *sb) {
        // do nothing
    }

    void RtmpPacket::write(SimpleBuffer *sb) {
        // do nothing
    }

    char *RtmpPacket::array() {
        return nullptr;
    }

    uint32_t RtmpPacket::size() {
        return 0;
    }

    void RtmpPacket::write(SimpleBuffer *sb, uint32_t chunkSize, ChunkStreamInfo *chunkStreamInfo) {
        SimpleBuffer bodySb;
        write(&bodySb);

        // 如果是音视频数据，bodySb为空，应使用array() & size()
        char *bodyPtr = array() ? array() : bodySb.data();
        uint32_t bodySize = array() ? size() : bodySb.size();

        header->message_length = bodySize;

        // 这里用只用type 0，不使用文档中的 5.3.1.2.2. Type 1 （The message stream ID is not included; this chunk takes the same stream ID as the preceding chunk.）
        // 就算是发stream id一样的packet，也用type 0
        header->write(sb, ChunkType::TYPE_0_FULL, chunkStreamInfo);
        int remainBodySize = bodySize;
        int bodyIndex = 0;
        while (remainBodySize > chunkSize) {
            sb->append(bodyPtr + bodyIndex, chunkSize);
            remainBodySize -= chunkSize;
            bodyIndex += chunkSize;

            // 这里可以参考specification, 5.3.2.2. Example 2
            // 还要处理拓展时间戳的问题
            header->write(sb, ChunkType::TYPE_3_RELATIVE_SINGLE_BYTE, chunkStreamInfo);
        }
        sb->append(bodyPtr + bodyIndex, remainBodySize);
    }
}
