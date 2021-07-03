#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <utility>
#include <type_traits>
#include <exception>

namespace alloc {

template<typename T>
class Allocator {
 public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

 public:
    Allocator() noexcept {};

    template<typename U>
    Allocator(const Allocator<U>&) noexcept {}

    Allocator& operator=(const Allocator&) = default;

    ~Allocator() noexcept {};

 private:
    size_type max_size() const noexcept {
        return std::size_t(-1) / sizeof(T);
    }

 public:
    [[nodiscard]]
    T* allocate(size_t n) {
        if (n > max_size())
            throw std::bad_alloc();
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    void
    deallocate(T* p, size_type t) {
        ::operator delete(p, t * sizeof(T));
    }
};

template<typename T, typename U>
bool operator==(const Allocator<T>&, const Allocator<U>&) noexcept {
    return true;
}


template<class T, class... Args>
auto construct(T* location, Args&&... args) {
    return ::new ((void*)location) T(std::forward<Args>(args)...);
}

template<class InputIt, class... Args>
auto construct(InputIt pos, Args&&... args) {
    return construct(pos.base(), std::forward<Args>(args)...);
}

template<class InputIt, class... Args>
void construct(InputIt begin, InputIt end, Args&&... args) {
    while (begin != end) construct(begin++, std::forward<Args>(args)...);
}

template<class InputIt, class OutputIt>
void construct_from(InputIt begin, InputIt end, OutputIt to) {
    for (; begin != end; begin++, to++) construct(to, *begin);
}

template <class T>
void destroy(T* location) {
    location->~T();
}

template <class InputIt>
void destroy(InputIt pos) {
    destroy(pos.base());
}

template<class InputIt>
void destroy(InputIt begin, InputIt end) {
    while (begin != end) destroy(begin++);
}

}  // namespace alloc

#endif  // ALLOCATOR_H
