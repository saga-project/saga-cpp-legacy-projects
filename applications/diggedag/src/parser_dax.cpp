
#include <string>
#include <fstream>
#include <iostream>

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
      parse_dag ();
    }

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
          std::string s_id   = job->GetAttributeOrDefault ("id",   "Unknown");
          std::string s_name = job->GetAttributeOrDefault ("name", "Unknown");

          std::cout << "job [" << s_id << " - " << s_name << "] - create node" << std::endl;

          diggedag::node n (s_name);
          dag_.add_node (s_id, n);
        }


        // second run: we have input and output files specified for each jobs.
        // Find pairs, and add as edges
        std::vector <std::pair <std::string, std::string> > inputs;
        std::vector <std::pair <std::string, std::string> > outputs;

        for ( job = job.begin (adag); job != job.end (); job++ )
        {
          std::string s_id   = job->GetAttributeOrDefault ("id",   "Unknown");
          std::string s_name = job->GetAttributeOrDefault ("name", "Unknown");

          std::cout << "job [" << s_id << " - " << s_name << "] - check edges" << std::endl;

          ticpp::Iterator <ticpp::Element> uses ("uses"); 

          for ( uses = uses.begin (job.Get ()); uses != uses.end (); uses++ )
          {
            std::string s_file = uses->GetAttributeOrDefault ("file", "Unknown");
            std::string s_link = uses->GetAttributeOrDefault ("link", "Unknown");

            if ( s_link == "input" )
            {
              std::cout << "  input :   " << s_id << " - " << s_file << std::endl;
              inputs.push_back (std::pair <std::string, std::string> (s_file, s_id));
            }
            else if ( s_link == "output" )
            {
              std::cout << "  output:   " << s_id << " - " << s_file << std::endl;
              outputs.push_back (std::pair <std::string, std::string> (s_file, s_id));
            }
            else
            {
              std::cerr << "cannot handle link type " << s_link << std::endl;
            }
          }
        }


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
              std::cout << "adding edge " << o_node << " - " << i_node << " : " << file << std::endl;

              // add edge
              diggedag::edge e (file);
              dag_.add_edge (e, o_node, i_node);

              // stop loop
              j = inputs.size ();
            }
          }

          if ( o_node == "" )
          {
              std::cout << "WARNING: cannot find source node for " << file 
                        << " required by node " << i_node << std::endl;
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

