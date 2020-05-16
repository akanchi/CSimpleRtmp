#ifndef TRUNK_RTMPHEADER_H
#define TRUNK_RTMPHEADER_H

#include <cstdint>
#include "../amf/amf0/simple_buffer.h"

class ChunkMessageType {
public:
    static const uint8_t SET_CHUNK_SIZE = 0x01;
    static const uint8_t ABORT_MESSAGE = 0x02;
    static const uint8_t ACKNOWLEDGEMENT = 0x03;
    static const uint8_t USER_CONTROL_MESSAGE = 0x04;
    static const uint8_t WINDOW_ACKNOWLEDGEMENT_SIZE = 0x05;
    static const uint8_t SET_PEER_BANDWIDTH = 0X06;
    static const uint8_t AUDIO_MESSAGE = 0x08;
    static const uint8_t VIDEO_MESSAGE = 0x09;
    static const uint8_t DATA_AMF0 = 0x12;
    static const uint8_t COMMAND_MESSAGE_AMF0 = 0x14;
};

class ChunkType {
public:
    static const uint8_t TYPE_0_FULL = 0x00;
    static const uint8_t TYPE_1_RELATIVE_LARGE = 0x01;
    static const uint8_t TYPE_2_RELATIVE_TIMESTAMP_ONLY = 0x02;
    static const uint8_t TYPE_3_RELATIVE_SINGLE_BYTE = 0x03;
};

class RtmpChannel {
public:
    static const uint8_t RTMP_CONTROL_CHANNEL = 0x02;
    static const uint8_t RTMP_COMMAND_CHANNEL = 0x03;
};

namespace akanchi {
    class ChunkStreamInfo;

    class RtmpHeader {
    public:
        RtmpHeader();

        virtual ~RtmpHeader();

    public:
        void readBasicHeaderFromSocket(int fd);

        void readFromSocket(int fd);

        void write(SimpleBuffer *sb, uint8_t chunkType, ChunkStreamInfo *chunkStreamInfo);

    public:
        uint8_t fmt;
        uint32_t csid;
        uint32_t timestamp;
        uint32_t timestamp_delta;
        uint32_t message_length;
        uint8_t message_type_id;
        uint32_t message_stream_id;
        uint32_t extended_timestamp;
    };
}

#endif //TRUNK_RTMPHEADER_H
