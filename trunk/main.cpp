#include <iostream>
#include "amf/amf0/amf0.h"
#include "amf/amf0/simple_buffer.h"
#include "RtmpClient.h"

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib,"ws2_32.lib")
#endif // _WIN32

int main() {
#ifdef _WIN32
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(sockVersion, &wsaData) != 0) {
        return 0;
    }
#endif // _WIN32

    akanchi::RtmpClient *client = new akanchi::RtmpClient;
    std::cout << "Hello, World!" << std::endl;

    if (!client->connect("rtmp://127.0.0.1:1935/live/livestream")) {
        std::cerr << "connect failed" << std::endl;
        return -1;
    }

    if (!client->publish()) {
        std::cerr << "publish failed" << std::endl;
        return -1;
    }

    if (!client->set_chunk_size(4096)) {
        std::cerr << "set chunk size failed" << std::endl;
    }

    Amf0EcmaArray *ecmaArray = new Amf0EcmaArray;
    ecmaArray->put("encoder", new Amf0String("AKE"));
    ecmaArray->put("encoder_authors", new Amf0String("AKE"));
    ecmaArray->put("duration", new Amf0Number(0));
    ecmaArray->put("width", new Amf0Number(1920));
    ecmaArray->put("height", new Amf0Number(1080));
    ecmaArray->put("videodatarate", new Amf0Number(0));
    ecmaArray->put("framerate", new Amf0Number(0));
    ecmaArray->put("audiodatarate", new Amf0Number(128));
    ecmaArray->put("audiosamplerate", new Amf0Number(44100));
    ecmaArray->put("audiosamplesize", new Amf0Number(16));
    ecmaArray->put("stereo", new Amf0Boolean(true));
    ecmaArray->put("filesize", new Amf0Number(0));
    ecmaArray->put("videocodecid", new Amf0Number(7));
    ecmaArray->put("audiocodecid", new Amf0Number(10));
    client->send_metadata(std::shared_ptr<Amf0Data>(ecmaArray));

    return 0;
}