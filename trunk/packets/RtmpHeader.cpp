#include "RtmpHeader.h"

#include "../io/TcpWrapper.h"
#include "../ChunkStreamInfo.h"

namespace akanchi {
    const uint32_t MAX_TIMESTAMP = 0xffffff;

    RtmpHeader::RtmpHeader()
            : fmt(0), csid(0), timestamp(0), timestamp_delta(0), message_length(0), message_stream_id(0),
              extended_timestamp(0) {
    }

    RtmpHeader::~RtmpHeader() {

    }

    void RtmpHeader::readBasicHeaderFromSocket(int fd) {
        SimpleBuffer data;
        TcpWrapper::read(fd, &data, 1);
        uint32_t firstByte = data.read_1byte();
        fmt = (firstByte >> 6) & 0xff;
        csid = firstByte & 0x3f;
        if (csid == 0) {
            TcpWrapper::read(fd, &data, 1);
            csid = data.read_1byte() + 64;
        } else if (csid == 1) {
            TcpWrapper::read(fd, &data, 2);
            csid = data.read_2bytes() + 64;
        }
    }

    void RtmpHeader::readFromSocket(int fd) {
        readBasicHeaderFromSocket(fd);

        SimpleBuffer data;
        char *p = (char *) &message_stream_id;
        switch (fmt) {
            case ChunkType::TYPE_0_FULL:
                TcpWrapper::read(fd, &data, 11);
                timestamp = data.read_3bytes();
                message_length = data.read_3bytes();
                message_type_id = data.read_1byte();
                message_stream_id = data.read_4bytes();
                message_stream_id = p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
                break;
            case ChunkType::TYPE_1_RELATIVE_LARGE:
                TcpWrapper::read(fd, &data, 7);
                timestamp_delta = data.read_3bytes();
                message_length = data.read_3bytes();
                message_type_id = data.read_1byte();
                break;
            case ChunkType::TYPE_2_RELATIVE_TIMESTAMP_ONLY:
                TcpWrapper::read(fd, &data, 3);
                timestamp_delta = data.read_3bytes();
                break;
            case ChunkType::TYPE_3_RELATIVE_SINGLE_BYTE:
                break;
        }
    }

    void RtmpHeader::write(SimpleBuffer *sb, uint8_t chunkType, ChunkStreamInfo *chunkStreamInfo) {
        // 第一个字节。注：本项目只考虑 chunkstream id < 64 的情况
        // 0 1 2 3 4 5 6 7
        // +-+-+-+-+-+-+-+-+
        // |fmt|   cs id   |
        // +-+-+-+-+-+-+-+-+
        sb->write_1byte(((chunkType << 6) & 0xc0) | (csid & 0x3f));
        char *p = (char *) &message_stream_id;
        switch (chunkType) {
            case ChunkType::TYPE_0_FULL:
                chunkStreamInfo->markDeltaTimestampTx();
                // 时间戳
                sb->write_3bytes(timestamp > MAX_TIMESTAMP ? MAX_TIMESTAMP : timestamp);
                // rtmp packet长度，不包含header
                sb->write_3bytes(message_length);
                // message_type_id，详见ChunkMessageType
                sb->write_1byte(message_type_id);
                // message_stream_id，little-endian
                sb->write_1byte(p[0]);
                sb->write_1byte(p[1]);
                sb->write_1byte(p[2]);
                sb->write_1byte(p[3]);
                // 如果时间戳 > 0xffffff, 则需要传拓展时间戳
                if (timestamp > MAX_TIMESTAMP) {
                    // Extended Timestamp
                    extended_timestamp = timestamp;
                    sb->write_4bytes(timestamp);
                }
                break;
            case ChunkType::TYPE_1_RELATIVE_LARGE:
                timestamp_delta = chunkStreamInfo->markDeltaTimestampTx();
                timestamp = chunkStreamInfo->prevHeaderTx->timestamp + timestamp_delta;
                sb->write_3bytes(timestamp > MAX_TIMESTAMP ? MAX_TIMESTAMP : timestamp_delta);
                sb->write_3bytes(message_length);
                sb->write_1byte(message_type_id);
                if (timestamp > MAX_TIMESTAMP) {
                    // Extended Timestamp
                    extended_timestamp = timestamp;
                    sb->write_4bytes(timestamp);
                }
                break;
            case ChunkType::TYPE_2_RELATIVE_TIMESTAMP_ONLY:
                timestamp_delta = chunkStreamInfo->markDeltaTimestampTx();
                timestamp = chunkStreamInfo->prevHeaderTx->timestamp + timestamp_delta;
                sb->write_3bytes(timestamp > MAX_TIMESTAMP ? MAX_TIMESTAMP : timestamp_delta);
                if (timestamp > MAX_TIMESTAMP) {
                    // Extended Timestamp
                    extended_timestamp = timestamp;
                    sb->write_4bytes(timestamp);
                }
                break;
            case ChunkType::TYPE_3_RELATIVE_SINGLE_BYTE:
                if (extended_timestamp > 0) {
                    sb->write_4bytes(extended_timestamp);
                }
                break;
            default:
                // 不存在其它类型
                break;
        }
    }
}
