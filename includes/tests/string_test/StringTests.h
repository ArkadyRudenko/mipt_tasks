#include <iostream>

#include "String.h"
#include "test_runner.h"

using namespace std;

void first_test() {
//    String s(10, 's');
    String s("a");
//    s += 'd';

    cout << s.front() << " " << s.back() << '\n';

    String ss;
    cout << ss << endl;
}

void all_tests() {
    TestRunner tr;
    RUN_TEST(tr, first_test);
}