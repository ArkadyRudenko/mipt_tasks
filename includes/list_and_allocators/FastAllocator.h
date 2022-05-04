#pragma once

#include <algorithm>
#include <vector>

#include "FixedAllocator.h"

template<typename T>
class FastAllocator {
private:
    // 48 bytes for string, 24 for int
    static const size_t chunkSize = 48;
    std::vector<void*> free_data_;
    std::vector<void*> my_data_;
    std::shared_ptr<FixedAllocator<chunkSize>> fixed_allocator_;
public:
    FastAllocator() noexcept : fixed_allocator_(make_shared<FixedAllocator<chunkSize>>()) {};

    FastAllocator(const FastAllocator& other) noexcept : fixed_allocator_(other.fixed_allocator_) {}

    template<typename U>
    FastAllocator(const FastAllocator<U>& other) noexcept : fixed_allocator_(make_shared<
            FixedAllocator<chunkSize>>()) {}

    using pointer = T*;
    using value_type = T;

    template<class U>
    struct rebind {
        using other = FastAllocator<U>;
    };

    template<typename U>
    using rebind_alloc = allocator<U>;

public:
    T* allocate(size_t n) {
        if (n * sizeof(T) == chunkSize) {
            if (!free_data_.empty()) {
                void* ptr = free_data_.back();
                free_data_.pop_back();
                return reinterpret_cast<T*>(ptr);
            }
            return reinterpret_cast<T*>(fixed_allocator_->allocate(n * sizeof(T)));
        } else {
            my_data_.push_back(::operator new(n * sizeof(T)));
            return reinterpret_cast<T*>(my_data_.back());
        }
    }

    void deallocate(T* ptr, size_t) {
        free_data_.push_back(ptr);
    }

    template<typename... Args>
    void construct(T* ptr, const Args& ... args) /* without move */ {
        ::new(ptr) T(args...);
    }

    void destroy(T* ptr) {
        ptr->~T();
    }

    ~FastAllocator() {
        for (const auto ptr: my_data_) {
            ::operator delete(ptr);
        }
    }

    FastAllocator<T> select_on_container_copy_construction() const {
        return FastAllocator<T>(*this);
    }
};


