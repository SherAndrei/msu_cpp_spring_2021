#ifndef MY_ALLOCATOR
#define MY_ALLOCATOR
#include <memory>

class Allocator {
 public:
    Allocator() = default;
    explicit Allocator(size_t maxSize);

    Allocator(Allocator&) = delete;
    Allocator& operator=(const Allocator&) = delete;

    ~Allocator() = default;

 public:
    void makeAllocator(size_t maxSize);
    char* alloc(size_t size);
    size_t max_size() const;
    void reset();

 private:
    std::unique_ptr<char> _mem = nullptr;
    size_t _maxSize = 0u;
    char* _offset = nullptr;
};

#endif  // MY_ALLOCATOR
