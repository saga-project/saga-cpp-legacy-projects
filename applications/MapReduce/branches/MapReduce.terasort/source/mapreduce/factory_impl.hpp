//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAPREDUCE_FACTORY_IMPL_HPP_
#define MAPREDUCE_FACTORY_IMPL_HPP_

// Common code needed for the different class factories.

#include <map>
#include "factory.hpp"

using ::factory_ns::factory; 
using ::factory_ns::signatures;

namespace factory_ns {

template <class ManufacturedType, typename ClassIDKey=std::string>
class GenericFactory {
  typedef ManufacturedType* (*BaseCreateFn)();
  typedef std::map<ClassIDKey, BaseCreateFn> FnRegistry;

  GenericFactory() {}

 public:
  static GenericFactory& instance() {
    static GenericFactory<ManufacturedType, ClassIDKey> base;
    return base;
  }
  void RegCreateFn(const ClassIDKey& key, BaseCreateFn fn) {
    registry_[key] = fn;
  }
  ManufacturedType* Create(const ClassIDKey &key) const;
  static ManufacturedType* get_by_key(const ClassIDKey &className) {
    const GenericFactory<ManufacturedType, ClassIDKey>& factory =
        GenericFactory<ManufacturedType, ClassIDKey>::instance();
    return factory.Create(className);
  }
 private:
  FnRegistry registry_;
  // Disallow copy constructor and assignment operator.
  GenericFactory(const GenericFactory&) {}
  GenericFactory &operator=(const GenericFactory&) {}
};

template <class ManufacturedType, typename ClassIDKey>
ManufacturedType* GenericFactory<ManufacturedType, ClassIDKey>::Create(
    const ClassIDKey& key) const {
  ManufacturedType* the_object = NULL;
  typename FnRegistry::const_iterator entry = registry_.find(key);
  if (entry != registry_.end()) {
    the_object = entry->second();
  }
  return the_object;
}

template <class AncestorType, class ManufacturedType, typename ClassIDKey=std::string>
class RegisterInFactory {
 public:
  static AncestorType* CreateInstance() {
    return new ManufacturedType;
  }
  RegisterInFactory(const ClassIDKey &id) {
    GenericFactory<AncestorType, ClassIDKey>::instance().RegCreateFn(id, CreateInstance);
  }
};

}   // namespace factory_ns

using factory_ns::GenericFactory;
using factory_ns::RegisterInFactory;

#endif    // MAPREDUCE_FACTORY_IMPL_HPP_
