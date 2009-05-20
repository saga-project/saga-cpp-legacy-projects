
#ifndef DIGEDAG_UTIL_SPLIT_HPP
#define DIGEDAG_UTIL_SPLIT_HPP

#include <string>
#include <vector>

namespace digedag
{
  std::vector <std::string> split (std::string line, 
                                   std::string delim = " \t");
} // namespace digedag

#endif // DIGEDAG_UTIL_SPLIT_HPP

