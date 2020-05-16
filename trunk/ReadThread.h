#ifndef TRUNK_READTHREAD_H
#define TRUNK_READTHREAD_H

#include <thread>
#include <functional>

#include "thread_wrapper/ThreadWrapper.h"

namespace akanchi {
    class ReadThread : public ThreadWrapper {
    public:
        ReadThread();
        void setRunFunc(std::function<void()> const &func);
        void Start(bool background = false) override ;

        void stop();

    protected:
        void Body() override;

    private:
        std::atomic<bool> _is_exit;
        std::function<void()> _func;
    };
}

#endif //TRUNK_READTHREAD_H
