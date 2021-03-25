#include "allocator.h"

Allocator::Allocator(size_t maxSize) {
    makeAllocator(maxSize);
}

void Allocator::makeAllocator(size_t maxSize) {
    _mem =  std::make_unique<char[]>(maxSize);
    _offset  = _mem.get();
    _maxSize = maxSize;
}

char* Allocator::alloc(size_t size) {
    if (_offset + size > _mem.get() + _maxSize) {
        return nullptr;
    }
    char* ptr = _offset;
    _offset += size;
    return ptr;
}

size_t Allocator::max_size() const {
    return _maxSize;
}

void Allocator::reset() {
    _offset = _mem.get();
}
