//  Copyright (c) 2005-2007 Hartmut Kaiser (hartmut.kaiser@gmail.com)
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(SAGA_EXAMPLE_DAYINLIFE_HPP)
#define SAGA_EXAMPLE_DAYINLIFE_HPP

#include "migrating_application.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace dayinlife 
{
    ///////////////////////////////////////////////////////////////////////////
    //  This is our application from 'A Day in the Life of a SAGA Application'
    //  It mainly overloads some of the hooks provided by the base class 
    //  (migrating::application).
    class application
    :   public migrating::application<application>
    {
    private:
        typedef migrating::application<application> base_type;
        
        int counter_;           // this is our state: the counter in all its glory
        int max_iterations_;    // this is the number of maximal iterations
        std::string next_host_; // host name to migrate this application to
        
    protected:        
        bool must_stop();

    public:
        application(int argc, char *argv[], char const* name);
        ~application();
        
    // functions that are overloaded from the base class
        bool startup();
        void read_data();
        bool compute();
        void write_data();
        void migrate();
        int terminate(bool finished);
    };

///////////////////////////////////////////////////////////////////////////////
} // namespace dayinlife

#endif // !SAGA_EXAMPLE_DAYINLIFE_HPP
