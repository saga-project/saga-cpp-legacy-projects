//  Copyright (c) 2005-2008 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <map>
#include <vector>

#include <boost/assert.hpp>
#include <boost/tokenizer.hpp>

#include <saga/saga.hpp>
#include <saga/saga/adaptors/task.hpp>
#include <common_helpers.hpp>

#include <saga/impl/config.hpp>
#include <saga/impl/exception_list.hpp>

#include "config.hpp"
#include "cpr_checkpoint.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace cpr
{

  ///////////////////////////////////////////////////////////////////////////////
  //  constructor
  cpr_checkpoint_cpi_impl::cpr_checkpoint_cpi_impl (proxy* p, 
                                                    cpi_info       const & info,
                                                    saga::ini::ini const & glob_ini, 
                                                    saga::ini::ini const & adap_ini,
                                                    TR1::shared_ptr<saga::adaptor> adaptor)
    :   base_cpi (p, info, adaptor, cpi::Noflags),
    thrd_          (NULL), 
    cond_          (NULL),
    thread_alive_  (false), 
    cancel_thread_ (false)
  {
      //check whether Migol has been initialized 
    std::string guid("");
    {//scoped lock
          adaptor_data_t d(this);
          guid = d->migol_guid;
          if(guid==""){
              guid=migol::init_migol_context(adap_ini);
              if ( guid == "")
              {
                  SAGA_ADAPTOR_THROW (std::string ("Migol infrastructure could not be initialized."), 
                                      saga::IncorrectState);
              }
              d->migol_guid=guid;
              
          }
          std::cout << "cpr_checkpoint_cpi_impl ctor: " << guid << std::endl;
    }
      
     // first usage of this adaptor
    instance_data data (this);
    saga::url cpr_url (data->location_);
    std::cout<< "checkpoint url: "<< cpr_url <<std::endl;
    //No action since Migol does not support checkpoint containers
      
  }

  ///////////////////////////////////////////////////////////////////////////////
  void cpr_checkpoint_cpi_impl::check_lf (void)
  {
    if ( lf_ == NULL )
    {
      SAGA_ADAPTOR_THROW ("cpr::cpr_checkpoint_cpi_impl instance is not"
                          "initialized.", saga::IncorrectState);
    }
  }

  ///////////////////////////////////////////////////////////////////////////////
  //  destructor
  cpr_checkpoint_cpi_impl::~cpr_checkpoint_cpi_impl (void)
  {
      std::string guid("");
      {//scoped lock
          adaptor_data_t d(this);
          guid = d->migol_guid;
      }
      
      migol::instance()->change_service_state(guid, "done");  
      
    if ( lf_ != NULL )
    {
      //delete (lf_);
    }
  }

  ///////////////////////////////////////////////////////////////////////////////
  //  SAGA CPI functions 

  ///////////////////////////////////////////////////////////////////////////////
  // attribute functions
  void cpr_checkpoint_cpi_impl::sync_attribute_exists (bool      & ret, 
                                                       std::string key)
  {
    check_lf ();
    //ret = lf_->attribute_exists (key);
  }

  void cpr_checkpoint_cpi_impl::sync_attribute_is_readonly (bool      & ret, 
                                                            std::string key)
  {
    check_lf ();
    //ret = lf_->attribute_is_readonly (key);
  }

  void cpr_checkpoint_cpi_impl::sync_attribute_is_writable (bool      & ret, 
                                                            std::string key)
  {
    check_lf ();
    //ret = lf_->attribute_is_writable (key);
  }

  void cpr_checkpoint_cpi_impl::sync_attribute_is_vector (bool      & ret, 
                                                          std::string key)
  {
    check_lf ();
    //ret = lf_->attribute_is_vector (key);
  }

  void cpr_checkpoint_cpi_impl::sync_attribute_is_extended (bool      & ret, 
                                                            std::string key)
  {
    check_lf ();
    // FIXME: that is a private method?!  Do we need in API?
    // ret = lf_->attribute_is_extended (key);
  }

  ///////////////////////////////////////////////////////////////////////////////
  void cpr_checkpoint_cpi_impl::sync_get_attribute (std::string & ret, 
                                                    std::string   key)
  {
    check_lf ();
    //ret = lf_->get_attribute (key);
  }

  ///////////////////////////////////////////////////////////////////////////////
  void cpr_checkpoint_cpi_impl::sync_get_vector_attribute (std::vector <std::string> & ret, 
                                                           std::string                 key)
  {
    check_lf ();
    //ret = lf_->get_vector_attribute (key);
  }

  ///////////////////////////////////////////////////////////////////////////////
  void cpr_checkpoint_cpi_impl::sync_set_attribute (saga::impl::void_t &,
                                                    std::string key, 
                                                    std::string val)
  {
    check_lf ();
    //lf_->set_attribute (key, val);
  }

  ///////////////////////////////////////////////////////////////////////////////
  void cpr_checkpoint_cpi_impl::sync_set_vector_attribute (saga::impl::void_t &, 
                                                           std::string              key, 
                                                           std::vector <std::string> val)
  {
    check_lf ();
    //lf_->set_vector_attribute (key, val);
  }

  ///////////////////////////////////////////////////////////////////////////////
  void cpr_checkpoint_cpi_impl::sync_remove_attribute (saga::impl::void_t &, 
                                                       std::string key)
  {
    check_lf ();
    //lf_->remove_attribute (key);
  }

  ///////////////////////////////////////////////////////////////////////////////
  void cpr_checkpoint_cpi_impl::sync_list_attributes (std::vector <std::string> & keys)
  {
    check_lf ();
   // keys = lf_->list_attributes ();
  }

  ///////////////////////////////////////////////////////////////////////////////
  void cpr_checkpoint_cpi_impl::sync_find_attributes (std::vector <std::string> & keys, 
                                                      std::string                 pattern)
  {
    check_lf ();
   // keys = lf_->find_attributes (pattern);
  }


  ///////////////////////////////////////////////////////////////////////////////
  // namespace_entry functions
  void cpr_checkpoint_cpi_impl::sync_get_url (saga::url & url)
  {
    check_lf ();
    //url = lf_->get_url ();
  }

  void cpr_checkpoint_cpi_impl::sync_get_cwd (saga::url & url)
  {
    check_lf ();
    //url = lf_->get_cwd ();
  }

  void cpr_checkpoint_cpi_impl::sync_get_name (saga::url & url)
  {
    check_lf ();
    //url = lf_->get_name ();
  }

  void cpr_checkpoint_cpi_impl::sync_is_dir (bool & ret)
  {
    check_lf ();
    //ret = lf_->is_dir ();
  }

  void cpr_checkpoint_cpi_impl::sync_is_entry (bool & ret)
  {
    check_lf ();
    //ret = lf_->is_entry ();
  }

  void cpr_checkpoint_cpi_impl::sync_is_link (bool & ret)
  {
    check_lf ();
    //ret = lf_->is_link ();
  }

  void cpr_checkpoint_cpi_impl::sync_remove (saga::impl::void_t &, 
                                             int flags)
  {
    check_lf ();
    //lf_->remove (flags);
  }

  void cpr_checkpoint_cpi_impl::sync_close (saga::impl::void_t &,
                                            double timeout)
  {
    check_lf ();
    //lf_->close (timeout);
  }


  ///////////////////////////////////////////////////////////////////////////////
  // cpr_checkpoint functions
  void cpr_checkpoint_cpi_impl::sync_get_parent (saga::url & ret,
                                                 int         idx)
  {
    saga::url parent;

    if ( idx > 1 )
    {
      // get parent
      check_lf ();
    //  parent = lf_->get_attribute ("SAGA_CPR_PARENT");
      idx--;
    }


    while ( idx > 1 )
    {
      // get grand...-parent
      saga::cpr::checkpoint cp (parent);
      parent = cp.get_parent ();
      idx--;
    }

    ret = parent;
  }

  void cpr_checkpoint_cpi_impl::sync_get_file (saga::url & ret,
                                               int idx)
  {
    check_lf ();
    //std::vector <saga::url> listing = lf_->list_locations ();
    
    // FIXME: range check
    
   // ret = listing[idx];
  }

  void cpr_checkpoint_cpi_impl::sync_open_file (saga::filesystem::file & ret,
                                                int                      idx)
  {
    check_lf ();
    //std::vector <saga::url> listing = lf_->list_locations ();
    
    // FIXME: range check
    
   // saga::filesystem::file f (listing[idx]);

    //ret = f;
  }
    
  /**
  * adds file profile to Grid Service Object
  */
  void cpr_checkpoint_cpi_impl::sync_add_file (int &ret,
                                                 saga::url url)
  {
      std::string guid;
      {//scoped lock
            adaptor_data_t d(this);
            guid = d->migol_guid;
            if ( guid == "")
            {
              SAGA_ADAPTOR_THROW (std::string ("Migol infrastructure could not be initialized."), 
                                  saga::IncorrectState);
            }
            std::cout << "cpr_checkpoint_cpi_impl::add_file: " << guid << std::endl;
      }
      
      // translate 'any' and 'cpr' url schemes to lfn, decline all others
      std::string scheme (url.get_scheme ());
      
      if ( ! scheme.empty () && 
          scheme != "gsiftp")
      {
          SAGA_OSSTREAM strm;
          strm << "cpr::migol_cpr_checkpoint_cpi_impl::add_file: "
          "cannot handle checkpoint name: " << url;
          SAGA_ADAPTOR_THROW(SAGA_OSSTREAM_GETSTRING(strm), saga::BadParameter);
      }
      
      bool result = false;
      {
          result =  migol::instance()->register_checkpoint(guid, url.get_url());
      }
      
      if (!result)
      { 
          SAGA_OSSTREAM strm;
          strm << "cpr::migol_cpr_checkpoint_cpi_impl::add_file: "
          "cannot handle checkpoint name: " << url.get_url();
          SAGA_ADAPTOR_THROW(SAGA_OSSTREAM_GETSTRING(strm), saga::BadParameter);
      }
  }
    
    /**
     * list all file urls in Grid Service Object
     */
    void cpr_checkpoint_cpi_impl::sync_list_files (std::vector<saga::url>  &ret){
        std::string guid;
        {//scoped lock
            adaptor_data_t d(this);
            guid = d->migol_guid;
            if ( guid == "")
            {
                SAGA_ADAPTOR_THROW (std::string ("Migol infrastructure could not be initialized."), 
                                    saga::IncorrectState);
            }
            std::cout << "cpr_checkpoint_cpi_impl::add_file: " << guid << std::endl;
        }
        std::vector<saga::url> files;
        {
            files=migol::instance()->get_files(guid); 
        }
        ret = files;
    }
   
  ///////////////////////////////////////////////////////////////////////////////
}   // namespace cpr

