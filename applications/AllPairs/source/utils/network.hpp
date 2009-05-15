//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AP_NETWORK_HPP
#define AP_NETWORK_HPP

#include <string>
#include <saga/saga.hpp>

namespace AllPairs {
 namespace network {
   class networkException {
    public:
      networkException(const std::string &message);
     ~networkException();
      std::string what() const;
   private:
      std::string msg_;
   };

   std::string read(saga::stream::stream &server);

   void expect(const std::string &expected,
               const std::string &received);

   bool test(const std::string &expected,
             const std::string &received);
 } //network
} //AllPairs

#endif //AP_NETWORK_HPP
