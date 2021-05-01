#ifndef VECTOR_TCC
#define VECTOR_TCC

#include <cstdint>
#include <utility>
#include <algorithm>
#include <ranges>

#include "Vector.h"

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(const Alloc& alloc)
    : _alloc(alloc) {}

template<typename T, typename Alloc>
Vector<T, Alloc>::~Vector() noexcept {
    alloc::destroy(begin(), end());
    _alloc.deallocate(_begin, _size);
}

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(size_t size, const Alloc& alloc)
    : _alloc(alloc)
    , _size(size), _capacity(size)
    , _begin(_alloc.allocate(size)) {
    alloc::construct(begin(), end());
}

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(size_t size, const T& default_value, const Alloc& alloc)
    : _alloc(alloc)
    , _size(size), _capacity(size)
    , _begin(_alloc.allocate(size)) {
    alloc::construct(begin(), end(), default_value);
}

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(std::initializer_list<T> ilist, const Alloc& alloc)
    : _alloc(alloc)
    , _size(ilist.size())
    , _capacity(ilist.size())
    , _begin(_alloc.allocate(_capacity)) {
    alloc::construct_from(ilist.begin(), ilist.end(), begin());;
}

template<typename T, typename Alloc>
Vector<T, Alloc>& Vector<T, Alloc>::operator=(std::initializer_list<T> ilist) {
    Vector<T> tmp(ilist);
    this->swap(tmp);
    return *this;
}

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(const Vector& other)
    : _size(other._size)
    , _capacity(other._capacity)
    , _begin(_alloc.allocate(other._capacity)) {
    alloc::construct_from(other.begin(), other.end(), begin());
}

template<typename T, typename Alloc>
Vector<T, Alloc>& Vector<T, Alloc>::operator =(const Vector& rhs) {
    if (&rhs != this) {
        Vector<T, Alloc> tmp(rhs);
        this->swap(tmp);
    }
    return *this;
}

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(Vector&& other) noexcept {
    this->swap(other);
}

template<typename T, typename Alloc>
Vector<T, Alloc>& Vector<T, Alloc>::operator =(Vector&& rhs) noexcept {
    Vector<T, Alloc> temp(std::move(rhs));
    this->swap(temp);
    return *this;
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::reserve(size_type new_cap) {
    if (_capacity >= new_cap) return;

    auto new_begin = _alloc.allocate(new_cap);
    alloc::construct_from(
        std::make_move_iterator(begin()),
        std::make_move_iterator(end()),
        new_begin
    );
    _alloc.deallocate(_begin, _capacity);
    _begin    = new_begin;
    _capacity = new_cap;
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::resize(size_type count) {
    if (count == _size) return;

    if (count < _size) {
        alloc::destroy(begin() + count, end());
        _size = count; return;
    }

    if (count <= _capacity) {
        alloc::construct(end(), end() + (count - _size));
        _size = count; return;
    }

    Vector temp;
    temp._size = count;
    temp.reserve(count);
    std::ranges::move(*this, temp.begin());
    alloc::construct(temp.begin() + _size, temp.end());
    this->swap(temp);
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::swap(Vector& other) noexcept {
    std::swap(other._begin, _begin);
    std::swap(other._size, _size);
    std::swap(other._capacity, _capacity);
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::clear() noexcept {
    alloc::destroy(begin(), end());
    _size = 0ul;
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::push_back(T value) {
    if (_size >= _capacity) reserve(2 * _capacity + 1);
    alloc::construct(begin() + _size++, std::move(value));
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::pop_back() noexcept {
    if (empty()) return;
    alloc::destroy(begin() + _size--);
}


template<typename T, typename Alloc>
inline bool operator==(const Vector<T, Alloc>& lhs, const Vector<T, Alloc>& rhs) {
    return std::ranges::equal(lhs, rhs);
}

template<typename T, typename Alloc>
inline auto operator<=>(const Vector<T, Alloc>& lhs, const Vector<T, Alloc>& rhs) {
    return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(),
            rhs.begin(), rhs.end());
}

#endif  // VECTOR_TCC
