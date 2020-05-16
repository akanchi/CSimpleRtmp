#include "ContentData.h"

namespace akanchi {
    ContentData::ContentData()
            : RtmpPacket() {

    }

    ContentData::~ContentData() {

    }

    void ContentData::read(SimpleBuffer *sb) {

    }

    void ContentData::set_data(char *data, uint32_t size) {
        this->_data = data;
        this->_size = size;
    }

    char *ContentData::array() {
        return this->_data;
    }

    uint32_t ContentData::size() {
        return this->_size;
    }
}