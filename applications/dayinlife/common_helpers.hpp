//  Copyright (c) 2005-2007 Hartmut Kaiser (hartmut.kaiser@gmail.com)
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(SAGA_EXAMPLE_DAYINLIFE_COMMON_HELPERS_HPP)
#define SAGA_EXAMPLE_DAYINLIFE_COMMON_HELPERS_HPP

#include <ctime>
#include <cstdlib>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/lexical_cast.hpp>

#include <saga/saga.hpp>

#ifndef   MAX_PATH
# ifdef   SAGA_APPLE
#  define MAX_PATH PATH_MAX
# else
#  define MAX_PATH _POSIX_PATH_MAX
# endif
#endif

///////////////////////////////////////////////////////////////////////////////
namespace common
{
    template <typename T1, typename T2>
    inline T1 minval (T1 const& t1, T2 const& t2)
    {
        return (t1 < (T1)t2) ? t1 : (T1)t2;
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
    //  return the current resource manager responsible for this job
    inline std::string current_rm()
    {
        saga::url host("any:");   // we don't care which resource manager to use
        host.set_host(get_hostname());
        return host.get_url();    // for now we just assume a local job 
    }
    
    ///////////////////////////////////////////////////////////////////////////
    //  return the current resource this job is running on
    inline std::string current_host()
    {
        return get_hostname();
    }
    
    ///////////////////////////////////////////////////////////////////////////
    //  return the next resource to migrate this job to
    inline std::string next_host()
    {
        return get_hostname();    // for now we just restart the job locally
    }
    
    ///////////////////////////////////////////////////////////////////////////
    //  return the name of the current user
    inline std::string get_username()
    {
        std::string result;
#if defined(BOOST_WINDOWS)
        DWORD cchBuff = MAX_PATH;          // size of user name
        char chBuffer[MAX_PATH + 1];       // buffer for expanded string
        GetUserName (chBuffer, &cchBuff);  // Get and display the user name.
        result = chBuffer;
#else
        result = boost::lexical_cast<std::string>(getuid());
#endif
        return result;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    inline std::string get_current_time()
    {
        std::time_t current = 0;
        std::time(&current);
        std::string current_time(std::ctime(&current));
        current_time.resize(current_time.size()-1);
        return current_time;
    }
    
    inline std::string get_jobid()
    {
        using namespace saga;
        
        job::service js(current_rm());
        job::self self = js.get_self();
        return self.get_job_id();
    }
    
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
    //  generate logfilename paths
    inline std::string get_log_path(std::string const& name)
    {
        SAGA_OSSTREAM log;
        log << "/tmp/" << name << get_username() << "/" ;
        return SAGA_OSSTREAM_GETSTRING(log); 
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // supporting functions        
    template <typename T1>
    inline void 
    writelog(std::string const& name, T1 const& t1)
    {
        using namespace saga;

        try {
#if defined(DAYINLIFE_LOG_TO_ADVERT)
            int mode = advert::ReadWrite | advert::CreateParents;
            SAGA_OSSTREAM adv;
            adv << name << "/" << get_hostname() << ":" << getpid() << ":"
                << get_sequence_number();
            advert logadv(SAGA_OSSTREAM_GETSTRING(adv), mode);

            SAGA_OSSTREAM log;
            log << /*get_jobid() << ":" << */t1;
            logadv.store_string(SAGA_OSSTREAM_GETSTRING(log));
#elif defined(BOOST_WINDOWS) && defined(DAYINLIFE_LOG_TO_DEBUG)
            SAGA_OSSTREAM log;
            log << get_current_time() << ":" << get_jobid() << ":" 
                << get_sequence_number() << ":"
                << t1 << std::endl;

            OutputDebugString(SAGA_OSSTREAM_GETSTRING(log).c_str());
#else
            SAGA_OSSTREAM log;
            log << get_current_time() << ":" << get_jobid() << ":" 
                << get_sequence_number() << ":"
                << t1 << std::endl;
                
            int directory_mode = filesystem::Create | 
                                 filesystem::CreateParents | 
                                 filesystem::ReadWrite;
            filesystem::directory  logdir (name, directory_mode);

            int mode = filesystem::ReadWrite | filesystem::Create 
                                             | filesystem::Append;
            filesystem::file logf = logdir.open (name + "Log", mode);
            std::string b(SAGA_OSSTREAM_GETSTRING(log));
            logf.write(const_buffer(const_cast<saga::uint8_t*>((const saga::uint8_t*)(b.c_str())), b.size()), b.size());
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
#if defined(DAYINLIFE_LOG_TO_ADVERT)
            int mode = advert::ReadWrite | advert::CreateParents;
            SAGA_OSSTREAM adv;
            adv << name << "/" << get_hostname() << ":" << getpid() << ":"
                << get_sequence_number();
            advert logadv(SAGA_OSSTREAM_GETSTRING(adv), mode);

            SAGA_OSSTREAM log;
            log << /*get_jobid() << ":" << */t1 << t2;
            logadv.store_string(SAGA_OSSTREAM_GETSTRING(log));
#elif defined(BOOST_WINDOWS) && defined(DAYINLIFE_LOG_TO_DEBUG)
            SAGA_OSSTREAM log;
            log << get_current_time() << ":" << get_jobid() << ":" 
                << get_sequence_number() << ":"
                << t1 << t2 << std::endl;
                
            OutputDebugString(SAGA_OSSTREAM_GETSTRING(log).c_str());
#else
            SAGA_OSSTREAM log;
            log << get_current_time() << ":" << get_jobid() << ":" 
                << get_sequence_number() << ":"
                << t1 << t2 << std::endl;
                
            int directory_mode = filesystem::Create | 
                                 filesystem::CreateParents | 
                                 filesystem::ReadWrite;
            filesystem::directory  logdir (name, directory_mode);

            int mode = filesystem::ReadWrite | filesystem::Create 
                                             | filesystem::Append;
            filesystem::file logf = logdir.open (name + "Log", mode);

            std::string b(SAGA_OSSTREAM_GETSTRING(log));
            logf.write(buffer(const_cast<saga::uint8_t*>((const saga::uint8_t*)(b.c_str())), b.size()), b.size());
#endif
        }
        catch (saga::exception const& e) {
            std::cerr << "caught saga::exception while logging: " << e.what() 
                      << std::endl;
        }
    }
    
}   // namespace 

#endif // !SAGA_EXAMPLE_DAYINLIFE_COMMON_HELPERS_HPP
