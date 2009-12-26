
#include <iostream>
#include <saga/saga.hpp>

#include "dag.hpp"
#include "config.hpp"

#undef USE_BOOST

#ifdef USE_BOOST
# include <boost/shared_ptr.hpp>
  using namespace boost;
#else
# include "util/shared_ptr.hpp"
  using namespace digedag;
  using namespace digedag::util;
#endif

#define ESFT

class A 
#ifdef ESFT
  : public enable_shared_from_this <A>
#endif
{
  public:
    A (void)
#ifdef ESFT
#endif
    {
      std::cout << " A: ctor" << std::endl;
    }

    ~A (void)
    {
      std::cout << " A: dtor" << std::endl;
    }

    void a (void)
    {
      std::cout << " A: a" << std::endl;
    }

#ifdef ESFT
    shared_ptr <A> sp (void)
    {
      std::cout << " A: sp" << std::endl;
      return shared_from_this ();
    }
#endif
};

int main ()
{
  try 
  {

    A * a = new A;

    std::cout << " = 1 ============================== " << a << std::endl;

    shared_ptr <A> a1 = shared_ptr <A> (a);

    std::cout << " = 2 ============================== " << &a1 << std::endl;

#ifdef ESFT
    shared_ptr <A> a2 = a1->sp ();

    std::cout << " = 3 ============================== " << &a2 << std::endl;

    a2->a ();
#endif

  }
  catch ( const saga::exception & e )
  {
  }

  return (0);
}

