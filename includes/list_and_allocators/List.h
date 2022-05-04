#include <type_traits>
#include "boost/container/allocator_traits.hpp"

template<typename T, typename Allocator = std::allocator<T>>
class List {
public:
    explicit List(const Allocator& alloc = Allocator());

    explicit List(size_t count, const T& value = T(), const Allocator& alloc = Allocator());

    List(const List& other);
    List& operator=(const List& list);
    ~List();

    Allocator& get_allocator() const {
        return alloc_;
    }

    void pop_back();
    void pop_front();
    void push_front(const T& value);
    void push_back(const T& value);

    size_t size() const {
        return sz;
    }

    void delete_nodes();

private:

    struct BaseNode {
        BaseNode* next = nullptr;
        BaseNode* prev = nullptr;
    };

    struct Node : BaseNode {
        T value;
    };

    BaseNode fake_node;
    size_t sz = 0;

//    using RebindAlloc = typename Allocator::template rebind<Node>::other;
    using RebindAlloc = typename allocator_traits<Allocator>::template rebind_alloc<Node>;
    using AllocTraits = allocator_traits<RebindAlloc>;
    RebindAlloc alloc_;

    template<bool IsConst>
    class common_iterator {
    private:
        friend class List;
        BaseNode* ptr;
        using ConditionalPtr = std::conditional_t<IsConst, const T, T>;
    public:
        explicit common_iterator(BaseNode* node) : ptr(node) {}
        ConditionalPtr& operator*() const {
            return static_cast<Node*>(ptr)->value; // TODO UB!
        }
        common_iterator& operator--() {
            ptr = ptr->prev;
            return *this;
        }
        common_iterator& operator++() {
            ptr = ptr->next;
            return *this;
        }
        common_iterator& operator++(int) {
            ++*this;
            return *this;
        }
        common_iterator& operator--(int) {
            --*this;
            return *this;
        }
        bool operator==(const common_iterator& it) {
            return ptr->next == it.ptr->next and
                    ptr->prev == it.ptr->prev;
        }
        bool operator!=(const common_iterator& it) {
            return !(*this == it);
        }
    };

    using iterator = common_iterator<false>;
    using const_iterator = common_iterator<true>;

    template<typename Iterator>
    class common_reverse_iterator {
    private:
        Iterator iter;
        using ConditionalReversePtr = std::conditional_t<is_same_v<const_iterator, Iterator>, const T, T>;
    public:
        explicit common_reverse_iterator(const Iterator& iter) : iter(iter) {}

        ConditionalReversePtr& operator*() const {
            return *iter;
        }
        common_reverse_iterator& operator++() {
            --iter;
            return *this;
        }
        common_reverse_iterator& operator++(int) {
            --iter;
            return *this;
        }
        common_reverse_iterator& operator--() {
            ++iter;
            return *this;
        }
        common_reverse_iterator& operator--(int) {
            ++iter;
            return *this;
        }
        bool operator==(const common_reverse_iterator<Iterator>& it) {
            return this->iter == it.iter;
        }
        Iterator base() const {
            return iter;
        }
    };

    using reverse_iterator = common_reverse_iterator<iterator>;
    using const_reverse_iterator = common_reverse_iterator<const_iterator>;

public:
    iterator begin() {
        return iterator(fake_node.next);
    }
    iterator end() {
        return iterator(&fake_node);
    }
    const_iterator begin() const {
        return const_iterator(fake_node.next);
    }
    const_iterator end() const {
        return const_iterator(fake_node.next->prev); // TODO fake_node is const, but ctr const_iterator get BaseNode*
    }
    const_iterator cbegin() {
        return const_iterator(fake_node.next);
    }
    const_iterator cend() {
        return const_iterator(&fake_node);
    }
    reverse_iterator rbegin() {
        return reverse_iterator(iterator(fake_node.prev));
    }
    reverse_iterator rend() {
        return reverse_iterator(iterator(&fake_node));
    }
    const_reverse_iterator rcbegin() {
        return const_reverse_iterator(const_iterator(fake_node.prev));
    }
    const_reverse_iterator rcend() {
        return const_reverse_iterator(const_iterator(&fake_node));
    }

    template<bool isConstIter>
    void insert(const common_iterator<isConstIter>& it, const T& value);
    template<bool isConstIter>
    void erase(const common_iterator<isConstIter>& it);
};

template<typename T, typename Allocator>
List<T, Allocator>::List(const Allocator& alloc) : alloc_(alloc) {}

template<typename T, typename Allocator>
void List<T, Allocator>::push_front(const T& value) {
    Node* new_node = AllocTraits::allocate(alloc_, 1);
    if (!fake_node.next) {
        AllocTraits::construct(alloc_, new_node, Node{&fake_node, &fake_node, value});
        fake_node.next = new_node;
        fake_node.prev = new_node;
    } else {
        AllocTraits::construct(alloc_, new_node, Node{fake_node.next, &fake_node, value});
        fake_node.next->prev = new_node;
        fake_node.next = new_node;
    }
    ++sz;
}

template<typename T, typename Allocator>
void List<T, Allocator>::push_back(const T& value) {
    Node* new_node = AllocTraits::allocate(alloc_, 1);
    if (!fake_node.prev) {
        AllocTraits::construct(alloc_, new_node, Node{&fake_node, &fake_node, value});
        fake_node.prev = new_node;
        fake_node.next = new_node;
    } else {
        AllocTraits::construct(alloc_, new_node, Node{&fake_node, fake_node.prev, value});
        fake_node.prev->next = new_node;
        fake_node.prev = new_node;
    }
    ++sz;
}

template<typename T, typename Allocator>
void List<T, Allocator>::pop_front() {
    if (fake_node.next) {
        Node* remove_node = static_cast<Node*>(fake_node.next); // TODO BAD_CAST
        fake_node.next->next->prev = &fake_node;
        fake_node.next = fake_node.next->next;
        AllocTraits::destroy(alloc_, remove_node);
        AllocTraits::deallocate(alloc_, remove_node, 1);
        sz--;
    }
}

template<typename T, typename Allocator>
void List<T, Allocator>::pop_back() {
    if (fake_node.prev) {
        Node* remove_node = static_cast<Node*>(fake_node.prev); // TODO BAD_CAST
        fake_node.prev->prev->next = &fake_node;
        fake_node.prev = fake_node.prev->prev;
        AllocTraits::destroy(alloc_, remove_node);
        AllocTraits::deallocate(alloc_, remove_node, 1);
        sz--;
    }
}

template<typename T, typename Allocator>
template<bool isConstIter>
void List<T, Allocator>::insert(const List::common_iterator<isConstIter>& it, const T& value) {
    Node* new_node = AllocTraits::allocate(alloc_, 1);
    AllocTraits::construct(alloc_, new_node, Node{it.ptr, it.ptr->prev, value});
    it.ptr->prev->next = new_node;
    it.ptr->prev = new_node;
    ++sz;
}

template<typename T, typename Allocator>
template<bool isConstIter>
void List<T, Allocator>::erase(const List::common_iterator<isConstIter>& it) {
    it.ptr->prev->next = it.ptr->next;
    it.ptr->next->prev = it.ptr->prev;
    AllocTraits::destroy(alloc_, static_cast<Node*>(it.ptr));
    AllocTraits::deallocate(alloc_, static_cast<Node*>(it.ptr), 1);
}

template<typename T, typename Allocator>
void List<T, Allocator>::delete_nodes() {
    BaseNode* base_node = fake_node.next;
    while (base_node != &fake_node and base_node) {
        BaseNode* tmp_node = base_node->next;
        AllocTraits::destroy(alloc_, static_cast<Node*>(base_node)); // TODO BAD_CAST
        AllocTraits::deallocate(alloc_, static_cast<Node*>(base_node), 1); // TODO BAD_CAST
        base_node = tmp_node;
    }
    sz = 0;
}

template<typename T, typename Allocator>
List<T, Allocator>::~List() {
     delete_nodes();
}

template<typename T, typename Allocator>
List<T, Allocator>::List(size_t count, const T& value, const Allocator& alloc)
    : alloc_(AllocTraits::select_on_container_copy_construction(alloc)) {
    for (size_t i = 0; i < count; ++i) {
        this->push_back(value);
    }
}

template<typename T, typename Allocator>
List<T, Allocator>::List(const List& other) { // TODO
    for (const auto iter : other) {
        this->push_back(iter);
    }
}

template<typename T, typename Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(const List& list) {
    delete_nodes();
    alloc_(list.get_allocator());
    for (auto it : list) {
        this->push_back(*it);
    }
}
