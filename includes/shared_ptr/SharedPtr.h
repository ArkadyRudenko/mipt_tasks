

template<typename T>
class SharedPtr {
private:
    struct BaseControlBlock {
        size_t shared_count = 0;
        size_t weak_count = 0;

        virtual void destroy(T* obj) {};
        virtual void deallocate() {};

        virtual ~BaseControlBlock() = default;
    };

    template<typename Deleter> // for simple ptr
    struct ControlBlockRegular : public BaseControlBlock {
        ControlBlockRegular(T* ptr, Deleter del) : ptr(ptr), del(del) {}

        T* ptr = nullptr;
        Deleter del;

        void destroy(T* obj) override {
            del(obj);
        }
    };

    // for makeShared
    struct ControlBlockMakeShared : public BaseControlBlock {
        T* ptr = &object;
        T object;

        template<typename... Args>
        explicit ControlBlockMakeShared(Args&& ... args) : object(std::forward<Args>(args)...) {}

        void destroy(T* obj) override {
            ptr->~T();
        }
    };

    BaseControlBlock* cptr = nullptr;
    T* ptr = nullptr;

    template<typename U>
    friend
    class WeakPtr;

    template<typename U, typename Alloc, typename... Args>
    friend SharedPtr<U>
    allocateShared(const Alloc& alloc, Args&& ... args);

    struct AllocateSharedBase {
        virtual void deallocate(ControlBlockMakeShared* block_ptr) {}
    };

    template<typename Alloc>
    struct AllocateSharedTag : public AllocateSharedBase {
        AllocateSharedTag(Alloc alloc) : alloc(alloc) {}

        Alloc alloc;

        void deallocate(ControlBlockMakeShared* block_ptr) override {
            alloc.deallocate(block_ptr, 1);
        }
    };

    AllocateSharedBase* allocate_shared_tag = nullptr;

    template<typename Alloc, typename... Args>
    SharedPtr(AllocateSharedBase allocate_shared_base, Alloc alloc, Args&& ... args) {
        using RebindAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<ControlBlockMakeShared>;
        RebindAlloc alloc_;
        ControlBlockMakeShared* new_ptr = alloc_.allocate(1);
        alloc_.construct(new_ptr, std::forward<Args>(args)...);
        ptr = &new_ptr->object;
        cptr = static_cast<BaseControlBlock*>(new_ptr);
        allocate_shared_tag = new AllocateSharedTag<RebindAlloc>(alloc_);
        cptr->shared_count = 1;
    }

    struct weak_ptr_tag {
    };

    SharedPtr(weak_ptr_tag, BaseControlBlock* ptr) : cptr(ptr) {}

public:
    // TODO need inheritance for all ctrs
    SharedPtr() = default;

    explicit SharedPtr(T* ptr) : ptr(ptr) {
        cptr = new ControlBlockRegular<std::default_delete<T>>(ptr, std::default_delete<T>());
        cptr->shared_count = 1;
    }

    SharedPtr(const SharedPtr<T>& other) :
            cptr(other.cptr),
            ptr(other.ptr),
            allocate_shared_tag(other.allocate_shared_tag) {
        ++cptr->shared_count;
    }

    SharedPtr(SharedPtr<T>&& other) noexcept: cptr(other.cptr), ptr(other.ptr) {
        other.cptr = other.ptr = nullptr;
    }

    template<typename U>
    SharedPtr<T>& operator=(const SharedPtr<U> other) {
//        if( /* we have only BaseControlBlock*/) {
//
//        }
    }

    template<typename Deleter>
    SharedPtr(T* ptr_, Deleter del) {
        cptr = new ControlBlockRegular<Deleter>(ptr_, del);
        cptr->shared_count = 1;
    }

    template<typename Deleter, typename Alloc>
    SharedPtr(T* ptr_, Deleter del, Alloc alloc) {
        cptr = new ControlBlockRegular<Deleter>(ptr_, del);
        cptr->shared_count = 1;
    }

    template<typename Y>
    void reset(Y* ptr) {

    }

    T& operator*() const {
        return *static_cast<ControlBlockRegular<int>*>(cptr)->ptr; // TODO
    }

    T* operator->() const {
        return static_cast<ControlBlockRegular<int>*>(cptr)->ptr; // TODO
    }

    size_t use_count() const {
        return cptr->shared_count;
    }

    ~SharedPtr() {
        --cptr->shared_count;
        if (cptr->shared_count > 0) {
            return;
        }
        cptr->destroy(ptr);
        if (cptr->weak_count == 0) {
            if (allocate_shared_tag) {
                allocate_shared_tag->deallocate(static_cast<ControlBlockMakeShared*>(cptr));
                ::operator delete(allocate_shared_tag); // TODO
            } else {
                ::operator delete(cptr); // TODO
            }
        }
    }
};

template<typename U, typename Alloc, typename... Args>
SharedPtr<U> allocateShared(const Alloc& alloc, Args&& ... args) {
    return SharedPtr<U>(typename SharedPtr<U>::AllocateSharedBase(), alloc, std::forward<Args>(args)...);
}

template<typename T, typename... Args>
SharedPtr<T> makeShared(Args&& ... args) {
    return allocateShared<T>(std::allocator<T>(), std::forward<Args>(args)...);
}

template<typename T>
class WeakPtr {
private:
    typename SharedPtr<T>::template ControlBlock<T> cptr;
public:
    WeakPtr(const SharedPtr<T>& ptr) : cptr(ptr.cptr) {

    }

    bool expired() const {
        return cptr->shared_count > 0;
    }

    SharedPtr<T> lock() const {

    }

    ~WeakPtr() {
        --cptr->weak_count;
        if (cptr->weak_count == 0 && cptr->shared_count == 0) {
            ::operator delete(cptr);
        }
    }
};
