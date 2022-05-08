#include "test_runner.h"
#include "UnorderedMap.h"
#include <random>

struct S {
    S() = default;

    S(const S& s) = default;

    S(S&& s) : a(s.a) {
//        cout << "I`m moved\n";
    }

    S& operator=(S&& s) {
//        cout << "I`m operator= move\n";
    }

    S(int a_) : a(a_) {}

    int a;
};

void first_test() {
    UnorderedMap<int, int> m;
    const std::pair<const int, int> p = {1, 1};
    const std::pair<const int, int> p2 = {1, 2};
    auto val1 = m.insert(p);
    auto val2 = m.insert(p2);
    ASSERT_EQUAL(val1.second, true);
    ASSERT_EQUAL(1, (*val1.first).second);
    ASSERT_EQUAL(false, val2.second);
    for (auto& it: m) {
//        cout << it.second << " ";
    }
    auto find_it = m.find(1);
    ASSERT_EQUAL(1, find_it->second);
}

void insert_move_test() {
    UnorderedMap<int, S> m;
    for (size_t i = 0; i < 10; ++i) {
        m.insert({i, S(i)});
    }
    ASSERT_EQUAL(10, m.size());
    UnorderedMap<int, S> m2;
    for (size_t i = 9; i < 19; ++i) {
        m2.insert({i, S(i)});
    }
    ASSERT_EQUAL(10, m2.size());
    auto end = m2.begin();
    for (int i = 0; i < 9; i++) { ++end; };
    m.insert(m2.begin(), end);
    ASSERT_EQUAL(19, m.size());
    m.erase(m.begin());
    ASSERT_EQUAL(18, m.size());
    m.emplace(2, S(2));
}

void pure_unordered_map() {
    unordered_map<int, S> m;
//    m.insert({1, S(1)});
//    std::pair<int, S> p = {2, S(1)};
    m.insert({1, S(7)});
    m.insert({2, S(2)});
    m.insert({3, S(3)});
    m.insert({4, S(4)});
    m.insert({5, S(5)});
    m.insert({6, S(6)});
    m.insert({6, S(6)});
    int key = 2;
//    cout << m.at(1).a << '\n';
//    ASSERT_EQUAL(4, m[4]);
}

template<typename Container>
void print(const Container& container) {
    for (const auto item: container) {
        std::cout << item.first << " -> " << item.second << "\n";
    }
}

void copy_ctr_test() {
    UnorderedMap<int, S> m;
    for (size_t i = 0; i < 10; ++i) {
        m.insert({i, S(i)});
    }
    UnorderedMap<int, S> copy(m);
    for (size_t i = 10; i < 20; ++i) {
        copy.insert({i, S(i)});
    }
//    print(m);
//    print(copy);
}

void move_ctr_test() {
    UnorderedMap<int, int> m;
    for (size_t i = 0; i < 100; ++i) {
        m.insert({i, i});
    }
    {
        UnorderedMap<int, int> copy(move(m));
        ASSERT_EQUAL(100, copy.size());
    }
    ASSERT_EQUAL(0, m.size());
}

struct Key {
    int a;
    int b;

    Key(int a, int b) : a(a), b(b) {};
};

struct Value {
    int a;
//    Value() = default;
    Value(int a) : a(a) {};
};

template<>
struct hash<Key> {
    size_t operator()(const Key& key) const {
        return key.b;
    }
};

namespace std {
    template<>
    struct equal_to<Key> {
        bool operator()(const Key& lhs, const Key& rhs) const {
            return lhs.a == rhs.a;
        }
    };
}

void operator_copy_ctr_test() {
    hash<Key> hasher;
    std::equal_to<Key> equal_to_;
    FastAllocator<int> allocator;

    UnorderedMap<Key, Value, hash<Key>, equal_to<Key>, FastAllocator<int>> unorderedMap(100, hasher, equal_to_, allocator);

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(0, 1000); // distribution in range [1, 6]

    {
        LOG_DURATION("ALLOC");
        for (size_t i = 0; i < 10; ++i) {
            unorderedMap.insert({Key(dist6(rng), dist6(rng)), Value(dist6(rng))});
        }
    }
    unorderedMap.insert({Key(1, 1), Value(1)});
    auto it = unorderedMap.find(Key(1,1));
    if(it != unorderedMap.end())  { cout << "finded\n"; }
    unorderedMap[Key(1,1)] = Value(7);
    ASSERT_EQUAL(7, (*unorderedMap.find(Key(1,1))).second.a);

// 928 834
// 1301 845
//1076 654 626

//    cout << unorderedMap.size() << '\n';
//    {
//        LOG_DURATION("COPY");
//        UnorderedMap m(unorderedMap);
//    }
//    {
//        LOG_DURATION("MOVE");
//        UnorderedMap m(std::move(unorderedMap));
//    }
//
//    {
//        UnorderedMap<int, int> m;
//        m.insert({1,1});
//        auto it = m.find(1);
//        cout << it->second <<'\n';
//    }
}

void unordered_map_tests() {
    TestRunner tr;
    RUN_TEST(tr, first_test);
    RUN_TEST(tr, insert_move_test);
    RUN_TEST(tr, pure_unordered_map);
    RUN_TEST(tr, copy_ctr_test);
    RUN_TEST(tr, move_ctr_test);
    RUN_TEST(tr, operator_copy_ctr_test);
}