
template<
        typename Key,
        typename Value,
        typename Hash = std::hash<Key>,
        typename Equal = std::equal_to<Key>,
        typename Allocator = std::allocator<std::pair<const Key, Value>>
>
class UnorderedMap {
public:
    using NodeType = std::pair<const Key, Value>;

    UnorderedMap(size_t n = 1000,
                 const Hash& hasher = Hash(),
                 const Equal& key_equal = Equal(),
                 const Allocator& alloc = Allocator());

    UnorderedMap(UnorderedMap&& other) noexcept;

    UnorderedMap(const UnorderedMap& other);

    UnorderedMap& operator=(const UnorderedMap& other);

    UnorderedMap& operator=(UnorderedMap&& other);

    ~UnorderedMap() = default;

private:
    class List {
    public:
        List(const Allocator& alloc) : alloc_(
                allocator_traits<Allocator>::select_on_container_copy_construction(alloc)) {}

        List(const List& other) : List(other.alloc_) {
            for (BaseNode* node = other.fake_node.next; node != &fake_node;
                 node = node->next) {
                this->push_back(static_cast<Node*>(node)->node, static_cast<Node*>(node)->cached_hash);
            }
        }

        List(List&& other) noexcept:
                alloc_(std::move(other.alloc_)), // ???
                fake_node(other.fake_node),
                sz(other.sz) {
            fake_node.prev->next = &fake_node;
            other.fake_node.next = other.fake_node.prev = nullptr;
            other.sz = 0;
        }

        List& operator=(List&& other) {
            delete_nodes();
            alloc_(std::move(other.alloc_));
            fake_node(other.fake_node);
            fake_node.prev->next = &fake_node;
            sz(other.sz);
            other.fake_node.next = other.fake_node.prev = nullptr;
            other.sz = 0;
            return *this;
        }

        struct BaseNode {
            BaseNode* next = nullptr;
            BaseNode* prev = nullptr;
        };
        struct Node : BaseNode {
            NodeType node;
            size_t cached_hash;
        };
    private:
        BaseNode fake_node;
        size_t sz = 0;

        using RebindAlloc = typename allocator_traits<Allocator>::template rebind_alloc<Node>;
        using AllocTraits = allocator_traits<RebindAlloc>;
        RebindAlloc alloc_;
    public:

        const RebindAlloc& get_allocator() const {
            return alloc_;
        }

        BaseNode* begin() const {
            return fake_node.next;
        }

        BaseNode* end() const {
            return fake_node.next->prev;
        }

        ~List() {
            delete_nodes();
        }

    private:
        void delete_nodes() {
            BaseNode* base_node = fake_node.next;
            while (base_node and base_node != &fake_node) {
                BaseNode* tmp_node = base_node->next;
                AllocTraits::destroy(alloc_, static_cast<Node*>(base_node)); // TODO BAD_CAST
                AllocTraits::deallocate(alloc_, static_cast<Node*>(base_node), 1); // TODO BAD_CAST
                base_node = tmp_node;
            }
        }

    public:

        template<typename T>
        Node* push_back(T&& value, size_t ch) {
//            cout << sizeof(Node) << '\n';
            Node* new_node = AllocTraits::allocate(alloc_, 1);
            if (!fake_node.prev) {
                AllocTraits::construct(alloc_, new_node, Node{&fake_node, &fake_node, std::forward<T>(value), ch});
                fake_node.prev = new_node;
                fake_node.next = new_node;
            } else {
                AllocTraits::construct(alloc_, new_node, Node{&fake_node, fake_node.prev, std::forward<T>(value), ch});
                fake_node.prev->next = new_node;
                fake_node.prev = new_node;
            }
            ++sz;
            return static_cast<Node*>(fake_node.prev);
        }

        template<typename T>
        void insert(Node* ptr, T&& value, size_t ch) {
            Node* new_node = AllocTraits::allocate(alloc_, 1);
            AllocTraits::construct(alloc_, new_node, Node{ptr, ptr->prev, std::forward<T>(value), ch});
            ptr->prev->next = new_node;
            ptr->prev = new_node;
            ++sz;
        }

        void erase(Node* ptr) {
            ptr->prev->next = ptr->next;
            ptr->next->prev = ptr->prev;
            AllocTraits::destroy(alloc_, static_cast<Node*>(ptr));
            AllocTraits::deallocate(alloc_, static_cast<Node*>(ptr), 1);
            --sz;
        }

        size_t size() const { return sz; }
    };

private:
    constexpr static const double MAX_LOAD_FACTOR = 0.75;
    double load_factor_ = 0.;
    size_t size_of_buckets_ = 1000;
    List nodes_;
    Hash hasher_;
    Equal key_equal_;

    using ListBaseNode = typename List::BaseNode;
    using ListNode = typename List::Node;

    vector<ListNode*> buckets_;

    void delete_table() {
        nodes_.delete_nodes();
        buckets_.clear();
    }

    template<bool IsConst>
    class common_iterator {
    private:
        friend class UnorderedMap;

        ListBaseNode* ptr;
        using ConditionalPtr = std::conditional_t<IsConst, const NodeType, NodeType>;
    public:
        explicit common_iterator(ListBaseNode* node_ptr) : ptr(node_ptr) {}

        ConditionalPtr& operator*() const {
            return static_cast<ListNode*>(ptr)->node; // TODO UB!
        }

        ConditionalPtr* operator->() const {
            return &static_cast<ListNode*>(ptr)->node; // TODO UB!
        }

        common_iterator& operator++() {
            ptr = ptr->next;
            return *this;
        }

        common_iterator& operator++(int) {
            ++*this;
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

public:
    pair<iterator, bool> insert(const NodeType& nodeType) {
        return inside_insert(nodeType);
    }

    pair<iterator, bool> insert(NodeType&& nodeType) {
        return inside_insert(std::move(nodeType));
    }

private:
    template<typename T>
    pair<iterator, bool> inside_insert(T&& value) {
        size_t hash_of_node_type = hasher_(value.first) % size_of_buckets_;
        // ++load_factor_ and then rehash...
        if (!buckets_[hash_of_node_type]) {
            buckets_[hash_of_node_type] = nodes_.push_back(std::forward<T>(value),
                                                                              hash_of_node_type);
            return {iterator(static_cast<ListBaseNode*>
                             (buckets_[hash_of_node_type % size_of_buckets_])), true};
        } else {
            ListNode* ptr = buckets_[hash_of_node_type];
            while (ptr->cached_hash == hash_of_node_type) {
                if (key_equal_(value.first, ptr->node.first)) {
                    return {iterator(nodes_.end()), false};
                }
                ptr = static_cast<ListNode*>(static_cast<ListBaseNode*>(ptr)->next); // TODO
            }
            nodes_.insert(ptr, std::forward<T>(value), hash_of_node_type);
            return {iterator(static_cast<ListBaseNode*>(ptr)), true};
        }
    }

public:

    void insert(iterator first, iterator second) {
        for (; first != second; first++) {
            this->insert(*first);
        }
        this->insert(*second);
    }

    template<typename... Args>
    pair<iterator, bool> emplace(Args&& ... args) {
        return this->insert(make_pair(std::forward<Args>(args)...));
    }

    void erase(iterator erased) {
        nodes_.erase(static_cast<ListNode*>(erased.ptr));
    }

    void erase(iterator first, iterator second) {
        for (; first != second; ++first) {
            nodes_.erase(static_cast<ListNode*>(first.ptr));
        }
        nodes_.erase(static_cast<ListNode*>(second.ptr));
    }

    iterator find(const Key& key) const {
        size_t find_hash = hasher_(key) % size_of_buckets_;
        ListNode* find_node = buckets_[find_hash];
        while (find_node and find_node->cached_hash == find_hash) {
            if (key_equal_(key, find_node->node.first)) {
                return iterator(static_cast<ListBaseNode*>(find_node));
            }
            find_node = static_cast<ListNode*>(static_cast<ListBaseNode*>(find_node)->next); // TODO
        }
        return iterator(nodes_.end());
    }

    Value& at(const Key& key) { // throw std::out_of_range
        size_t find_hash = hasher_(key) % size_of_buckets_;
        ListNode* find_node = buckets_[find_hash];
        while (find_node and find_node->cached_hash == find_hash) {
            if (key_equal_(key, find_node->node.first)) {
                return find_node->node.second;
            }
            find_node = static_cast<ListNode*>(static_cast<ListBaseNode*>(find_node)->next); // TODO
        }
        throw out_of_range("Haven`t node\n");
    }

    Value& operator[](const Key& key) {
        size_t find_hash = hasher_(key) % size_of_buckets_;
        ListNode* find_node = buckets_[find_hash];
        while (find_node and find_node->cached_hash == find_hash) {
            if (key_equal_(key, find_node->node.first)) {
                return find_node->node.second;
            }
            find_node = static_cast<ListNode*>(static_cast<ListBaseNode*>(find_node)->next); // TODO
        }
        if constexpr(is_default_constructible_v<Value>) {
            return (this->insert({key, Value()})).first->second;
        }
    }

    void reserve(size_t size) {
        size_of_buckets_ = size;
    }

    double load_factor() const { return load_factor_; }

    double max_load_factor() const { return MAX_LOAD_FACTOR; }

    double max_load_factor(double max_load_factor) {
        load_factor_ = max_load_factor;
    }

    void rehash() {
        size_of_buckets_ *= 2;
        buckets_.clear();
        decltype(buckets_) new_buckets(size_of_buckets_, nullptr);
        buckets_ = new_buckets;
        // TODO
    }

    size_t size() const { return nodes_.size(); }

    iterator begin() noexcept {
        return iterator(nodes_.begin());
    }

    iterator end() noexcept {
        return iterator(nodes_.end());
    }

    const_iterator begin() const noexcept {
        return const_iterator(nodes_.begin());
    }

    const_iterator end() const noexcept {
        return const_iterator(nodes_.end());
    }

    const_iterator cbegin() noexcept {
        return const_iterator(nodes_.begin());
    }

    const_iterator cend() noexcept {
        return const_iterator(nodes_.end());
    }
};

template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
UnorderedMap<Key, Value, Hash, Equal, Allocator>::UnorderedMap(
        size_t n,
        const Hash& hasher,
        const Equal& key_equal,
        const Allocator& alloc)
        : size_of_buckets_(n),
          buckets_(size_of_buckets_, nullptr),
          key_equal_(key_equal),
          hasher_(hasher),
          nodes_(alloc) {}

template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
UnorderedMap<Key, Value, Hash, Equal, Allocator>::UnorderedMap(const UnorderedMap& other) :
        load_factor_(other.load_factor_), size_of_buckets_(other.size_of_buckets_), buckets_(size_of_buckets_),
        nodes_(allocator_traits<Allocator>::select_on_container_copy_construction(other.nodes_.get_allocator())) {
    for (const auto node: other) {
        this->insert(node);
    }
}


template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
UnorderedMap<Key, Value, Hash, Equal, Allocator>::UnorderedMap(UnorderedMap&& other) noexcept
        : nodes_(std::move(other.nodes_)), buckets_(std::move(other.buckets_)), load_factor_(other.load_factor_),
          size_of_buckets_(other.size_of_buckets_) {
    other.load_factor_ = other.size_of_buckets_ = 0;
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
UnorderedMap<Key, Value, Hash, Equal, Allocator>&
UnorderedMap<Key, Value, Hash, Equal, Allocator>::operator=(const UnorderedMap& other) {
    if (this == &other) return *this;

    delete_table();
    for (const auto item: other) {
        this->insert(item);
    }
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
UnorderedMap<Key, Value, Hash, Equal, Allocator>&
UnorderedMap<Key, Value, Hash, Equal, Allocator>::operator=(UnorderedMap&& other) {
    nodes_ = std::move(other.nodes_);
    buckets_ = std::move(other.buckets_);
    size_of_buckets_ = other.other.size_of_buckets_;
    load_factor_ = other.load_factor_;
    other.load_factor_ = other.size_of_buckets_ = 0;
    return *this;
}