#include <memory>
#include <vector>

template<size_t chunkSize>
class FixedAllocator {
public:
    FixedAllocator() {
        pools_.push_back((reinterpret_cast<byte*>(
                                ::operator new(SizeOfPool))));
    }

    FixedAllocator(const FixedAllocator& other) // TODO
            : pools_(other.pools_), offset_(other.offset_) {}

    void* allocate(size_t) {
        if (offset_ + chunkSize < SizeOfPool) {
            offset_ += chunkSize;
            return pools_.back() + offset_;
        } else {
            pools_.push_back((reinterpret_cast<byte*>(
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

private:
    std::vector<std::byte*> pools_;
    size_t offset_ = 0;
    static const size_t SizeOfPool = 100000 * chunkSize;
};