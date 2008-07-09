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
#define DAYINLIFE_MAX_ITERATIONS      2

#define DAYINLIFE_DATA_INFILE         "counter.in"
#define DAYINLIFE_DATA_OUTFILE        "counter.out"

///////////////////////////////////////////////////////////////////////////////
namespace common
{
    ///////////////////////////////////////////////////////////////////////////
    // generate the full paths of the local copies of the input and output 
    // data files
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
  : migrating::application<application>(argc, argv, name),
    counter_(0), max_iterations_(DAYINLIFE_MAX_ITERATIONS)
{
    // set the file names to use for the local copies of our data file
    this->base_type::set_input_filename(common::get_infile_name());
    this->base_type::set_output_filename(common::get_outfile_name());

    // the first two arguments are the next resource manager and the next 
    // host name; if there is a 3rd parameter it gives the number of times
    // this application should be instantiated
    if (argc > 3)
        max_iterations_ = boost::lexical_cast<int>(argv[3]);
}

///////////////////////////////////////////////////////////////////////////////
// This is called after successful initialization of the application
void application::read_data()
{
    // The base class provides us with a local copy of the data file as 
    // written by the previous instance of this application
    saga::filesystem::file infile;
    if (this->base_type::begin_read_data(infile)) 
    {
        // the file exists, read the data we need
        char data[255];
        saga::ssize_t bytes = infile.read(saga::buffer(data));
        data[common::minval(bytes, sizeof(data)-1)] = '\0';
        counter_ = boost::lexical_cast<int>(data);

        this->base_type::end_read_data(infile);
    }
    else {
        // this is the first instance of this application, so no data exists 
        // yet, restart the application
        counter_ = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Here we have to do the useful work
bool application::compute()
{
    if (this->base_type::compute()) 
    {
        ++counter_;                           // increment the counter
        return counter_ > max_iterations_;    // return true if the application should terminate
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// This is called just after the compute() function returned
void application::write_data()
{
    // The base class provides us with a saga::filesystem::file object we can 
    // use to store our data in
    saga::filesystem::file outfile;
    if (this->base_type::begin_write_data(outfile)) 
    {
        outfile.write(buffer(boost::lexical_cast<std::string>(counter_)));
        this->base_type::end_write_data(outfile);
    }
}

///////////////////////////////////////////////////////////////////////////////
// This is called only if the compute function returned true
void application::migrate()
{
    // we need to provide additional arguments to supply to the newly spawned
    // application instance, these arguments will be put on the comman line of 
    // starting at position three
    std::vector<std::string> arguments;
    arguments.push_back(boost::lexical_cast<std::string>(max_iterations_));
    this->base_type::migrate(arguments);
}

///////////////////////////////////////////////////////////////////////////////
}   // namespace dayinlife
