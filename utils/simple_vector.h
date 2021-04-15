#include <cstdint>
#include <utility>
#include <memory>
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

    SimpleVector(SimpleVector&& other)
        : _size(other._size), _capacity(other._capacity)
        , _begin(std::move(other._begin)) {
            other._capacity = other._size = 0u;
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

    SimpleVector& operator =(SimpleVector&& rhs) {
        _capacity = std::exchange(rhs._capacity, 0ul);
        _size     = std::exchange(rhs._size, 0ul);
        _begin    = std::exchange(rhs._begin, nullptr);
        return *this;
    }

    ~SimpleVector() = default;

    T& operator[](size_t index) { return _begin[index]; }

    T* begin() { return _begin.get(); }
    T* end()   { return _begin.get() + _size; }

    const T* begin() const { return _begin.get(); }
    const T* end()   const { return _begin.get() + _size; }

    size_t size()     const { return _size; }
    size_t capacity() const { return _capacity; }

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

 private:
    std::unique_ptr<T[]> _begin;
    size_t _size     = 0ul;
    size_t _capacity = 0ul;
};
