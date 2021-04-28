#ifndef SIMPLE_VECTOR_H
#define SIMPLE_VECTOR_H

#include <cstdint>
#include <utility>
#include <memory>
#include <iterator>
#include <algorithm>
#include <ranges>

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
            std::ranges::copy(other, *this);
    }

    SimpleVector& operator =(const SimpleVector& rhs) {
        SimpleVector<T> tmp(rhs);
        this->swap(tmp);
        return *this;
    }

    SimpleVector(SimpleVector&& other) noexcept
        : _size(other._size), _capacity(other._capacity)
        , _begin(std::move(other._begin)) {
            other._capacity = other._size = 0u;
    }

    SimpleVector& operator =(SimpleVector&& rhs) noexcept {
        SimpleVector<T> tmp(std::move(rhs));
        this->swap(tmp);
        return *this;
    }

    ~SimpleVector() noexcept = default;

 public:
    T& operator[](size_t index) noexcept { return _begin[index]; }
    const T& operator[](size_t index) const noexcept { return _begin[index]; }

    auto begin() noexcept { return data(); }
    auto end()   noexcept { return data() + _size; }
    const auto begin() const noexcept { return data(); }
    const auto end()   const noexcept { return data() + _size; }

    auto rbegin() noexcept { return std::make_reverse_iterator(end()); }
    auto rend()   noexcept { return std::make_reverse_iterator(begin()); }
    const auto rbegin() const noexcept { return std::make_reverse_iterator(end()); }
    const auto rend()   const noexcept { return std::make_reverse_iterator(begin()); }

    T& front() noexcept { return *begin(); }
    T& back()  noexcept { return *std::prev(end()); }
    const T& front() const noexcept { return *begin(); }
    const T& back()  const noexcept { return *std::prev(end()); }

    T* data() noexcept { return _begin.get(); }
    const T* data() const noexcept { return _begin.get(); }

    size_t size()     const noexcept { return _size; }
    size_t capacity() const noexcept { return _capacity; }

    bool empty() const noexcept { return begin() == end(); }

 private:
    void expand_if_needed() {
        if (_size >= _capacity) {
            size_t new_capacity = (_capacity == 0 ? 1 : 2 * _capacity);
            std::unique_ptr new_begin = std::make_unique<T[]>(new_capacity);
            std::ranges::move(*this, new_begin.get());
            _begin    = std::move(new_begin);
            _capacity = new_capacity;
        }
    }

 public:
    void swap(SimpleVector& other) noexcept {
        std::swap(other._begin, _begin);
        std::swap(other._size, _size);
        std::swap(other._capacity, _capacity);
    }

    void push_back(T value) {
        expand_if_needed();
        _begin[_size++] = std::move(value);
    }

    void pop_back() noexcept {
        if (empty()) return;
        _size--;
    }

 private:
    size_t _size     = 0ul;
    size_t _capacity = 0ul;
    std::unique_ptr<T[]> _begin = nullptr;
};

template<typename T>
inline bool operator==(const SimpleVector<T>& lhs, const SimpleVector<T>& rhs) {
    return std::ranges::equal(lhs, rhs);
}

template<typename T>
inline auto operator<=>(const SimpleVector<T>& lhs, const SimpleVector<T>& rhs) {
    return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(),
                rhs.begin(), rhs.end());
}


#endif  // SIMPLE_VECTOR_H
