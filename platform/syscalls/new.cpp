#include <cstddef>
#include <new>
#include <rtthread.h>
#include <rthw.h>

void* operator new(size_t size) {
    void* p = rt_malloc(size);
    if (!p){
			// failed to do
		}
    return p;
}

void operator delete(void* p) noexcept {
    if (p) rt_free(p);
}

// array
void* operator new[](size_t size) {
    return operator new(size);
}

void operator delete[](void* p) noexcept {
    operator delete(p);
}

// nothrow
void* operator new(size_t size, const std::nothrow_t&) noexcept {
    return rt_malloc(size);
}

void operator delete(void* p, const std::nothrow_t&) noexcept {
    rt_free(p);
}