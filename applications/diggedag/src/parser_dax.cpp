
#include <string>
#include <iostream>
#include <unistd.h>

#include "util/split.hpp"
#include "ticpp.hpp"
#include "parser_dax.hpp"

namespace diggedag
{
  namespace dax
  {
    parser::parser (const std::string & filename)
      : filename_ (filename)
    {
      dag_ = new diggedag::dag ();
      parse_dag ();
    }


    // Note that, when parsing file names and arguments, paths are interpreted
    // to be relative to the applications working directory.
    void parser::parse_dag (void)
    {
      try
      {
        ticpp::Document doc (filename_);

        doc.LoadFile ();

        // get the top adag element
        ticpp::Element * adag = doc.FirstChildElement ("adag");

        // list nodes
        ticpp::Iterator <ticpp::Element> job ("job"); 


        // we parse jobs twice.  On the first run, we add all nodes.  On the
        // second run, we add all edges (connected nodes are known now).
        for ( job = job.begin (adag); job != job.end (); job++ )
        {
          diggedag::node_description nd;

          std::string s_id   = job->GetAttribute ("id");
          std::string s_name = job->GetAttribute ("name");

          nd.set_attribute ("Executable", s_name);


          // get args
          ticpp::Element * args = job->FirstChildElement ("argument");

          if ( args )
          {
            // iterate over args, if we have them
            ticpp::Iterator <ticpp::Node> arg; 

            std::vector <std::string> s_args;

            for ( arg = arg.begin (args); arg != arg.end (); arg++ )
            {
              if ( arg->Type () == TiXmlNode::ELEMENT )
              {
                ticpp::Element * elem   = arg->ToElement ();
                std::string      s_file = elem->GetAttribute ("file");

                s_args.push_back (s_file);
              }
              else if ( arg->Type () == TiXmlNode::TEXT )
              {
                std::stringstream ss;
                ss << *arg;
                std::string tmp  = ss.str ();

                if ( tmp.size () )
                {
                  std::vector <std::string> s_tmp = split (tmp);

                  for ( unsigned int j = 0; j < s_tmp.size (); j++ )
                  {
                    if ( s_tmp [j] == "." )
                    {
                      s_args.push_back (s_tmp[j]);
                    }
                    else
                    {
                      s_args.push_back (s_tmp[j]);
                    }
                  }
                }
              }
            }
            
            nd.set_vector_attribute ("Arguments", s_args);
          }

          diggedag::node * n = new diggedag::node (nd, s_name);

          dag_->add_node (s_id, n);
        }


        // second run: we have input and output files specified for each jobs.
        // Find pairs, and add as edges
        std::vector <std::pair <std::string, std::string> > inputs;
        std::vector <std::pair <std::string, std::string> > outputs;

        for ( job = job.begin (adag); job != job.end (); job++ )
        {
          std::string s_id   = job->GetAttribute ("id");
          std::string s_name = job->GetAttribute ("name");

          ticpp::Iterator <ticpp::Element> uses ("uses"); 

          for ( uses = uses.begin (job.Get ()); uses != uses.end (); uses++ )
          {
            std::string s_file     = uses->GetAttribute ("file");
            std::string s_link     = uses->GetAttribute ("link");
            std::string s_transfer = uses->GetAttribute ("transfer");

            if ( s_link == "input" )
            {
              inputs.push_back (std::pair <std::string, std::string> (s_file, s_id));
            }
            else if ( s_link == "output" )
            {
              outputs.push_back (std::pair <std::string, std::string> (s_file, s_id));
            }
            else
            {
              std::cerr << "cannot handle link type " << s_link << std::endl;
            }
            
          }
        }


        // iterate over inputs, and find outputs which produce them.
        // inputs not produced by some outputting node are assumed to be staged
        // in from a data src.  Also, data which are produced but not consumed
        // by another node are to be staged to an output data sink.  In both
        // cases, we simply add edges with emoty src/tgt nodes, and leave it to
        // the scheduler to interprete that correctly.

        // first, iterate over inputs, and add edges for those inputs which are
        // produced by another node, and also for those which need to be staged
        // in.
        for ( unsigned int i = 0; i < inputs.size (); i++ )
        {
          std::string file   = inputs[i].first;
          std::string i_node = inputs[i].second;
          std::string o_node = "";

          // for each input node, find output node
          for ( unsigned int j = 0; j < outputs.size (); j++ )
          {
            if ( outputs[j].first == file )
            {
              o_node = outputs[j].second;

              // stop loop
              j = inputs.size ();
            }
          }

          if ( o_node == "" )
          {
            // std::cout << "adding edge " << "INPUT" << " - " << i_node << " : " << file << std::endl;
            // // need to stage data in from data src
            // std::cout << "WARNING: cannot find source node for " << file 
            //           << " required by node " << i_node << std::endl;
          }

          if ( o_node != i_node )
          {
            saga::url loc (file);
            loc.set_scheme ("any");
            diggedag::edge * e = new diggedag::edge (loc);
            dag_->add_edge (e, o_node, i_node);
          }
        }

        // inputs have been iterated above - now iterate over outputs, and look
        // for remaining ones which do not have a partner.
        for ( unsigned int k = 0; k < outputs.size (); k++ )
        {
          std::string file   = outputs[k].first;
          std::string i_node = "";
          std::string o_node = outputs[k].second;

          // for each output node, find the input node
          for ( unsigned int l = 0; l < inputs.size (); l++ )
          {
            if ( inputs[l].first == file )
            {
              i_node = inputs[l].second;

              // stop loop
              l = inputs.size ();
            }
          }

          if ( i_node == "" )
          {
            // will stage data out to data sink

            saga::url loc (file);
            loc.set_scheme ("any");
            diggedag::edge * e = new diggedag::edge (loc);
            dag_->add_edge (e, o_node, i_node);
          }
        }
      }
      catch ( const ticpp::Exception & e )
      {
        std::cout << e.what () << std::endl;
      }
    }

  } // namespace dax

} // namespace diggedag

