
#ifndef DIGEDAG_EDGE_HPP
#define DIGEDAG_EDGE_HPP

#include <vector>

#include <saga/saga.hpp>

#include "util/scoped_lock.hpp"

#include "config.hpp"
#include "enum.hpp"
#include "dag.hpp"
#include "node.hpp"


namespace digedag
{
  class scheduler;
  class edge : public boost::enable_shared_from_this <edge>
  {
    private:
      saga::url                  src_url_;     // src location of data
      saga::url                  tgt_url_;     // tgt location of data

      state                      state_;       // state of instance

      sp_t <node>                src_node_;
      sp_t <node>                tgt_node_;
                                   
      bool                       is_void_;     // void edge?
      bool                       fired_;       // dependent node fired after Done?


      util::mutex                mtx_;

      saga::task                 task_;        // our async workload
      bool                       t_valid_;     // async workload was created

      sp_t <scheduler>           scheduler_;
      saga::session              session_;     // session from scheduler


    protected:
      saga::url & get_src_url (void) { return src_url_; }
      saga::url & get_tgt_url (void) { return tgt_url_; }
      friend class scheduler;


    public:
      edge  (const saga::url & src, 
             const saga::url & tgt,
             sp_t <scheduler>  scheduler, 
             saga::session     session);
      edge  (sp_t <scheduler>  scheduler,
             saga::session     session);
      ~edge (void);

      bool operator== (const edge & e);
      

      void             dryrun        (void);
      void             reset         (void);
      void             fire          (void);
      void             stop          (void);
      void             dump          (void);
      saga::task       work_start    (void);
      void             work_done     (void);
      void             work_failed   (void);
      void             erase_src     (void);
      void             erase_tgt     (void);
      void             add_src_node  (sp_t <node> src);
      void             add_tgt_node  (sp_t <node> tgt);
      void             set_state     (state s);
      state            get_state     (void);
      std::string      get_name_s    (void) const;
      edge_id_t        get_name      (void) const;

      saga::url        get_src       (void) const { return src_url_; }
      saga::url        get_tgt       (void) const { return tgt_url_; }


      sp_t <node>      get_src_node  (void) const { return src_node_; }
      sp_t <node>      get_tgt_node  (void) const { return tgt_node_; }

      void             set_pwd_src   (std::string pwd);
      void             set_pwd_tgt   (std::string pwd);
      void             set_host_src  (std::string host);
      void             set_host_tgt  (std::string host);
  };

} // namespace digedag

#endif // DIGEDAG_EDGE_HPP

