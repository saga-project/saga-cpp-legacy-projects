
#ifndef MANDELBROT_HPP
#define MANDELBROT_HPP

#include <saga/saga.hpp>

#include "output_base.hpp"
// this class encapsulates a master/worker paradigm.  We start
// the jobs, and distribute work items to them.
class mandelbrot 
{
  private:

    std::string   odev_;     // name of output device
    output_base * dev_;      // output device

    std::vector <saga::job::job>     jobs_;            // list of saga jobs
    std::string                      job_bucket_name_; // name of job directory
    saga::advert::directory          job_bucket_;      // advert container for new jobs 
    saga::advert::directory          done_bucket_;     // advert container for finished jobs 

    // jobs startup
    void job_startup (void);

  public:
    mandelbrot  (std::string  odev  = "x11"); // default output device
    ~mandelbrot (void);

    void compute    (void);
};

#endif // MANDELBROT_HPP

