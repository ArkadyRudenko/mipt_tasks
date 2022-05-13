

template<typename T>
class SharedPtr {
private:
    struct BaseControlBlock {
        size_t shared_count = 0;
        size_t weak_count = 0;

        virtual void destroy(T* obj) {};
        virtual void deallocate() {};
        virtual T* get_ptr() {};

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

        T* get_ptr() override {
            return ptr;
        }

        void deallocate() override {
            std::allocator<ControlBlockRegular<Deleter>> alloc_for_cbr;
            alloc_for_cbr.destroy(this);
            alloc_for_cbr.deallocate(this, 1);
        }
    };

    // for makeShared
    template<typename Alloc>
    struct ControlBlockMakeShared : public BaseControlBlock {
        T* ptr = &object;
        T object;
        Alloc alloc;
        template<typename... Args>
        explicit ControlBlockMakeShared(const Alloc& alloc, Args&& ... args) : object(std::forward<Args>(args)...), alloc(alloc) {}

        void destroy(T* obj) override {
            ptr->~T();
        }

        T* get_ptr() override {
            return ptr;
        }

        void deallocate() override {
            using RebindAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<ControlBlockMakeShared<Alloc>>;
            RebindAlloc alloc_for_cbr;
            alloc_for_cbr.deallocate(this, 1);
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

    struct allocate_shared_tag {};

    template<typename Alloc, typename... Args>
    SharedPtr(allocate_shared_tag, const Alloc& alloc, Args&& ... args) {
        using RebindAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<ControlBlockMakeShared<Alloc>>;
        RebindAlloc alloc_;
        ControlBlockMakeShared<Alloc>* new_ptr = alloc_.allocate(1);
        alloc_.construct(new_ptr, alloc, std::forward<Args>(args)...);
        ptr = &new_ptr->object;
        cptr = static_cast<BaseControlBlock*>(new_ptr);
        cptr->shared_count = 1;
    }

    void destroy_shared() {
        if (cptr) {
            --cptr->shared_count;
            if (cptr->shared_count > 0) {
                return;
            }
            cptr->destroy(ptr);
            if (cptr->weak_count == 0) {
                cptr->deallocate();
            }
        }
    }

    struct weak_ptr_tag {};

    SharedPtr(weak_ptr_tag, BaseControlBlock* cptr_) : cptr(cptr), ptr(cptr->get_ptr()) {
        ++cptr->shared_count;
    }
public:
    // TODO need inheritance for all ctrs
    SharedPtr() = default;

    explicit SharedPtr(T* ptr) : ptr(ptr) {
        cptr = new ControlBlockRegular<std::default_delete<T>>(ptr, std::default_delete<T>());
        cptr->shared_count = 1;
    }

    SharedPtr(const SharedPtr<T>& other) :
            cptr(other.cptr),
            ptr(other.ptr) {
        ++cptr->shared_count;
    }

    SharedPtr(SharedPtr<T>&& other) noexcept: cptr(other.cptr), ptr(other.ptr) {
        other.cptr = other.ptr = nullptr;
    }

    SharedPtr& operator=(const SharedPtr& other) noexcept {
        if (this == &other) { return *this; }
        destroy_shared();
        cptr = other.cptr;
        ptr = other.ptr;
        ++cptr->shared_count;
        return *this;
    }

    template<typename Y>
    SharedPtr& operator=(const SharedPtr<Y>& other) noexcept {
        // TODO for derived
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
        return *cptr->get_ptr();
    }

    T* operator->() const {
        return cptr->get_ptr();
    }

    size_t use_count() const {
        return cptr->shared_count;
    }

    ~SharedPtr() {
        destroy_shared();
    }
};

template<typename U, typename Alloc, typename... Args>
SharedPtr<U> allocateShared(const Alloc& alloc, Args&& ... args) {
    return SharedPtr<U>(typename SharedPtr<U>::allocate_shared_tag(), alloc, std::forward<Args>(args)...);
}

template<typename T, typename... Args>
SharedPtr<T> makeShared(Args&& ... args) {
    return allocateShared<T>(std::allocator<T>(), std::forward<Args>(args)...);
}

template<typename T>
class WeakPtr {
private:
    typename SharedPtr<T>::BaseControlBlock* cptr;
public:
    WeakPtr(const SharedPtr<T>& ptr) : cptr(ptr.cptr) {
        ++cptr->weak_count;
    }

    bool expired() const {
        return cptr->shared_count == 0;
    }

    SharedPtr<T> lock() const {
        if (this->expired()) {
            return SharedPtr<T>();
        }
        return SharedPtr<T>(typename SharedPtr<T>::weak_ptr_tag{}, cptr);
    }

    ~WeakPtr() {
        --cptr->weak_count;
        if (cptr->weak_count == 0 && cptr->shared_count == 0) {
            ::operator delete(cptr);
        }
    }
};
