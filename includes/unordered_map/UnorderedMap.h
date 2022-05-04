
template<typename Key,
        typename Value,
        typename Hash = std::hash<Key>,
        typename Equal = std::equal_to<Key>,
        typename Allocator = std::allocator<std::pair<const Key, Value>>>
class UnorderedMap {
public:
    using NodeType = std::pair<const Key, Value>;
    UnorderedMap();
    UnorderedMap(UnorderedMap&& other);
    UnorderedMap(const UnorderedMap& other);
    UnorderedMap& operator=(const UnorderedMap& other);
    UnorderedMap& operator=(UnorderedMap&& other);
    ~UnorderedMap();

    Value& operator[](const Key&& key);
    Value& at(Key&& key) const;

    auto begin();
    auto end();
    auto begin() const;
    auto end() const;
    auto cbegin();
    auto cend();
    auto cbegin() const;
    auto cend() const;

    pair<auto, bool> insert(const NodeType& nodeType);
    pair<auto, bool> insert(NodeType&& nodeType);

    size_t size() const {return sz;}

private:
    size_t sz;
};
