#include <unordered_map>
#include <list>
#include <vector>
#include "ListTest.h"
#include "unordered_map_tests.h"

struct MyClass {
    MyClass(int&) {
        cout << "&\n";
    }
    MyClass(int&&) {
        cout << "&&\n";
    }
};

template<typename T>
void g(T&& a) {
    MyClass s(std::forward<T>(a));
}

void f(int&& a) {
    g(std::move(a));
}

void f(int& a) {
    g(a);
}

int main() {
    unordered_map_tests();
    allocator<int> a;
    allocator<int> b = std::move(a);
//    vector<>
    return 0;
}