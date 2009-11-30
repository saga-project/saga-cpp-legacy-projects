
#ifndef DIGEDAG_PEGASUS_PARSER_HPP
#define DIGEDAG_PEGASUS_PARSER_HPP

#include <string>
#include <vector>

#include "util/split.hpp"

#include "digedag.hpp"


namespace digedag
{
  namespace dax
  {
    class parser
    {
      private:
        sp_t <dag>      dag_;
        std::string     dag_file_;
        std::string     scheduler_file_;

        void parse_dag  (void);
        void parse_node (const std::string spec);
        void parse_edge (const std::string spec);


      public:
        parser (const std::string & dag_file, 
                const std::string & scheduler_file);
        ~parser (void);

        sp_t <dag> get_dag (void) 
        { 
          return dag_; 
        }
    };

  } // namespace dax

} // namespace digedag

#endif // DIGEDAG_PEGASUS_PARSER_HPP

