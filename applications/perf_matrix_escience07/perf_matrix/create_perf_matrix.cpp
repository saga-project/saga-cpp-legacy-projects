//  Copyright (c) 2007 Hartmut Kaiser (hartmut.kaiser@gmail.com)
//  Copyright (c) 2007 Ole Weidner (oweidner@cct.lsu.edu) 
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <fstream>
#include <boost/lexical_cast.hpp>

#include <saga.hpp>
#include <time.h>

#include "create_perf_matrix.hpp"
#include "common_helper.hpp"

///////////////////////////////////////////////////////////////////////////////
static bool establish_client_endpoint(std::string parent, std::string advert_key)
{
    // for netperf: make sure the netserver deamon is installed and available
    using namespace saga;

    try {
        // create and initialize job description for launching a netperf endpoint
        job_description jd;
        jd.set_attribute(attributes::job_description_candidatehosts, "localhost");

        // create command line arguments for "netserver" (the netperf endpoint)
        std::vector<std::string> arguments;
        //arguments.push_back("-p 66666"); // Listen on port 66666
        //arguments.push_back("-4");       // Do IPv4

        //jd.set_vector_attribute(attributes::job_description_arguments,
        //    arguments);
        
        // set the netserver executable
        jd.set_attribute(attributes::job_description_executable, "./netserver");
    
        // Ensure that we use the local job adaptor
        job_service js("fork://localhost"); 
        job j = js.create_job(jd);

        j.run();

        std::string job_id = j.get_job_id();

	common::writelog(advert_key, 
            "Successfuly launched \"netserver\" endpoint for ", 
            parent + " (JobID: " + job_id + ")");
    }

    catch (saga::exception const& e) {
	common::writelog(advert_key, 
            "Failed to launch \"netserver\" endpoint for: ", 
            parent + " (Error: " + std::string(e.what()) + ")");
        return false;
    }

    return true;    
}

///////////////////////////////////////////////////////////////////////////////
static double establish_connection(std::string target, std::string advert_key)
{
    // for netperf: start a netperf session
    using namespace saga;

    float throughput = 0.0;

    saga::url target_url(target); 

    sleep(10);
    try {
        // create and initialize job description for launching a netperf endpoint
        job_description jd;
        jd.set_attribute(attributes::job_description_candidatehosts, "localhost");

        // create command line arguments for "netserver" (the netperf endpoint)
        std::vector<std::string> arguments;
        //arguments.push_back("-p 66666");     // Connect on port 66666
        arguments.push_back(std::string("-H"+target_url.get_host())); // Connect to host
        jd.set_vector_attribute(attributes::job_description_arguments,
            arguments);
       
        // set the netserver executable
        jd.set_attribute(attributes::job_description_executable, "./netperf");

        // this job is interactive
        jd.set_attribute(attributes::job_description_interactive, attributes::common_true);
    
        // I/O handles
        saga::istream my_stdout; //, stderr;

        // Ensure that we use the local job adaptor
        job_service js("fork://localhost"); 
        job j = js.create_job(jd);
        j.run();
        my_stdout = j.get_stdout();

        std::string job_id = j.get_job_id();
	
	int linecount = 1;
	std::string value = "0.0";
        while (my_stdout.good())
        {
	    std::string line;
            std::getline(my_stdout, line);
	    std::cout << line << std::endl;
            if(linecount == 7) // the desired value is in line 7 ;-)
	    {
		std::size_t pos = line.rfind("."); // last decimal point
		value = line.substr(0, pos+3);
		pos = value.rfind(" ");
		value = value.substr(++pos);
		throughput = boost::lexical_cast<float>(value);
            }
 	
            ++linecount;
        }

	common::writelog(advert_key, 
            "Successfuly finished \"netperf\" session with endpoint ", 
            target + " (Throughput: " + value +")");
    }

    catch (saga::exception const& e) {
	common::writelog(advert_key, 
            "Failed to establish \"netperf\" session with endpoint: ", 
            target + " (Error: " + std::string(e.what()) + ")");
        return -1.0;
    }

    return throughput;     // return -1 on failure
}

///////////////////////////////////////////////////////////////////////////////
static int 
store_result(std::string advert_key, int level, std::size_t i, double val, std::string desc,
    std::string& error)
{
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    using namespace saga;
    try {
        std::string key (advert_key);
        key += std::string(PERF_MATRIX_TYPE) + "/M" 
                  + boost::lexical_cast<std::string>(level) 
                  + boost::lexical_cast<std::string>(i);
        advert adv(key, advert::CreateParents | advert::ReadWrite);
        adv.store_string(boost::lexical_cast<std::string>(val));
        adv.set_attribute("description", desc);
    }
    catch (saga::exception const& e) {
        error = e.what();
        std::cerr << "perf_matrix: saga::exception caught: " << e.what () << std::endl;
        return -3;
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
inline void 
add_argument(std::vector<std::string>& arguments, char const* opt, T arg)    
{
    std::string argument(opt);
    argument += boost::lexical_cast<std::string>(arg);
    arguments.push_back(argument);
}

static int 
spawn_target_process(int level, std::string advert_key, std::string target, 
    std::vector<std::string> const& hosts, std::string& error, std::string& job_id)
{
    using namespace saga;
    
    try {
        // create and initialize job description for new job
        job_description jd;
        jd.set_attribute(attributes::job_description_candidatehosts, target);

        // create command line arguments for new job
        std::vector<std::string> arguments;
        add_argument(arguments, "-l", level);
        add_argument(arguments, "-k", advert_key);
        
        typedef std::vector<std::string>::const_iterator iterator;
        iterator end = hosts.end();
        for (iterator it = hosts.begin(); it != end; ++it)
            add_argument(arguments, "-t", *it);
        
#if defined(BOOST_WINDOWS)
//        add_argument(arguments, "-d", "");   // enable debug support in spawned instance
#endif

        jd.set_vector_attribute(attributes::job_description_arguments,
            arguments);
        
        // set the executable to use
        jd.set_attribute(attributes::job_description_executable, 
            PERF_MATRIX_EXECUTABLE);

        // create and run the new job
        job_service js(hosts[level]);
        job j = js.create_job(jd);

        j.run();

        job_id = j.get_job_id();


    }
    catch (saga::exception const& e) {
        error = e.what();
        std::cerr << "perf_matrix: saga::exception caught: " << e.what () << std::endl;
        return -3;
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
//  This function get's a list of url's representing resource to use to 
//  create the needed performance matrix:
//
//                 Resource URL's
//                 hostA   hostB   hostC   hostD
//         hostA     1      AB      AC      AD 
//         hostB     .       1      BC      BD
//         hostC     .       .       1      CD
//         hostD     .       .       .       1
//
//      hostA spawns instance on hostB
//      hostB spawns instance on hostC
//      hostC spawns instance on hostD
//
//  The measured numbers have to be stored in the advert service using the 
//  given parent key:
//
//          advert_key/M12   value for hostA_hostB
//          advert_key/M13   value for hostA_hostC
//          advert_key/M14   value for hostA_hostD
//          advert_key/M23   value for hostB_hostC
//          advert_key/M24   value for hostB_hostD
//          advert_key/M34   value for hostC_hostD
//
int initial_job_spawn(std::string advert_key,
    std::vector<std::string> const& hosts)
{
    //due to globus proxy restriction, the first host (localhost) have to
    //spawn all jobs on the remote host.
    int level = 1;

    std::vector<std::string>::const_iterator c_it;
    for(c_it = hosts.begin()+1; c_it != hosts.end(); ++c_it)
    {
        std::string job_id;
        std::string error;
        int result = spawn_target_process(level, advert_key, (*c_it), hosts, error, job_id);

        if(result) {
            common::writelog(advert_key,
                "Spawning process on host: ",
                    (*c_it) + " FAILED (" + error + ")");
            return result;
        }
        else {
            common::writelog(advert_key,
                "Spawning process on host: ",
                    (*c_it) + " SUCCESSFUL (ID: " + job_id + ")");
       }
       ++level;
    }
    return 0;
}

int 
create_perf_matrix(int level, std::string advert_key, 
    std::vector<std::string> const& hosts)
{
    try {
    // establish the endpoint provided by this instance
        std::string error;
        if (level > 0) {
            common::writelog(advert_key, 
                "Establishing client endpoint for host: ", hosts[level]);

            if (!establish_client_endpoint(hosts[level], advert_key)) {
                return -1;
            }
        }
        
    // to calculate the performance matrix we need to spawn the target 
    // process on the next of the given hosts
    //    if (std::size_t(level+1) < hosts.size()) {
    //        common::writelog(advert_key, 
    //            "Spawning process on host: ", hosts[level+1]);
    //
    //        int result = spawn_target_process(level+1, advert_key, 
    //            hosts[level+1], hosts, error);
    //        if (result) {
    //            common::writelog(advert_key, 
    //                "Failed to spawn process on host: ", 
    //                hosts[level+1] + " (" + error + ")");
    //            return result;
    //        }
    //    }
        
    // connect to all endpoints established by all spawned instances
        for (std::size_t i = level+1; i < hosts.size(); ++i)
        {
	    //if (i == level)
	    // 	continue;
		
            common::writelog(advert_key, 
                "Establishing connection to process on host: ", hosts[i]);

            double val = establish_connection(hosts[i], advert_key);
            if (-1 == val) {
                common::writelog(advert_key, 
                    "Failed to establish connection to process on host: ", 
                    hosts[i] + " (" + error + ")");
                return -1;
            }
        
            std::string desc = hosts[level] + "->" + hosts[i];           

            common::writelog(advert_key, 
                "Storing result for connection: ", desc); 

            int result = store_result(advert_key, level, i, val, desc, error);
            if (result) {
                common::writelog(advert_key, 
                    "Failed to store result for connection: ", 
                    hosts[level] + "-->" + hosts[i] + " (" + error + ")");
                return result;
            }
        }
    }
    catch (std::exception const& e) {
        std::cerr << "perf_matrix: std::exception caught: " << e.what() << std::endl;
        return -2;
    }
    return 0;
}
