/*=============================================================================
    Copyright (c) 2007-2008 Marco Costalba

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

#include "overload_list.hpp"

namespace factory_ns {
    /* Forward declaration used by overload_list */
    template<class base, typename key> struct factory;
}

namespace factory_ns { namespace detail {

    using ref_wrapper::unwrap;
    using type_list::next_id;

    template<class base, typename key, int id = type_list::begin>
    struct dispatcher
    {
        /* Get overload_item type at position id or the first valid */
        typedef typename overload_list<factory<base, key> >::type list;
        static const int valid_id = (id == type_list::begin ? next_id<id, list>::value : id);
        typedef typename type_list::at_c<valid_id, list>::head Ovl;

        /* Get dispatcher type at next valid id */
        typedef dispatcher<base, key, next_id<valid_id, list>::value> next_dispatcher;

        /* Start of argument arity dependant part */

        static base* get_by_key(key const& v)
        {
            if (Ovl::is_match(v))
                return Ovl()();

            return next_dispatcher::get_by_key(v);
        }

        template<typename A0>
        static base* get_by_key(key const& v, A0 const& a0)
        {
            if (Ovl::is_match(v))
                return Ovl()(unwrap<A0>::value(a0));

            return next_dispatcher::get_by_key(v, a0);
        }

        template<typename A0, typename A1>
        static base* get_by_key(key const& v, A0 const& a0, A1 const& a1)
        {
            if (Ovl::is_match(v))
                return Ovl()(unwrap<A0>::value(a0), unwrap<A1>::value(a1));

            return next_dispatcher::get_by_key(v, a0, a1);
        }
    };

    template<class base, typename key>
    struct dispatcher<base, key, type_list::end>
    {
        static base* get_by_key(key const&) { return 0; }

        template<typename A0>
        static base* get_by_key(key const&, A0 const&) { return 0; }

        template<typename A0, typename A1>
        static base* get_by_key(key const&, A0 const&, A1 const&) { return 0; }
    };
}}

#endif
