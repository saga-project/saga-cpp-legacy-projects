
#ifndef JOB_STARTER_HPP
#define JOB_STARTER_HPP

#include <string>
#include <vector>

#include <saga/saga.hpp>

class job_starter 
{
  private:

    class endpoint_
    {
      public:
        endpoint_ (std::string  name,
                   std::string  url,
                   std::string  ctype,
                   std::string  user,
                   std::string  pass,
                   std::string  cert,
                   std::string  key,
                   std::string  proxy,
                   std::string  cadir,
                   std::string  exe,
                   std::string  pwd, 
                   std::string  njobs);

        saga::job::service service_;
        std::string        name_;
        std::string        url_;
        std::string        ctype_;
        std::string        user_;
        std::string        pass_;
        std::string        cert_;
        std::string        key_;
        std::string        proxy_;
        std::string        cadir_;
        std::string        exe_;
        std::string        pwd_;
        unsigned int       njobs_;
    };



    std::vector <endpoint_>           endpoints_;
    std::vector <saga::job::service>  services_;
    std::vector <saga::job::job>      jobs_;

  public:
    job_starter  (std::string  a_dir);  // advert directory (== job bucket)
    ~job_starter (void);

    std::vector <saga::job::job> get_jobs (void)
    {
      return jobs_;
    }

};
#endif // JOB_STARTER_HPP

