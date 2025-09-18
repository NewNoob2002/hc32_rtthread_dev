#include <cstddef>
#include <new>
#include <rtthread.h>
#include <rthw.h>

void* operator new(std::size_t size) {
    void* ptr = rt_malloc(size);
    if (ptr == nullptr) {
        while(1) {
				}
    }
    return ptr;
}

void operator delete(void* ptr) noexcept {
    rt_free(ptr);
}

void* operator new[](std::size_t size) {
    return operator new(size);
}
void operator delete[](void* ptr) noexcept {
    operator delete(ptr);
}