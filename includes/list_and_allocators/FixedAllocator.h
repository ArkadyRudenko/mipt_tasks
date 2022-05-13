#include <memory>
#include <vector>

template<size_t chunkSize>
class FixedAllocator {
private:
    std::vector<std::byte*> pools_;
    size_t offset_ = 0;
    static const size_t SizeOfPool = 100000 * chunkSize;
public:
    FixedAllocator() {
        pools_.push_back((reinterpret_cast<std::byte*>(
                                ::operator new(SizeOfPool))));
    }

    FixedAllocator(const FixedAllocator& other) = default;

    void* allocate(size_t) {
        if (offset_ + chunkSize < SizeOfPool) {
            offset_ += chunkSize;
            return pools_.back() + offset_;
        } else {
            pools_.push_back((reinterpret_cast<std::byte*>(
                    ::operator new(SizeOfPool))));
            offset_ = 0;
            return pools_.back() + offset_;
        }
    }

    void deallocate(void* ptr) {}

    ~FixedAllocator() {
        for (const auto pool: pools_) {
            ::operator delete(pool);
        }
    }
};