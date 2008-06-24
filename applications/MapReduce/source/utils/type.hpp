//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <saga/saga.hpp>
#include <boost/iostreams/stream.hpp>

#ifndef MR_TYPE_HPP
#define MR_TYPE_HPP

class saga_file_device
{
  private:
    std::string            src_;
    saga::filesystem::file file_;

  public:
    typedef char                                   char_type;
    typedef boost::iostreams::seekable_device_tag  category;

    saga_file_device (std::string src)   //Default constructor creates a file from a string
      : src_  (src),
        file_ (saga::url (src_), 
               saga::filesystem::ReadWrite |
               saga::filesystem::Create   )
    {
    }

    saga_file_device (saga::filesystem::file f) //Defalt constructor that creates a file
      : file_ (f)
    {
      src_ = file_.get_url ().get_string ();
    }

    std::streamsize read (char_type * data, std::streamsize size) //required for boost::iostreams
    {
      saga::ssize_t res = file_.read (saga::buffer (data, size), size);

      return static_cast <std::streamsize> (res);
    }

    std::streamsize write (char_type const * data, std::streamsize size) //required for boost::iostreams
    {
      saga::ssize_t res = file_.write (saga::buffer (data, size), size);

      return static_cast <std::streamsize> (res);
    }

    boost::iostreams::stream_offset seek   //required for boost::iostreams
      (boost::iostreams::stream_offset off, 
       std::ios_base::seekdir          whence)
      {
        saga::filesystem::seek_mode mode;

        if ( whence == std::ios_base::beg )
        {
          mode = saga::filesystem::Start;
        } 
        else if ( whence == std::ios_base::cur )
        {
          mode = saga::filesystem::Current;
        } 
        else// if ( whence == std::ios_base::end )    //Did this to stop compiler from complaining
        {
          mode = saga::filesystem::End;
        }

        saga::off_t res = file_.seek (off, mode);

        return static_cast <boost::iostreams::stream_offset> (res);
      }

    saga::filesystem::file get_saga_file (void) 
    {
      return file_; 
    }
};

#endif // MR_TYPE_HPP

