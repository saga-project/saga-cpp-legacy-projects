//  Copyright (c) 2005-2007 Andre Merzky (andre@merzky.net)
//  Copyright (c) 2009 João Abecasis
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef _MB_UTIL_IMPL_INI_H_
#define _MB_UTIL_IMPL_INI_H_ 1

#include <string>
#include <map>
#include <vector>
#include <iostream>

#include <saga/saga.hpp>

#define SAGA_INI_MAXLEN 256
#define SAGA_INI_ERRLEN SAGA_INI_MAXLEN

//////////////////////////////////////////
//
// C++ interface
//
/// @cond
/** Hide from Doxygen */

namespace mb_util
{
  namespace impl
  {
    namespace ini
    {
      class   section;

      typedef section ini;

      typedef std::map <std::string, std::string> entry_map;
      typedef std::map <std::string, TR1::shared_ptr<section> > section_map;

      class section
         : public TR1::enable_shared_from_this<section>
      {
         section *this_() { return this; }

       protected:
         entry_map entries;
         section_map sections;

         void saga_ini_line_msg (std::string msg, std::string file, int lnum = 0);

         std::string    name;  // section name.  Empty for the root section
         std::string    fname; // ini file name
         section      * root;

       public:
         explicit section (std::string filename = "", section* root = NULL);
        ~section (void) { };

         void           read         (std::string   filename);
         void           parse        (std::string   sourcename, 
                                      std::vector <std::string> lines);
         void           merge        (std::string   second);
         void           merge        (TR1::shared_ptr<section> second);
         void           dump         (int           ind,
                                      std::ostream& strm,
                                      std::string   parent = "")  const;

         void           dump         (std::ostream& strm,
                                      std::string   parent = "")  const;

         bool           has_section  (std::string   sec_name) const;
         bool           has_section_full
                                     (std::string   sec_name) const;
         TR1::shared_ptr<section>
                        get_section  (std::string   sec_name);

         void           add_entry    (std::string   key,
                                      std::string   val);
         bool           has_entry    (std::string   key)      const;
         std::string    get_entry    (std::string   key)      const;
         std::string    get_entry    (std::string   key,
                                      std::string   dflt_val) const;

         std::string       expand_entry   (std::string   in)  const;
         void              expand_entry   (std::string&, std::string::size_type) const;
         void              expand_bracket (std::string&, std::string::size_type) const;
         void              expand_brace   (std::string&, std::string::size_type) const;

         section         * get_root       () const  { return root; }
         std::string       get_name       () const  { return name; }
         std::string       get_fname      () const  { return fname; }

         section_map     & get_sections   ()        { return sections; }

               entry_map   get_entries    () const
         {
           entry_map result;
           entry_map::const_iterator end = entries.end();
           for (entry_map::const_iterator it = entries.begin(); it != end; ++it)
             result [(*it).first] = expand_entry ((*it).second);

           return result;
         }
      };

    } // namespace ini

  } // namespace impl

} // namespace mb_util

///@endcond

#endif // _MB_UTIL_IMPL_INI_H_

