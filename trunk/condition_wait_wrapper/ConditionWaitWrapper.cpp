#include "ConditionWaitWrapper.h"

namespace akanchi {
    ConditionWaitWrapper::ConditionWaitWrapper()
        : _condition(false) {

    }

    ConditionWaitWrapper::~ConditionWaitWrapper() {

    }

    bool ConditionWaitWrapper::wait_for(int sec) {
        std::unique_lock<std::mutex> ul(_mutex);
        _cv.wait_for(ul, std::chrono::seconds(sec), [this] { return _condition; });

        return _condition;
    }

    bool ConditionWaitWrapper::get_condition() {
        std::lock_guard<std::mutex> lg(_mutex);
        return _condition;
    }

    void ConditionWaitWrapper::set_condition(bool condition) {
        {
            std::lock_guard<std::mutex> lg(_mutex);
            _condition = condition;
        }
        _cv.notify_all();
    }
}
