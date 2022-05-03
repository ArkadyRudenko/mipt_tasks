#include <list>

#include "test_runner.h"
#include "profile.h"
#include "FastAllocator.h"
#include "List.h"

namespace ListTest {
    template<typename T>
    void f(const T&) = delete;

    void simple_test() {
        const int size = 1000000;
        using type = std::string;
        {
            LOG_DURATION("stl Alloc")
            list <type> stl_list;

            for (size_t i = 0; i < size; ++i) {
                stl_list.push_front("String");
            }
            auto it = stl_list.begin();
            for (size_t i = size / 2; i < size; ++i) {
                auto removed = it;
                it++;
                stl_list.erase(removed);
            }
            auto inserter = stl_list.begin();
            for (size_t i = size / 2; i < size; ++i) {
                inserter++;
                stl_list.insert(inserter, "string");
            }
        }

        {
            LOG_DURATION("My Alloc")

            list <type, FastAllocator<type>> stl_list;
            for (size_t i = 0; i < size; ++i) {
                stl_list.push_front("String");
            }
            auto it = stl_list.begin();
            for (size_t i = size / 2; i < size; ++i) {
                auto removed = it;
                it++;
                stl_list.erase(removed);
            }
            auto inserter = stl_list.begin();
            for (size_t i = size / 2; i < size; ++i) {
                inserter++;
                stl_list.insert(inserter, "string");
            }
        }
    }

    void my_list_test() {
        List<int, FastAllocator<int>> my_list((FastAllocator<int>()));
        my_list.push_back(1);
        my_list.push_back(2);
        auto it = my_list.begin();
        ASSERT_EQUAL(1, *it);
        ASSERT_EQUAL(2, *++it);
        my_list.insert(my_list.begin(), 7);

        ASSERT_EQUAL(7, *my_list.begin());
        ASSERT_EQUAL(3, my_list.size());
        my_list.insert(my_list.cbegin(), 9);
        ASSERT_EQUAL(9, *my_list.cbegin());


        my_list.pop_back();
        ASSERT_EQUAL(3, my_list.size());
        my_list.pop_back();
        my_list.pop_back();
        my_list.pop_back();

    }

    class TestClass {
        int a;
    public:
        TestClass() = delete;

        TestClass(int a) : a(a) {}

        int getA() const { return a; }

        ~TestClass() {}
    };

    void class_TestClass_test() {
        List<TestClass, FastAllocator<TestClass>> my_list((FastAllocator<TestClass>()));
        for (int i = 0; i < 1000; i++) {
            my_list.push_back(TestClass(i));
        }
        int i = 0;
        for (auto begin = my_list.begin(); !(begin == my_list.end()); ++begin) {
            ASSERT_EQUAL(i++, (*begin).getA());
        }
    }

    void push_pop_front_test() {
        List<TestClass, FastAllocator<TestClass>> my_list((FastAllocator<TestClass>()));
        for (int i = 0; i < 1000; ++i) {
            my_list.push_front(TestClass(i));
        }
        int i = 0;
        for (auto rbegin = my_list.rbegin(); !(rbegin == my_list.rend()); ++rbegin) {
            ASSERT_EQUAL(i++, (*rbegin).getA());
        }
        for (int j = 0; j < 500; ++j) {
            my_list.pop_front();
        }
        i = 0;
        for (auto rbegin = my_list.rbegin(); !(rbegin == my_list.rend()); ++rbegin) {
            ASSERT_EQUAL(i++, (*rbegin).getA());
        }
        auto it = my_list.begin();
        ++it;
        my_list.erase(it);
        ASSERT_EQUAL(497, (*++my_list.begin()).getA());
    }

    void ctr_test() {
        List<TestClass, FastAllocator<TestClass>> my_list((FastAllocator<TestClass>()));
        for (int i = 0; i < 1000; ++i) {
            my_list.push_front(TestClass(i));
        }
//        List<TestClass, FastAllocator<TestClass>> copy_list(my_list);

    }

    void alloc_copy_test() {
        List<std::string, FastAllocator<std::string>> my_list((FastAllocator<std::string>()));
        my_list.push_back("test");
        my_list.push_back("test");
        my_list.push_back("test");
        List<std::string, FastAllocator<std::string>> list(my_list);
        ASSERT_EQUAL(*list.begin(), "test");
        for(auto rcbegin = my_list.rcbegin(); !(rcbegin == my_list.rcend()); rcbegin++) {
            ASSERT_EQUAL(*list.begin(), "test");
        }
    }

    void my_list_long_test() {
        const int size = 1000000;
        using type = std::string;
        {
            LOG_DURATION("stl Alloc and MyList")
            List <type, FastAllocator<type>> my_list;

            for (size_t i = 0; i < size; ++i) {
                my_list.push_front("String");
            }
            auto it = my_list.begin();
            for (size_t i = size / 2; i < size; ++i) {
                auto removed = it;
                it++;
                my_list.erase(removed);
            }
            auto inserter = my_list.begin();
            for (size_t i = size / 2; i < size; ++i) {
                inserter++;
                my_list.insert(inserter, "string");
            }
        }

        {
            LOG_DURATION("My Alloc and MyList")

            List <type, FastAllocator<type>> my_list;
            for (size_t i = 0; i < size; ++i) {
                my_list.push_front("String");
            }
            auto it = my_list.begin();
            for (size_t i = size / 2; i < size; ++i) {
                auto removed = it;
                it++;
                my_list.erase(removed);
            }
            auto inserter = my_list.begin();
            for (size_t i = size / 2; i < size; ++i) {
                inserter++;
                my_list.insert(inserter, "string");
            }
        }
    }

    void list_tests() {
        TestRunner tr;
        RUN_TEST(tr, simple_test);
        RUN_TEST(tr, my_list_test);
        RUN_TEST(tr, class_TestClass_test);
        RUN_TEST(tr, push_pop_front_test);
        RUN_TEST(tr, ctr_test);
        RUN_TEST(tr, alloc_copy_test);
        RUN_TEST(tr, my_list_long_test);
    }
}