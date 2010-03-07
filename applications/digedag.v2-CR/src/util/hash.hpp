/*
 #  Copyright (c) 2010 Katerina Stamou (kstamou@cct.lsu.edu)
 #
 #  Distributed under the Boost Software License, Version 1.0. (See accompanying
 #  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/


#ifndef DIGEDAG_UTIL_HASH_HPP
#define DIGEDAG_UTIL_HASH_HPP

#include <string>

namespace digedag
{
  namespace util
  {
    class hash {
	private:
        std::stringstream daghash;

    	public:
    	hash (std::string filename);
	~hash (void);
	std::string get_hash (void);
    };
  }
} // namespace digedag

#endif // DIGEDAG_UTIL_HASH_HPP
