#include <iostream>
#include <string>
#include <vector>
#include <deque>

#include "StringTests.h"
#include "DequeTest.h"

using namespace std;

template<typename T>
class Base {
    template<class U>
    class iterator {
    };

};

int main() {
    all_deque_tests();
    const deque v = {1,2,3};
    auto it = v.begin();
    return 0;
}
