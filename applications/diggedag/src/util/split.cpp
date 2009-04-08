
#include "util/split.hpp"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace diggedag
{
  std::vector <std::string> split (std::string line)
  {
    std::vector <std::string> list;
    
    boost::split (list, line, boost::is_any_of (" \t")); 
    
    return list;
  }
} // namespace diggedag

