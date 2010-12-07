
#ifndef JOB_STARTER_HPP
#define JOB_STARTER_HPP

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


  public:
    job_starter  (std::string       a_dir,  // advert directory (== job bucket)
                  mb_util::ini::ini ini);   // control data
    ~job_starter (void);

    std::vector <boost::shared_ptr <client> > get_clients (void)
    {
      return clients_;
    }

};
#endif // JOB_STARTER_HPP

