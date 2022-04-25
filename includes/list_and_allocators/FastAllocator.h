#pragma once

#include <algorithm>
#include <unordered_set>

#include "FixedAllocator.h"

template<typename T>
class FastAllocator {
private:
    // 48 bytes for string, 24 for int
    static const size_t chunkSize = 48;
    std::unordered_set<void*> fixed_chunks_;
    FixedAllocator<chunkSize> fixedAllocator_;
public:
    FastAllocator() noexcept = default;

    FastAllocator(const FastAllocator& other) noexcept
    : fixedAllocator_(other.fixedAllocator_)
    {
    }

    template<typename U>
    FastAllocator(const FastAllocator<U>& other) noexcept {}

    using pointer = T*;
    using value_type = T;

    template<class U>
    struct rebind {
        using other = FastAllocator<U>;
    };

public:
    T* allocate(size_t n) {
        if (n * sizeof(T) == chunkSize) {
            auto it = fixed_chunks_.insert(fixedAllocator_.allocate(n * sizeof(T)));
            return reinterpret_cast<T*>(*it.first);
        } else {
            return reinterpret_cast<T*>(::operator new(n * sizeof(T)));
        }
    }

    void deallocate(T* ptr, size_t) {
        if(std::find(fixed_chunks_.begin(), fixed_chunks_.end(), ptr) != fixed_chunks_.end()) {
            fixedAllocator_.deallocate(ptr);
        } else {
            ::operator delete(ptr);
        }
    }

    template<class... Args>
    void construct(T* ptr, const Args&& ... args) /* without move */ {
        ::new(ptr) T(args...);
    }

    void destroy(T* ptr) {
        ptr->~T();
    }
};


