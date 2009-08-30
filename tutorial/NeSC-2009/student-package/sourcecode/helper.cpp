//  Copyright (c) 2005-2009 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <unistd.h>                 // getpid/getuid

#include <string>
#include <saga/saga.hpp>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

namespace fs = boost::filesystem;

// This file implements helper functions for the example depending_jobs 

///////////////////////////////////////////////////////////////////////////////
// function allowing to deploy the current executable to the remote machine, 
// where it is supposed to be launched. return the directory name the job got 
// deployed
std::string deploy_me(std::string path, std::string target_host)
{
    // construct unique target directory
    std::string targetdir("/tmp/saga_tutorial/");
    targetdir += boost::lexical_cast<std::string>(getuid());
    targetdir += "/";
    targetdir += boost::lexical_cast<std::string>(getpid());
    targetdir += "/";

    // copy the executable to the remote directory
    fs::path exe(path);

    // Create remote directory
    saga::url target(targetdir);
    target.set_host(target_host);
    saga::filesystem::directory d(target, saga::filesystem::Create|saga::filesystem::Create);

    // copy the executable
    saga::url source(exe.filename());
    saga::filesystem::file f(source);
    f.copy(d.get_cwd());

    return targetdir;
}

///////////////////////////////////////////////////////////////////////////////
// return a user specific advert path
std::string get_advert_result_store(std::string basepath)
{
    basepath += "/";
    basepath += boost::lexical_cast<std::string>(getuid());
    basepath += "/";
    basepath += boost::lexical_cast<std::string>(getpid());
    basepath += "/";

    return basepath;
}
