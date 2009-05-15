//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "network.hpp"

namespace AllPairs {
 namespace network {
   networkException::networkException(const std::string &message) : msg_(message) {}
   networkException::~networkException() {}
   std::string networkException::what() const { return msg_; }

   std::string read(saga::stream::stream &server)
   {
      char buff[255];
      saga::ssize_t read_bytes = server.read(saga::buffer(buff));
      return std::string(buff, read_bytes);
   }

   void expect(const std::string &expected,
               const std::string &received)
   {
      if(expected != received) {
         std::string error("Expected: ");
         error += expected;
         error += ", Received: ";
         error += received;
         throw(networkException(error));
      }
   }

   bool test(const std::string &expected,
             const std::string &received)
   {
      if(expected == received) {
         return true;
      }
      return false;
   }
 } //network
} //AllPairs
