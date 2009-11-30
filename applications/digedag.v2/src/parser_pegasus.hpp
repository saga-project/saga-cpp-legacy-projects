
#ifndef DIGEDAG_PEGASUS_PARSER_HPP
#define DIGEDAG_PEGASUS_PARSER_HPP

#include <string>
#include <vector>

#include "util/split.hpp"
#include "digedag.hpp"


namespace digedag
{
  namespace pegasus
  {
    class parser
    {
      private:
        digedag::dag           * dag_;

        std::string               filename_;
        std::string               basename_;
        std::string               basedir_;
        std::vector <std::string> files_;

        void        parse_dag  (void);
        void        parse_node (const std::string spec);
        void        parse_edge (const std::string spec);

        std::string get_name   (const std::string filename);
        std::string get_dir    (const std::string filename);


      public:
        parser  (const std::string & filename);
        ~parser (void);

        digedag::dag * get_dag (void) { return dag_; }
    };
  }
} // namespace digedag

#endif // DIGEDAG_PEGASUS_PARSER_HPP

