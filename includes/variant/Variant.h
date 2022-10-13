#include <iostream>

namespace my {
// ------------------------ get_index_by_type
    template<typename T, typename... Types>
    struct get_index_by_type {
        static const size_t value = 0;
    };

    template<typename T, typename Head, typename... Tail>
    struct get_index_by_type<T, Head, Tail...> {
        static const size_t value = std::is_same_v<T, Head> ? 0 :
                                    get_index_by_type<T, Tail...>::value + 1;
    };

    template<typename... Types>
    constexpr size_t get_index_by_type_v = get_index_by_type<Types...>::value;

// -------------------- type_by_index
    template<size_t Index, typename Head, typename... Types>
    struct type_by_index {
        using type = typename type_by_index<Index - 1, Types...>::type;
    };

    template<typename Head, typename... Types>
    struct type_by_index<0, Head, Types...> {
        using type = Head;
    };

    template<size_t Index, typename... Types>
    using type_by_index_t = typename type_by_index<Index, Types...>::type;
// ----------------------------------

    template<typename... Types>
    class variant;

    template<typename T, typename... Types>
    struct VariantAlternative {
        // CRTP
        using Derived = variant<Types...>;
        static const size_t index = get_index_by_type_v<T, Types...>;

        VariantAlternative() {}

        VariantAlternative(const T& value) {
            static_cast<Derived*>(this)->storage.template put<index>(value);
        }

        VariantAlternative(T&& value) {
            static_cast<Derived*>(this)->storage.template put<index>(std::move(value));
        }

        void destroy() {
            auto this_ptr = static_cast<Derived*>(this);
            if (index == this_ptr->current) {

            }
        }
    };

    template<typename... Types>
    class variant : private VariantAlternative<Types, Types...> ... {
    private:

        template<typename T, typename... Ts>
        friend struct VariantAlternative;

        template<size_t N, typename... Ts>
        friend type_by_index_t<N, Ts...>& get(variant<Ts...>&);


        template<typename... TTypes>
        union VariadicUnion {
            template<size_t N, typename T>
            void put(const T&) {
                // nothing
            }
        };

        template<typename Head, typename... Tail>
        union VariadicUnion<Head, Tail...> {
            Head head;
            VariadicUnion<Tail...> tail;

            VariadicUnion() {}
            ~VariadicUnion() {}

            template<size_t N, typename T>
            void put(const T& value) {
                if constexpr (N == 0) {
                    new(std::launder(&head)) T(value);
                } else {
                    tail.template put<N - 1>(value);
                }
            }

            template<size_t Index>
            type_by_index_t<Index, Head, Tail...>& get() {
                if constexpr(Index == 0) {
                    return head;
                } else {
                    return tail.template get<Index - 1>();
                }
            }

        };

        VariadicUnion<Types...> storage;
        size_t current = 0;
    public:
        // constructors!
        using VariantAlternative<Types, Types...>::VariantAlternative...;

        template<typename T>
        variant& operator=(const T& value) {
            storage.template put<get_index_by_type_v<T, Types...>>(value);
        }

        size_t index() const {
            return current;
        }

        ~variant() {
            (VariantAlternative<Types, Types...>::destroy(), ...);
        }
    };


    template<size_t Index, typename... Types>
    type_by_index_t<Index, Types...>& get(variant<Types...>& v) {
        if (Index != v.index())
            throw std::bad_variant_access();
        return v.storage.template get<Index>();
    }

    template<typename T, typename... Types>
    T& get(variant<Types...>& v) {
        return get<get_index_by_type_v<T, Types...> >(v);
    }

} // namespace my