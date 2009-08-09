//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UTILS_LOGGING_HPP_
#define UTILS_LOGGING_HPP_

#include <boost/log/core.hpp>
#include <boost/log/sources/severity_logger.hpp>
//#include <boost/log/sources/global_logger_storage.hpp>

namespace mapreduce {

// Severity levels needed.
enum severity_level {
  debug,
  info,
  warning,
  error,
  critical
};

// The formatting logic for the severity level.
template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT >& operator<< (
    std::basic_ostream< CharT, TraitsT >& strm, severity_level lvl)
{
    static const char* const str[] =
    {
        "DEBUG",
        "INFO",
        "WARNING",
        "ERROR",
        "CRITICAL"
    };
    if (static_cast< std::size_t >(lvl) < (sizeof(str) / sizeof(*str)))
        strm << str[lvl];
    else
        strm << static_cast< int >(lvl);
    return strm;
}

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace src = boost::log::sources;

typedef src::severity_logger<severity_level> logger;

#define DECLARE_LOGGER(clazz)  ::mapreduce::logger log_

#define LOG_INFO  BOOST_LOG_SEV(log_, info)
#define LOG_WARN  BOOST_LOG_SEV(log_, warning)
#define LOG_ERROR BOOST_LOG_SEV(log_, error)
#define LOG_DEBUG BOOST_LOG_SEV(log_, debug)

}   // namespace mapreduce

#endif  // UTILS_LOGGING_HPP_
