#include "gtest/gtest.h"
#include "Variant.h"
#include <variant>
#include <vector>
#include <string>

TEST(Variant, simpleTest1) {
    my::variant<int, double, std::string> v = 5;
    ASSERT_EQ(5, my::get<int>(v));
//    v = 6;
}

TEST(Variant, simpleTest) {
    using variant_v = std::variant<int, double, std::string>;
    std::vector<variant_v> vec = {1, "gfs", 5.0};

    std::vector<variant_v> vec2;
    for (const auto& v : vec) {
        vec2.emplace_back(
                std::visit([](auto&& x) -> variant_v {
                    return x + x;
                }, v)
                );
    }
}
