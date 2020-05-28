#include "RtmpSessionInfo.h"

namespace akanchi {
    RtmpSessionInfo::RtmpSessionInfo()
            : chunk_size(128)
            , out_chunk_size(128) {

    }

    RtmpSessionInfo::~RtmpSessionInfo() {

    }

    ChunkStreamInfo *RtmpSessionInfo::getChunkStreamInfo(uint32_t chunkStreamId) {
        ChunkStreamInfo *chunkStreamInfo;
        auto it = chunkChannels.find(chunkStreamId);
        if (it != chunkChannels.end()) {
            chunkStreamInfo = it->second.get();
        } else {
            chunkStreamInfo = new ChunkStreamInfo;
            chunkChannels[chunkStreamId] = std::shared_ptr<ChunkStreamInfo>(chunkStreamInfo);
        }

        return chunkStreamInfo;
    }

    std::string RtmpSessionInfo::takeInvokedCommand(uint32_t transactionId) {
        std::lock_guard<std::mutex> lk(_mutex);
        std::string ret = "";
        if (invokedMethods.find(transactionId) != invokedMethods.end()) {
            ret = invokedMethods[transactionId];
            invokedMethods.erase(transactionId);
        }

        return ret;
    }

    std::string RtmpSessionInfo::addInvokedCommand(uint32_t transactionId, const std::string &commandName) {
        std::lock_guard<std::mutex> lk(_mutex);
        return invokedMethods[transactionId] = commandName;
    }

    void RtmpSessionInfo::reset() {
        chunk_size = 128;
        out_chunk_size = 128;
        chunkChannels.clear();
        {
            std::lock_guard<std::mutex> lk(_mutex);
            invokedMethods.clear();
        }
    }
}
