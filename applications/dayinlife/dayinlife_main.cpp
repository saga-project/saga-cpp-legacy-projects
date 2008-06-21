//  Copyright (c) 2005-2007 Hartmut Kaiser (hartmut.kaiser@gmail.com)
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <saga/saga.hpp>

#include "dayinlife.hpp"
#include "common_helpers.hpp"

static char const* const app_name = "DayInLife";

///////////////////////////////////////////////////////////////////////////////
//  main application entry point
int main(int argc, char *argv[])
{
    // The main work will be done in the application class instantiated below.
    // We mainly will do error handling here.
    try {
        dayinlife::application app(argc, argv, app_name);
        return app.run();
    }
    catch (saga::exception const& e) {
        common::writelog(common::get_log_path(app_name), 
            "caught saga::exception: ", e.what());
        std::cerr << "caught saga::exception: " << e.what() << std::endl;
        return -3;
    }
    catch (std::exception const& e) {
        common::writelog(common::get_log_path(app_name), 
            "caught std::exception: ", e.what());
        std::cerr << "caught std::exception: " << e.what() << std::endl;
        return -2;
    }
    catch (...) {
        common::writelog(common::get_log_path(app_name), 
            "caught unexpected exception");
        std::cerr << "caught unexpected exception" << std::endl;
        return -1;
    }
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
    
