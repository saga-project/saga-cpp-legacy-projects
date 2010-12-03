
#ifndef MANDELBROT_HPP
#define MANDELBROT_HPP

#include <saga/saga.hpp>

#include "output_base.hpp"

// this class encapsulates a master/worker paradigm.  We start
// the jobs, and distribute work items to them.

class mandelbrot 
{
  private:

    std::vector <output_base *>      odevs_;           // output devices

    std::string                      ini_file_;        // control file
    std::vector <saga::job::job>     jobs_;            // list of saga jobs
    std::string                      job_bucket_name_; // name of job directory
    saga::advert::directory          job_bucket_;      // advert container for new jobs 
    saga::advert::directory          done_bucket_;     // advert container for finished jobs 

    // jobs startup
    void job_startup (void);

    // all job buckets are created under that advert directory, by
    // appending the session uid and the client jobnum:
    // saga::advert::directory (<advert_dir>/<session_id>/<client_id>
    std::string advert_dir_;

    // mandelbrot algorithm parameters
    int plane_x_0_;
    int plane_y_0_;
    int plane_x_1_;
    int plane_y_1_;
    int limit_;
    int escape_;

    // work item definitions (box == work item)
    int box_size_x_;
    int box_size_y_;
    int box_num_x_;
    int box_num_y_;




  public:
    mandelbrot   (std::string ini_file); // control file
    ~mandelbrot  (void);

    int compute  (void);
};

#endif // MANDELBROT_HPP

