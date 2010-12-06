
#ifndef JOB_STARTER_HPP
#define JOB_STARTER_HPP

#include <string>
#include <vector>

#include <saga/saga.hpp>
#include "ini.hpp"

class job_starter 
{
  private:

    class endpoint_
    {
      public:
        endpoint_ (std::string           name,
                   mb_util::ini::section ini);

        saga::job::service service_;
        std::string             name_;
        mb_util::ini::section   ini_;

        std::string        url_;
        std::string        ctype_;
        std::string        user_;
        std::string        pass_;
        std::string        cert_;
        std::string        key_;
        std::string        proxy_;
        std::string        cadir_;
        std::string        exe_;
        std::string        args_;
        std::string        pwd_;
        unsigned int       njobs_;
    };



    mb_util::ini::section             ini_;
    std::vector <endpoint_>           endpoints_;
    std::vector <saga::job::service>  services_;
    std::vector <saga::job::job>      jobs_;

  public:
    job_starter  (std::string       a_dir,  // advert directory (== job bucket)
                  mb_util::ini::ini ini);   // control data
    ~job_starter (void);

    std::vector <saga::job::job> get_jobs (void)
    {
      return jobs_;
    }

};
#endif // JOB_STARTER_HPP

