
#ifndef DIGEDAG_EDGE_HPP
#define DIGEDAG_EDGE_HPP

#include <vector>

#include <saga/saga.hpp>

#include "util/thread.hpp"
#include "util/scoped_lock.hpp"

#include "enum.hpp"
#include "dag.hpp"
#include "node.hpp"
#include "scheduler.hpp"


namespace digedag
{
  class edge : public digedag::util::thread
  {
    private:
      saga::url             src_url_;   // src location of data
      saga::url             tgt_url_;   // tgt location of data

      std::string           src_path_;  // original src path
      std::string           tgt_path_;  // original tgt path

      digedag::state       state_; // state of instance

      digedag::node      * src_node_;
      digedag::node      * tgt_node_;

      digedag::dag       * dag_;
      digedag::scheduler * scheduler_;

      util::mutex           mtx_;


    protected:
      saga::url      & get_src_url   (void) { return src_url_; }
      saga::url      & get_tgt_url   (void) { return tgt_url_; }
      friend class scheduler;


    public:
      edge  (const saga::url & src, 
             const saga::url & tgt = "");
      ~edge (void);

      bool operator== (const edge & e);
      

      void             dryrun        (void);
      void             reset         (void);
      void             fire          (void);
      void             stop          (void);
      void             dump          (void);
      void             thread_work   (void);
      void             erase_src     (void);
      void             erase_tgt     (void);
      void             add_src_node  (digedag::node * src);
      void             add_tgt_node  (digedag::node * tgt);
      void             set_state     (state s);
      digedag::state   get_state     (void);
      std::string      get_name_s    (void) const;
      edge_id_t        get_name      (void) const;

      saga::url        get_src       (void) const { return src_url_; }
      saga::url        get_tgt       (void) const { return tgt_url_; }


      digedag::node * get_src_node  (void) const { return src_node_; }
      digedag::node * get_tgt_node  (void) const { return tgt_node_; }

      void             set_pwd_src   (std::string pwd);
      void             set_pwd_tgt   (std::string pwd);
      void             set_host_src  (std::string host);
      void             set_host_tgt  (std::string host);

      void             set_dag       (digedag::dag * d);
  };

} // namespace digedag

#endif // DIGEDAG_EDGE_HPP

