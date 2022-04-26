#include <type_traits>

template<typename T, typename Allocator = std::allocator<T>>
class List {
public:
    explicit List(const Allocator& alloc = Allocator());
    List(size_t count, const T& value = T(), const Allocator& alloc = Allocator());
    List(const List& other);

    Allocator get_allocator() const;

    void insert();
    void erase();
    void pop_back();
    void pop_front();
    void push_front();
    void push_back();

    size_t size() const;

    List& operator=(const List& list);

    ~List();
private:

    size_t sz = 0;

    template<typename U>
    struct Node {
        T value;
        Node* prev;
        Node* next;
    };
    Node<T> fakeNode;

    template<bool IsConst>
    struct common_iterator {
    private:
        using ConditionalPtr = std::conditional_t<IsConst, const T*, T>;
    public:
        ConditionalPtr operator*() {

        }
    };

    using iterator = common_iterator<false>;
    using const_iterator = common_iterator<true>;
};
