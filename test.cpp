#include <iostream>
#include <string>
#include <cassert>
#include "07/Allocator.h"

struct Checker {
    Checker() {
        std::cout << '+' << std::endl;
    }
    ~Checker() {
        std::cout << '-' << std::endl;
    }
};

int main() {
    // {
    //     Allocator<Checker> al;
    //     Checker* p = al.allocate(3);
    //     construct(p, p + 3);
    //     destroy(p, p + 3);
    //     al.deallocate(p, 3);
    // }
    {
        Allocator<std::string> al;
        size_t size = 1;
        std::string* p = al.allocate(size);
        construct(p, p + size, "hello");
        assert(*p == "hello");
        destroy(p, p + size);
        al.deallocate(p, size);
    }
}