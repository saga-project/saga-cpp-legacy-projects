/*
 *  exception.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/22/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_EXCEPTION_HPP
#define FAUST_EXCEPTION_HPP

#include <exception>
#include <iostream>
#include <string>

#include <faust/faust/object.hpp>

namespace faust 
{
  enum error {
    NotImplemented        = 1,
    IncorrectURL          = 2,
    BadParameter          = 3,
    AlreadyExists         = 4,
    DoesNotExist          = 5,
    IncorrectState        = 6,
    PermissionDenied      = 7,
    AuthorizationFailed   = 8,
    AuthenticationFailed  = 9,
    Timeout               = 10,
    NoSuccess             = 11,
  };
  
  ///@cond - exclude from Doxygen
  char const* const error_names[] = {
    "Success",
    "NotImplemented",
    "IncorrectURL",
    "BadParameter",
    "AlreadyExists",
    "DoesNotExist",
    "IncorrectState",
    "PermissionDenied",
    "AuthorizationFailed",
    "AuthenticationFailed",
    "Timeout",
    "NoSuccess"
  };
 
  
  
  class FAUST_EXPORT exception : public std::exception
  {
private:
  
  std::string msg_;
  mutable faust::error err_;
  faust::object object_;
  
  
public:
  
  /*! \brief todo: add doc
   *
   */
  exception (faust::object obj, std::string const& m, error e = NoSuccess) 
  : msg_(""), err_(e), object_(obj)
  {
    
    // prefix the message with the FAUST exception name (only if it's not 
    // already there)
    std::string::size_type p = m.find("FAUST(");
    if (std::string::npos == p || 0 != p)
      msg_ = std::string("FAUST(") + error_names[e] + "): ";
    msg_ += m;
  }
  
  /*! \brief todo: add doc
   *
   */
  explicit exception (std::string const& m, error e = NoSuccess) 
  : msg_(""), err_(e)
  {      
    // prefix the message with the SAGA exception name (only if it's not 
    // already there)
    std::string::size_type p = m.find("FAUST(");
    if (std::string::npos == p || 0 != p)
      msg_ = std::string("FAUST(") + error_names[e] + "): ";
    msg_ += m;
  }
  
  /*! \brief Destroys the exception.    
   *
   *
   */
  ~exception() throw() {}
  
  /*! \brief Brief %description starts here     
   *
   *
   */
  char const* what() const throw() 
  { 
    return (msg_.c_str ());
  }
  
  /*! \brief Gets the message associated with the exception 
   * \return The error message.
   *
   *
   */
  char const* get_message() const throw() 
  { 
    return what();
  }
  
  /*! \brief Brief %description starts here     
   *
   *
   */
  faust::error get_error () const  
  { 
    return err_; 
  }
  
  /*! \brief Gets the SAGA object associated with exception. 
   * \return The object associated with the exception.
   *
   *
   */
  faust::object get_object () const throw() 
  { 
    return object_; 
  }
}; 
}

#endif /* FAUST_EXCEPTION_HPP */

 ///@endcond - exclude from Doxygen
