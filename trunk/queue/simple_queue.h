#ifndef TRUNK_SIMPLE_QUEUE_H
#define TRUNK_SIMPLE_QUEUE_H

#include <queue>
#include <mutex>

template <class T>
class SimpleQueue {
public:
//    SimpleQueue() {};
//    virtual ~SimpleQueue() {};

public:
    void enqueue(T t);
    T dequeue();

private:
    std::queue<T> _queue;
    std::mutex _mutex;
};

template <class T>
void SimpleQueue<T>::enqueue(T t)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _queue.push(t);
}

template <class T>
T SimpleQueue<T>::dequeue()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_queue.empty()) {
        return nullptr;
    }

    T t = _queue.front();
    _queue.pop();

    return t;
}


#endif //TRUNK_SIMPLE_QUEUE_H
