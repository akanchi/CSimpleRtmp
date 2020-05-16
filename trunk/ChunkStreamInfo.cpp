#include "ChunkStreamInfo.h"

#include <time.h>

#include "io/TcpWrapper.h"

namespace akanchi {
    ChunkStreamInfo::ChunkStreamInfo() {

    }

    ChunkStreamInfo::~ChunkStreamInfo() {

    }

    bool ChunkStreamInfo::canReusePrevHeaderTx(uint8_t forMessageType) {
        return (prevHeaderTx != nullptr && prevHeaderTx->message_type_id == forMessageType);
    }

    uint64_t ChunkStreamInfo::markRealAbsoluteTimestampTx() {
        uint64_t timestamp = (uint64_t) ::time(NULL) * 1000;
        real_last_timestamp = timestamp - real_last_timestamp;
        return real_last_timestamp;
    }

    bool ChunkStreamInfo::storePacketChunk(int fd, int chunkSize) {
        uint32_t remainingBytes = prevHeaderRx->message_length - baos.size();
        uint32_t readBytes = remainingBytes < chunkSize ? readBytes : chunkSize;
        TcpWrapper::read(fd, &baos, readBytes);
        return baos.size() == prevHeaderRx->message_length;
    }

    void ChunkStreamInfo::getStoredPacketInputStream(SimpleBuffer *sb) {
        sb->append(baos.data(), baos.size());
        baos.clear();
    }

    void ChunkStreamInfo::clearStoredPacket() {
        baos.clear();
    }

    uint64_t ChunkStreamInfo::markDeltaTimestampTx() {
        uint64_t currentTimestamp = (uint64_t) ::time(NULL) * 1000;
        uint64_t diffTimestamp = currentTimestamp - real_last_timestamp;
        real_last_timestamp = currentTimestamp;
        return diffTimestamp;
    }
}
