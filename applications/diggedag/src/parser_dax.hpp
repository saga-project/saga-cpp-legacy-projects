
#ifndef DIGGEDAG_PEGASUS_PARSER_HPP
#define DIGGEDAG_PEGASUS_PARSER_HPP

#include <string>
#include <vector>

#include "util/split.hpp"

#include "diggedag.hpp"


namespace diggedag
{
  namespace dax
  {
    class parser
    {
      private:
        diggedag::dag dag_;
        std::string   filename_;

        void parse_dag  (void);
        void parse_node (const std::string spec);
        void parse_edge (const std::string spec);


      public:
        parser  (const std::string & filename);
        ~parser (void) { }

        diggedag::dag get_dag (void) { return dag_; }
    };

  } // namespace dax

} // namespace diggedag

#endif // DIGGEDAG_PEGASUS_PARSER_HPP

