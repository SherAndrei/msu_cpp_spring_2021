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
    destroy(begin(), end());
    _alloc.deallocate(_begin, _size);
}

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(size_t size, const Alloc& alloc)
    : _alloc(alloc)
    , _size(size), _capacity(size)
    , _begin(_alloc.allocate(size)) {
    construct(_begin, _begin + _size);
}

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(size_t size, const T& default_value, const Alloc& alloc)
    : _alloc(alloc)
    , _size(size), _capacity(size)
    , _begin(_alloc.allocate(size)) {
    construct(begin(), end(), default_value);
}

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(std::initializer_list<T> ilist, const Alloc& alloc)
    : _alloc(alloc)
    , _size(ilist.size())
    , _capacity(ilist.size())
    , _begin(_alloc.allocate(_capacity)) {
    construct_from(ilist.begin(), ilist.end(), begin());;
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
    construct_from(other.begin(), other.end(), begin());
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

// template<class T>
// Vector<T>& Vector<T>::operator =(Vector&& rhs) noexcept {
//     Vector<T> tmp(std::move(rhs));
//     this->swap(tmp);
//     return *this;
// }

// template<class T>
// void Vector<T>::expand_if_needed() {
//     if (_size >= _capacity) {
//         Vector tmp(2 * _capacity + 1);
//         tmp._size = _size;
//         std::ranges::move(*this, tmp._begin.get());
//         this->swap(tmp);
//     }
// }

template<typename T, typename Alloc>
void Vector<T, Alloc>::swap(Vector& other) noexcept {
    std::swap(other._begin, _begin);
    std::swap(other._size, _size);
    std::swap(other._capacity, _capacity);
}

// template<class T>
// void Vector<T>::push_back(T value) {
//     expand_if_needed();
//     _begin[_size++] = std::move(value);
// }

// template<class T>
// void Vector<T>::pop_back() noexcept {
//     if (empty()) return;
//     _size--;
// }


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
