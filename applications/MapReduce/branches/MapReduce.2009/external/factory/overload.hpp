/*=============================================================================
    Copyright (c) 2006 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)

    Modified by Marco Costalba (c) 2007-2008
==============================================================================*/
#ifndef OVERLOAD_HPP
#define OVERLOAD_HPP

#include "type_list.hpp"

namespace factory_ns { namespace detail {

    /* A struct overload it's a hierarchy of ctor_function functors
     * each implementing a specific operator() for a given number of
     * arguments and types. All the operator() are overloaded.
     *
     * When calling overload(<some args>) compiler chooses the correct
     * overload of operator() and links it.
     */
    template<class T, typename Base, typename Sig>
    struct ctor_function;

    template<class T, typename Base, typename R>
    struct ctor_function<T, Base, R()> : Base
    {
        using Base::operator();

        R* operator()()
        {
            return new T();
        }
    };

    template<class T, typename Base, typename R, typename A0>
    struct ctor_function<T, Base, R(A0)> : Base
    {
        using Base::operator();

        R* operator()(A0 a0)
        {
            return new T(a0);
        }
    };

    template<class T, typename Base, typename R, typename A0, typename A1>
    struct ctor_function<T, Base, R(A0, A1)> : Base
    {
        using Base::operator();

        R* operator()(A0 a0, A1 a1)
        {
            return new T(a0, a1);
        }
    };


    /* Store class type on which overload is defined */
    template<class T>
    struct class_type
    {
        typedef T type;
    };

    /* Take any argument, is the final base in an overload */
    template<class T>
    struct take_all : class_type<T>
    {
        struct any_conversion
        {
            /* A viable non-template function ctor_function::operator() it's
             * always a better match then the template any_conversion()
             */
            template<typename U> any_conversion(const volatile U&) {}
        };
       /* We cannot simply use operator()(...) here because passing
        * user-defined types through ellipses is undefined in C++, and
        * causes a big fat warning under gcc. So we are forced to use
        * arity dependent code.
        */
        T* operator()() { return 0; }
        T* operator()(any_conversion) { return 0; }
        T* operator()(any_conversion, any_conversion) { return 0; }
    };

    /* The overload builder */
    template<class T, typename Seq>
    struct make_overload;

    template<class T, typename head, typename tail>
    struct make_overload<T, type_list::TL<head, tail> >
    {
        typedef ctor_function<
                              T,
                              typename make_overload<T, tail>::type,
                              head
                             > type;
    };

    template<class T>
    struct make_overload<T, type_list::null_type>
    {
        typedef take_all<T> type;
    };

    /* Our (semi) public API here */
    template<class T, typename Sig>
    struct overload : make_overload<T, typename Sig::type>::type {};
}}

#endif
