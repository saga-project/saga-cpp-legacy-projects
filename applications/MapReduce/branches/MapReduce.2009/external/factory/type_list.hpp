/*=============================================================================
    Copyright (c) 2007-2008 Marco Costalba

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef TYPE_LIST_HPP
#define TYPE_LIST_HPP

namespace factory_ns { namespace detail { namespace type_list {

    static const int begin = -2;
    static const int end = -1;

    struct null_type;

    /* Simple trait to check for null_type */
    template<class T> struct is_null { static const bool value = false; };
    template<> struct is_null<null_type> { static const bool value = true; };

    /* A simplified type list */
    template<class H, class T> struct TL;

    /* Get the n-th element of a type_list, first is at 0 */
    template<int n, class TL> struct at_c;

    template<int n, class H, class T>
    struct at_c<n, TL<H, T> > : at_c<n - 1, T> {};

    template<class H, class T>
    struct at_c<0, TL<H, T> >
    {
         typedef H head;
         typedef T tail;
    };

    /* Increment list iterator skipping uninteresting items */
    template<typename T> struct skip; // to be specialized for types to skip

    template<int id, class T, bool skip>
    struct next_id_impl { static const int value = id; };

    template<int id>
    struct next_id_impl<id, null_type, true> { static const int value = end; };

    template<int id, class H, class T>
    struct next_id_impl<id, TL<H, T>, true> : next_id_impl<id + 1, T, skip<H>::value> {};

    template<int id, class TL>
    struct next_id : next_id_impl<id, typename at_c<id, TL>::tail, true> {};

    template<class H, class T>
    struct next_id<begin, TL<H, T> > : next_id_impl<0, T, skip<H>::value> {};

    /* Helper to build up a type list */
    template<class T1, class T2, class T3, class T4, class T5, class T6>
    struct make
    {
         typedef TL<T1, typename make<T2, T3, T4, T5, T6, null_type>::type> type;
    };

    template<>
    struct make<null_type, null_type, null_type, null_type, null_type, null_type>
    {
         typedef null_type type;
    };
}}}

/* Simple utility to pass arguments by reference instead of const
 * reference. This could be necessary as example when arguments
 * are modified inside object constructors.
 *
 * Needed only when quering for objects at runtime, i.e. only when
 * using factory::get_by_key()
 */
namespace factory_ns { namespace detail { namespace ref_wrapper {

    template<typename T> struct wrapper
    {
         explicit wrapper(T& v) : value(v) {}
         T& value;

         wrapper& operator=(wrapper const&); // fix a warning with MSVC /W4
    };

    template<typename T> struct unwrap
    {
         static const T& value(T const& v) { return v; }
    };

    template<typename U> struct unwrap<wrapper<U> >
    {
         static U& value(wrapper<U> const& v) { return v.value; }
    };

    /* Our public API here */
    template<typename T> inline
    wrapper<T> byRef(T& v) { return wrapper<T>(v); }
}}}

#endif
