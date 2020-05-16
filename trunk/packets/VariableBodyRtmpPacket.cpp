#include "VariableBodyRtmpPacket.h"

namespace akanchi {
    VariableBodyRtmpPacket::VariableBodyRtmpPacket()
            : RtmpPacket() {

    }

    VariableBodyRtmpPacket::~VariableBodyRtmpPacket() {

    }

    void VariableBodyRtmpPacket::readVariableData(SimpleBuffer *sb, int bytesAlreadyRead) {
        do {
            int beginPos = sb->pos();
            Amf0Data *data = Amf0Data::create_amf0data(sb);
            if (data) {
                addData(std::shared_ptr<Amf0Data>(data));
            }
            int endPos = sb->pos();
            bytesAlreadyRead += endPos - beginPos;
        } while (bytesAlreadyRead < header->message_length);
    }

    void VariableBodyRtmpPacket::writeVariableData(SimpleBuffer *sb) {
        if (!_datas.empty()) {
            for (auto it = _datas.begin(); it != _datas.end(); it++) {
                (*it)->write(sb);
            }
        } else {
            // Write a null
            Amf0Null amf0Null;
            amf0Null.write(sb);
        }
    }

    std::shared_ptr<Amf0Data> VariableBodyRtmpPacket::getDataAt(int index) {
        if (index < _datas.size()) {
            return _datas[index];
        }

        return nullptr;
    }

    void VariableBodyRtmpPacket::addString(const std::string &s) {
        _datas.push_back(std::shared_ptr<Amf0Data>(new Amf0String(s)));
    }

    void VariableBodyRtmpPacket::addNumber(double n) {
        _datas.push_back(std::shared_ptr<Amf0Data>(new Amf0Number(n)));
    }

    void VariableBodyRtmpPacket::addBoolean(bool b) {
        _datas.push_back(std::shared_ptr<Amf0Data>(new Amf0Boolean(b)));
    }

    void VariableBodyRtmpPacket::addData(std::shared_ptr<Amf0Data> d) {
        _datas.push_back(d);
    }
}
