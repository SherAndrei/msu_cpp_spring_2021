#ifndef VECTOR_H
#define VECTOR_H

#include <memory>

#include "Allocator.h"
#include "Iterator.h"

template <typename T, typename Alloc = alloc::Allocator<T>>
class Vector {
 public:
    using value_type = T;
    using allocator_type = Alloc;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    using reference = value_type&;
    using const_reference = const value_type&;

    using pointer = typename std::allocator_traits<allocator_type>::pointer;
    using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;

    using iterator = Iterator<value_type*>;
    using const_iterator = Iterator<const value_type*>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

 public:
    explicit Vector(const Alloc& alloc = alloc::Allocator<T>());
    explicit Vector(size_t size, const Alloc& alc = alloc::Allocator<T>());
    Vector(size_t size, const T& default_value, const Alloc& alloc = alloc::Allocator<T>());

    Vector(std::initializer_list<T> list, const Alloc& alloc = alloc::Allocator<T>());
    Vector& operator=(std::initializer_list<T> list);

    Vector(const Vector& other);
    Vector& operator =(const Vector& rhs);

    Vector(Vector&& other) noexcept;
    Vector& operator =(Vector&& rhs) noexcept;

    ~Vector() noexcept;

 public:
    reference       operator[](size_t index)       noexcept { return _begin[index]; }
    const_reference operator[](size_t index) const noexcept { return _begin[index]; }

    iterator       begin()       noexcept { return iterator(_begin); }
    iterator       end()         noexcept { return iterator(_begin + _size); }
    const_iterator begin() const noexcept { return const_iterator(_begin); }
    const_iterator end()   const noexcept { return const_iterator(_begin + _size); }

    reverse_iterator       rbegin()       noexcept { return std::make_reverse_iterator(end()); }
    reverse_iterator       rend()         noexcept { return std::make_reverse_iterator(begin()); }
    const_reverse_iterator rbegin() const noexcept { return std::make_reverse_iterator(end()); }
    const_reverse_iterator rend()   const noexcept { return std::make_reverse_iterator(begin()); }

    reference       front()       noexcept { return *begin(); }
    reference       back()        noexcept { return *std::prev(end()); }
    const_reference front() const noexcept { return *begin(); }
    const_reference back()  const noexcept { return *std::prev(end()); }

    pointer       data()       noexcept { return _begin; }
    const_pointer data() const noexcept { return _begin; }

    size_type size()     const noexcept { return _size; }
    size_type capacity() const noexcept { return _capacity; }

    bool empty() const noexcept { return begin() == end(); }

 public:
    void swap(Vector& other) noexcept;
    void clear() noexcept;

    void reserve(size_type new_cap);
    void resize(size_type count);

    void push_back(value_type value);
    void pop_back() noexcept;

    template<class... Args>
    reference emplace_back(Args&&... args);

 private:
    Alloc     _alloc;
    size_type _size     = 0ul;
    size_type _capacity = 0ul;
    pointer _begin = nullptr;
};

template<typename T, typename Alloc>
inline bool operator==(const Vector<T, Alloc>& lhs, const Vector<T, Alloc>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
template<typename T, typename Alloc>
inline bool operator!=(const Vector<T, Alloc>& lhs, const Vector<T, Alloc>& rhs) {
    return !(lhs == rhs);
}
template<typename T, typename Alloc>
inline bool operator< (const Vector<T, Alloc>& lhs, const Vector<T, Alloc>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                        rhs.begin(), rhs.end());
}
template<typename T, typename Alloc>
inline bool operator> (const Vector<T, Alloc>& lhs, const Vector<T, Alloc>& rhs) {
    return rhs < lhs;
}
template<typename T, typename Alloc>
inline bool operator<=(const Vector<T, Alloc>& lhs, const Vector<T, Alloc>& rhs) {
    return !(lhs > rhs);
}
template<typename T, typename Alloc>
inline bool operator>=(const Vector<T, Alloc>& lhs, const Vector<T, Alloc>& rhs) {
    return !(lhs < rhs);
}


#include "Vector.tcc"

#endif  // VECTOR_H
