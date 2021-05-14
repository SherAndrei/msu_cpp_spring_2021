#ifndef ITERATOR_H
#define ITERATOR_H

#include <iterator>

template<typename Iter>
class Iterator : public std::iterator<
    typename std::iterator_traits<Iter>::iterator_category,
    typename std::iterator_traits<Iter>::value_type,
    typename std::iterator_traits<Iter>::difference_type,
    typename std::iterator_traits<Iter>::pointer,
    typename std::iterator_traits<Iter>::reference
> {
 protected:
    Iter _current;

 public:
    using iterator_type     = Iter;
    using iterator_category = typename std::iterator_traits<Iter>::iterator_category;
    using value_type        = typename std::iterator_traits<Iter>::value_type;
    using difference_type   = typename std::iterator_traits<Iter>::difference_type;
    using reference         = typename std::iterator_traits<Iter>::reference;
    using pointer           = typename std::iterator_traits<Iter>::pointer;

 public:
    Iterator() noexcept
      : _current(Iter()) { }

    explicit Iterator(const Iter& i) noexcept
      : _current(i) { }

    template<typename It>
    explicit Iterator(const Iterator<It> i) noexcept
        : _current(i.base()) { }


    reference operator*() const noexcept { return *_current; }
    pointer operator->() const noexcept { return _current; }

    Iterator& operator++() noexcept { ++_current; return *this; }
    Iterator& operator--() noexcept { --_current; return *this; }

    Iterator operator++(int) noexcept { return Iterator(_current++); }
    Iterator operator--(int) noexcept { return Iterator(_current--); }

    reference operator[](difference_type n) const noexcept { return _current[n]; }

    Iterator& operator+=(difference_type n) noexcept { _current += n; return *this; }
    Iterator& operator-=(difference_type n) noexcept { _current -= n; return *this; }

    Iterator operator+(difference_type n) const noexcept { return Iterator(_current + n); }
    Iterator operator-(difference_type n) const noexcept { return Iterator(_current - n); }

    const Iter& base() const noexcept { return _current; }
};


template<typename IteratorL, typename IteratorR>
bool operator==(const Iterator<IteratorL>& lhs,
                const Iterator<IteratorR>& rhs)
    { return lhs.base() == rhs.base(); }

template<typename IteratorL, typename IteratorR>
bool operator!=(const Iterator<IteratorL>& lhs,
                const Iterator<IteratorR>& rhs)
    { return !(lhs == rhs); }

template<typename IteratorL, typename IteratorR>
bool operator<(const Iterator<IteratorL>& lhs,
                const Iterator<IteratorR>& rhs)
    { return lhs.base() < rhs.base(); }

template<typename IteratorL, typename IteratorR>
bool operator>(const Iterator<IteratorL>& lhs,
                const Iterator<IteratorR>& rhs)
    { return rhs < lhs; }

template<typename IteratorL, typename IteratorR>
bool operator<=(const Iterator<IteratorL>& lhs,
                const Iterator<IteratorR>& rhs)
    { return !(lhs > rhs); }

template<typename IteratorL, typename IteratorR>
bool operator>=(const Iterator<IteratorL>& lhs,
                const Iterator<IteratorR>& rhs)
    { return !(lhs < rhs); }

template<typename IteratorL, typename IteratorR>
inline auto operator-(const Iterator<IteratorL>& lhs,
                      const Iterator<IteratorR>& rhs) noexcept
    -> decltype(lhs.base() - rhs.base())
    { return lhs.base() - rhs.base(); }

template<typename Iter>
inline typename Iterator<Iter>::difference_type
operator-(const Iterator<Iter>& lhs,
          const Iterator<Iter>& rhs) noexcept
    { return lhs.base() - rhs.base(); }

template<typename Iter>
inline Iterator<Iter>
operator+(typename Iterator<Iter>::difference_type n,
          const Iterator<Iter>& i) noexcept
    { return Iterator<Iter>(i.base() + n); }


#endif  // ITERATOR_H
