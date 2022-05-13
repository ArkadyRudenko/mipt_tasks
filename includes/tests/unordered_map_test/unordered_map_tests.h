#include "test_runner.h"
#include "profile.h"
#include "UnorderedMap.h"
#include "FastAllocator.h"
#include <random>

struct Base {
    Base() = default;

    Base(const Base& s) = default;

    Base(Base&& s) : a(s.a) {
//        cout << "I`m moved\n";
    }

    Base& operator=(Base&& s) {
//        cout << "I`m operator= move\n";
    }

    Base(int a_) : a(a_) {}

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
    UnorderedMap<int, Base> m;
    for (size_t i = 0; i < 10; ++i) {
        m.insert({i, Base(i)});
    }
    ASSERT_EQUAL(10, m.size());
    UnorderedMap<int, Base> m2;
    for (size_t i = 9; i < 19; ++i) {
        m2.insert({i, Base(i)});
    }
    ASSERT_EQUAL(10, m2.size());
    auto end = m2.begin();
    for (int i = 0; i < 9; i++) { ++end; };
    m.insert(m2.begin(), end);
    ASSERT_EQUAL(19, m.size());
    m.erase(m.begin());
    ASSERT_EQUAL(18, m.size());
    m.emplace(2, Base(2));
}

void pure_unordered_map() {
    std::unordered_map<int, Base> m;
//    m.insert({1, Base(1)});
//    std::pair<int, Base> p = {2, Base(1)};
    m.insert({1, Base(7)});
    m.insert({2, Base(2)});
    m.insert({3, Base(3)});
    m.insert({4, Base(4)});
    m.insert({5, Base(5)});
    m.insert({6, Base(6)});
    m.insert({6, Base(6)});
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
    UnorderedMap<int, Base> m;
    for (size_t i = 0; i < 10; ++i) {
        m.insert({i, Base(i)});
    }
    UnorderedMap<int, Base> copy(m);
    for (size_t i = 10; i < 20; ++i) {
        copy.insert({i, Base(i)});
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
struct std::hash<Key> {
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
    std::hash<Key> hasher;
    std::equal_to<Key> equal_to_;
    FastAllocator<int> allocator;

    UnorderedMap<Key, Value, std::hash<Key>, std::equal_to<Key>, FastAllocator<int>> unorderedMap(100, hasher,
                                                                                                  equal_to_, allocator);

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
    auto it = unorderedMap.find(Key(1, 1));
    if (it != unorderedMap.end()) { cout << "finded\n"; }
    unorderedMap[Key(1, 1)] = Value(7);
    ASSERT_EQUAL(7, (*unorderedMap.find(Key(1, 1))).second.a);
}

void speed_test() {
    {
        LOG_DURATION("MY_MAP");
        std::hash<Key> hasher;
        std::equal_to<Key> equal_to_;
        FastAllocator<int> allocator;

        UnorderedMap<Key, Value, std::hash<Key>, std::equal_to<Key>, FastAllocator<int>> unorderedMap_(100, hasher,
                                                                                                       equal_to_,
                                                                                                       allocator);
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist6(0, 1000); // distribution in range [1, 6]
        for (size_t i = 0; i < 10000; ++i) {
            unorderedMap_.insert({Key(dist6(rng), dist6(rng)), Value(dist6(rng))});
        }

    }
    {
        LOG_DURATION("STL_MAP");
        std::hash<Key> hasher;
        std::equal_to<Key> equal_to_;

        unordered_map<Key, Value, std::hash<Key>, std::equal_to<Key>> unorderedMap1(100, hasher, equal_to_);

        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist6(0, 1000); // distribution in range [1, 6]

        for (size_t i = 0; i < 10000; ++i) {
            unorderedMap1.insert({Key(dist6(rng), dist6(rng)), Value(dist6(rng))});
        }

    }
}

void unordered_map_tests() {
    TestRunner tr;
    RUN_TEST(tr, first_test);
    RUN_TEST(tr, insert_move_test);
    RUN_TEST(tr, pure_unordered_map);
    RUN_TEST(tr, copy_ctr_test);
    RUN_TEST(tr, move_ctr_test);
    RUN_TEST(tr, operator_copy_ctr_test);
    RUN_TEST(tr, speed_test);
}