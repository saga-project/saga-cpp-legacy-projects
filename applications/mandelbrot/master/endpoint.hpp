
#ifndef SAGA_MANDELBROT_ENDPOINT_HPP
#define SAGA_MANDELBROT_ENDPOINT_HPP

#include <string>
#include <iostream>
#include <fstream>

#include <saga/saga.hpp>

#include "ini.hpp"


class endpoint 
{
  public:
    endpoint  (std::string           name,
               mb_util::ini::section ini);
    ~endpoint (void);


    std::string             name_;
    saga::job::service      service_;
    mb_util::ini::section   ini_;
    std::stringstream       log_;
    bool                    valid_;

    std::string             url_;
    std::string             ctype_;
    std::string             user_;
    std::string             pass_;
    std::string             cert_;
    std::string             key_;
    std::string             proxy_;
    std::string             cadir_;
    std::string             exe_;
    std::string             args_;
    std::string             pwd_;

    unsigned int            njobs_;
    unsigned int            cnt_jreq_; // #jobs requested
    unsigned int            cnt_jrun_; // #jobs run
    unsigned int            cnt_jreg_; // #jobs registered
    unsigned int            cnt_iass_; // #items assigned
    unsigned int            cnt_iok_ ; // #items done

    bool                    debug_;
};

#endif // SAGA_MANDELBROT_ENDPOINT_HPP

