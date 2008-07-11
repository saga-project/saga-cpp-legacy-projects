
#include <saga/saga.hpp>

#include <iostream>

#include "framework.hpp"
#include "workload.hpp"
#include "logger.hpp"

int main (int argc, char** argv)
{
  // our main creates a application framework, and a workload for it.
  // And then, behold, it runs it! ;-)
  //
  // The fw will run the workload, and migrate itself until the workload
  // finishes whatever it is doing... 
  try 
  {
    logger    l  ("dayinlife.log");
    framework fw (l, argc, argv);
    workload  wl (l);

    fw.run (wl);

    l.log ("done\n");
  }
  catch ( const saga::exception & e )
  {
    std::cerr << "Catched : " << e.what () << std::endl;
  }

  return (0);
}

