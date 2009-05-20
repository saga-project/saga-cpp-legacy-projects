
#include "ticpp.hpp"

#include <iostream>

int main (int argc, char** argv)
{
  if ( argc < 2 )
  {
    std::cerr << "\n\tUsage: " << argv[0] << " <file.xml>\n" << std::endl;
    return (1);
  }

  std::string file (argv[1]);

  try
  {
    ticpp::Document doc (file);
    doc.LoadFile ();

    // get the top adag element
    ticpp::Element * adag = doc.FirstChildElement ("adag");


    // list nodes
    ticpp::Iterator <ticpp::Element> job ("job"); 

    for ( job = job.begin (adag); job != job.end (); job++ )
    {
      std::string s_id        = job->GetAttributeOrDefault ("id",        "Unknown");
      std::string s_namespace = job->GetAttributeOrDefault ("namespace", "Unknown");
      std::string s_name      = job->GetAttributeOrDefault ("name",      "Unknown");
      std::string s_version   = job->GetAttributeOrDefault ("version",   "Unknown");
      std::string s_runtime   = job->GetAttributeOrDefault ("runtime",   "Unknown");

      std::cout << "job [" << s_id << " - " << s_name << "]" << std::endl;

      ticpp::Iterator <ticpp::Element> uses ("uses"); 

      for ( uses = uses.begin (job.Get ()); uses != uses.end (); uses++ )
      {
        std::string s_file     = uses->GetAttributeOrDefault ("file",     "Unknown");
        std::string s_link     = uses->GetAttributeOrDefault ("link",     "Unknown");
        std::string s_register = uses->GetAttributeOrDefault ("register", "Unknown");
        std::string s_transfer = uses->GetAttributeOrDefault ("transfer", "Unknown");
        std::string s_optional = uses->GetAttributeOrDefault ("optional", "Unknown");
        std::string s_type     = uses->GetAttributeOrDefault ("type",     "Unknown");
        std::string s_size     = uses->GetAttributeOrDefault ("size",     "Unknown");

        std::cout << "  uses " << s_link << "\t: " << s_file << std::endl;
      }
    }
    

    // list edges
    ticpp::Iterator <ticpp::Element> child ("child"); 

    for ( child = child.begin (adag); child != child.end (); child++ )
    {
      std::string s_ref  = child->GetAttributeOrDefault ("ref", "Unknown");
      
      std::cout << "child [" << s_ref << "]" << std::endl;

      ticpp::Iterator <ticpp::Element> parent ("parent"); 

      for ( parent = parent.begin (child.Get ()); parent != parent.end (); parent++ )
      {
        std::string s_pref  = parent->GetAttributeOrDefault ("ref", "Unknown");
      
        std::cout << "  parent [" << s_pref << "]" << std::endl;
      }
    }
    
  }
  catch ( const ticpp::Exception & e )
  {
    std::cout << e.what () << std::endl;
  }

  return 0;

}

