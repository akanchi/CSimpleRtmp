#include "Handshake.h"

#include <time.h>

#include "amf/amf0/simple_buffer.h"
#include "io/TcpClient.h"
#include "util/util.h"
#include <random>

static const uint32_t RTMP_SIG_SIZE = 1536;
static const uint32_t DIGEST_LENGTH = 32;

static const uint8_t GenuineFPKey[] = {
    0x47, 0x65, 0x6E, 0x75, 0x69, 0x6E, 0x65, 0x20,
    0x41, 0x64, 0x6F, 0x62, 0x65, 0x20, 0x46, 0x6C,
    0x61, 0x73, 0x68, 0x20, 0x50, 0x6C, 0x61, 0x79,
    0x65, 0x72, 0x20, 0x30, 0x30, 0x31,			/* Genuine Adobe Flash Player 001 */
    0xF0, 0xEE, 0xC2, 0x4A, 0x80, 0x68, 0xBE, 0xE8,
    0x2E, 0x00, 0xD0, 0xD1, 0x02, 0x9E, 0x7E, 0x57,
    0x6E, 0xEC, 0x5D, 0x2D, 0x29, 0x80, 0x6F, 0xAB,
    0x93, 0xB8, 0xE6, 0x36, 0xCF, 0xEB, 0x31, 0xAE
}; /* 62 bytes*/

namespace akanchi {
    Handshake::Handshake(TcpClient *tcp)
            : _tcp(tcp) {

    }

    Handshake::~Handshake() {

    }

    int Handshake::write_c0() {
        /*!
         *  0 1 2 3 4 5 6 7
         * +-+-+-+-+-+-+-+-+
         * |    version    |
         * +-+-+-+-+-+-+-+-+
         */
        // version 写死为0x03
        SimpleBuffer c0;
        c0.write_1byte(0x03);
        return _tcp->write(&c0);
    }

    int Handshake::write_c1() {
        /*!
         *  0                   1                   2                   3
         *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |                      time (4 bytes)                           |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |                      version (4 bytes)                        |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |                       random bytes                            |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |                       random bytes                            |
         * |                          (cont)                               |
         * |                           ....                                |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         *                         C1 and S1 bits
         */
        // 如果采用上面的结构握手的话，Flash播放不了h264及aac，下面这部分算法是从rtmpdump拷贝过来的
        uint32_t timestamp = (uint32_t) ::time(NULL);
        uint32_t version = 0x80000702;
        std::string randomData = generate_random_string(RTMP_SIG_SIZE - 8);

        SimpleBuffer c1(RTMP_SIG_SIZE, 0);
        char *p = c1.data();
        c1.set_data(0, (char *) &timestamp, 4);
        c1.set_data(4, (char *) &version, 4);
        c1.set_data(8, randomData.c_str(), randomData.size());
        // 至此，数据结构和文档给出的一模一样

        // 为了解决Flash播放的问题，我们将C1的结构改成如下
        // digestOffset设定是与包头的偏移量
        /*!
         *  0                   1                   2                   3       需要计算digest部分
         *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |                      time (4 bytes)                           |            √
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |                      version (4 bytes)                        |            √
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |                 digestOffset - 12 (4 bytes)                   |            √
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |            random bytes (digestOffset - 12 bytes)             |            √
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |                     digest (32 bytes)                         |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |                       random bytes                            |
         * |                          (cont)                               |            √
         * |                           ....                                |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         *                         C1 and S1 bits
         */

        // 728怎么来的呢？
        // 1536 - 8（time + version）= 1528
        // 1528 / 2 = 764 // 因为digest有可能在前半部分也有可能在后半部分，所以除以2
        // 764 - 32（digest）- 4（offset）= 728 // 所以offset最大值要小于728
        // digest在前半部分 uint32_t digestOffset = (((uint8_t)p[8] + (uint8_t)p[9] + (uint8_t)p[10] + (uint8_t)p[11]) % 728) + 12;
        // digest在后半部分 uint32_t digestOffset = (((uint8_t)p[772] + (uint8_t)p[773] + (uint8_t)p[774] + (uint8_t)p[775]) % 728) + (764 + 8 + 4);
        uint32_t digestOffset = (((uint8_t)p[8] + (uint8_t)p[9] + (uint8_t)p[10] + (uint8_t)p[11]) % 728) + 12;

        SimpleBuffer message;
        message.append(p, digestOffset);
        message.append(p + digestOffset + DIGEST_LENGTH, RTMP_SIG_SIZE - digestOffset - DIGEST_LENGTH);
        hmac_sha265(GenuineFPKey, 30, (unsigned char *) &message.data()[0], message.size(),
                    (unsigned char *) &c1.data()[digestOffset]);
        return _tcp->write(&c1);
    }

    int Handshake::write_c2() {
        // （c2 & s2）的格式和（c1 & s1）一致，这里直接将s1当作c2 原封不动发送给服务器
        return _tcp->write(&_s1);
    }

    int Handshake::read_s0() {
        SimpleBuffer s0;
        return _tcp->read(&s0, 1);
    }

    int Handshake::read_s1() {
        _s1.clear();
        return _tcp->read(&_s1, RTMP_SIG_SIZE);
    }

    int Handshake::read_s2() {
        SimpleBuffer s2;
        return _tcp->read(&s2, RTMP_SIG_SIZE);
    }
}
