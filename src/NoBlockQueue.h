#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>

template <typename T>
class NoBlockQueue {
public:
    NoBlockQueue() {}

    unsigned int size() {
        std::lock_guard<std::mutex> guard(_m);
        return _queue.size();
    }

    bool empty() {
        std::lock_guard<std::mutex> guard(_m);
        return _queue.empty();
    }

    void clear() {
      std::lock_guard<std::mutex> guard(_m);

      std::queue<T> q;
      _queue.swap(q);
    }

    //Push: no limits of size
    void push(const T& elem) {
        std::lock_guard<std::mutex> guard(_m);

        _queue.push(elem);
    }

    T pop() {
        std::lock_guard<std::mutex> guard(_m);

        if(_queue.empty()) {
          return T{};
        }

        auto elem = _queue.front();
        _queue.pop();

        return elem;
    }

private:
    std::mutex _m;
    std::queue<T> _queue;
};