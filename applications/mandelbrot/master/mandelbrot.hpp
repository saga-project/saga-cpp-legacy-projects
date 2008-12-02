
#ifndef MANDELBROT_HPP
#define MANDELBROT_HPP

#include <saga/saga.hpp>

#include "output_base.hpp"
#include "output_x11.hpp"

#define JOBS_PER_SERVICE 2

class mandelbrot 
{
  private:

    struct work_t
    {
      double box_x;
      double box_y;
      double off_x;
      double off_y;
      double res_x;
      double res_y;
      double num_x;
      double num_y;
      int    limit;
      int    ident;
    };

    std::string   odev_;       // name of output device
    output_base * dev_;        // output device
    unsigned int  njobs_;      // number of jobs to use for computing
    bool          running_;    // flag true if compute jobs are running

    std::vector <saga::job::service> job_services_; // list of saga job services
    std::vector <saga::job::job>     jobs_;         // list of saga jobs

  public:
    mandelbrot (void);
    ~mandelbrot (void);

    void set_output (std::string odev);
    void set_njobs  (int n = 1);
    void compute    (void);
};

#endif // MANDELBROT_HPP

