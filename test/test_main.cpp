#include "gtest/gtest.h"

#include "shared_ptr_test/shared_ptr_test.h"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
