//  Copyright (c) 2005-2008 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(SAGA_EXAMPLE_MIGRATING_APPLICATION_HPP)
#define SAGA_EXAMPLE_MIGRATING_APPLICATION_HPP

#include <string>
#include <vector>
#include <algorithm>

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
          : name_(name), next_rm_(common::current_host()), 
            next_host_(common::current_host())
        {
        // if there are parameters on the command line we interpret these as the
        // next resource manager and next host name where we should migrate to
            if (argc > 1) {
                next_host_ = next_rm_ = argv[1];
                if (argc > 2)
                    next_host_ = argv[2];
            }
        }
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

    // set next host and next resource manager
        void set_next_host(std::string const& nexthost)
        {
            next_host_ = nexthost;
        }
        void set_next_rm(std::string const& nextrm)
        {
            next_rm_ = nextrm;
        }

    // set names for input and output filenames to use as the local copies of 
    // our data file
        void set_input_filename(std::string const& infile_name)
        {
            input_filename_ = infile_name;
        }
        void set_output_filename(std::string const& outfile_name)
        {
            output_filename_ = outfile_name;
        }

    // placeholders for functions that may be overloaded by the derived class

    //  This is called at application startup. If the return value is false, 
    //  terminate will be called without any further action.
        bool startup() 
        { 
            // write log
            common::writelog(common::get_log_path(this->get_name()), 
                "application::startup");

            using namespace saga;
            try {
                // verify we don't have to exit right away
                if (must_stop()) 
                    return false;

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
            }
            catch (saga::exception const& e) {
                common::writelog(common::get_log_path(this->get_name()), 
                    "application::startup: failed: ", e.what());
                return false;
            }
            return true; 
        }

    // This is called after successful initialization of the application
        void read_data()
        {
            saga::filesystem::file infile;
            if (begin_read_data(infile)) 
                end_read_data(infile);
        }

        bool begin_read_data(saga::filesystem::file& infile) 
        {
            common::writelog(common::get_log_path(this->get_name()), 
                "application::begin_read_data");

            // try to locate data file, restart if not available
            using namespace saga;
            try {
            // the data file is registered with the advert service
                advert::entry adv (common::get_db_path(this->get_name(), "Counter"));
                replica::logical_file lf (adv.retrieve_object());

                if (0 != lf.list_locations().size()) {
                // create the local input file name
                    saga::url local_url("any:");
                    local_url.set_host(common::get_hostname());
                    local_url.set_path(input_filename_);
                    
                // create local replica of the data file
                    lf.replicate(local_url.get_url());

                // read counter from local file
                    infile = filesystem::file(local_url.get_url());
                    return true;
                }
                else {
                // replica entry does not contain any entries
                // fall through, return false
                }
            }
            catch (saga::exception const& e) {
                if (BadParameter != e.get_error() && DoesNotExist != e.get_error())
                    throw;    // something more serious
                // data file is not locatable, just restart
            }
            return false;
        }

        void end_read_data(saga::filesystem::file& infile)
        {
            using namespace saga;

            // try to locate data file, restart if not available
            try {
                infile.close();     // close the file
                infile.remove();    // we don't need this anymore

            // the data file is registered with the advert service
                advert::entry adv (common::get_db_path(this->get_name(), "Counter"));
                replica::logical_file lf (adv.retrieve_object());

            // remove replica all together
                lf.remove();
            }
            catch (saga::exception const& e) {
                if (BadParameter != e.get_error() && DoesNotExist != e.get_error())
                    throw;    // something more serious
                // data file is not locatable, just restart
            }

            common::writelog(common::get_log_path(this->get_name()), 
                "application::end_read_data");
        }

    // Here we have to do the useful work
        bool compute() 
        { 
            common::writelog(common::get_log_path(this->get_name()), 
                "application::compute");
            return true; 
        }

    // This is called just after the compute() function returned
        void write_data()
        {
            saga::filesystem::file outfile;
            if (begin_write_data(outfile)) 
                end_write_data(outfile);
        }

        bool begin_write_data(saga::filesystem::file& outfile) 
        {
            common::writelog(common::get_log_path(this->get_name()), 
                "application::begin_write_data");

            using namespace saga;
            try {
                // create url of output file
                saga::url local_url("any:");
                local_url.set_host(common::get_hostname());
                local_url.set_path(output_filename_);

                // write counter to a the new output file
                outfile = filesystem::file(local_url.get_url(), 
                    filesystem::Create | filesystem::Truncate | filesystem::ReadWrite);
                return true;
            }
            catch (saga::exception const& e) {
                common::writelog(common::get_log_path(this->get_name()), 
                    "application::begin_write_data: failed: ", e.what());
            }
            return false;
        }

        void end_write_data(saga::filesystem::file& outfile)
        {
            common::writelog(common::get_log_path(this->get_name()), 
                "application::end_write_data");

            using namespace saga;
            try {
                outfile.close();    // close the file

                // create a new replica and add the output file 
                std::string lfn (common::get_db_path(this->get_name(), "Data"));
                replica::logical_file lf (lfn, 
                    replica::CreateParents | replica::ReadWrite);
                lf.add_location(outfile.get_url());

                // store the replica into the advert service
                advert::entry adv(common::get_db_path(this->get_name(), "Counter"), 
                    advert::CreateParents | advert::ReadWrite);
                adv.store_object(lf);
            }
            catch (saga::exception const& e) {
                common::writelog(common::get_log_path(this->get_name()), 
                    "application::end_write_data: failed: ", e.what());
            }
        }

    // This is called if the compute function returned true
        void migrate()
        {
            migrate(std::vector<std::string>());
        }

        void migrate(std::vector<std::string> const& args) 
        {
            using namespace saga;
            try {
                common::writelog(common::get_log_path(this->get_name()), 
                    "application::migrate");

                // get our own job description
                job::service js(common::current_rm(next_rm_));
                job::job self (js.get_self());
                job::description jd (self.get_description());

                // modify job description to point to the next resource
                std::vector <std::string> chosts;
                chosts.push_back (next_host_);
                jd.set_vector_attribute(job::attributes::description_candidate_hosts, 
                    chosts);

                // set the current host as a command line argument for the new 
                // instance this will make this application get migrated back 
                // to the current host
                std::vector<std::string> arguments;
                arguments.push_back(next_rm_);
                arguments.push_back(next_host_);
                std::copy(args.begin(), args.end(), std::back_inserter(arguments));
                jd.set_vector_attribute(job::attributes::description_arguments,
                    arguments);

                // we do this before the migrate to avoid race conditions 
                // with the new job accessing the same entry 
                int mode = advert::CreateParents | advert::ReadWrite;

                // store the next host name in the advert service
                advert::entry advnext (common::get_db_path(name_, "NextHost"), mode);
                advnext.store_string(next_host_);

                // remove our job from the advert service during shutdown. This
                // flags that there is no application running anymore.
                advert::entry advjob (common::get_db_path(name_, "Job"), advert::ReadWrite);
                advjob.remove();

                // migrate this job to the new machine (given by next_host())
                self.migrate(jd);
            }
            catch (saga::exception const& e) {
                common::writelog(common::get_log_path(this->get_name()), 
                    "application::migrate: failed: ", e.what());
            }
        }

    // This is called at application shutdown. The parameter is true, if this is
    // the last instance of the application (it's the return value of compute()).
        int terminate(bool finished) 
        { 
            common::writelog(common::get_log_path(this->get_name()), 
                "application::terminate: finished: ", finished);

            try {
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
            }
            catch (saga::exception const& e) {
                common::writelog(common::get_log_path(this->get_name()), 
                    "application::terminate: failed: ", e.what());
            }
            return 0; 
        }

    protected:
        //  look into the advert service to find out, if this application has to stop 
        //  (that's a debug aid)
        bool must_stop()
        {
            using namespace saga;
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

    private:
        std::string name_;
        std::string next_rm_;
        std::string next_host_;
        std::string input_filename_;
        std::string output_filename_;
    };

///////////////////////////////////////////////////////////////////////////////
}   // namespace migrating

#endif // !SAGA_EXAMPLE_MIGRATING_APPLICATION_HPP


