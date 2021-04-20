#ifndef SIMPLE_VECTOR_H
#define SIMPLE_VECTOR_H

#include <cstdint>
#include <utility>
#include <memory>
#include <iterator>
#include <algorithm>

template <typename T>
class SimpleVector {
 public:
    SimpleVector() = default;

    explicit SimpleVector(size_t size)
        : _size(size), _capacity(size)
        , _begin(std::make_unique<T[]>(size)) {}

    SimpleVector(const SimpleVector& other)
        : _size(other._size), _capacity(other._capacity)
        , _begin(std::make_unique<T[]>(other._capacity)) {
            std::copy(other.begin(), other.end(), _begin);
    }

    SimpleVector& operator =(const SimpleVector& rhs) {
        if (rhs._size <= _capacity) {
            std::copy(rhs.begin(), rhs.end(), begin());
            _size = rhs._size;
        } else {
            SimpleVector<T> tmp(rhs);
            std::swap(tmp._begin, _begin);
            std::swap(tmp._size, _size);
            std::swap(tmp._capacity, _capacity);
        }
        return *this;
    }

    SimpleVector(SimpleVector&& other)
        : _size(other._size), _capacity(other._capacity)
        , _begin(std::move(other._begin)) {
            other._capacity = other._size = 0u;
    }

    SimpleVector& operator =(SimpleVector&& rhs) {
        _size     = std::exchange(rhs._size, 0ul);
        _capacity = std::exchange(rhs._capacity, 0ul);
        _begin    = std::exchange(rhs._begin, nullptr);
        return *this;
    }

    ~SimpleVector() = default;

 public:
    T& operator[](size_t index) { return _begin[index]; }
    const T& operator[](size_t index) const { return _begin[index]; }

    auto begin() { return _begin.get(); }
    auto end()   { return _begin.get() + _size; }
    const auto begin() const { return _begin.get(); }
    const auto end()   const { return _begin.get() + _size; }

    auto rbegin() { return std::make_reverse_iterator(end()); }
    auto rend()   { return std::make_reverse_iterator(begin()); }
    const auto rbegin() const { return std::make_reverse_iterator(end()); }
    const auto rend()   const { return std::make_reverse_iterator(begin()); }

    T& front() { return *begin(); }
    T& back()  { return (_size == 0u) ? front() : *std::prev(end()); }
    const T& front() const { return *_begin; }
    const T& back()  const { return (_size == 0u) ? front() : *std::prev(end()); }

    size_t size()     const { return _size; }
    size_t capacity() const { return _capacity; }

 public:
    bool operator==(const SimpleVector& other) const {
        return (size() == other.size() &&
                std::equal(begin(), end(), other.begin()));
    }

    auto operator<=>(const SimpleVector& other) const {
        return std::lexicographical_compare_three_way(begin(), end(),
                    other.begin(), other.end());
    }

 private:
    void expand_if_needed() {
        if (_size >= _capacity) {
            size_t new_capacity = (_capacity == 0 ? 1 : 2 * _capacity);
            std::unique_ptr new_begin = std::make_unique<T[]>(new_capacity);
            std::move(begin(), end(), new_begin.get());
            _begin    = std::move(new_begin);
            _capacity = new_capacity;
        }
    }

 public:
    void push_back(T value) {
        expand_if_needed();
        _begin[_size++] = std::move(value);
    }

    void pop_back() {
        if (_size == 0ul)
            return;
        _size--;
    }

 private:
    size_t _size     = 0ul;
    size_t _capacity = 0ul;
    std::unique_ptr<T[]> _begin = nullptr;
};

#endif  // SIMPLE_VECTOR_H
