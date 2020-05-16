#include "RtmpClient.h"
#include <chrono>
#include <regex>
#ifdef _WIN32
#include <winsock2.h>
#include <WS2tcpip.h>
#else
#include <netdb.h>
#include <arpa/inet.h>
#endif // _WIN32
#include "io/TcpClient.h"
#include "io/TcpWrapper.h"
#include "Handshake.h"
#include "packets/CommandPacket.h"
#include "ReadThread.h"
#include "packets/RtmpPacket.h"
#include "RtmpSessionInfo.h"
#include <iostream>
#include "packets/AudioPacket.h"
#include "packets/VideoPacket.h"
#include "packets/DataPacket.h"
#include "packets/AcknowledgementPacket.h"
#include "packets/WindowAckSizePacket.h"
#include "packets/SetChunkSizePacket.h"
#include "packets/AbortPacket.h"
#include "packets/UserControlPacket.h"
#include "RtmpDecoder.h"

namespace akanchi {
    const int RTMP_DEFAULT_PORT = 1935;
    const std::string RTMP_URL_PATTERN = "^rtmp:\\/\\/([^\\/:]+)([:](\\d+))*\\/([^\\/]+)(\\/(.*))*$";

    RtmpClient::RtmpClient()
            : _tcp_client(std::make_shared<TcpClient>())
            , _rtmp_session(std::make_shared<RtmpSessionInfo>())
            , port(1935)
            , _transaction_id_counter(0)
            , _current_stream_id(0) {
        _rtmp_decoder = std::make_shared<RtmpDecoder>(_rtmp_session);
        _read_thread = std::make_shared<ReadThread>();
        _read_thread->setRunFunc(std::bind(&RtmpClient::handle_rx_packet_loop, this));
    }

    RtmpClient::~RtmpClient() {

    }

    bool RtmpClient::parse_url(const std::string &url) {
        std::cmatch match;
        bool ret = std::regex_match(url.c_str(), match, std::regex(RTMP_URL_PATTERN));
        if (!ret) {
            // TODO: handle error
            return false;
        }

        // 举个例子，如果输入的url为 rtmp://127.0.0.1:1935/live/livestream，则 match 的值如下
        // match[0] = rtmp://127.0.0.1:1935/live/livestream
        // match[1] = 127.0.0.1
        // match[2] = :1935
        // match[3] = 1935
        // match[4] = live
        // match[5] = /livestream
        // match[6] = livestream

        // host & ip
        host = match[1];
        char ipBuffer[16] = { 0 };
        // 如果输入的地址是域名的话，要获取一下ip地址
        hostent *host_result = gethostbyname(host.c_str());
        for (int i = 0; i < host_result->h_length; i++) {
            // 获取到第一个ip地址后返回
            inet_ntop(AF_INET, host_result->h_addr_list[i], ipBuffer, sizeof(ipBuffer));
            break;
        }
        ip = ipBuffer;

        // port
        std::string portString = match[3];
        port = portString.length() > 0 ? std::atoi(portString.c_str()) : RTMP_DEFAULT_PORT;

        // app name
        app_name = match[4];

        // stream name
        stream_name = match[6];

        // tc url, 一般是rtmp://127.0.0.1:1935/live 这样的格式，但是specification好像没描述清楚
        tc_url = "rtmp://" + host;
        if (portString.length() > 0) {
            tc_url += ":" + portString;
        }
        tc_url += "/" + app_name;

        return ret;
    }

    bool RtmpClient::connect(const std::string &url) {
        bool ret = parse_url(url);
        if (!ret) {
            return ret;
        }

        _tcp_client->socket();
        if (!(ret = _tcp_client->connect(ip, port) >= 0)) {
            // tcp 连接失败
            return ret;
        }

        // 握手
        if (!(ret = handshake())) {
            // 握手失败
            return ret;
        }

        // rtmp包接收线程
        _read_thread->Start();

        // 发送connect packet
        send_connect_packet();

        // 阻塞等待服务器响应 connect packet，超时时间5秒
        ret = _connecting_wait.wait_for(5);

        return ret;
    }

    bool RtmpClient::publish() {
        // librtmp是在服务器回connect后就发以下3个包。我们的实现让用户自己调用。
        CommandPacket *releaseStream = new CommandPacket;
        releaseStream->header->csid = RtmpChannel::RTMP_COMMAND_CHANNEL;
        releaseStream->command_name = "releaseStream";
        releaseStream->transaction_id = ++_transaction_id_counter;
        releaseStream->addData(std::shared_ptr<Amf0Data>(new Amf0Null));
        releaseStream->addString(stream_name);
        send_packet(std::shared_ptr<RtmpPacket>(releaseStream));

        CommandPacket *FCPublish = new CommandPacket;
        FCPublish->header->csid = RtmpChannel::RTMP_COMMAND_CHANNEL;
        FCPublish->command_name = "FCPublish";
        FCPublish->transaction_id = ++_transaction_id_counter;
        FCPublish->addData(std::shared_ptr<Amf0Data>(new Amf0Null));
        FCPublish->addString(stream_name);
        send_packet(std::shared_ptr<RtmpPacket>(FCPublish));

        CommandPacket *createStream = new CommandPacket;
        createStream->header->csid = RtmpChannel::RTMP_COMMAND_CHANNEL;
        createStream->command_name = "createStream";
        createStream->transaction_id = ++_transaction_id_counter;
        send_packet(std::shared_ptr<RtmpPacket>(createStream));

        return _publish_wait.wait_for(5);
    }

    bool RtmpClient::handshake() {
        /*!
         * 5.2.5. Handshake Diagram
         * +-------------+ 										+-------------+
           |   Client    | 		          TCP/IP Network		|	Server    |
           +-------------+						|				+-------------+
     		      |								|					   |
           Uninitialized						|				Uninitialized
  			      |				C0				|					   |
  			      |---------------------------->|			C0		   |
  			      |								|--------------------->|
  			      |				C1				|					   |
  			      |---------------------------->|			S0		   |
  			      |								|<---------------------|
  			      |							    |		    S1		   |
            Version sent						|<---------------------|
  			      |				S0				|				       |
  			      |<----------------------------|					   |
  			      |				S1				|					   |
  			      |<----------------------------|		         Version sent
  			      |								|			C1		   |
  			      |								|--------------------->|
  			      |				C2				|					   |
  			      |---------------------------->|           S2		   |
                  |								|<---------------------|
               Ack Sent							|					Ack Sent
     		      |				S2				|					   |
     		      |<----------------------------|					   |
     		      |								|			C2		   |
     		      |								|--------------------->|
            Handshake Done					    |				Handshake Done
   			      |								|					   |
         */

        // 根据文档的时序图，我们使用c0 c1 s0 s2 c2 s2 这种握手实现
        // 但更常用的可能是 c0c1 s0s1s2 c2这种实现
        bool ret = false;
        Handshake hs(_tcp_client.get());

        if (!(ret = hs.write_c0() >= 0)) {
            return ret;
        }

        if (!(ret = hs.write_c1() >= 0)) {
            return ret;
        }

        if (!(ret = hs.read_s0() >= 0)) {
            return ret;
        }

        if (!(ret = hs.read_s1() >= 0)) {
            return ret;
        }

        if (!(ret = hs.write_c2() >= 0)) {
            return ret;
        }

        if (!(ret = hs.read_s2() >= 0)) {
            return ret;
        }

        return ret;
    }

    void RtmpClient::close() {
        CommandPacket *closeStream = new CommandPacket;
        closeStream->header->csid = RtmpChannel::RTMP_COMMAND_CHANNEL;
        closeStream->command_name = "closeStream";
        closeStream->transaction_id = 0;
        closeStream->addData(std::shared_ptr<Amf0Data>(new Amf0Null));
        send_packet(std::shared_ptr<RtmpPacket>(closeStream));
        _tcp_client->close();
        _read_thread->stop();

        reset();
    }

    void RtmpClient::reset() {
        _connecting_wait.set_condition(false);
        _publish_wait.set_condition(false);

        _transaction_id_counter = 0;
        _current_stream_id = 0;
    }

    void RtmpClient::send_connect_packet() {
        /*！
         * 7.2.1.1. connect
         */
        CommandPacket *connectPacket = new CommandPacket;
        connectPacket->header->csid = RtmpChannel::RTMP_COMMAND_CHANNEL;
        // Command Name, Name of the command. Set to "connect".
        connectPacket->command_name = "connect";
        // Transaction ID, Always set to 1.
        connectPacket->transaction_id = ++_transaction_id_counter;

        Amf0Object *amf0Object = new Amf0Object;
        amf0Object->put("app", new Amf0String(app_name));
        amf0Object->put("flashVer", new Amf0String("LNX 11,2,202,233"));
        amf0Object->put("swfUrl", new Amf0String(swf_url));
        amf0Object->put("tcUrl", new Amf0String(tc_url));
        amf0Object->put("fpad", new Amf0Boolean(false));
        amf0Object->put("capabilities", new Amf0Number(239));
        amf0Object->put("audioCodecs", new Amf0Number(3575));
        amf0Object->put("videoCodecs", new Amf0Number(252));
        amf0Object->put("videoFunction", new Amf0Number(1));
        connectPacket->addData(std::shared_ptr<Amf0Data>(amf0Object));

        send_packet(std::shared_ptr<RtmpPacket>(connectPacket));
    }

    void RtmpClient::send_publish_packet() {
        // 7.2.2.6. publish
        CommandPacket *publish = new CommandPacket;
        publish->header->csid = 0x04;
        publish->command_name = "publish";
        publish->transaction_id = 0;
        publish->addData(std::shared_ptr<Amf0Data>(new Amf0Null));
        publish->addString(stream_name);
        // Type of publishing. Set to "live",  "record", or "append".
        // record: The stream is published and the data is recorded to a new file.The file is stored on the server in a subdirectory within the directory that contains the server application. If the file already exists, it is overwritten.
        // append: The stream is published and the data is appended to a file. If no file is found, it is created.
        // live: Live data is published without recording it in a file.
        publish->addString("live");

        send_packet(std::shared_ptr<RtmpPacket>(publish));
    }

    void RtmpClient::send_audio(char *data, int size, uint32_t dts) {
        AudioPacket *audio = new AudioPacket;
        audio->set_data(data, size);
        audio->header->timestamp = dts;
        audio->header->message_stream_id = _current_stream_id;

        send_packet(std::shared_ptr<RtmpPacket>(audio));
    }

    void RtmpClient::send_video(char *data, int size, uint32_t dts) {
        VideoPacket *video = new VideoPacket;
        video->set_data(data, size);
        video->header->timestamp = dts;
        video->header->message_stream_id = _current_stream_id;

        send_packet(std::shared_ptr<RtmpPacket>(video));
    }

    void RtmpClient::send_metadata(std::shared_ptr<Amf0Data> ecmaArray) {
        DataPacket *metadata = new DataPacket;
        metadata->type = "@setDataFrame";
        metadata->header->csid = RtmpChannel::RTMP_COMMAND_CHANNEL;
        metadata->header->message_stream_id = _current_stream_id;
        metadata->header->message_type_id = ChunkMessageType::DATA_AMF0;
        metadata->addString("onMetaData");
        metadata->addData(ecmaArray);

        send_packet(std::shared_ptr<RtmpPacket>(metadata));
    }

    void RtmpClient::handle_rx_packet_loop() {
        std::shared_ptr<RtmpPacket> packet = _rtmp_decoder->readPacket(_tcp_client->fd());
        if (!packet) {
            return;
        }

        switch (packet->header->message_type_id) {
            case ChunkMessageType::SET_CHUNK_SIZE:
                _rtmp_session->chunk_size = ((SetChunkSizePacket *)packet.get())->chunk_size;
                break;
            case ChunkMessageType::ABORT_MESSAGE:
                handle_abort_packet((AbortPacket *)packet.get());
                break;
            case ChunkMessageType::ACKNOWLEDGEMENT:
                break;
            case ChunkMessageType::USER_CONTROL_MESSAGE:
                handle_user_control_packet((UserControlPacket *)packet.get());
                break;
            case ChunkMessageType::WINDOW_ACKNOWLEDGEMENT_SIZE:
                _rtmp_session->acknowledgement_window_size = ((WindowAckSizePacket *)packet.get())->acknowledgement_window_size;
                break;
            case ChunkMessageType::SET_PEER_BANDWIDTH:
                break;
            case ChunkMessageType::AUDIO_MESSAGE:
                break;
            case ChunkMessageType::VIDEO_MESSAGE:
                break;
            case ChunkMessageType::COMMAND_MESSAGE_AMF0:
                handle_invoke(packet);
                break;
            default:
                break;
        }
    }

    void RtmpClient::handle_abort_packet(AbortPacket *packet) {
        if (packet) {
            ChunkStreamInfo *streamInfo = _rtmp_session->getChunkStreamInfo(packet->chunk_stream_id);
            if (streamInfo) {
                streamInfo->clearStoredPacket();
            }
        }
    }

    void RtmpClient::handle_user_control_packet(UserControlPacket *packet) {
        if (!packet) {
            return;
        }

        UserControlPacket *pong = nullptr;
        switch (packet->event_type) {
            case UserControlEventType::STREAM_BEGIN:
                break;
            case UserControlEventType::PING_REQUEST:
                pong = new UserControlPacket;
                pong->event_type = packet->event_type;
                pong->event_data = packet->event_data;
                send_packet(std::shared_ptr<RtmpPacket>(pong));
                break;
            case UserControlEventType::STREAM_EOF:
                break;
            default:
                // Ignore...
                break;
        }
    }

    void RtmpClient::handle_invoke(std::shared_ptr<RtmpPacket> invoke) {
        CommandPacket *invokePacket = (CommandPacket *) invoke.get();

        std::string commandName = invokePacket->command_name;
        std::cout << commandName << std::endl;
        if (commandName == "_result") {
            std::string method = _rtmp_session->takeInvokedCommand(invokePacket->transaction_id);
            std::cout << "method\t" << method << std::endl;
            if ("connect" == method) {
                // _connecting_wait 等待结束，通知RtmpClient::connect 解除阻塞
                _connecting_wait.set_condition(true);
            } else if ("createStream" == method) {
                Amf0Number *number = dynamic_cast<Amf0Number *>(invokePacket->getDataAt(1).get());
                if (number) {
                    _current_stream_id = number->value;
                }
                send_publish_packet();
            }
        } else if ("onStatus" == commandName) {
            Amf0String *code = dynamic_cast<Amf0String *>(dynamic_cast<Amf0Object *>(invokePacket->getDataAt(
                    1).get())->value_at("code"));
            if (code && code->value == "NetStream.Publish.Start") {
                // _publish_wait 等待结束，通知RtmpClient::publish 解除阻塞。这时候就可以发音视频数据了
                _publish_wait.set_condition(true);
            }
        }
    }

    void RtmpClient::send_packet(std::shared_ptr<RtmpPacket> packet) {
        if (packet) {
            ChunkStreamInfo *chunkStreamInfo = _rtmp_session->getChunkStreamInfo(packet->header->csid);
            chunkStreamInfo->prevHeaderTx = packet->header;
            if (!(packet->header->message_type_id == ChunkMessageType::VIDEO_MESSAGE ||
                  packet->header->message_type_id == ChunkMessageType::AUDIO_MESSAGE)) {
                packet->header->timestamp = chunkStreamInfo->markRealAbsoluteTimestampTx();
            }

            if (packet->header->message_type_id == ChunkMessageType::COMMAND_MESSAGE_AMF0) {
                _rtmp_session->addInvokedCommand((uint32_t)((CommandPacket *)packet.get())->transaction_id,
                                                 ((CommandPacket *)packet.get())->command_name);
            }

            SimpleBuffer sb;
            packet->write(&sb, 128, chunkStreamInfo);
            int ret = TcpWrapper::write(_tcp_client->fd(), &sb);
            if (ret < 0) {
                std::cerr << "send_packet failed: " << ret << std::endl;
            }
        }
    }
}
