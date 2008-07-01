//  Copyright (c) 2005-2008 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <string>
#include <cstdlib>

#include <boost/lexical_cast.hpp>

#include <saga/saga.hpp>

#include "dayinlife.hpp"
#include "common_helpers.hpp"

using namespace saga;

///////////////////////////////////////////////////////////////////////////////
#define DAYINLIFE_MAX_ITERATIONS      3

#define DAYINLIFE_DATA_INFILE         "counter.in"
#define DAYINLIFE_DATA_OUTFILE        "counter.out"

///////////////////////////////////////////////////////////////////////////////
namespace common
{
    ///////////////////////////////////////////////////////////////////////////
    //  generate the full paths of the input and output files
    inline std::string get_infile_name()
    {
        using namespace boost::filesystem;
        path cwd (initial_path());
        cwd /= DAYINLIFE_DATA_INFILE;
        return cwd.string();
    }
    
    inline std::string get_outfile_name()
    {
        using namespace boost::filesystem;
        path cwd (initial_path());
        cwd /= DAYINLIFE_DATA_OUTFILE;
        return cwd.string();
    }
    
///////////////////////////////////////////////////////////////////////////////
}   // namespace common

///////////////////////////////////////////////////////////////////////////////
namespace dayinlife
{

application::application(int argc, char *argv[], char const* name)
:   migrating::application<application>(argc, argv, name),
    counter_(0), max_iterations_(DAYINLIFE_MAX_ITERATIONS), 
    next_rm_(common::current_host()), next_host_(common::current_host())
{
    // if there were parameters on the command line we interpret these as the
    // resource manager and next host name where we should migrate to
    if (argc > 1) {
        next_rm_ = next_host_ = argv[1];
        if (argc > 2)
            next_host_ = argv[2];
    }
}

application::~application()
{
}

///////////////////////////////////////////////////////////////////////////////
//  This is called at application startup. If the return value is false, 
//  terminate will be called without any further action.
bool application::startup()
{
    common::writelog(common::get_log_path(this->get_name()), 
        "application::startup");
    
    // verify we don't have to exit right away
    if (must_stop()) 
        return false;

    // the base class implements all the necessary functionality 
    return base_type::startup();
}

///////////////////////////////////////////////////////////////////////////////
// This is called after successful initialization of the application
void application::read_data()
{
    common::writelog(common::get_log_path(this->get_name()), 
        "application::read_data");

    // try to locate data file, restart if not available
    try {
    // the data file is registered with the advert service
        advert::entry adv (common::get_db_path(this->get_name(), "Counter"));
        replica::logical_file lf (adv.retrieve_object());

        if (0 != lf.list_locations().size()) {
        // create the local input file name
            saga::url local_url("any:");
            local_url.set_host(common::get_hostname());
            local_url.set_path(common::get_infile_name());
            
        // create local replica of the data file
            lf.replicate(local_url.get_url());

        // read counter from local file
            filesystem::file infile (local_url.get_url());
            char indata[255];
            saga::ssize_t bytes = infile.read(buffer(indata));
            
            indata[common::minval(bytes, sizeof(indata)-1)] = '\0';
            counter_ = boost::lexical_cast<int>(indata);

        // remove the file when done
            infile.remove();
        }
        else {
        // replica entry does not contain any entries
            counter_ = 0;
        }
        
    // remove replica all together
        lf.remove();
    }
    catch (saga::exception const& e) {
        if (BadParameter != e.get_error() && DoesNotExist != e.get_error())
            throw;    // something more serious
        // data file is not locatable, just restart
        counter_ = 0;
    }

    common::writelog(common::get_log_path(this->get_name()), 
        "application::read_data: counter: ", counter_);
}

///////////////////////////////////////////////////////////////////////////////
// Here we have to do the useful work
bool application::compute()
{
    common::writelog(common::get_log_path(this->get_name()), 
        "application::compute");

    ++counter_;     // increment the counter
//     sleep(1);       // sleep for some time to pretend doing more useful things
    
    // return true if the application should terminate
    return counter_ > max_iterations_;
}

///////////////////////////////////////////////////////////////////////////////
// This is called just after the compute() function returned
void application::write_data()
{
    common::writelog(common::get_log_path(this->get_name()), 
        "application::write_data: counter: ", counter_);

    // create url of output file
    saga::url local_url("any:");
    local_url.set_host(common::get_hostname());
    local_url.set_path(common::get_outfile_name());

    // write counter to a the new output file
    filesystem::file outfile(local_url.get_url(), 
        filesystem::Create | filesystem::Truncate | filesystem::ReadWrite);
    outfile.write(buffer(boost::lexical_cast<std::string>(counter_)));
    outfile.close();
    
    // create a new replica and add the output file 
    std::string lfn (common::get_db_path(this->get_name(), "Data"));
    replica::logical_file lf (lfn, 
        replica::CreateParents | replica::ReadWrite);
    lf.add_location(local_url.get_url());
    
    // store the replica into the advert service
    advert::entry adv(common::get_db_path(this->get_name(), "Counter"), 
        advert::CreateParents | advert::ReadWrite);
    adv.store_object(lf);
}

///////////////////////////////////////////////////////////////////////////////
// This is called only if the compute function returned true
void application::migrate()
{
    common::writelog(common::get_log_path(this->get_name()), 
        "application::migrate");

    // get our own job description
    job::service js(common::current_rm(next_rm_));
    job::job self (js.get_self());
    job::description jd (self.get_description());

    // modify job description to point to the next resource
    jd.set_attribute(job::attributes::description_candidatehosts, 
        next_host_);

    // set the current host as a command line argument for the new instance
    // this will make this application get migrated back to the current host
    std::vector<std::string> arguments;
    arguments.push_back(next_rm_);
    arguments.push_back(next_host_);
    jd.set_vector_attribute(job::attributes::description_arguments,
        arguments);

    // migrate this job to the new machine (given by next_host())
    self.migrate(jd);

    // the base class implements additional functionality 
    base_type::migrate(next_host_);
}

///////////////////////////////////////////////////////////////////////////////
// This is called at application shutdown. The parameter is true, if this is
// the last instance of the application (it's the return value of compute()).
int application::terminate(bool finished)
{
    common::writelog(common::get_log_path(this->get_name()), 
        "application::terminate: finished: ", finished);

    return base_type::terminate(finished);    // base class needs to be called
}

///////////////////////////////////////////////////////////////////////////////
//  Helper functions

//  look into the advert service to find out, if this application has to stop 
//  (that's a debug aid)
bool application::must_stop()
{
    try {
        advert::entry advhost (common::get_db_path(this->get_name(), "Stop"));
        std::string stop (advhost.retrieve_string());
        return boost::lexical_cast<int>(stop) != 0;
    }
    catch (saga::exception const& /*e*/) {
        // no Stop key is available, run normal
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
}   // namespace dayinlife
