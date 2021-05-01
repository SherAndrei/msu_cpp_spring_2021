#ifndef VECTOR_H
#define VECTOR_H

#include <memory>
#include <iterator>

#include "Allocator.h"
#include "Iterator.h"

template <typename T, typename Alloc = Allocator<T>>
class Vector {
 public:
    using value_type = T;
    using allocator_type = Alloc;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    using reference = value_type&;
    using const_reference = const value_type&;

    using pointer = std::allocator_traits<Allocator<T>>::pointer;
    using const_pointer = std::allocator_traits<Allocator<T>>::const_pointer;

    using iterator = T*;
    using const_iterator = const T*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

 public:
    explicit Vector(const Alloc& alloc = Allocator<T>());
    explicit Vector(size_t size, const Alloc& alc = Allocator<T>());
    Vector(size_t size, const T& default_value, const Alloc& alloc = Allocator<T>());

    Vector(std::initializer_list<T> list, const Alloc& alloc = Allocator<T>());
    Vector& operator=(std::initializer_list<T> list);

    Vector(const Vector& other);
    Vector& operator =(const Vector& rhs);

    Vector(Vector&& other) noexcept;
    Vector& operator =(Vector&& rhs) noexcept;

    ~Vector() noexcept;

 public:
    reference       operator[](size_t index)       noexcept { return _begin[index]; }
    const_reference operator[](size_t index) const noexcept { return _begin[index]; }

    iterator       begin()       noexcept { return _begin; }
    iterator       end()         noexcept { return _begin + _size; }
    const_iterator begin() const noexcept { return _begin; }
    const_iterator end()   const noexcept { return _begin + _size; }

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

    void push_back(value_type value);
    void pop_back() noexcept;

 private:
    void expand_if_needed();

 private:
    Alloc     _alloc;
    size_type _size     = 0ul;
    size_type _capacity = 0ul;
    pointer _begin = nullptr;
};

template<typename T, typename Alloc>
inline bool operator==(const Vector<T, Alloc>& lhs, const Vector<T, Alloc>& rhs);

template<typename T, typename Alloc>
inline auto operator<=>(const Vector<T, Alloc>& lhs, const Vector<T, Alloc>& rhs);

#include "Vector.tcc"

#endif  // VECTOR_H
