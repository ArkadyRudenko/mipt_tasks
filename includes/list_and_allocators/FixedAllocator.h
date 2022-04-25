#include <unordered_set>
#include <memory>

template<size_t chunkSize>
class FixedAllocator {
public:
    FixedAllocator() : pool_(
            reinterpret_cast<byte*>(
                    ::operator new(SizeOfPool))) {}

    FixedAllocator(const FixedAllocator& other)
    : pool_(other.pool_), offset_(other.offset_), free_ptrs_(other.free_ptrs_)
    {}

    void* allocate(size_t) {
        if(!free_ptrs_.empty()) {
            void* free_ptr = *free_ptrs_.begin();
            free_ptrs_.erase(free_ptrs_.begin());
            return free_ptr;
        } else if(offset_ + chunkSize <= SizeOfPool) {
//            cout << "Allocated\n";
            offset_ += chunkSize;
            return pool_ + (offset_ - chunkSize);
        } else {
            throw bad_alloc();
        }
    }

    void* deallocate(void* ptr) {
        free_ptrs_.insert(ptr);
    }

    ~FixedAllocator() {
        ::operator delete(pool_);
    }

private:
    std::byte* pool_;
    size_t offset_ = 0;
    std::unordered_set<void*> free_ptrs_;
    static const size_t SizeOfPool = 1000000 * chunkSize;
};