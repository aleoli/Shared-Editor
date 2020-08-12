#pragma once

#include <unordered_map>
#include <queue>
#include <shared_mutex>

template <typename T, typename U>
class FifoMap {
public:
  FifoMap() : _size(_map.max_size()) {}
  explicit FifoMap(unsigned int size) : _size(size) {}

  unsigned int size() { return _map.size(); }

  bool empty() { return _map.empty(); }

  unsigned int max_size() { return _size; }

  void clear() {
    _map.clear();

    std::queue<T> q;
    _queue.swap(q);
  }

  void insert(const T& key, const U& value) {
    if(count(key) > 0) return;

    if(_map.size() == _size) {
      //remove an element
      auto oldest_key = _queue.front();
      _queue.pop();
      _map.erase(oldest_key);
    }

    _map.insert({key, value});
    _queue.push(key);
  }

  U& at(const T& key) {
    return _map.at(key);
  }

  U& operator[](const T& key) {
    return _map[key];
  }

  int count(const T& key) {
    return _map.count(key);
  }

  auto begin() { return _map.begin(); }
  auto cbegin() { return _map.cbegin(); }
  auto end() { return _map.end(); }
  auto cend() { return _map.cend(); }

  std::shared_mutex& getMutex() {
    return this->_mutex;
  }

private:
  unsigned int _size;
  std::unordered_map<T, U> _map;
  std::queue<T> _queue;
  std::shared_mutex _mutex;
};
