
#include <saga/saga.hpp>
#include "../ini.hpp"

int main ()
{
  mb_util::ini::ini ini ("./test_ini.ini");

  ini.dump ();

  return (0);
}

