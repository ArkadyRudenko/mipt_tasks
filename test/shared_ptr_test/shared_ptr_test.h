#include "SharedPtr.h"
#include <iostream>
#include <memory>
#include <UnorderedMap.h>
#include "gtest/gtest.h"

struct Base {
    ~Base() {
//        std::cout << "Deleted Base\n";
    }
};

struct Derived : public Base {
    Derived(int x) {

    }
    ~Derived() {
//        std::cout << "Deleted Derived\n";
    }
};

TEST(SharedPtr, simpleTest) {
    int* ptr = new int(1);
    SharedPtr<int> s(ptr);
}

TEST(SharedPtr, makeSharedTest) {
    SharedPtr<Derived> s = makeShared<Derived>(1);
    SharedPtr<Derived> s2(s);
    ASSERT_EQ(2, s2.use_count());
}

TEST(SharedPtr, CopyMakeSharedTest) {
    SharedPtr<Derived> s = makeShared<Derived>(1);
    SharedPtr<Derived> s2 = makeShared<Derived>(3);
    s2 = s;
    ASSERT_EQ(2, s2.use_count());

    int* ptr = new int(100);
    SharedPtr<int> sh(ptr);
    SharedPtr<int> sh2 = makeShared<int>(3);
    sh2 = sh;
    ASSERT_EQ(2, sh2.use_count());
}

TEST(SharedPtr, weak_ptr) {
    SharedPtr<int> s = makeShared<int>(1);
    WeakPtr<int> weakPtr(s);

    ASSERT_EQ(false, weakPtr.expired());
    s = makeShared<int>(2);
    ASSERT_EQ(true, weakPtr.expired());
    auto new_ptr = weakPtr.lock();
    WeakPtr<int> weakPtr_2(std::move(weakPtr));
    ASSERT_EQ(true, weakPtr_2.expired());
    ASSERT_EQ(1, s.use_count());
}

struct Node {
    Node* left = nullptr;
    Node* right = nullptr;
};


struct Del {
    void operator()(Node* a) {
//        std::cout << "I`m deleter of Node\n";
    }
};


TEST(SharedPtr, Deleter) {
    Node* node = new Node();
    SharedPtr<Node> sharedPtr(node, Del());
    delete node;
}

TEST(SharedPtr, shared_ptr_test) {

}
