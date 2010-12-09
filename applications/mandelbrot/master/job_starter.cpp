
#include <sstream>
#include <iostream>
#include <fstream>

#include <saga/saga/adaptors/utils.hpp>

#include "job_starter.hpp"


//////////////////////////////////////////////////////////////////////
job_starter::job_starter (std::string       a_dir, 
                          mb_util::ini::ini ini)
  : ini_ (ini)
{
  // first, initialize all endpoints according to the ini file
  mb_util::ini::section   cfg      = ini_.get_section ("mandelbrot");
  mb_util::ini::section   ep_cfg   = cfg.get_section  ("backends");
  mb_util::ini::entry_map backends = ep_cfg.get_entries ();
  mb_util::ini::entry_map :: iterator it;

  for ( it = backends.begin (); it != backends.end (); it++ )
  {
    std::string key = (*it).first;
    std::string val = (*it).second;

    if ( val == "yes" )
    {
      mb_util::ini::section backend_config = ep_cfg.get_section (key);

      std::string url = backend_config.get_entry ("url"  , "");
      std::cout << "creating  endpoint " << key << " \t ... " << std::flush;

      boost::shared_ptr <endpoint> ep (new endpoint (key, backend_config));

      if ( ep->valid_ )
      {
        endpoints_.push_back (ep);
        std::cout << "ok (" << url << ")" << std::endl;
      }
      else
      {
        std::cout << "failed (" << url << ")" << std::endl;
      }
    }
  }


  // for each endpoint, we run 'njobs' jobs.  
  for ( unsigned int e = 0; e < endpoints_.size (); e++ )
  {
    // try the next endpoint
    boost::shared_ptr <endpoint> ep = endpoints_[e];

    if ( ep->valid_ )
    {
      for ( unsigned int j = 0; j < ep->njobs_; j++ )
      {
        try
        {
          int         jobnum = clients_.size ();
          std::string ident  = boost::lexical_cast <std::string> (jobnum);

          // create a job description
          saga::job::description jd;
          jd.set_attribute (saga::job::attributes::description_executable, ep->exe_);

          // client parameters:
          // 0: path to advert directory to be used (job bucket)
          // 1: jobnum, == name of work bucket for that job (is that in loop later)
          std::vector <std::string> args;
          args.push_back ("mandelbrot_client ");
          args.push_back (a_dir);
          args.push_back (ident);

          // append ep args
          std::vector <std::string> epargs = saga::adaptors::utils::split (ep->args_, ' ');
          for ( unsigned int a = 0; a < epargs.size (); a++ )
          {
            args.push_back (epargs[a]);
          }


          jd.set_vector_attribute (saga::job::attributes::description_arguments, args);

          if ( ! ep->pwd_.empty () )
          {
            jd.set_attribute (saga::job::attributes::description_working_directory, ep->pwd_);
          }

          // let the clients store stdout/stderr to /tmp/mandelbrot_client.[id].out/err
          // FIXME: this should get enabled once the bes adaptor supports it, and
          // is able to stage the output files back into the pwd
          # if 0
          {
            std::string out;
            std::string err;

            out += "/tmp/mandelbrot_client." + ident + ".out";
            err += "/tmp/mandelbrot_client." + ident + ".err";

            jd.set_attribute (saga::job::attributes::description_output, out);
            jd.set_attribute (saga::job::attributes::description_error,  err);
          }
          # endif

          ep->cnt_j1_++;
          std::cout << "starting  job "
                    << ident
                    << " on "
                    << ep->name_ 
                    << " \t ... " << std::flush;

          saga::job::job j = ep->service_.create_job (jd);

          j.run ();

          if ( saga::job::Running != j.get_state () )
          {
            std::cout << "failure - could not run " 
                      << ep->exe_ << " " << ep->args_ 
                      << std::endl;
            j.cancel (); // clean up resources

            ep->log_ << "spawning failed [1]: " << ep->exe_ << " " << ep->args_ << "\n";

            // do not use this job
          }
          else
          {
            // keep job (wrapped in client)
            boost::shared_ptr <client> c (new client (ident, j, ep)); 
            clients_.push_back (c);
            client_map_[c->id_] = c;

            // store full jobid in ep log
            ep->cnt_j2_++;
            std::cout << "ok "              << c->id_short_ << std::endl;
            ep->log_  << "spawned client "  << jobnum 
                      << " on "             << ep->name_ 
                      << ": "               << c->id_short_
                      << "\n";
          }
        }
        catch ( const saga::exception & e )
        {
          std::cout << "failure - could not start exe " << ep->exe_ << " " << ep->args_ 
                    << std::endl;
          ep->log_ << "spawning failed [2]: " << ep->exe_ << " " << ep->args_ << "\n" << e.what () << "\n\n";
        }
      }
    }
  }

  if ( clients_.size () == 0 )
  {
    throw "Could not start any jobs!";
  }
}


job_starter::~job_starter (void)
{
}

