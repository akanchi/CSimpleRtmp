#ifndef TRUNK_RTMPDECODER_H
#define TRUNK_RTMPDECODER_H

#include <memory>

namespace akanchi {
    class RtmpPacket;
    class RtmpSessionInfo;
    class RtmpDecoder {
    public:
        RtmpDecoder(std::shared_ptr<RtmpSessionInfo> sessionInfo);

        virtual ~RtmpDecoder();

    public:
        std::shared_ptr<RtmpPacket> readPacket(int fd);

    private:
        std::shared_ptr<RtmpSessionInfo> _session_info;
    };
}

#endif //TRUNK_RTMPDECODER_H
