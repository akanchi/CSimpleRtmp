#ifndef TRUNK_RTMPPACKET_H
#define TRUNK_RTMPPACKET_H

#include <cstdint>
#include <memory>

#include "RtmpHeader.h"
#include "../io/TcpWrapper.h"

class SimpleBuffer;
namespace akanchi {
    class RtmpHeader;

    class ChunkStreamInfo;

    class RtmpPacket {
    public:
        RtmpPacket();

        virtual ~RtmpPacket();

    public:
        virtual void read(SimpleBuffer *sb);

        virtual void write(SimpleBuffer *sb);

        virtual void write(SimpleBuffer *sb, uint32_t chunkSize, ChunkStreamInfo *chunkStreamInfo);

        /*!
         * 音视频包数据首地址
         * @return 数据首地址
         */
        virtual char *array();
        /*!
         * 音视频包数据长度
         * @return 数据长度
         */
        virtual uint32_t size();

    public:
        std::shared_ptr<RtmpHeader> header;
    };
}

#endif //TRUNK_RTMPPACKET_H
