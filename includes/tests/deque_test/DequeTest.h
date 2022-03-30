#include "Deque.h"
#include "profile.h"
#include "test_runner.h"

using namespace std;

void simple_test() {
    Deque<int> d;
    d.pop_front();
    ASSERT_EQUAL(0, d.size());
    d.pop_back();
    ASSERT_EQUAL(0, d.size());
    d.push_front(1);
    d.push_front(2);
    d.pop_front();
    ASSERT_EQUAL(1, d[0]);
}

void push_and_pop_front_test() {
    const int n = 100;
    const int size = 1000;
    Deque<int> d(1);
    for(int i = 0; i < n; i++) {
        d.push_front(i);
    }
    for(int i = 0; i < n/2; i++) {
        d.pop_front();
    }
    for(int i = 0; i < n/2; i++) {
        ASSERT_EQUAL(n/2 - 1 - i, d[i]);
    }
//    ASSERT_EQUAL(4, d.size());
}

void many_push_test() {
    Deque<int> d(11);
    for (int i = 0; i < 200; i++) {
        d.push_front(i);
    }
    for (int i = 0; i < 200; i++) {
        ASSERT_EQUAL(199 - i, d[i]);
    }
    ASSERT_EQUAL(200, d.size());
}

void push_and_pop_back_test() {
    Deque<int> d(101);
    for(int i = 0; i < 100; i++) {
        d.push_back(i);
    }
    for(int i = 0; i < 50; i++) {
        d.pop_back();
    }
    for(int i = 0; i < 50; i++) {
        ASSERT_EQUAL(i, d[i]);
    }
    ASSERT_EQUAL(50, d.size());
}

void push_and_pop_back_front_test() {
    Deque<int> d;
    for(int i = 0; i < 50; i++) {
        d.push_back(i);
    }
    for(int i = 0; i < 20; i++) {
        d.pop_front();
    }
    for(int i = 0; i < 30; i++) {
        ASSERT_EQUAL(i + 20, d[i]);
    }
    ASSERT_EQUAL(30, d.size());
    for(int i = 0; i < 100; i++) {
        d.push_front(i);
    }
    for(int i = 0; i < 100; i++) {
        ASSERT_EQUAL(99 - i, d[i]);
    }
//    for(int i = 0; i < d.size(); i++) {
//       cout << d[i] << " ";
//    }
}

void hard_test() {
    Deque<int> d;
    for(int i = 0; i < 10; i++) {
        d.push_back(i);
    }
    for(int i = 0; i < 10; i++) {
        d.push_front(i);
    }
    for(int i = 0; i < 50; i++) {
        d.pop_front();
    }
    for(int i = 0; i < 50; i++) {
        d.pop_back();
    }
    for(int i = 0; i < 10; i++) {
        d.push_back(i);
    }
    for(int i = 0; i < 10; i++) {
        d.push_front(i);
    }
    for(int i = 0; i < 10; i++) {
        ASSERT_EQUAL(9 - i, d[i]);
    }
    for(int i = 0; i < 5; i++) {
        ASSERT_EQUAL(0, d[10]);
    }
    ASSERT_EQUAL(20, d.size());
}

void at_test() {
    Deque<double> d(10);
    d.push_back(4.);
    ASSERT_EQUAL(4., d[0]);
    bool flag = false;
    try {
        d.at(1);
    } catch (out_of_range& e) {
        flag = true;
    }
    ASSERT_EQUAL(true, flag);
}

void copy_constr_test() {
    Deque<int> d(1);
    for(int i = 0; i < 500; i++) {
        d.push_back(i);
    }
    Deque copy(d);
    for(size_t i = 0; i < copy.size(); i++) {
        ASSERT_EQUAL(i, copy[i]);
    }
    Deque<int> copy_2;
    copy_2 = copy;
    for(size_t i = 0; i < copy_2.size(); i++) {
        ASSERT_EQUAL(i, copy_2[i]);
    }
    ASSERT_EQUAL(500, copy.size());
}

void copy_constr_with_item_test() {
    Deque<int> d(10, 5);
    for(size_t i = 0; i < d.size(); i++) {
        ASSERT_EQUAL(5, d[i]);
    }
    ASSERT_EQUAL(10, d.size());
}

void link_test() {
//    LOG_DURATION("link_test");
    Deque<int> d(1);
    for(size_t i = 0; i < 1000; i++) {
        d.push_front(i);
    }
    auto it = d.begin();
    ASSERT_EQUAL(999, *it);
    for(size_t i = 0; i < 10000; i++) {
        d.push_front(2);
    }
    ASSERT_EQUAL(999, *it);
}

void iterator_test() {
    Deque<int> d(10);
    for(size_t i = 0; i < 10; i++) {
        d.push_back(i);
    }
    auto it = d.begin();
    auto end = d.end();
    *it = 44;
    ASSERT_EQUAL(44, *it);
    int i = 0;
    *it = 0;
    for(auto begin = d.begin(); begin != d.end(); begin++) {
        ASSERT_EQUAL(i++, *begin);
    }
    ASSERT_EQUAL(true, (it < end));
    ASSERT_EQUAL(true, (it != end));
    ASSERT_EQUAL(true, (it <= end));
    ASSERT_EQUAL(true, it > (end - it));
    Deque<int>::deque_iterator b = d.begin();
}

void const_iterator_test() {

//    for(auto begin = d.begin(); begin != d.end(); begin++) {
//        ASSERT_EQUAL(i++, *begin);
//    }
}

void insert_test() {
    Deque<int> d;
    for(size_t i = 0; i < 50; i++) {
        d.push_back(i);
    }
    auto it = d.begin() + 10;
    d.insert(it, 77);
    int i = 0;
    for(auto begin = d.begin(); begin != d.end(); begin++) {
        if(*(begin + 1) == 10) {
            ASSERT_EQUAL(77, *begin);
        } else {
            ASSERT_EQUAL(i++, *begin);
        }
    }
}

void erase_test() {
    Deque<int> d;
    const Deque<int> p;
    auto g = p.begin();
    *g = 10;
    for(size_t i = 0; i < 50; i++) {
        d.push_back(i);
    }
    auto it = d.begin() + 10;
    auto res = d.erase(it);
    ASSERT_EQUAL(11, *res);
    ASSERT_EQUAL(1, *d.erase(d.begin()));
//    ASSERT_EQUAL()
}


void all_deque_tests() {
    TestRunner tr;
    RUN_TEST(tr, simple_test);
    RUN_TEST(tr, push_and_pop_front_test);
    RUN_TEST(tr, many_push_test);
    RUN_TEST(tr, push_and_pop_back_test);
    RUN_TEST(tr, push_and_pop_back_front_test);
    RUN_TEST(tr, hard_test);
    RUN_TEST(tr, at_test);
    RUN_TEST(tr, copy_constr_test);
    RUN_TEST(tr, copy_constr_with_item_test);
//    RUN_TEST(tr, link_test);
    RUN_TEST(tr, iterator_test);
    RUN_TEST(tr, insert_test);
    RUN_TEST(tr, erase_test);
}