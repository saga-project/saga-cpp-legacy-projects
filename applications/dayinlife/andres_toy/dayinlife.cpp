
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
  }
  catch ( const saga::exception & e )
  {
    std::cerr << "catched : " << e.what () << std::endl;
  }

  return (0);
}

