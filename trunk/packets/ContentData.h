#ifndef TRUNK_CONTENTDATA_H
#define TRUNK_CONTENTDATA_H

#include "RtmpPacket.h"

namespace akanchi {
    class ContentData : public RtmpPacket {
    public:
        ContentData();

        virtual ~ContentData();

    public:
        void read(SimpleBuffer *sb);

        void set_data(char *data, uint32_t size);

        char *array();

        uint32_t size();

    private:
        char *_data;
        uint32_t _size;
    };
}

#endif //TRUNK_CONTENTDATA_H
