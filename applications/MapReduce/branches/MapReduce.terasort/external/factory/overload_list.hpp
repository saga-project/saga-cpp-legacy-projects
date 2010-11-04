/*=============================================================================
    Copyright (c) 2007-2008 Marco Costalba

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef OVERLOAD_LIST_HPP
#define OVERLOAD_LIST_HPP

#include "overload.hpp"

#define REGISTER_CLASS(cl, fact, key_value, sig, id)                       \
                                                                           \
    namespace factory_ns { namespace detail {                              \
                                                                           \
        template<> struct overload_item<fact, id> : overload<cl, sig>      \
        {                                                                  \
            template<typename key>                                         \
            static bool is_match(key const& v) { return v == key_value; }  \
        };                                                                 \
    }}

namespace factory_ns { namespace detail {

    /* Primary template, specializations in REGISTER_CLASS() */
    template<class fact, int n>
    struct overload_item : class_type<type_list::null_type> {};

namespace type_list {

    /* specialization to skip default overload_item during list walking */
    template<class fact, int n>
    struct skip<overload_item<fact, n> >
    {
        static const bool
        value = is_null<typename overload_item<fact, n>::type>::value;
    };
}

    template<class fact, int n = 0>
    struct overload_list
    {
        /* If a specialization of overload_item is found then corresponding
         * base class overload<> is used, otherwise fallback on primary template.
         *
         * For this to work overload_list must be instantiated during second
         * phase name lookup, when overload_item specializations are in scope.
         *
         * Build up a type list as a list of overload_item, where each
         * overload<> base class interfaces constructors of one class. One
         * struct overload per class.
         */
        typedef type_list::TL<
                              overload_item<fact, n>,
                              typename overload_list<fact, n + 1>::type
                             > type;
    };

    template<class fact>
    struct overload_list<fact, MAX_FACTORY_SIZE>
    {
        typedef type_list::null_type type;
    };
}}

#endif
