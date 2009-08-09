/*=============================================================================
    Copyright (c) 2007-2008 Marco Costalba

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef FACTORY_HPP
#define FACTORY_HPP

#define MAX_FACTORY_SIZE 7 /* Max number of classes per factory */

#include "dispatcher.hpp"

namespace factory_ns {

    template<class base, typename key>
    struct factory : detail::dispatcher<base, key>
    {
        template<int id>
        struct class_id
        {
            /* This is well defined only for valid id values */
            typedef typename detail::dispatcher<base, key, id>::Ovl::type get;
        };
    };

    /* Hide type lists to the user */
    template<typename sig1 = detail::type_list::null_type,
             typename sig2 = detail::type_list::null_type,
             typename sig3 = detail::type_list::null_type,
             typename sig4 = detail::type_list::null_type,
             typename sig5 = detail::type_list::null_type,
             typename sig6 = detail::type_list::null_type
            >
    struct signatures : detail::type_list::make<sig1, sig2, sig3, sig4, sig5, sig6> {};

    /* Export here byRef helper function */
    using detail::ref_wrapper::byRef;
}

#endif
