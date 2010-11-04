//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "mapreduce.hpp"

#include <functional>     // workaround for TR1 problem in Boost V1.35
#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <sstream>
#ifndef BL_SUXX
#include <boost/log/formatters/attr.hpp>
#include <boost/log/formatters/date_time.hpp>
#include <boost/log/formatters/format.hpp>
#include <boost/log/formatters/message.hpp>
#include <boost/log/formatters/ostream.hpp>
#include <boost/log/utility/init/to_console.hpp>
#include <boost/log/utility/init/to_file.hpp>
#include <boost/log/utility/init/common_attributes.hpp>
#include <boost/log/filters/attr.hpp>
#include <boost/log/filters/basic_filters.hpp>
#include <boost/log/attributes/attribute_values_view.hpp>
#endif
#include "worker/WorkerThread.hpp"
#include "../utils/logging.hpp"

// Define shorthand notations.
namespace po = boost::program_options;
#ifndef BL_SUXX
namespace fmt = boost::log::formatters;
namespace flt = boost::log::filters;
#endif
using boost::shared_ptr;

namespace mapreduce {
  /*********************************************************
   * parseCommand uses boost program options to parse the  *
   * command lines passed directly from main.  The user may*
   * parse the command lines before passing argv and argc. *
   * ******************************************************/
  bool ParseCommand(int argCount, char *argList[], po::variables_map& vm, bool& is_worker) {
    po::options_description desc_cmdline ("Usage: "+std::string("<mr-client>")+" [options]",0);
    try {
      desc_cmdline.add_options()
        ("help,h", "Display this information and exit")
        ("version,v", "Print version information and exit")
        ("input,i", "Input Data Location")

        // Common master/worker option.
        ("database,d", po::value<std::string>(),
         "Hostname of the orchestrator database")
        // Master-only option.
        ("config,c", po::value<std::string>(),
         "The config filename for this session")
        // Worker-only options.
        ("worker,w", "Behave as a worker")
        ("log,l", po::value<std::string>(),
         "Advert url to log all information to")
        ("session,s", po::value<std::string>(),
         "Session UUID this agent should register with")
        ("output,o", po::value<std::string>(),
         "File url output to")
      ;
      po::positional_options_description p;
      po::store(po::parse_command_line(argCount, argList, desc_cmdline), vm);
      po::notify(vm);
      if (vm.count("help")) {
         std::cout << std::endl << desc_cmdline << std::endl;
         return false;
      }
      
      if (vm.count("input")) {
         std::cout << std::endl << desc_cmdline << std::endl;
         return false;
      }
 
      
      if (vm.count("version")) {
         std::cout << std::endl << SAGA_MAPREDUCE_FRAMEWORK_VERSION << std::endl;
         return false;
      }
      // Check if we must behave as a worker.
      if (vm.count("worker")) {
        is_worker = true;
        if (!vm.count("session")) {
           std::cerr << "Missing session UUID: use --session" << std::endl;
           return false;
        }
        else if (!vm.count("database")) {
           std::cerr << "Missing orchestrator database hostname: use --database"
                     << std::endl;
           return false;
        }
        else if (!vm.count("log")) {
           std::cerr << "Missing log url: use --log" << std::endl;
           return false;
        }
      }
      else if (!vm.count("config")) {
       std::cerr << "Missing config filename: use --config" << std::endl;
             return false;
      }
    }
    catch (std::exception const& e) {
       std::cerr << std::endl << e.what() << std::endl 
                 << std::endl << desc_cmdline << std::endl;
       return false;
    }
    return true;
  }

  // Global command line parameters holder.
  boost::program_options::variables_map g_command_line_parameters;

  /**
   * Initializes the SAGA MapReduce framework. Decides how this executable
   * instance must act; if as a worker, starts up a worker thread.
   */
  void InitFramework(int argCount, char** argList) {
    bool is_worker = false;

    if(!ParseCommand(argCount, argList, g_command_line_parameters, is_worker)) {
      throw saga::exception("Incorrect command line arguments", saga::BadParameter);
    }

    if (is_worker) {
      // Set highest verbosity for SAGA.
      putenv("SAGA_VERBOSE=0");
      // Redirect stderr and stdout.
     srand(time(0));
      int random_integer = rand();
      std::stringstream sserr,ssout,sslog;
      sserr<< "/work/smaddi2/workerop/mr-worker-stderr" <<  g_command_line_parameters["session"].as<std::string>() << "-" << random_integer << ".txt";
      ssout<< "/work/smaddi2/workerop/mr-worker-stdout" <<  g_command_line_parameters["session"].as<std::string>() << "-" << random_integer << ".txt";
      sslog<< "/work/smaddi2/workerop/mr-worker-log" <<  g_command_line_parameters["session"].as<std::string>() << "-" << random_integer << ".txt"; 
      std::freopen(sserr.str().c_str(),"w", stderr);
      std::freopen(ssout.str().c_str(), "w", stdout);
       
      // Init logging.
      std::string log_file(sslog.str().c_str());
    //Log messages with at least INFO level to console.
#ifndef BL_SUXX
      logging::init_log_to_console()->set_filter(
        flt::attr< severity_level >("Severity") >= 1);
      boost::function< void (std::ostream&,
        logging::attribute_values_view const&, std::string const&) > formatter =
        fmt::ostrm
            << fmt::date_time< boost::posix_time::ptime >("TimeStamp") << " ["
            << fmt::attr< severity_level >("Severity") << "] " << fmt::message();
      // Log to file all messages.
      logging::init_log_to_file(log_file)->locked_backend()->set_formatter(
        formatter);
      // Register common attributes.
      logging::add_common_attributes();

      DECLARE_LOGGER(main);
      #endif
      LOG_INFO << "Initializing worker";
      // Run worker thread.
      worker::WorkerThread worker(g_command_line_parameters);
      worker.Run();
      exit(1);
    }
    // Otherwise hand over execution to callee.
  }


}   // namespace mapreduce
