
#ifndef SAGA_MANDELBROT_CLIENT_HPP
#define SAGA_MANDELBROT_CLIENT_HPP

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

    saga::job::state get_state (void);
    void             cancel    (void);

    std::string                  name_;     // client num
    std::string                  id_;       // cached job id
    std::string                  id_short_; // shortened job id
    saga::job::job               job_;      // client job instance
    boost::shared_ptr <endpoint> ep_;       // spawning endpoint
    std::stringstream            log_;      // logger
    unsigned int                 cnt_a_;    // #items assigned
    unsigned int                 cnt_d_;    // #items done
    bool                         valid_;    // client is usable
};

#endif // SAGA_MANDELBROT_CLIENT_HPP

