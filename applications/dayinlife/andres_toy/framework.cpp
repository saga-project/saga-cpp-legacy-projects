
#include "framework.hpp"

#include <iostream>
#include <fstream>
#include <vector>

#define DEFAULT_APP_NAME    "DayInLife"
#define ADVERT_DIR          "/workload/"

// fw ctor
framework::framework (logger & log, int argc, char** argv)
  : log_ (log)
{
  log_.log (" -- fw ctor\n");
  if ( argc >= 2 )
  {
    name_ = argv[1];
  }
  else
  {
    // default name and max counter
    name_ = DEFAULT_APP_NAME;
  }
}


// fw dtor
framework::~framework (void)
{
  log_.log (" -- fw dtor\n");
}


// fw main routine.  
// 
// input: a valid workload.
//
// run only returns when the wl is finished - that may be in another (migrated)
// instance!
void framework::run (workload & w)
{
  log_.log (" -- fw run\n");

  // replicate input data to the worker location (i.e. to localhost)
  prepare_data ();

  // communicate input and output file locations to the worker
  w.set_input  (in_.get_string ());  // tell worker where to retr. data
  w.set_output (out_.get_string ()); // tell worker where to store data

  // let worker do whatever it does
  w.work ();

  // check if worker is done
  if ( w.test () )
  {
    // if not, register the output data files, ...
    collect_data ();

    // and migrate elsewhere, to continue
    migrate ();

    // after migration, this instance is done -- we should never reach this line
    // throw "migrated away";
  }

  return;
}


// prepare_data stages input data to localhost
//
// The data are available via a logical_file, which in turn is stored in an
// advert entry.
void framework::prepare_data (void)
{
  log_.log (" -- fw prepare_data\n");

  // determine input/output file names...
  in_  = "input.dat";
  out_ = "output.dat";

  // try to open the advert and to get the logical file
  

  // open the advert's parent directory
  saga::advert::directory  ad      (std::string (ADVERT_DIR), saga::advert::ReadWrite | saga::advert::Create);

  if ( ! ad.exists (name_) )
  {
    // on the very first run, there is no advert, and no logical file.  Thus we
    // initate the logical file name, for later creation of these entities.
    lfn_ = std::string ("/") + name_;
  }
  else
  {
    saga::advert::entry ae = ad.open (name_, saga::advert::Read);

    // connect to the logical file
    saga::replica::logical_file lf (ae.retrieve_object ());

    // store logical file url for later use
    lfn_ = lf.get_url ();

    // fetch to the input location, i.e. localhost
    lf.replicate (in_);

    // unregister input location: no idea what workload is doing with the file,
    // so we consider it invalid.  Control is now with the worker, which should
    // delete it after use.
    lf.remove_location (in_);
  }
}



// colect_date is actually the inverse to prepare_date: the workers output data
// are registered  to the logical file, which is in turn stored in the adv ert
// service.
void framework::collect_data (void)
{
  // if there is no input, yet, we need to create a new logical file
  log_.log (" -- fw collect_data: ");
  log_.log (lfn_);
  log_.log ("\n");

  // register the output file location, after removing all other locations (we
  // assume that the output data are the only valid data)
  saga::replica::logical_file lf (lfn_, saga::replica::ReadWrite | saga::replica::Create);
  std::vector <saga::url> urls = lf.list_locations ();

  for ( int i = 0; i < urls.size (); i++ )
  {
     lf.remove_location (urls[i]);
  }

  // regisgter output data 
  lf.add_location (out_);

  // open the advert
  saga::advert::directory  ad      (std::string (ADVERT_DIR), saga::advert::ReadWrite | saga::advert::Create);
  saga::advert::entry ae = ad.open (name_,                    saga::advert::ReadWrite | saga::advert::Create);

  // store the logical file
  ae.store_object (lf);

  // output data are stored, we can finish
}



// if the worker did not finish its job, we migrate ourself to a new location,
// and continue work there
void framework::migrate (void)
{
  // get job service, and job handle to THIS application instance
  saga::job::service     js;
  saga::job::self        self = js.get_self ();
  saga::job::description jd   = self.get_description ();

  // add a new host as target host.  That's boring localhost for now (FIXME)
  std::vector <std::string> chosts;
  chosts.push_back ("localhost");
  jd.set_vector_attribute (saga::job::attributes::description_candidatehosts, chosts);

  log_.log (" -- fw migrate: ");
  log_.log (self.get_job_id ());
  log_.log (" -> ");
  log_.log (chosts[0]);
  log_.log ("\n");

  // and off we go!
  self.migrate (jd);

  // after migration, this instance is done - we should never reach this line
}

