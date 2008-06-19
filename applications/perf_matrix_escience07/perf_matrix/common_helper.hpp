//  Copyright (c) 2007 Hartmut Kaiser (hartmut.kaiser@gmail.com)
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(PERF_MATRIX_COMMON_HELPERS_JUN_18_2007_0118PM)
#define PERF_MATRIX_COMMON_HELPERS_JUN_18_2007_0118PM

#include <ctime>
#include <cstdlib>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/lexical_cast.hpp>

#include <saga.hpp>

#ifndef   MAX_PATH
# ifdef   __APPLE__
#  define MAX_PATH PATH_MAX
# else
#  define MAX_PATH _POSIX_PATH_MAX
# endif
#endif

namespace common
{
    ///////////////////////////////////////////////////////////////////////////
    inline std::string get_current_time()
    {
        std::time_t current = 0;
        std::time(&current);
        std::string current_time(std::ctime(&current));
        current_time.resize(current_time.size()-1);
        return current_time;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // wrapper for gethostname()
    inline std::string get_hostname()
    {
        char buffer[MAX_PATH] = { '\0' };
        gethostname(buffer, sizeof(buffer));
        return std::string(buffer);
    }
    
    ///////////////////////////////////////////////////////////////////////////
    inline int get_sequence_number()
    {
        static int sequence = 0;
        return ++sequence;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    //  generate advert paths
    inline std::string get_db_path(std::string const& name, char const* node)
    {
        SAGA_OSSTREAM adv;
        adv << "/" << name << "/" << node;
        return SAGA_OSSTREAM_GETSTRING(adv); 
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // supporting functions        
    template <typename T1>
    inline void 
    writelog(std::string const& name, T1 const& t1)
    {
        using namespace saga;

        try {
#if defined(PERF_MATRIX_LOG_TO_ADVERT)
            int mode = advert::ReadWrite | advert::CreateParents;
            SAGA_OSSTREAM adv;
            adv << name << PERF_MATRIX_TYPE << "/log/" << get_hostname() << ":" 
                << getpid() << ":" << get_sequence_number();
            advert logadv(SAGA_OSSTREAM_GETSTRING(adv), mode);

            SAGA_OSSTREAM log;
            log << t1;
            logadv.store_string(SAGA_OSSTREAM_GETSTRING(log));
#elif defined(BOOST_WINDOWS) && defined(PERF_MATRIX_LOG_TO_DEBUG)
            SAGA_OSSTREAM log;
            log << get_current_time() << ":" << getpid() << ":" 
                << get_sequence_number() << ":"
                << t1 << std::endl;

            OutputDebugString(SAGA_OSSTREAM_GETSTRING(log).c_str());
#else
            SAGA_OSSTREAM log;
            log << get_current_time() << ":" << getpid() << ":" 
                << get_sequence_number() << ":"
                << t1 << std::endl;
                
            int directory_mode = directory::Create | directory::CreateParents | 
                directory::ReadWrite;
            directory  logdir (PERF_MATRIX_TYPE, directory_mode);

            int mode = file::ReadWrite | file::Create | file::Append;
            file logf = logdir.open (std::string(PERF_MATRIX_TYPE) + "Log", mode);
            std::string b(SAGA_OSSTREAM_GETSTRING(log));
            logf.write(b.size(), buffer(const_cast<saga::uint8_t*>((const saga::uint8_t*)(b.c_str())), b.size()));
#endif
        }
        catch (saga::exception const& e) {
            std::cerr << "caught saga::exception while logging: " << e.what() 
                      << std::endl;
        }
    }
    
    template <typename T1, typename T2>
    inline void 
    writelog(std::string const& name, T1 const& t1, T2 const& t2)
    {
        using namespace saga;
        
        try {
#if defined(PERF_MATRIX_LOG_TO_ADVERT)
            int mode = advert::ReadWrite | advert::CreateParents;
            SAGA_OSSTREAM adv;
            adv << name << PERF_MATRIX_TYPE << "/log/" << get_hostname() << ":" 
                << getpid() << ":" << get_sequence_number();
            advert logadv(SAGA_OSSTREAM_GETSTRING(adv), mode);

            SAGA_OSSTREAM log;
            log << t1 << t2;
            logadv.store_string(SAGA_OSSTREAM_GETSTRING(log));
#elif defined(BOOST_WINDOWS) && defined(PERF_MATRIX_LOG_TO_DEBUG)
            SAGA_OSSTREAM log;
            log << get_current_time() << ":" << getpid() << ":" 
                << get_sequence_number() << ":"
                << t1 << t2 << std::endl;
                
            OutputDebugString(SAGA_OSSTREAM_GETSTRING(log).c_str());
#else
            SAGA_OSSTREAM log;
            log << get_current_time() << ":" << getpid() << ":" 
                << get_sequence_number() << ":"
                << t1 << t2 << std::endl;
                
            int directory_mode = directory::Create | directory::CreateParents | 
                directory::ReadWrite;
            directory  logdir (PERF_MATRIX_TYPE, directory_mode);

            int mode = file::ReadWrite | file::Create | file::Append;
            file logf = logdir.open (std::string(PERF_MATRIX_TYPE) + "Log", mode);

            std::string b(SAGA_OSSTREAM_GETSTRING(log));
            logf.write(b.size(), buffer(const_cast<saga::uint8_t*>((const saga::uint8_t*)(b.c_str())), b.size()));
#endif
        }
        catch (saga::exception const& e) {
            std::cerr << "caught saga::exception while logging: " << e.what() 
                      << std::endl;
        }
    }

///////////////////////////////////////////////////////////////////////////////
}

#endif
