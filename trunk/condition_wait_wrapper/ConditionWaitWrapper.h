#ifndef TRUNK_CONDITIONWAITWRAPPER_H
#define TRUNK_CONDITIONWAITWRAPPER_H

#include <mutex>
#include <condition_variable>

namespace akanchi {
    class ConditionWaitWrapper {
    public:
        ConditionWaitWrapper();
        virtual ~ConditionWaitWrapper();

    public:
        bool wait_for(int sec);
        bool get_condition();
        void set_condition(bool condition);

    private:
        bool _condition;
        std::mutex _mutex;
        std::condition_variable _cv;
    };
}

#endif //TRUNK_CONDITIONWAITWRAPPER_H
