//
// Created by Mick Li on 2019/8/30.
//

#ifndef MLIST_LIST_HPP
#define MLIST_LIST_HPP

namespace ML
{
    // Length type
    using length_t = unsigned long long;

    // List declarations
    template<typename... Ts>
    struct list
    {
        static constexpr length_t size = sizeof...(Ts);
        template<template<typename...> typename other>
        using rebind = other<Ts...>;
    };
    template<typename T, T... vs>
    struct list_c
    {
        using type = T;
        static constexpr length_t size = sizeof...(vs);
        template<template<T...> typename other>
        using rebind = other<vs...>;
    };

    // Wrappers
    template<typename T, T v>
    struct wrapper_t
    {
        using type = T;
        static constexpr T value = v;
    };

    // Range
    template<typename T, T l, T r, T d = 1>
    struct range_c
    {
        using type = T;
        static constexpr T left = l, right = r, distance = d;
    };

    // Type definitions
    template<int i> using int_t = wrapper_t<int, i>;
    template<char c> using char_t = wrapper_t<char, c>;
    template<bool b> using bool_t = wrapper_t<bool, b>;
    using true_t = bool_t<true>;
    using false_t = bool_t<false>;
    template<length_t l> using index_t = wrapper_t<length_t, l>;
    template<int... is> using int_list = list_c<int, is...>;
    template<char... cs> using char_list = list_c<char, cs...>;
    template<bool... bs> using bool_list = list_c<bool, bs...>;
    template<length_t... ls> using index_list = list_c<length_t, ls...>;
    template<int i1, int i2, int d = 1> using int_range = range_c<int, i1, i2, d>;
    template<char c1, char c2, char d = 1> using char_range = range_c<char, c1, c2, d>;
    template<length_t l1, length_t l2, length_t d = 1> using index_range = range_c<length_t, l1, l2, d>;

    namespace literal
    {
        // User-defined literals to create char_lists
        template<char... cs> constexpr char_list<cs...> operator ""_cl () {return {};}
    } // namespace literal

    // conditional: select from two values
    template<typename B, typename T, typename F> struct conditional;
    template<typename T, typename F>
    struct conditional<true_t, T, F>
    {
        using type = T;
    };
    template<typename T, typename F>
    struct conditional<false_t, T, F>
    {
        using type = F;
    };
    template<typename B, typename T, typename F>
    using conditional_t = typename conditional<B, T, F>::type;

    // Algorithms
    // concat: concat two lists
    template<typename L1, typename L2> struct concat;
    template<typename... T1s, typename... T2s>
    struct concat<list<T1s...>, list<T2s...>>
    {
        using type = list<T1s..., T2s...>;
    };
    template<typename T, T... v1s, T... v2s>
    struct concat<list_c<T, v1s...>, list_c<T, v2s...>>
    {
        using type = list_c<T, v1s..., v2s...>;
    };
    template<typename L1, typename L2>
    using concat_t = typename concat<L1, L2>::type;

    // join: join n lists
    template<typename L1, typename... Ls> struct join;
    template<typename L1>
    struct join<L1>
    {
        using type = L1;
    };
    template<typename L1, typename L2, typename... Ls>
    struct join<L1, L2, Ls...>
    {
        using type = typename join<concat_t<L1, L2>, Ls...>::type;
    };
    template<typename L1, typename... Ls>
    using join_t = typename join<L1, Ls...>::type;

    // length: get the length of a list
    template<typename L> struct length;
    template<typename... Ts>
    struct length<list<Ts...>>
    {
        static constexpr length_t value = sizeof...(Ts);
    };
    template<typename T, T... vs>
    struct length<list_c<T, vs...>>
    {
        static constexpr length_t value = sizeof...(vs);
    };
    template<typename L>
    constexpr length_t length_v = length<L>::value;

    // get: get the nth item of a list
    template<typename L, length_t n> struct get;
    template<typename T1, typename... Ts>
    struct get<list<T1, Ts...>, 0>
    {
        using type = T1;
    };
    template<typename T1, typename... Ts, length_t n>
    struct get<list<T1, Ts...>, n>
    {
        using type = typename get<list<Ts...>, n - 1>::type;
    };
    template<typename T, T v1, T... vs>
    struct get<list_c<T, v1, vs...>, 0>
    {
        using type = wrapper_t<T, v1>;
    };
    template<typename T, T v1, T... vs, length_t n>
    struct get<list_c<T, v1, vs...>, n>
    {
        using type = typename get<list_c<T, vs...>, n - 1>::type;
    };
    template<typename L, length_t n>
    using get_t = typename get<L, n>::type;

    // set: set the nth item of a list
    template<typename L, length_t n, typename T> struct set;
    template<typename T1, typename... Ts, typename T>
    struct set<list<T1, Ts...>, 0, T>
    {
        using type = list<T, Ts...>;
    };
    template<typename T1, typename... Ts, length_t n, typename T>
    struct set<list<T1, Ts...>, n, T>
    {
        using type = concat_t<list<T1>, typename set<list<Ts...>, n - 1, T>::type>;
    };
    template<typename T, T v1, T... vs, T v>
    struct set<list_c<T, v1, vs...>, 0, wrapper_t<T, v>>
    {
        using type = list_c<T, v, vs...>;
    };
    template<typename T, T v1, T... vs, length_t n, T v>
    struct set<list_c<T, v1, vs...>, n, wrapper_t<T, v>>
    {
        using type = concat_t<list_c<T, v1>, typename set<list_c<T, vs...>, n - 1, wrapper_t<T, v>>::type>;
    };
    template<typename L, length_t n, typename T>
    using set_t = typename set<L, n, T>::type;

    // insert: insert an item into list
    template<typename L, length_t n, typename T> struct insert;
    template<typename... Ts, typename T>
    struct insert<list<Ts...>, 0, T>
    {
        using type = list<T, Ts...>;
    };
    template<typename T1, typename... Ts, length_t n, typename T>
    struct insert<list<T1, Ts...>, n, T>
    {
        using type = concat_t<list<T1>, typename insert<list<Ts...>, n - 1, T>::type>;
    };
    template<typename T, T... vs, T v>
    struct insert<list_c<T, vs...>, 0, wrapper_t<T, v>>
    {
        using type = list_c<T, v, vs...>;
    };
    template<typename T, T v1, T... vs, length_t n, T v>
    struct insert<list_c<T, v1, vs...>, n, wrapper_t<T, v>>
    {
        using type = concat_t<list_c<T, v1>, typename insert<list_c<T, vs...>, n - 1, wrapper_t<T, v>>::type>;
    };
    template<typename L, length_t n, typename T>
    using insert_t = typename insert<L, n, T>::type;

    // erase: erase an item from list
    template<typename L, length_t n> struct erase;
    template<typename T1, typename... Ts>
    struct erase<list<T1, Ts...>, 0>
    {
        using type = list<Ts...>;
    };
    template<typename T1, typename... Ts, length_t n>
    struct erase<list<T1, Ts...>, n>
    {
        using type = concat_t<list<T1>, typename erase<list<Ts...>, n - 1>::type>;
    };
    template<typename T, T v1, T... vs>
    struct erase<list_c<T, v1, vs...>, 0>
    {
        using type = list_c<T, vs...>;
    };
    template<typename T, T v1, T... vs, length_t n>
    struct erase<list_c<T, v1, vs...>, n>
    {
        using type = concat_t<list_c<T, v1>, typename erase<list_c<T, vs...>, n - 1>::type>;
    };
    template<typename L, length_t n>
    using erase_t = typename erase<L, n>::type;

    // find: find an item in list
    template<typename L, typename T> struct find;
    template<typename T, typename... Ts>
    struct find<list<T, Ts...>, T>
    {
        static constexpr length_t value = 0;
    };
    template<typename T1, typename... Ts, typename T>
    struct find<list<T1, Ts...>, T>
    {
        static constexpr length_t value = 1 + find<list<Ts...>, T>::value;
    };
    template<typename T, T v, T... vs>
    struct find<list_c<T, v, vs...>, wrapper_t<T, v>>
    {
        static constexpr length_t value = 0;
    };
    template<typename T, T v1, T... vs, T v>
    struct find<list_c<T, v1, vs...>, wrapper_t<T, v>>
    {
        static constexpr length_t value = 1 + find<list_c<T, vs...>, wrapper_t<T, v>>::value;
    };
    template<typename L, typename T>
    constexpr length_t find_v = find<L, T>::value;

    // find_with_fail: find a item in list, return -1 when not find
    template<typename L, typename T> struct find_with_fail;
    template<typename T>
    struct find_with_fail<list<>, T>
    {
        static constexpr length_t value = -1;
    };
    template<typename T, typename... Ts>
    struct find_with_fail<list<T, Ts...>, T>
    {
        static constexpr length_t value = 0;
    };
    template<typename T1, typename... Ts, typename T>
    struct find_with_fail<list<T1, Ts...>, T>
    {
    private:
        static constexpr length_t val = find_with_fail<list<Ts...>, T>::value;
    public:
        static constexpr length_t value = (val == -1 ? -1 : (val + 1));
    };
    template<typename T, T v, T... vs>
    struct find_with_fail<list_c<T, v, vs...>, wrapper_t<T, v>>
    {
        static constexpr length_t value = 0;
    };
    template<typename T, T v1, T... vs, T v>
    struct find_with_fail<list_c<T, v1, vs...>, wrapper_t<T, v>>
    {
    private:
        static constexpr length_t val = find_with_fail<list_c<T, vs...>, wrapper_t<T, v>>::value;
    public:
        static constexpr length_t value = (val == -1 ? -1 : (val + 1));
    };
    template<typename L, typename T>
    constexpr length_t find_with_fail_v = find_with_fail<L, T>::value;

    // replace: replace a item in list, only replace first
    template<typename L, typename T, typename U> struct replace;
    template<typename T, typename U>
    struct replace<list<>, T, U>
    {
        using type = list<>;
    };
    template<typename T, typename... Ts, typename U>
    struct replace<list<T, Ts...>, T, U>
    {
        using type = list<U, Ts...>;
    };
    template<typename T1, typename... Ts, typename T, typename U>
    struct replace<list<T1, Ts...>, T, U>
    {
        using type = concat_t<list<T1>, typename replace<list<Ts...>, T, U>::type>;
    };
    template<typename T, typename U>
    struct replace<list_c<T>, T, U>
    {
        using type = list_c<T>;
    };
    template<typename T, T v, T... vs, T w>
    struct replace<list_c<T, v, vs...>, wrapper_t<T, v>, wrapper_t<T, w>>
    {
        using type = list_c<T, w, vs...>;
    };
    template<typename T, T v1, T... vs, T v, T w>
    struct replace<list_c<T, v1, vs...>, wrapper_t<T, v>, wrapper_t<T, w>>
    {
        using type = concat_t<list_c<T, v1>, typename replace<list_c<T, vs...>, wrapper_t<T, v>, wrapper_t<T, w>>::type>;
    };
    template<typename L, typename T, typename U>
    using replace_t = typename replace<L, T, U>::type;

    // replace_all: replace all selected items in list
    template<typename L, typename T, typename U> struct replace_all;
    template<typename T, typename U>
    struct replace_all<list<>, T, U>
    {
        using type = list<>;
    };
    template<typename T, typename... Ts, typename U>
    struct replace_all<list<T, Ts...>, T, U>
    {
        using type = concat_t<list<U>, typename replace_all<list<Ts...>, T, U>::type>;
    };
    template<typename T1, typename... Ts, typename T, typename U>
    struct replace_all<list<T1, Ts...>, T, U>
    {
        using type = concat_t<list<T1>, typename replace_all<list<Ts...>, T, U>::type>;
    };
    template<typename T, typename U>
    struct replace_all<list_c<T>, T, U>
    {
        using type = list_c<T>;
    };
    template<typename T, T v, T... vs, T w>
    struct replace_all<list_c<T, v, vs...>, wrapper_t<T, v>, wrapper_t<T, w>>
    {
        using type = concat_t<list_c<T, w>, typename replace_all<list_c<T, vs...>, wrapper_t<T, v>, wrapper_t<T, w>>::type>;
    };
    template<typename T, T v1, T... vs, T v, T w>
    struct replace_all<list_c<T, v1, vs...>, wrapper_t<T, v>, wrapper_t<T, w>>
    {
        using type = concat_t<list_c<T, v1>, typename replace_all<list_c<T, vs...>, wrapper_t<T, v>, wrapper_t<T, w>>::type>;
    };
    template<typename L, typename T, typename U>
    using replace_all_t = typename replace_all<L, T, U>::type;

    // table: create a list with meta function
    template<template<auto> typename F, typename R> struct table;
    template<typename T, template<T> typename F, T l, T r, T d>
    struct table<F, range_c<T, l, r, d>>
    {
        using type = conditional_t<bool_t<l <= r>,
            concat_t<list<F<l>>, typename table<F, range_c<T, l + d, r, d>>::type>, list<>>;
    };
    template<template<auto> typename F, typename R>
    using table_t = typename table<F, R>::type;

    // array: a simplified table
    template<template<auto> typename F, auto v, decltype(v) start = 1>
    struct array : public table<F, range_c<decltype(v), start, v, 1>>
    {};
    template<template<auto> typename F, auto v, decltype(v) start = 1>
    using array_t = typename array<F, v, start>::type;

    // range: generate list_c
    template<typename T, T l, T r, T d = 1>
    struct range
    {
        using type = conditional_t<bool_t<l <= r>,
            concat_t<list_c<T, l>, typename range<T, l + d, r, d>::type>, list_c<T>>;
    };

    // first, last: specified get
    template<typename L>
    struct first : public get<L, 0> {};
    template<typename L>
    using first_t = typename first<L>::type;
    template<typename L>
    struct last : public get<L, length_v<L> - 1> {};
    template<typename L>
    using last_t = typename last<L>::type;

    // take: take first n elements
    template<typename L, length_t n> struct take;
    template<>
    struct take<list<>, 0>
    {
        using type = list<>;
    };
    template<length_t n>
    struct take<list<>, n>
    {
        using type = list<>;
    };
    template<typename... Ts>
    struct take<list<Ts...>, 0>
    {
        using type = list<>;
    };
    template<typename T1, typename... Ts, length_t n>
    struct take<list<T1, Ts...>, n>
    {
        using type = concat_t<list<T1>, typename take<list<Ts...>, n - 1>::type>;
    };
    template<typename T>
    struct take<list_c<T>, 0>
    {
        using type = list_c<T>;
    };
    template<typename T, length_t n>
    struct take<list_c<T>, n>
    {
        using type = list_c<T>;
    };
    template<typename T, T... vs>
    struct take<list_c<T, vs...>, 0>
    {
        using type = list_c<T>;
    };
    template<typename T, T v1, T... vs, length_t n>
    struct take<list_c<T, v1, vs...>, n>
    {
        using type = concat_t<list_c<T, v1>, typename take<list_c<T, vs...>, n - 1>::type>;
    };
    template<typename L, length_t n>
    using take_t = typename take<L, n>::type;

    // drop: drop first n elements
    template<typename L, length_t n> struct drop;
    template<>
    struct drop<list<>, 0>
    {
        using type = list<>;
    };
    template<length_t n>
    struct drop<list<>, n>
    {
        using type = list<>;
    };
    template<typename L>
    struct drop<L, 0>
    {
        using type = L;
    };
    template<typename T1, typename... Ts, length_t n>
    struct drop<list<T1, Ts...>, n>
    {
        using type = typename drop<list<Ts...>, n - 1>::type;
    };
    template<typename T>
    struct drop<list_c<T>, 0>
    {
        using type = list_c<T>;
    };
    template<typename T, length_t n>
    struct drop<list_c<T>, n>
    {
        using type = list_c<T>;
    };
    template<typename T, T v1, T... vs, length_t n>
    struct drop<list_c<T, v1, vs...>, n>
    {
        using type = typename drop<list_c<T, vs...>, n - 1>::type;
    };
    template<typename L, length_t n>
    using drop_t = typename drop<L, n>::type;

    // extract: extract several parts
    template<typename L, typename IL>
    struct extract
    {
    private:
        // Helper
        template<length_t l>
        using helper = get_t<L, get_t<IL, l>::value>;

    public:
        using type = table_t<helper, index_range<0, length_v<IL> - 1>>;
    };
    template<typename L, typename IL>
    using extract_t = typename extract<L, IL>::type;

    // select: select some element from the list
    template<typename L, template<typename> typename F> struct select;
    template<template<typename> typename F>
    struct select<list<>, F>
    {
        using type = list<>;
    };
    template<typename T1, typename... Ts, template<typename> typename F>
    struct select<list<T1, Ts...>, F>
    {
        using type = concat_t<conditional_t<F<T1>, list<T1>, list<>>,
            typename select<list<Ts...>, F>::type>;
    };
    template<typename T, template<typename> typename F>
    struct select<list_c<T>, F>
    {
        using type = list_c<T>;
    };
    template<typename T, T v1, T... vs, template<typename> typename F>
    struct select<list_c<T, v1, vs...>, F>
    {
        using type = concat_t<conditional_t<F<wrapper_t<T, v1>>, list_c<T, v1>, list_c<T>>,
            typename select<list_c<T, vs...>, F>::type>;
    };
    template<typename L, template<typename> typename F>
    using select_t = typename select<L, F>::type;

    // count: the number of specified item
    template<typename L, typename T> struct count;
    template<typename T>
    struct count<list<>, T>
    {
        static constexpr length_t value = 0;
    };
    template<typename T, typename... Ts>
    struct count<list<T, Ts...>, T>
    {
        static constexpr length_t value = 1 + count<list<Ts...>, T>::value;
    };
    template<typename T1, typename... Ts, typename T>
    struct count<list<T1, Ts...>, T>
    {
        static constexpr length_t value = count<list<Ts...>, T>::value;
    };
    template<typename T, T v>
    struct count<list_c<T>, wrapper_t<T, v>>
    {
        static constexpr length_t value = 0;
    };
    template<typename T, T v, T... vs>
    struct count<list_c<T, v, vs...>, wrapper_t<T, v>>
    {
        static constexpr length_t value = 1 + count<list_c<T, vs...>, wrapper_t<T, v>>::value;
    };
    template<typename T, T v1, T... vs, T v>
    struct count<list_c<T, v1, vs...>, wrapper_t<T, v>>
    {
        static constexpr length_t value = count<list_c<T, vs...>, wrapper_t<T, v>>::value;
    };
    template<typename L, typename T>
    constexpr length_t count_v = count<L, T>::value;

    // member: determine if an item is in a list
    template<typename L, typename T>
    struct member : public bool_t<(find_with_fail_v<L, T> >= 0)>
    {};
    template<typename L, typename T>
    constexpr bool member_v = member<L, T>::value;

    // sort: using quick sort to sort the list
    template<typename L> struct sort;
    template<typename T>
    struct sort<list_c<T>>
    {
        using type = list_c<T>;
    };
    template<typename T, T v>
    struct sort<list_c<T, v>>
    {
        using type = list_c<T, v>;
    };
    template<typename T, T v1, T... vs>
    struct sort<list_c<T, v1, vs...>>
    {
    private:
        // Helpers
        template<typename V> struct lt;
        template<T v>
        struct lt<wrapper_t<T, v>> : public bool_t<(v < v1)>
        {};
        template<typename V>
        using lt_t = bool_t<lt<V>::value>;
        template<typename V> struct eq;
        template<T v>
        struct eq<wrapper_t<T, v>> : public bool_t<(v == v1)>
        {};
        template<typename V>
        using eq_t = bool_t<eq<V>::value>;
        template<typename V> struct gt;
        template<T v>
        struct gt<wrapper_t<T, v>> : public bool_t<(v > v1)>
        {};
        template<typename V>
        using gt_t = bool_t<gt<V>::value>;

    private:
        // 3 Parts
        using rest = list_c<T, vs...>;
        using lower = typename sort<select_t<rest, lt_t>>::type;
        using equal = select_t<rest, eq_t>; // no need to sort
        using great = typename sort<select_t<rest, gt_t>>::type;

    public:
        using type = join_t<lower, list_c<T, v1>, equal, great>;
    };
    template<typename L>
    using sort_t = typename sort<L>::type;

    // unique: delete successive duplicates (need the list to be sorted)
    template<typename L> struct unique;
    template<>
    struct unique<list<>>
    {
        using type = list<>;
    };
    template<typename T>
    struct unique<list<T>>
    {
        using type = list<T>;
    };
    template<typename T1, typename... Ts>
    struct unique<list<T1, T1, Ts...>>
    {
        using type = typename unique<list<T1, Ts...>>::type;
    };
    template<typename T1, typename T2, typename... Ts>
    struct unique<list<T1, T2, Ts...>>
    {
        using type = concat_t<list<T1>, typename unique<list<T2, Ts...>>::type>;
    };
    template<typename T>
    struct unique<list_c<T>>
    {
        using type = list_c<T>;
    };
    template<typename T, T v>
    struct unique<list_c<T, v>>
    {
        using type = list_c<T, v>;
    };
    template<typename T, T v1, T... vs>
    struct unique<list_c<T, v1, v1, vs...>>
    {
        using type = typename unique<list_c<T, v1, vs...>>::type;
    };
    template<typename T, T v1, T v2, T... vs>
    struct unique<list_c<T, v1, v2, vs...>>
    {
        using type = concat_t<list_c<T, v1>, typename unique<list_c<T, v2, vs...>>::type>;
    };
    template<typename L>
    using unique_t = typename unique<L>::type;

    // flatten: make a list flat
    template<typename L, length_t n = static_cast<length_t>(-1)> struct flatten;
    template<>
    struct flatten<list<>, 0>
    {
        using type = list<>;
    };
    template<typename L>
    struct flatten<L, 0>
    {
        using type = L;
    };
    template<length_t n>
    struct flatten<list<>, n>
    {
        using type = list<>;
    };
    template<typename... T1s, typename... Ts, length_t n>
    struct flatten<list<list<T1s...>, Ts...>, n>
    {
        using type = concat_t<typename flatten<list<T1s...>>::type, typename flatten<list<Ts...>>::type>;
    };
    template<typename T1, typename... Ts, length_t n>
    struct flatten<list<T1, Ts...>, n>
    {
        using type = concat_t<list<T1>, typename flatten<list<Ts...>>::type>;
    };
    template<typename L>
    using flatten_t = typename flatten<L>::type;

    // map: map a function to a list
    template<template<typename> typename F, typename L, length_t n = 1> struct map;
    template<template<typename> typename F, typename... Ts>
    struct map<F, list<Ts...>, 0>
    {
        using type = F<list<Ts...>>;
    };
    template<template<typename> typename F, typename T, T... vs>
    struct map<F, list_c<T, vs...>, 0>
    {
        using type = F<list_c<T, vs...>>;
    };
    template<template<typename> typename F, typename L>
    struct map<F, L, 0>
    {
        using type = L;
    };
    template<template<typename> typename F, typename... Ts, length_t n>
    struct map<F, list<Ts...>, n>
    {
        using type = list<typename map<F, Ts, n - 1>::type...>;
    };
    template<template<typename> typename F, typename T, T... vs, length_t n>
    struct map<F, list_c<T, vs...>, n>
    {
        using type = list<F<wrapper_t<T, vs>>...>;
    };
    template<template<typename> typename F, typename T, length_t n>
    struct map
    {
        using type = F<T>;
    };
    template<template<typename> typename F, typename L, length_t n = 1>
    using map_t = typename map<F, L, n>::type;

    // apply: apply function to list
    template<template<typename...> typename F, typename L, length_t n = 0> struct apply;
    template<template<typename...> typename F, typename... Ts>
    struct apply<F, list<Ts...>, 0>
    {
        using type = F<Ts...>;
    };
    template<template<typename...> typename F, typename T, T... vs>
    struct apply<F, list_c<T, vs...>, 0>
    {
        using type = F<wrapper_t<T, vs>...>;
    };
    template<template<typename...> typename F, typename... Ts, length_t n>
    struct apply<F, list<Ts...>, n>
    {
        using type = list<typename apply<F, Ts, n - 1>::type...>;
    };
    template<template<typename...> typename F, typename T, T... vs, length_t n>
    struct apply<F, list_c<T, vs...>, n>
    {
        using type = list<F<wrapper_t<T, vs>...>>;
    };
    template<template<typename...> typename F, typename T, length_t n>
    struct apply
    {
        using type = T;
    };
    template<template<typename...> typename F, typename L, length_t n = 0>
    using apply_t = typename apply<F, L, n>::type;

    // thread: thread over a function
    template<template<typename...> typename F, typename... Ls> struct thread;
    template<template<typename...> typename F>
    struct thread<F>
    {
        using type = list<>;
    };
    template<template<typename...> typename F, typename L1, typename... Ls>
    struct thread<F, L1, Ls...>
    {
    private:
        // Helper
        template<length_t l>
        using helper = F<get_t<L1, l>, get_t<Ls, l>...>;

    public:
        using type = table_t<helper, index_range<0, length_v<L1> - 1>>;
    };
    template<template<typename...> typename F, typename... Ls>
    using thread_t = typename thread<F, Ls...>::type;

    // fold: left-fold the list
    template<template<typename, typename> typename F, typename S, typename L> struct fold;
    template<template<typename, typename> typename F, typename S>
    struct fold<F, S, list<>>
    {
        using type = S;
    };
    template<template<typename, typename> typename F, typename S, typename T1, typename... Ts>
    struct fold<F, S, list<T1, Ts...>>
    {
        using type = F<S, typename fold<F, T1, list<Ts...>>::type>;
    };
    template<template<typename, typename> typename F, typename S, typename T>
    struct fold<F, S, list_c<T>>
    {
        using type = S;
    };
    template<template<typename, typename> typename F, typename S, typename T, T v1, T... vs>
    struct fold<F, S, list_c<T, v1, vs...>>
    {
        using type = F<S, typename fold<F, wrapper_t<T, v1>, list_c<T, vs...>>::type>;
    };
    template<template<typename, typename> typename F, typename S, typename L>
    using fold_t = typename fold<F, S, L>::type;

    // difference: different function
    template<template<typename, typename> typename F, typename L> struct difference;
    template<template<typename, typename> typename F>
    struct difference<F, list<>>
    {
        using type = list<>;
    };
    template<template<typename, typename> typename F, typename T>
    struct difference<F, list<T>>
    {
        using type = list<T>;
    };
    template<template<typename, typename> typename F, typename T1, typename T2, typename... Ts>
    struct difference<F, list<T1, T2, Ts...>>
    {
        using type = concat_t<list<F<T1, T2>>, typename difference<F, list<T2, Ts...>>::type>;
    };
    template<template<typename, typename> typename F, typename T>
    struct difference<F, list_c<T>>
    {
        using type = list_c<T>;
    };
    template<template<typename, typename> typename F, typename T, T v>
    struct difference<F, list_c<T, v>>
    {
        using type = list_c<T, v>;
    };
    template<template<typename, typename> typename F, typename T, T v1, T v2, T... vs>
    struct difference<F, list_c<T, v1, v2, vs...>>
    {
        using type = concat_t<list<F<wrapper_t<T, v1>, wrapper_t<T, v2>>>,
            typename difference<F, list_c<T, v2, vs...>>::type>;
    };

    // all_true, any_true and none_true: specialized part of select
    template<typename L, template<typename> typename F>
    struct all_true : public bool_t<(length_v<select_t<L, F>> == length_v<L>)>
    {};
    template<typename L, template<typename> typename F>
    constexpr bool all_true_v = all_true<L, F>::value;
    template<typename L, template<typename> typename F>
    struct any_true : public bool_t<(length_v<select_t<L, F>> > 0)>
    {};
    template<typename L, template<typename> typename F>
    constexpr bool any_true_v = any_true<L, F>::value;
    template<typename L, template<typename> typename F>
    struct none_true : public bool_t<(length_v<select_t<L, F>> == 0)>
    {};
    template<typename L, template<typename> typename F>
    constexpr bool none_true_v = none_true<L, F>::value;

    // contains_* functions: specialized part of member and select
    template<typename L1, typename L2>
    struct contains_all
    {
    private:
        // Helper
        template<typename T>
        using helper = bool_t<member_v<L1, T>>;

    public:
        static constexpr bool value = all_true_v<L2, helper>;
    };
    template<typename L1, typename L2>
    constexpr bool contains_all_v = contains_all<L1, L2>::value;
    template<typename L1, typename L2>
    struct contains_any
    {
    private:
        // Helper
        template<typename T>
        using helper = bool_t<member_v<L1, T>>;

    public:
        static constexpr bool value = any_true_v<L2, helper>;
    };
    template<typename L1, typename L2>
    constexpr bool contains_any_v = contains_any<L1, L2>::value;
    template<typename L1, typename L2>
    struct contains_none
    {
    private:
        // Helper
        template<typename T>
        using helper = bool_t<member_v<L1, T>>;

    public:
        static constexpr bool value = none_true_v<L2, helper>;
    };
    template<typename L1, typename L2>
    constexpr bool contains_none_v = contains_none<L1, L2>::value;
    template<typename L1, typename L2>
    struct contains_only : public contains_all<L2, L1>
    {};
    template<typename L1, typename L2>
    constexpr bool contains_only_v = contains_only<L1, L2>::value;
    template<typename L1, typename L2>
    struct contains_exactly
    {
        static constexpr bool value = contains_all_v<L1, L2> && contains_all_v<L2, L1>;
    };
    template<typename L1, typename L2>
    constexpr bool contains_exactly_v = contains_exactly<L1, L2>::value;
} // namespace ML

#endif //MLIST_LIST_HPP
