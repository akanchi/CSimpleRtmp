#ifndef TRUNK_RTMPSESSIONINFO_H
#define TRUNK_RTMPSESSIONINFO_H


#include <cstdint>
#include <map>
#include <string>
#include <mutex>
#include "ChunkStreamInfo.h"

namespace akanchi {
    class RtmpSessionInfo {
    public:
        RtmpSessionInfo();

        virtual ~RtmpSessionInfo();

    public:
        ChunkStreamInfo *getChunkStreamInfo(uint32_t chunkStreamId);

        std::string takeInvokedCommand(uint32_t transactionId);

        std::string addInvokedCommand(uint32_t transactionId, const std::string &commandName);

    public:
        uint32_t window_bytes_read;
        uint32_t acknowledgement_window_size;
        uint32_t total_bytes_read;
        volatile uint32_t chunk_size;
        std::map<uint32_t, std::shared_ptr<ChunkStreamInfo>> chunkChannels;
        std::map<uint32_t, std::string> invokedMethods;

        std::mutex _mutex;
    };
}

#endif //TRUNK_RTMPSESSIONINFO_H
