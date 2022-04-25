#include <list>

#include "test_runner.h"
#include "profile.h"
#include "FastAllocator.h"
#include "List.h"

namespace ListTest {

    void simple_test() {
        const int size = 100000;
        using type = std::string;
        {
            LOG_DURATION("stl Alloc")
            list<type> stl_list;

            for (size_t i = 0; i < size; ++i) {
                stl_list.push_front("String");
            }
            auto it = stl_list.begin();
            for (size_t i = size / 2; i < size; ++i) {
                auto removed = it;
                it++;
                stl_list.erase(removed);
            }
        }

        {
            LOG_DURATION("My Alloc")

            list<type, FastAllocator<type>> stl_list;
            for (size_t i = 0; i < size; ++i) {
                stl_list.push_front("String");
            }
            auto it = stl_list.begin();
            for (size_t i = size / 2; i < size; ++i) {
                auto removed = it;
                it++;
                stl_list.erase(removed);
            }
        }
    }


    void list_tests() {
        TestRunner tr;
        RUN_TEST(tr, simple_test);
    }
}


