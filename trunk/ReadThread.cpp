#include "ReadThread.h"

namespace akanchi {
    ReadThread::ReadThread()
        : ThreadWrapper() {
        _is_exit = false;
    }
    void ReadThread::setRunFunc(std::function<void()> const &func) {
        this->_func = func;
    }

    void ReadThread::Start(bool background) {
        _is_exit = false;
        ThreadWrapper::Start(background);
    }

    void ReadThread::stop() {
        _is_exit = true;
        Join();
    }

    void ReadThread::Body() {
        while (!_is_exit) {
            this->SyncPoint();
            if (this->_func) {
                this->_func();
            }
        }
    }
}
