
#include "util/split.hpp"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace digedag
{
  std::vector <std::string> split (std::string line, 
                                   std::string delim)
  {
    std::vector <std::string> list;
    
    boost::split (list, line, boost::is_any_of (delim), boost::token_compress_on); 
    
    return list;
  }
} // namespace digedag

