/*
    Author: Marco Costalba (C) 2007-2008


 To compile:

	(gcc)  g++ -O2 -o test test.cpp
	(MSVC) cl /Ox /EHs test.cpp
*/
#include "factory.hpp"

#include <cassert>
#include <iostream>
#include <string>

using std::cout;
using std::endl;

/* Some stuff we will use in this demonstration */
class ParentClass;

struct BaseClass {
    BaseClass() { cout << "BaseClass() called" << endl; }
    BaseClass(ParentClass*) { cout << "BaseClass(ParentClass*) called" << endl; }
    virtual ~BaseClass() {}

    BaseClass(BaseClass const&); // check for hidden temporaries
};

struct DerivedClass : public BaseClass {
    DerivedClass() { cout << "DerivedClass() called" << endl; }
    DerivedClass(int v) { cout << "DerivedClass(int) called with value " << v << endl; }
    DerivedClass(int, std::string s) {
       cout << "DerivedClass(int, std::string) called with value \"" << s << "\"" << endl;
    }
};

struct OtherClass {
    OtherClass() { cout << "OtherClass() called" << endl; }
    OtherClass(int& v) { cout << "OtherClass(int&) called, arg incremented to " << ++v << endl; }
    OtherClass(BaseClass const&) { cout << "OtherClass(BaseClass const&) called" << endl; }
};


/* Before to use a factory we have to declare it, declare
 * constructors signatures for each class and register them
 * with the factory.
 *
 * Note that here typedefs are involved, so no allocation
 * or variable is created: zero overhead at runtime, only
 * when factories are used real code is instantiated
 */


/* Declare factories we are going to use, each factory will
 * be able to create objects belonging to the same hierarchy,
 * i.e. all objects must derive from the same base class.
 *
 * For each factory we must declare the corresponding
 * base class and the key type used to query the
 * factory, we pass both as template parameters.
 *
 * Only requirement on key type is that must be equally
 * comparable so to support '==' operator.
 */
using factory_ns::factory;

typedef factory<BaseClass, std::string> factory1;
typedef factory<OtherClass, int> factory2;

/* Declare constructor signatures of each class */
using factory_ns::signatures;

typedef signatures<BaseClass(), BaseClass(ParentClass*)> sigs1;
typedef signatures<OtherClass(), OtherClass(int&), OtherClass(BaseClass const&)> sigs2;
typedef signatures<DerivedClass(), DerivedClass(int), DerivedClass(int, std::string)> sigs3;

/* Finally register each class with the factory, shall be done
 * at namespace scope with macro
 *
 * REGISTER_CLASS(class, factory, key value, ctor signatures, index)
 *
 * The index is used to get a class at compile time, shall be unique
 * for each class of the same factory and satisfy the relation
 *
 *            0 <= index < MAX_FACTORY_SIZE
 *
 * where MAX_FACTORY_SIZE is defined in factory.hpp
 *
 * REGISTER_CLASS does not instantiate any code.
 */
static const int otherClassKey = 7;

REGISTER_CLASS(DerivedClass, factory1, "derived", sigs3, 6)
REGISTER_CLASS(BaseClass, factory1, "base", sigs1, 1)
REGISTER_CLASS(OtherClass, factory2, otherClassKey, sigs2, 1)

using factory_ns::byRef; // to pass arguments by reference, see below


/* Now we can start to use our factories, note that until
 * now no code has been instantiated.
 */
int main(int, char**)
{
    int v = 7;
    ParentClass* p = NULL;

    /* Get object by a compile time constant, 100% resolved
     * at compile time, zero overhead at runtime, a compile
     * error occurs if index or arguments are wrong
     */
    cout << "Resolving links at compile time...\n" << endl;

    DerivedClass* d = new factory1::class_id<6>::get();
    d = new factory1::class_id<6>::get(v);

    cout << endl << endl;

    BaseClass bc = factory1::class_id<1>::get();
    BaseClass* b = new factory1::class_id<1>::get(p);

    cout << endl << endl;

    OtherClass* o = new factory2::class_id<1>::get();
    OtherClass oc = factory2::class_id<1>::get(v);
    oc = factory2::class_id<1>::get(bc);

    assert(b && d && o);

    /* Get object by name, resolved at runtime as should be
     * supported by a real factory. Allows class names and
     * c'tor arguments to be stored in a variable.
     *
     * Function get_by_key() returns a pointer to requested
     * object upcasted the the factory base class.
     *
     * In case class is not found or arguments don't match
     * the registered ones then a NULL pointer is returned.
     */
    std::string base_name("base"), part("der");

    cout << "\n\nResolving links at run-time time...\n" << endl;

    BaseClass*  o1 = factory1::get_by_key(base_name);
    BaseClass*  o2 = factory1::get_by_key(part + "ived");
    OtherClass* o3 = factory2::get_by_key(otherClassKey);

    bool r1 = NULL != dynamic_cast<BaseClass*>(o1);
    bool r2 = NULL != dynamic_cast<DerivedClass*>(o2);
    bool r3 = NULL != dynamic_cast<OtherClass*>(o3);

    assert(r1 && r2 && r3);

    cout << endl << endl;

    o1 = factory1::get_by_key(base_name, p);
    o2 = factory1::get_by_key("derived", v);
    o3 = factory2::get_by_key(otherClassKey, bc); // compile error if bc copy ctor is called

    r1 = NULL != dynamic_cast<BaseClass*>(o1);
    r2 = NULL != dynamic_cast<DerivedClass*>(o2);
    r3 = NULL != dynamic_cast<OtherClass*>(o3);

    assert(r1 && r2 && r3);

    cout << "\n\nPassing arguments by reference is supported...\n\n";

    int old_v = v;

    // helper reference wrapper byRef must be used in this case
    o3 = factory2::get_by_key(otherClassKey, byRef(v));

    r1 = NULL != dynamic_cast<OtherClass*>(o3);

    assert(r1 && (v == old_v + 1));

    cout << "\n\nArguments implicit conversions is also supported...\n" << endl;

    o2 = factory1::get_by_key("derived", 3, "from const char* to std::string");

    r2 = NULL != dynamic_cast<DerivedClass*>(o2);

    assert(r2);

    cout << "\n\nTest passed succesfully\n\n";

    return 0;
}
