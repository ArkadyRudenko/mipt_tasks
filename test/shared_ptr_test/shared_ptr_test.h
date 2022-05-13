#include "SharedPtr.h"
#include <iostream>
#include <memory>
#include "gtest/gtest.h"

struct Base {
    ~Base() {
        std::cout << "Deleted Base\n";
    }
};

struct Derived : public Base {
    Derived(int x) {

    }
    ~Derived() {
        std::cout << "Deleted Derived\n";
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

TEST(SharedPtr, shared_ptr_test) {

}
