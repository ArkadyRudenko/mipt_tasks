#include <iostream>

template<typename... Types>
struct common_type;

template<typename First, typename Second, typename... Tail>
struct common_type<First, Second, Tail...> : std::type_identity<
        common_type<First, typename common_type<Second, Tail...>::type>
> {
};

template<typename First, typename Second>
struct common_type<First, Second> : std::type_identity<
        std::remove_reference_t<decltype(true ? std::declval<First>() : std::declval<Second>())
        >
> {
};

template<typename T>
struct common_type<T> : std::type_identity<T> {
};

struct Granny {};
struct Mother : Granny {};

int main() {
    std::cout << std::is_same_v<typename common_type<Mother, Granny>::type , int> << '\n';
    return 0;
}