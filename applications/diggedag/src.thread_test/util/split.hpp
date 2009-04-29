
#ifndef DIGGEDAG_UTIL_SPLIT_HPP
#define DIGGEDAG_UTIL_SPLIT_HPP

#include <string>
#include <vector>

namespace diggedag
{
  std::vector <std::string> split (std::string line, 
                                   std::string delim = " \t");
} // namespace diggedag

#endif // DIGGEDAG_UTIL_SPLIT_HPP

