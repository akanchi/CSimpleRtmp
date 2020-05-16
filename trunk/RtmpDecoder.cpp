#include "RtmpDecoder.h"

#include "packets/RtmpPacket.h"
#include "io/TcpWrapper.h"
#include "amf/amf0/simple_buffer.h"
#include "packets/AbortPacket.h"
#include "packets/SetChunkSizePacket.h"
#include "packets/AcknowledgementPacket.h"
#include "packets/CommandPacket.h"
#include "packets/SetPeerBandwidthPacket.h"
#include "packets/WindowAckSizePacket.h"
#include "packets/UserControlPacket.h"
#include "ChunkStreamInfo.h"
#include "RtmpSessionInfo.h"

namespace akanchi {
    RtmpDecoder::RtmpDecoder(std::shared_ptr<RtmpSessionInfo> sessionInfo)
            : _session_info(sessionInfo)
    {

    }

    RtmpDecoder::~RtmpDecoder()
    {

    }

    std::shared_ptr<RtmpPacket> RtmpDecoder::readPacket(int fd)
    {
        RtmpPacket *packet = nullptr;
        std::shared_ptr<RtmpHeader> header = std::make_shared<RtmpHeader>();
        header->readFromSocket(fd);

        ChunkStreamInfo *chunkStreamInfo = _session_info->getChunkStreamInfo(header->csid);
        chunkStreamInfo->prevHeaderRx = header;

        SimpleBuffer data;

        if (header->message_length > _session_info->chunk_size) {
            // This packet consists of more than one chunk; store the chunks in the chunk stream until everything is read
            if (!chunkStreamInfo->storePacketChunk(fd, _session_info->chunk_size)) {
                return nullptr; // packet is not yet complete
            } else {
                chunkStreamInfo->getStoredPacketInputStream(&data);
            }
        } else {
            TcpWrapper::read(fd, &data, header->message_length);
        }

        switch (header->message_type_id) {
            case ChunkMessageType::SET_CHUNK_SIZE:
                packet = new SetChunkSizePacket();
                break;
            case ChunkMessageType::ABORT_MESSAGE:
                packet = new AbortPacket();
                break;
            case ChunkMessageType::ACKNOWLEDGEMENT:
                packet = new AcknowledgementPacket();
                break;
            case ChunkMessageType::USER_CONTROL_MESSAGE:
                packet = new UserControlPacket();
                break;
            case ChunkMessageType::WINDOW_ACKNOWLEDGEMENT_SIZE:
                packet = new WindowAckSizePacket();
                break;
            case ChunkMessageType::SET_PEER_BANDWIDTH:
                packet = new SetPeerBandwidthPacket();
                break;
            case ChunkMessageType::AUDIO_MESSAGE:
                break;
            case ChunkMessageType::VIDEO_MESSAGE:
                break;
            case ChunkMessageType::COMMAND_MESSAGE_AMF0:
                packet = new CommandPacket();
                break;
            default:
                break;
        }

        if (packet) {
            header.swap(packet->header);
            packet->read(&data);
        }

        return std::shared_ptr<RtmpPacket>(packet);
    }
}
