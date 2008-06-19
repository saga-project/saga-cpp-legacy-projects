//  Copyright (c) 2007 Hartmut Kaiser (hartmut.kaiser@gmail.com)
//  Copyright (c) 2007 Ole Weidner (oweidner@cct.lsu.edu)
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <boost/program_options.hpp>

#include <fstream>

#if defined(BOOST_WINDOWS)
#include <windows.h>
#endif

#include "create_perf_matrix.hpp"

namespace po = boost::program_options;

///////////////////////////////////////////////////////////////////////////////
bool parse_commandline(int argc, char *argv[], po::variables_map& vm)
{
    try {
    po::options_description desc_cmdline ("Usage: perf_matrix [options]");
        
        desc_cmdline.add_options()
            ("help,h", "print out program usage (this message)")
            ("key,k", po::value<std::string>(), 
                "name of the advert key to store the results to")
            ("target,t", po::value<std::vector<std::string> >()->composing(), 
                "host name to use for creating the matrix")
            ("level,l", po::value<int>()->default_value(0), 
                "invocation level (default: 0)")
#if defined(BOOST_WINDOWS)
            ("debug,d", "enable debug support")
#endif
        ;

    po::positional_options_description p;

        po::store(po::parse_command_line(argc, argv, desc_cmdline), vm);
        po::notify(vm);

    // verify all needed arguments are given
        if (!vm.count("key")) {
            std::cerr << "Missing name of key in advert service: use --key" 
                      << std::endl;
            return false;
        }
        
    // verify we got at least 2 hosts
        if (!vm.count("target")) {
            std::cerr << "Please specify at least two target hosts: use --target" 
                      << std::endl;
            return false;
        }

    // print help screen
        if (vm.count("help")) {
            std::cout << desc_cmdline;
            return false;
        }
    }
    catch (std::exception const& e) {
        std::cerr << "perf_matrix: exception caught: " << e.what() << std::endl;
        return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    //sleep(10);

    try {
    // analyze the command line
        po::variables_map vm;
        if (!parse_commandline(argc, argv, vm))
            return -2;

#if defined(BOOST_WINDOWS)
        if (vm.count("debug"))
            DebugBreak();
#endif

    // extract command line arguments 
    int level (vm["level"].as<int>()); 
    std::string advert_key (vm["key"].as<std::string>());
    

    std::vector<std::string> hosts = 
        vm["target"].as<std::vector<std::string> >();



        if (hosts.size() < 2) {
            std::cerr << "Please specify at least two target hosts: use --target" 
                      << std::endl;
            return -2;
        }
        
        if( level == 0 )
        {
          initial_job_spawn(advert_key, hosts);
        }
        return create_perf_matrix(level, advert_key, hosts);
    }
    catch (...) {
        std::cerr << "perf_matrix: unexpected exception caught" << std::endl;
        return -1;
    }
    return 0;
}

#if defined(BOOST_WINDOWS)
///////////////////////////////////////////////////////////////////////////////
//  initialize Winsock library
struct init_winsocket
{
    init_winsocket()
    {
        WSADATA data_;
        WSAStartup(MAKEWORD(2, 2), &data_);
    }
    ~init_winsocket()
    {
        WSACleanup();
    }
};
init_winsocket init_winsock_lib;    // instantiate during startup
#endif
    
