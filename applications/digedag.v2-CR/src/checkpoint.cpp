/*
 #  Copyright (c) 2010 Katerina Stamou (kstamou@cct.lsu.edu)
 # 
 #  Distributed under the Boost Software License, Version 1.0. (See accompanying
 #  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/


#include <bitset>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/dynamic_bitset.hpp>

#include "config.hpp"
#include "dag.hpp"
#include "enum.hpp"
#include "node.hpp"
#include "edge.hpp"
#include "checkpoint.hpp"
#include "util/zfstream.h"

namespace digedag 
{
  checkpoint_mgr::checkpoint_mgr (void)
  : dagcomplete ( true )
  {
	checkpoint_.compress = true;
	checkpoint_.verbose  = true;
	checkpoint_.filepath = "/tmp/";
	checkpoint_.filename = "";
	checkpoint_.method   = LocalFile;
	checkpoint_.hashlog  = true;
	//checkpoint_.rm_log_on_complete = false;
  }

  checkpoint_mgr::~checkpoint_mgr (void)
  {
  }

  void checkpoint_mgr::set_dag_size (int node_size, int edge_size)
  {
	nodeset.resize(node_size);
	edgeset.resize(edge_size);
  }

  void checkpoint_mgr::set_file (std::string dagfile)
  {
	if (checkpoint_.hashlog) {
        	util::hash daghash(dagfile);
        	std::cout << "CHECKPOINT: DAG-hash: " << daghash.get_hash() << std::endl;

		std::stringstream ss;
		ss << checkpoint_.filepath << "digedag-CR-" << daghash.get_hash() << ".log";

		checkpoint_.filename = ss.str();
	}

	else {
		checkpoint_.filepath = ".";
		checkpoint_.filename = "digedag-CR.log";
	}

	if (checkpoint_.compress)
		checkpoint_.filename.append(".gz");

        std::cout << "DAG input: " << dagfile << std::endl;
	std::cout << "DAG checkpoint log: " << checkpoint_.filename << std::endl;
  }

  std::string checkpoint_mgr::get_filename (void)
  {
	return checkpoint_.filename;
  }

  void checkpoint_mgr::set_filepath (std::string fpath)
  {
	if (fpath[fpath.size()] != '/')
	  fpath.append("/");

	checkpoint_.filepath = fpath;
  }

  void checkpoint_mgr::set_verbose (bool flag)
  {
	// std::cout << "setting verbosity to: " << flag << std::endl;
	checkpoint_.verbose = flag;
  }

  bool checkpoint_mgr::get_verbose (void)
  {
	return checkpoint_.verbose;
  }

  void checkpoint_mgr::set_compress (bool flag)
  {
	checkpoint_.compress = flag;
  }

  bool checkpoint_mgr::get_compress (void)
  {
	return checkpoint_.compress;
  } 

  void checkpoint_mgr::set_hashlog (bool flag)
  {
	checkpoint_.hashlog = flag;
  }

  bool checkpoint_mgr::get_hashlog (void)
  {
	return checkpoint_.hashlog;
  }

  void checkpoint_mgr::set_method (checkpoint_method m)
  {
	checkpoint_.method = m;
  }

  checkpoint_method checkpoint_mgr::get_method (void)
  {
	return checkpoint_.method;
  }

  // hooks to scheduler::hook_node_run_done
  void checkpoint_mgr::node_commit (boost::shared_ptr <node> n,
				    std::map <node_id_t, node_map_t> dag_nodes)
  {
	std::map <node_id_t, boost::shared_ptr <node> > :: iterator node_it;
	std::map <node_id_t, boost::shared_ptr <node> > :: iterator node_begin = dag_nodes.begin ();
	std::map <node_id_t, boost::shared_ptr <node> > :: iterator node_end = dag_nodes.end ();

	int nodepos = std::distance ( node_begin, dag_nodes.find(n->get_name()) );

	if (checkpoint_.verbose)
        {
	  std::cout << "CR: node position in map: " << nodepos << std::endl;
	  std::cout << "CR: node id: " << n->get_name() << std::endl;
        }

	nodeset[nodepos] = 1;
	dumptofile();
  }

  // hooks to scheduler::hook_edge_run_done
  void checkpoint_mgr::edge_commit (boost::shared_ptr <edge> e,
                                    std::map <edge_id_t, edge_map_t> dag_edges)
  {
	/*
        std::map <edge_id_t, boost::shared_ptr <edge> > :: iterator edge_it;
        std::map <edge_id_t, boost::shared_ptr <edge> > :: iterator edge_begin = dag_edges.begin ();
        std::map <edge_id_t, boost::shared_ptr <edge> > :: iterator edge_end = dag_edges.end ();

        int edgepos = std::distance ( edge_begin, dag_edges.find(e->get_id()) );
	*/

	if (checkpoint_.verbose)
        {
          //std::cout << "CR: edge position in map: " << edgepos << std::endl;
          std::cout << "CR: edge_commit edge-id: " << e->get_id() << std::endl;
	}

	// edgeset[edgepos] = 1;
	edgeset[e->get_id()-1] = 1;
	dumptofile();
  }

  // hooks to scheduler::hook_dag_run_post
  void checkpoint_mgr::dag_complete (void)
  {
	remove(checkpoint_.filename.c_str());
	std::cout << "CR: DAG is complete, removing recover log." << std::endl;
  }

  void checkpoint_mgr::dump (std::map <node_id_t, node_map_t> dag_nodes,
                             std::map <edge_id_t, edge_map_t> dag_edges)
  {
	// create empty node and edge status bitmaps,
        // equal to the size of their respective node maps
	// initialized with zeros (by default)
        nodeset.resize(dag_nodes.size()); // determine from length of arg map
        edgeset.resize(dag_edges.size()); // determine from length of arg map

	std::cout << "DUMP: verbose: " << checkpoint_.verbose << std::endl;
	if (get_verbose()) {
	std::cout << "CHECKPOINT ::dump: dag_nodes.size: " << dag_nodes.size()
		  << " dag_edges.size: " << dag_edges.size() << std::endl;
	}

        // fill bitmaps with node and edge statuses
	std::map <node_id_t, boost::shared_ptr <node> > :: iterator node_it;
	std::map <node_id_t, boost::shared_ptr <node> > :: iterator node_begin = dag_nodes.begin ();
	std::map <node_id_t, boost::shared_ptr <node> > :: iterator node_end   = dag_nodes.end ();

	unsigned int i = 0;
	for ( node_it = node_begin; node_it != node_end; node_it++ ) { 
	   if (node_it->second->return_state() == Done) {
	   	nodeset[i] = 1; // nodeset[i].flip()
		if (checkpoint_.verbose) {
		std::cout << "CHECKPOINT setting nodeset[" << i << "] = 1" << 
			     " ( " << node_it->second->get_name() << " ) " << std::endl;
		}
	   }
	   i++;
	}

	std::map <edge_id_t, edge_map_t > :: iterator edge_it;
	std::map <edge_id_t, edge_map_t > :: iterator edge_begin = dag_edges.begin ();
	std::map <edge_id_t, edge_map_t > :: iterator edge_end   = dag_edges.end ();

	i = 0;
	for ( edge_it = edge_begin; edge_it != edge_end; edge_it++ ) {
	  if (edge_it->second->get_state() == Done) {
		edgeset[i] = 1; // edgeset[i].flip()
		if (checkpoint_.verbose) {
		std::cout << "CHECKPOINT setting edgeset[" << i << "] = 1" << 
			     " ( " << edge_it->second->get_name() << " ) " << std::endl;
		}
	  }
	  i++;
	}

	// TODO: implement batching (journal queueing)
        // in order to throttle large DAG checkpointing
        // balance between checkpoint overhead/latency for large DAGs
        // and fine-grained checkpoint markers (e.g. checkpoint-node-interval 10)
        // i.e. dequeue 10 checkpoints per dump
	// adjust dynamically depending on DAG size as metric
	//
        // do this asynchronously on checkpoint_mgr own worker thread, like the
        // scheduler class does

	// depending on method set, journal the DAG progress
	switch (checkpoint_.method) {
		case LocalFile:
			checkpoint_mgr::dumptofile(); // write hash-filename format on disk
			break;
		case AdvertService:
			checkpoint_mgr::dumptoadvert(); // write relevant db entries
			break;
		default: 
			std::cerr << "no checkpoint method selected, but checkpoint enabled" << std::endl;
	}
   }

   // node and edge map are passed by ref,
   // and have their state restored accordingly
   // this is hooked from: bool scheduler::hook_dag_run_pre (void)
   void checkpoint_mgr::restore (std::map <node_id_t, node_map_t> &dag_nodes,
				 std::map <edge_id_t, edge_map_t> &dag_edges)
   {
	switch (checkpoint_.method) {
		case LocalFile:
			checkpoint_mgr::restorefromfile(dag_nodes, dag_edges); // read hash-filename from filepath
			break;
		case AdvertService:
			checkpoint_mgr::restorefromadvert(); // read daghash entry from advdb
			break;
		default:
			std::cerr << "no checkpoint method selected, but checkpoint was enabled" << std::endl;
			std::cerr << "method: " << checkpoint_method_to_string(checkpoint_.method) << std::endl;
	}

	if (checkpoint_.verbose) {
        	std::map <node_id_t, boost::shared_ptr <node> > :: iterator node_it;
        	std::map <node_id_t, boost::shared_ptr <node> > :: iterator node_begin = dag_nodes.begin ();
        	std::map <node_id_t, boost::shared_ptr <node> > :: iterator node_end   = dag_nodes.end ();

        	for ( node_it = node_begin; node_it != node_end; node_it++ ) {
                	std::cout << "CHECKPOINT return from restorefrom-method nodemap: "
                          	  << node_it->second->get_name() << " : "
			  	  << node_it->second->get_state() << std::endl;
        	}

        	std::map <edge_id_t, edge_map_t > :: iterator edge_it;
        	std::map <edge_id_t, edge_map_t > :: iterator edge_begin = dag_edges.begin ();
        	std::map <edge_id_t, edge_map_t > :: iterator edge_end   = dag_edges.end ();

        	for ( edge_it = edge_begin; edge_it != edge_end; edge_it++ ) {
                	std::cout << "CHECKPOINT return from restorefrom-method edgemap: "
                          	  << edge_it->second->get_name() << " : "
                          	  << edge_it->second->get_state() << std::endl;
        	}
	}

    }

    void checkpoint_mgr::dumptofile(void)
    {
	std::string nodeset_str, edgeset_str;

	boost::to_string(nodeset, nodeset_str);
	boost::to_string(edgeset, edgeset_str);
	
	if (checkpoint_.verbose) 
        {
	  std::cout << "CHECKPOINT: dumptofile()" << std::endl
	            << "CHECKPOINT: nodeset bits: " << nodeset        << " <- " << std::endl
	            << "CHECKPOINT: nodeset size: " << nodeset.size()           << std::endl
	            << "CHECKPOINT: edgeset bits: " << edgeset_str    << " <- " << std::endl
	            << "CHECKPOINT: edgeset size: " << edgeset.size()           << std::endl;
	}

	std::string tempjournal = checkpoint_.filename;
	tempjournal.append(".tmp");

	if (checkpoint_.compress)
        {
          std::cout << "CR: dumptofile() (compressed)" << std::endl;
          gzofstream zjournal;
	  /* dump state into a temporary journal file, and then move it over to the original,
             avoiding the effect of truncated restore file in case of crash while opening the journal */ 
          zjournal.open(tempjournal.c_str(), std::ios::out | std::ios::trunc);
          /* do error checking here on ZLIB return vals, and toggle compression level */
          zjournal << nodeset_str << std::endl << edgeset_str << std::endl;
          zjournal.close();
	}

	else // compression is not desired, do plain i/o
        {
          std::cout << "CHECKPOINT dumptofile() filename " << checkpoint_.filename << std::endl;
          std::ofstream journal;
          journal.open(tempjournal.c_str(), std::ios::out | std::ios::trunc);
          if (!journal.is_open()) {
            std::cerr << "error opening file " << checkpoint_.filename << " for writing. check path/file write perms." << std::endl;
          }

          // dump nodeset/edgeset as strings into the checkpoint file
          // dumpfile structure: nodeset: nodeset.str() \n, edgeset: edgeset.str() \n
          // transform this into class to avoid reopening file, on each write op put stream to byte 0,
          journal << nodeset_str << std::endl << edgeset_str << std::endl;
          journal.close();
	}
	
	if ( rename(tempjournal.c_str(), checkpoint_.filename.c_str()) )
	{
	  std::cerr << "could not move temporary file " << tempjournal
		    << "over permanent journal file "   << checkpoint_.filename << std::endl;
	}

    }

    void checkpoint_mgr::dumptoadvert(void)
    {
	std::cout << "CR: dumptoadvert() method stub, not yet implemented" << std::endl;
    }

    void checkpoint_mgr::restorefromfile(std::map <node_id_t, node_map_t> &dag_nodes, std::map <edge_id_t, edge_map_t> &dag_edges)
    {
	std::cout << "CHECKPOINT: entering restorefromfile()" << std::endl;

	std::string nodeline, edgeline;

	std::cout << "CHECKPOINT: restorefromfile(): " << checkpoint_.filename << std::endl;

	if (checkpoint_.compress) 
        {
          gzifstream zjournal;
          zjournal.open(checkpoint_.filename.c_str(), std::ifstream::in);

          if (zjournal.is_open()) 
          {
            getline(zjournal, nodeline);
            getline(zjournal, edgeline);
          }
          else 
          {
            std::cerr << "no checkpoint restore journal file found corresponding to the supplied DAG"
                      << " ( " << get_filename() << " )" << std::endl;
            return;
          }

          zjournal.close();
	}
	else if (!checkpoint_.compress) 
        {	
          std::ifstream journal;
          journal.open(checkpoint_.filename.c_str(), std::ifstream::in);

          if (journal.is_open()) 
          {
            getline(journal, nodeline);
            getline(journal, edgeline);
            //std::cout << nodeline << std::endl << edgeline << std::endl;
          }
          else 
          {
            std::cerr << "no checkpoint restore journal file found corresponding to the supplied DAG"
                      << " ( " << get_filename() << " )" << std::endl;
            return;
          }

          journal.close();
	}

        std::cout << nodeline << std::endl << edgeline << std::endl;

	boost::dynamic_bitset<> bitmap_nodes(nodeline);
	boost::dynamic_bitset<> bitmap_edges(edgeline);

	nodeset = bitmap_nodes;
	edgeset = bitmap_edges;

	if (checkpoint_.verbose)
        {
	  std::cout << "CHECKPOINT: restored nodes/edges: "         << std::endl;
	  std::cout << "CHECKPOINT: bitmap_nodes: " << bitmap_nodes << std::endl
		    << "CHECKPOINT: bitmap_edges: " << bitmap_edges << std::endl;
	}

	// iterator for two bitsets, and set node/edge maps by ref
        // fill bitmaps with node and edge statuses

        // edges are restored before nodes, so that node state can be synced
        // according to outgoing edge status

        std::map <edge_id_t, edge_map_t > :: iterator edge_it;
        std::map <edge_id_t, edge_map_t > :: iterator edge_begin = dag_edges.begin ();
        std::map <edge_id_t, edge_map_t > :: iterator edge_end   = dag_edges.end ();

        unsigned int i = 0;
        for ( edge_it = edge_begin; edge_it != edge_end; edge_it++ ) {
	   if ( i < bitmap_edges.size() ) { // prevent restoration failure, in case DAG has been altered
           	if (bitmap_edges.test(i)) {
                	edge_it->second->set_state(Done);
                	if (checkpoint_.verbose) {
                	std::cout << "CHECKPOINT setting edgeset[" << i << "] = 1" <<
                             	     " ( " << edge_it->second->get_name() << " ) " << std::endl;
                	}
          	}
          	i++;
	  }
        }

        std::map <node_id_t, boost::shared_ptr <node> > :: iterator node_it;
        std::map <node_id_t, boost::shared_ptr <node> > :: iterator node_begin = dag_nodes.begin ();
        std::map <node_id_t, boost::shared_ptr <node> > :: iterator node_end   = dag_nodes.end ();

        i = 0;
        for ( node_it = node_begin; node_it != node_end; node_it++ ) {
	   if ( i < bitmap_nodes.size() ) {
	        if (bitmap_nodes.test(i)) {
			node_it->second->set_state(Done);
                	if (checkpoint_.verbose) {
                		std::cout << "CHECKPOINT restoring nodeset[" << i << "] = 1" <<
                             		     " ( " << node_it->second->get_name() << " ) " << std::endl;
                	}
           	}
	   i++;

	   // sync node-internal edge-state, and fire any unresolved outgoing edges
	   node_it->second->sync_state(checkpoint_.verbose); 
	   //node_it->second->get_state(); // update node-internal state, depending on edge-status

	   if (node_it->second->return_state() != Done)
		dagcomplete = 0;
	  }
        }

	   if (dagcomplete) {
		std::cout << "CHECKPOINT: DAG is complete, exiting." << std::endl
			  << "CHECKPOINT: (remove " << checkpoint_.filename
			  << " to reset checkpoint state)." << std::endl;
		exit(0);
	   }

    }

    void checkpoint_mgr::restorefromadvert(void)
    {
	std::cout << "CR: restorefromadvert() stub, not yet implemented" << std::endl;
	// read daghash entry from advdb
	// zlib stream inflate if compressed
    }

} // namespace digedag
