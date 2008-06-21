//  Copyright (c) 2005-2007 Hartmut Kaiser (hartmut.kaiser@gmail.com)
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(SAGA_EXAMPLE_MIGRATING_APPLICATION_HPP)
#define SAGA_EXAMPLE_MIGRATING_APPLICATION_HPP

#include <string>
#include <saga/saga.hpp>

#include "common_helpers.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace migrating 
{
    ///////////////////////////////////////////////////////////////////////////
    //  This class provides the basic functionality to:
    //    - register the running application (job) with the advert service
    //    - register the current host with the advert service (overlaps with 
    //      migration to next host
    //    - register the next host with the advert service
    template <typename Derived> 
    class application
    {
        Derived& derived() { return static_cast<Derived&>(*this); }
        
    public:
        application(int argc, char *argv[], char const* name) 
        :   name_(name)
        {}
        ~application() {}

    // do what has to be done :-P
        int run()
        {
            Derived& d = derived();
            bool finished = true;
            
            if (d.startup()) {
                d.read_data();                  // read input data
                finished = d.compute();         // do useful stuff
                d.write_data();                 // write output data
                if (!finished)
                    d.migrate();                // migrate if not finished
            }                
            return d.terminate(finished);
        }
        
        std::string const& get_name() const { return name_; }
        
    // placeholders for functions that may be overloaded by the derived class

    //  This is called at application startup. If the return value is false, 
    //  terminate will be called without any further action.
        bool startup() 
        { 
            using namespace saga;
            int mode = advert::CreateParents | advert::ReadWrite;

            // store our host name in the advert service to allow others to 
            // find out, where we are.
            advert::entry advhost (common::get_db_path(name_, "Host"), mode);
            advhost.store_string(common::current_host());

            // remove the next host name from the advert service
            advert::entry advnext (common::get_db_path(name_, "NextHost"), mode);
            advnext.remove();
            
            // store our job in the advert service as well. This flags the
            // application as currently running.
            job::service js (common::current_rm());
            advert::entry advjob (common::get_db_path(name_, "Job"), mode);
            advjob.store_object (js.get_self());

            return true; 
        }
        
    // This is called after successful initialization of the application
        void read_data() 
        {
        }
        
    // Here we have to do the useful work
        bool compute() 
        { 
            return true; 
        }
        
    // This is called just after the compute() function returned
        void write_data() 
        {
        }
        
    // This is called if the compute function returned true
        void migrate(std::string const& next_hostname) 
        {
            using namespace saga;
            int mode = advert::CreateParents | advert::ReadWrite;
            
            // store the next host name in the advert service
            advert::entry advnext (common::get_db_path(name_, "NextHost"), mode);
            advnext.store_string(next_hostname);
            
            // remove our job from the advert service during shutdown. This flags 
            // there is no application currently running.
            advert::entry advjob (common::get_db_path(name_, "Job"), advert::ReadWrite);
            advjob.remove();
        }
        
    // This is called at application shutdown. The parameter is true, if this is
    // the last instance of the application (it's the return value of compute()).
        int terminate(bool finished) 
        { 
            if (finished) {
                using namespace saga;
                int mode = advert::ReadWrite;
                
            // remove our job from the advert service during shutdown. This 
            // flags there is no application currently running.
                advert::entry advjob (common::get_db_path(name_, "Job"), mode);
                advjob.remove();

            // remove our host name in the advert service as well
                advert::entry advhost (common::get_db_path(name_, "Host"), mode);
                advhost.remove();
            }
            return 0; 
        }
        
    private:
        std::string name_;
    };
    
///////////////////////////////////////////////////////////////////////////////
}   // namespace migrating

#endif // !SAGA_EXAMPLE_MIGRATING_APPLICATION_HPP


