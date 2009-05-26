/*  ______ ___  _   _ _____ _____ 
 *  |  ___/ _ \| | | /  ___|_   _|
 *  | |_ / /_\ \ | | \ `--.  | |  
 *  |  _||  _  | | | |`--. \ | |  
 *  | |  | | | | |_| /\__/ / | |  
 *  \_|  \_| |_/\___/\____/  \_/ 
 *  
 *  Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/22/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <iostream>
#include <faust/faust.hpp>

int main (int argc, char* argv[])
{
  
  faust::resource_description localhost_rd;
  
  // you can load a resource description from a file
  localhost_rd.read_from_file("../misc/resource_descriptions/macpro01.cct.lsu.edu.faust");
  
  // create a resource object from the description
  faust::resource localhost (localhost_rd, false);
  
  for(int i=0; i < 100; ++i)
  {
    // read resource monitor attributes
    faust::resource_monitor localhost_rm = localhost.get_monitor();
    
    std::vector<std::string> attributes = localhost_rm.list_attributes();
    
    std::vector<std::string>::const_iterator it;
    for(it = attributes.begin(); it != attributes.end(); ++it)
    {
      std::cout << "monitor attribute: " << (*it) << ": " << std::flush;
      if(localhost_rm.attribute_is_vector(*it)) {
        std::vector<std::string> tmp(localhost_rm.get_vector_attribute(*it));
        for(int i=0; i < tmp.size(); ++i)
          std::cout << tmp.at(i) << ", " << std::flush;
        std::cout << std::endl;
      }
      else
        std::cout << localhost_rm.get_attribute(*it) << std::endl;
    }
    
    sleep(5);
  }

  
  return 0;
}
