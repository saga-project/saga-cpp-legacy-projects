//  Copyright (c) 2005-2007 Andre Merzky (andre@merzky.net)
//  Copyright (c) 2009 João Abecasis
// 
//  Use, modification and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

// System Header Files 

#include <iostream>
#include <boost/version.hpp>

#include "ini.hpp"
#include "impl/ini.hpp"

// --------------------------------------------------------
//
//
//
mb_util::ini::section::section (shared_sec  impl)
    : impl_ (impl)
{
}

// --------------------------------------------------------
//
//
//
mb_util::ini::section::section (std::string filename)
    : impl_ (new impl_sec (filename))
{
}

// --------------------------------------------------------
//
//
//
mb_util::ini::section::section (const section & in)
    : impl_ (in.get_impl())
{
}

// --------------------------------------------------------
//
//
//
mb_util::ini::section::section (impl_sec   * in)
#if BOOST_VERSION >= 103900
    : impl_ (in)
#else
    : impl_ (in->_internal_weak_this.use_count() ?
        in->shared_from_this() : shared_sec(in))
#endif
{
}

// --------------------------------------------------------
//
//
//
mb_util::ini::section::~section ()
{
}

// --------------------------------------------------------
//
//
//
void mb_util::ini::section::read (std::string filename)
{
    impl_->read (filename);
}

// --------------------------------------------------------
//
//
//
void mb_util::ini::section::parse (std::string sourcename, 
                                std::vector <std::string> lines)
{
    impl_->parse (sourcename, lines);
}

// --------------------------------------------------------
//
//
//
void mb_util::ini::section::merge (std::string second)
{
    impl_->merge (second);
}

// --------------------------------------------------------
//
//
//
void mb_util::ini::section::merge (const mb_util::ini::section & second)
{
    impl_->merge (second.impl_);
}

// --------------------------------------------------------
//
//
//
void mb_util::ini::section::dump (int            ind, 
                               std::ostream & strm) const
{
    impl_->dump (ind, strm);
}

// --------------------------------------------------------
//
//
//
void mb_util::ini::section::add_section (std::string   sec_name, 
                                const section     & sec)
{
    impl_->get_section (sec_name)->merge (sec.impl_);
}

// --------------------------------------------------------
//
//
//
bool mb_util::ini::section::has_section (std::string sec_name) const
{
    return impl_->has_section (sec_name);
}

// --------------------------------------------------------
//
//
//
bool mb_util::ini::section::has_section_full (std::string sec_name) const
{
    return impl_->has_section_full (sec_name);
}

// --------------------------------------------------------
//
//
//
mb_util::ini::section mb_util::ini::section::get_section (std::string sec_name) const
{
    return mb_util::ini::section (impl_->get_section (sec_name));
}

// --------------------------------------------------------
//
//
//
mb_util::ini::section_map mb_util::ini::section::get_sections () const
{
    typedef mb_util::impl::ini::section_map::reference section_impl;
    typedef mb_util::impl::ini::section_map::const_iterator iterator;

    mb_util::ini::section_map out;
    iterator end = impl_->get_sections().end();
    for (iterator it = impl_->get_sections().begin(); it != end; ++it)
        out[(*it).first] = mb_util::ini::section((*it).second);

    return out;
}

// --------------------------------------------------------
//
//
//
void mb_util::ini::section::add_entry (std::string key,                 
                                    std::string val)
{
    impl_->add_entry (key, val);
}

// --------------------------------------------------------
//
//
//
bool mb_util::ini::section::has_entry (std::string key) const
{
    return impl_->has_entry (key);
}

// --------------------------------------------------------
//
//
//
std::string mb_util::ini::section::get_entry (std::string key) const
{
    return impl_->get_entry (key);
}

// --------------------------------------------------------
//
//
//
std::string mb_util::ini::section::get_entry (std::string key,
                                           std::string dflt_val) const
{
    return impl_->get_entry (key, dflt_val);
}

// --------------------------------------------------------
//
//
//
mb_util::ini::entry_map mb_util::ini::section::get_entries (void) const
{
    return impl_->get_entries ();
}

// --------------------------------------------------------
//
//
//
mb_util::ini::section mb_util::ini::section::get_root (void) const  
{
    return mb_util::ini::section (impl_->get_root ());
}

// --------------------------------------------------------
//
//
//
std::string mb_util::ini::section::get_name (void) const  
{
    return impl_->get_name ();
}

// --------------------------------------------------------
//
//
//
void mb_util::ini::section::debug (std::string msg) const
{
  std::cout << "debuggin [" 
            << impl_.use_count ()
            << "] ======= " 
            << impl_->get_name ()
            << "\" -- \"" 
            << msg 
            << "\""
            << std::endl;
}

