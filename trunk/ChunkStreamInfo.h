#ifndef TRUNK_CHUNKSTREAMINFO_H
#define TRUNK_CHUNKSTREAMINFO_H

#include "packets/RtmpHeader.h"

namespace akanchi {
    class ChunkStreamInfo {
    public:
        ChunkStreamInfo();

        virtual ~ChunkStreamInfo();

    public:
        bool canReusePrevHeaderTx(uint8_t forMessageType);

        uint64_t markRealAbsoluteTimestampTx();

        bool storePacketChunk(int fd, int chunkSize);

        void getStoredPacketInputStream(SimpleBuffer *sb);
        void clearStoredPacket();

        uint64_t markDeltaTimestampTx();

    public:
        std::shared_ptr<RtmpHeader> prevHeaderRx;
        std::shared_ptr<RtmpHeader> prevHeaderTx;
        uint64_t real_last_timestamp;
        SimpleBuffer baos;
    };
}

#endif //TRUNK_CHUNKSTREAMINFO_H
