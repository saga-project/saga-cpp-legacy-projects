//  Copyright (c) 2005-2008 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(ADAPTORS_MIGOL_CPR_HELPERS_HPP)
#define ADAPTORS_MIGOL_CPR_HELPERS_HPP

#include <list>
#include <string>

#include <boost/config.hpp>
#include <boost/algorithm/string/predicate.hpp>
#if defined(BOOST_HAS_UNISTD_H)
#include <unistd.h>
#endif
#if defined(BOOST_WINDOWS)
#include <windows.h>
#endif

#include <saga/saga.hpp>
#include <saga/saga/adaptors/adaptor_data.hpp>

//#include <saga/impl/engine/ini/ini.hpp>
//#include <saga/adaptors/utils/ini/ini.hpp>
//#include <saga/context.hpp>
//#include <saga/session.hpp>

#include <saga/impl/config.hpp>
#include <saga/saga/exception.hpp>
#include <saga/saga/adaptors/task.hpp>
#include <saga/saga/adaptors/attribute.hpp>
#include <saga/saga/adaptors/adaptor.hpp>
#include <saga/saga/url.hpp>

#include "migol.hpp"
#include "migol_cpr_adaptor.hpp"
//#include "connectstring.hpp"
//#include <saga/adaptors/adaptor_data.hpp>

#ifndef  MAX_PATH
# define MAX_PATH _POSIX_PATH_MAX
#endif

///////////////////////////////////////////////////////////////////////////////
namespace cpr { 
    
    typedef saga::adaptors::adaptor_data<cpr::adaptor> adaptor_data_t;
    
    
    namespace detail
{
    ///////////////////////////////////////////////////////////////////////////
    // wrapper for gethostname()
    inline std::string 
    get_hostname()
    {
        char buffer[MAX_PATH] = { '\0' };
        gethostname(buffer, sizeof(buffer));
        return std::string(buffer);
    }
    
    inline std::string
    get_ais_url(saga::ini::ini const& ini)
    {
        // try to find database back end
        if (ini.has_section("preferences")) {
         saga::ini::ini prefs = ini.get_section ("preferences");
         std::string ais_url(prefs.has_entry("aisurl") ? prefs.get_entry("aisurl") : ""); 
        //std::string ais_url(ini.has_entry("aisurl") ? ini.get_entry("aisurl") : "");
            if (ais_url != ""){
                return ais_url;
            } 
        } else {
            std::cout<<"No section preferences";
        }
         
        SAGA_OSSTREAM strm;
        strm << "advert::advert_cpi_impl::init: "
        "No AIS URL specified";
        SAGA_ADAPTOR_THROW_NO_CONTEXT(SAGA_OSSTREAM_GETSTRING(strm), 
                                      saga::BadParameter);        
    }
    

    
    /////////////////////////////////////////////////////////////////////////////
    inline void handle_ini_file (saga::ini::ini    & ini, 
                                 std::string const & loc)
    {
        try { 
            ini.read (loc); 
        }
        catch ( saga::exception const & /*e*/ ) { 
            ;
        }
    }
    
    inline void handle_ini_file_env (saga::ini::ini & ini, 
                                     char const           * env_var, 
                                     char const           * file_suffix = NULL)
    {
        const char *env = saga::safe_getenv (env_var);
        if ( NULL != env ) 
        {
            namespace fs = boost::filesystem;
            
            fs::path inipath (env, fs::native);
            
            if ( NULL != file_suffix )
            {
                inipath /= fs::path (file_suffix, fs::native);
            }
            
            SAGA_VERBOSE(SAGA_VERBOSE_LEVEL_INFO) {
                std::cout << "handle_ini_file_env (" 
                << env
                << ", " 
                << file_suffix 
                << ") = " 
                << inipath.string ()
                << "\n";
            }
            
            handle_ini_file (ini, inipath.string ());
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    inline std::string 
    get_filepath(saga::url const& loc)
    {
        return loc.get_path();
    }

      // convert a string attribute in a vector attribute by splitting the 
    // attribute value at ',' symbols
    inline void
    tokenize_values(std::string val, std::vector<std::string>& values)
    {
        typedef boost::char_separator<char> char_separator_type;
        typedef boost::tokenizer<char_separator_type> tokenizer_type;
        
        char_separator_type sep (",");
        tokenizer_type tok (val, sep);
        tokenizer_type::iterator tok_end = tok.end();
        for (tokenizer_type::iterator tit = tok.begin (); tit != tok_end; ++tit)
        {
            values.push_back (*tit);
        }
    }

    // convert a vector attribute into a single value by concatenating the 
    // values into a list separated by ',' symbols
    inline std::string 
    concat_values(std::vector<std::string> const& values)
    {
        std::string result;
        std::vector<std::string>::const_iterator end = values.end();
        for (std::vector<std::string>::const_iterator it = values.begin();
             it != end; /**/)
        {
            result += *it;
            if (++it != end)
                result += ",";
        }
        return result;
    }

    // convert a SAGA pattern into a TR1::regex 
    inline std::string 
    regex_from_character_set(std::string::const_iterator& it, 
        std::string::const_iterator end)
    {
        std::string::const_iterator start = it;
        std::string result(1, *it);  // copy '['
        if (*++it == '!') {
            result.append(1, '^');   // negated character set
        }
        else if (*it == ']') {
            SAGA_ADAPTOR_THROW_NO_CONTEXT(
                "Invalid pattern (empty character set) at: " + 
                std::string(start, end), saga::NoSuccess);
        }
        else {
            result.append(1, *it);   // append this character
        }
        
        // copy while in character set
        while (++it != end) {
            result.append(1, *it);
            if (*it == ']')
                break;
        }
        
        if (it == end || *it != ']') {
            SAGA_ADAPTOR_THROW_NO_CONTEXT(
                "Invalid pattern (missing closing ']') at: " + 
                std::string(start, end), saga::NoSuccess);
        }

        return result;
    }
    
    inline std::string 
    regex_from_string_choice(std::string::const_iterator& it, 
        std::string::const_iterator end)
    {
        std::string::const_iterator start = it;
        std::string result(1, '(');

        // copy while in string choice
        bool has_one = false;
        while (++it != end && *it != '}') {
            if (*it == ',') {
                if (!has_one) {
                    SAGA_ADAPTOR_THROW_NO_CONTEXT(
                        "Invalid pattern (empty string alternative) at: " + 
                        std::string(start, end), saga::NoSuccess);
                }
                result.append(1, '|');
                has_one = false;
            }
            else {
                result.append(1, *it);
                has_one = true;
            }
        }
        
        if (it == end || *it != '}') {
            SAGA_ADAPTOR_THROW_NO_CONTEXT(
                "Invalid pattern (missing closing '}') at: " + 
                std::string(start, end), saga::NoSuccess);
        }
        
        result.append(1, ')');
        return result;
    }
    
    inline std::string 
    regex_from_pattern(std::string const& pattern)
    {
        std::string result;
        std::string::const_iterator end = pattern.end();
        for (std::string::const_iterator it = pattern.begin(); it != end; ++it)
        {
            char c = *it;
            switch (c) {
            case '*':
                result.append(".*");
                break;
                
            case '?':
                result.append(1, '.');
                break;
                
            case '[':
                result.append(regex_from_character_set(it, end));
                break;
                
            case '{':
                result.append(regex_from_string_choice(it, end));
                break;
                
            case '\\':
                if (++it == end) {
                    SAGA_ADAPTOR_THROW_NO_CONTEXT(
                        "Invalid escape sequence at: " + pattern,
                        saga::NoSuccess);
                }
                result.append(1, *it);
                break;
                
            default:
                result.append(1, c);
                break;
            }
        }
        return result;
    }
    
    inline void
    regex_from_patterns(std::vector<std::string> const& patterns,
        std::vector<std::string>& result)
    {
        std::transform(patterns.begin(), patterns.end(), std::back_inserter(result),
            regex_from_pattern);
    }
    
    ///////////////////////////////////////////////////////////////////////////
    inline void 
    parse_pattern(saga::impl::v1_0::cpi const* target, 
        std::string const& pattern, std::string& kpat, std::string& vpat)
    {
        std::string::size_type p = pattern.find_first_of("=");
        if (std::string::npos == p) {
            SAGA_ADAPTOR_THROW_VERBATIM(target, 
                "Pattern has invalid format: " + pattern, saga::BadParameter);
        }
        kpat = pattern.substr(0, p);
        vpat = pattern.substr(p+1);
    }
    
    template <typename Iterator>
    inline void 
    parse_patterns(saga::impl::v1_0::cpi const* target, 
        std::vector<std::string> const& patterns, Iterator kit, Iterator vit)
    {
        std::vector<std::string>::const_iterator end = patterns.end();
        for (std::vector<std::string>::const_iterator it = patterns.begin(); 
             it != end; ++it, ++kit, ++vit)
        {
            std::string kpat, vpat;
            parse_pattern(target, *it, kpat, vpat);
            *kit = kpat;
            *vit = vpat;
        }
    }
    
    inline void 
    parse_patterns(saga::impl::v1_0::cpi const* target, 
        std::vector<std::string> const& patterns, std::vector<std::string>& kpat, 
        std::vector<std::string>& vpat)
    {
        parse_patterns(target, patterns, std::back_inserter(kpat),
            std::back_inserter(vpat));
    }
    
///////////////////////////////////////////////////////////////////////////////
}}  // namespace logicalfile::detail

#endif // !ADAPTORS_MIGOL_LOGICAL_FILE_COMMON_HELPERS_HPP
