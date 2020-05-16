#ifndef TRUNK_VARIABLEBODYRTMPPACKET_H
#define TRUNK_VARIABLEBODYRTMPPACKET_H

#include <vector>
#include <memory>
#include "RtmpPacket.h"
#include "../amf/amf0/amf0.h"

namespace akanchi {
    class VariableBodyRtmpPacket : public RtmpPacket {
    public:
        VariableBodyRtmpPacket();

        virtual ~VariableBodyRtmpPacket();

    protected:
        virtual void readVariableData(SimpleBuffer *sb, int bytesAlreadyRead);

        virtual void writeVariableData(SimpleBuffer *sb);

    public:
        std::shared_ptr<Amf0Data> getDataAt(int index);

        void addString(const std::string &s);

        void addNumber(double n);

        void addBoolean(bool b);

        void addData(std::shared_ptr<Amf0Data> d);

    private:
        std::vector<std::shared_ptr<Amf0Data>> _datas;
    };
}

#endif //TRUNK_VARIABLEBODYRTMPPACKET_H
