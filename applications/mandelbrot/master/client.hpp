
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <iostream>
#include <fstream>

#include <saga/saga.hpp>

#include "endpoint.hpp"


class client 
{
  public:
    client  (std::string                  name, 
             saga::job::job               job, 
             boost::shared_ptr <endpoint> ep);
    ~client (void);


    std::string                  name_;
    std::string                  id_;
    saga::job::job               job_;
    boost::shared_ptr <endpoint> ep_;
    std::stringstream            log_;
};

#endif // CLIENT_HPP

