#ifndef TRUNK_RTMPCLIENT_H
#define TRUNK_RTMPCLIENT_H

#include <string>
#include <memory>
#include <thread>
#include <mutex>

#include "queue/simple_queue.h"
#include "condition_wait_wrapper/ConditionWaitWrapper.h"

class Amf0Data;

namespace akanchi {
    class TcpClient;
    class ReadThread;
    class RtmpPacket;
    class RtmpSessionInfo;
    class CommandPacket;
    class RtmpDecoder;
    class AbortPacket;
    class UserControlPacket;

    class RtmpClient {
    public:
        RtmpClient();

        virtual ~RtmpClient();

    public:
        /*!
         * 连接rtmp服务器，包含handshake及发送connect packet的过程。阻塞当前调用线程，5秒超时
         * @param url rtmp url
         * @return 连接成功则返回true
         */
        bool connect(const std::string &url);

        /*!
         * 发送releaseStream、FCPublish 及 createStream。阻塞当前调用线程，5秒超时
         * @return 成功则返回true，同时表明可以发送音视频数据了哦
         */
        bool publish();

        void close();

        void send_audio(char *data, int size, uint32_t dts);

        void send_video(char *data, int size, uint32_t dts);

        void send_metadata(std::shared_ptr<Amf0Data> ecmaArray);

    private:
        void reset();

        /*!
         * 解析rtmp url
         * @param url rtmp url
         * @return 解析成功则返回true
         */
        bool parse_url(const std::string &url);

        /*!
         * rtmp握手
         * @return 成功则返回true
         */
        bool handshake();

        /*!
         * 发送connect packet
         */
        void send_connect_packet();

        /*!
         * 发送publish包
         */
        void send_publish_packet();

        void handle_rx_packet_loop();

        void handle_invoke(std::shared_ptr<RtmpPacket> invoke);

        void send_packet(std::shared_ptr<RtmpPacket> packet);

        void handle_abort_packet(AbortPacket *packet);
        void handle_user_control_packet(UserControlPacket *packet);

    public:
        std::string host;
        std::string ip;
        int port;
        std::string app_name;
        std::string stream_name;
        std::string tc_url;
        std::string swf_url;

    private:
        std::shared_ptr<TcpClient> _tcp_client;
        std::shared_ptr<ReadThread> _read_thread;
        std::shared_ptr<RtmpSessionInfo> _rtmp_session;
        std::shared_ptr<RtmpDecoder> _rtmp_decoder;

        ConditionWaitWrapper _connecting_wait;
        ConditionWaitWrapper _publish_wait;

        uint32_t _transaction_id_counter;
        uint32_t _current_stream_id;
    };
}

#endif //TRUNK_RTMPCLIENT_H
