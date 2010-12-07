
#ifndef SAGA_MANDELBROT_JOB_STARTER_HPP
#define SAGA_MANDELBROT_JOB_STARTER_HPP

#include <map>
#include <string>
#include <vector>

#include <saga/saga.hpp>

#include "ini.hpp"
#include "endpoint.hpp"
#include "client.hpp"

class job_starter 
{
  private:
    mb_util::ini::section   ini_;
    std::vector <boost::shared_ptr <endpoint> > endpoints_;
    std::vector <boost::shared_ptr <client>   > clients_;

    // the client_map_ allows to map from a job id to the respective client
    // instance
    std::map    <std::string, boost::shared_ptr <client> > client_map_;


  public:
    job_starter  (void) { }                 // default c'tor
    job_starter  (std::string       a_dir,  // advert directory (== job bucket)
                  mb_util::ini::ini ini);   // control data
    ~job_starter (void);

    std::vector <boost::shared_ptr <client> > get_clients (void)
    {
      return clients_;
    }

    boost::shared_ptr <client> get_client (std::string id)
    {
      return client_map_[id];
    }

};
#endif // SAGA_MANDELBROT_JOB_STARTER_HPP

