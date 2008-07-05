#include <iostream>
#include <saga/saga.hpp>

/*use to make a database
 * program [absolute name of logical file] [list of replicas]
 */
int main(int argc,char **argv){
   saga::url plf(argv[1]);
   plf.set_host("localhost");
   plf.set_scheme("any");
   saga::logical_file::logical_file lf(plf, saga::logical_file::Create | saga::logical_file::CreateParents | saga::logical_file::ReadWrite);
   for(int x=2;x<argc;x++){
      saga::url replica(argv[x]);
      replica.set_host("localhost");
      replica.set_scheme("any");
      lf.add_location(replica);
   } 
   return 0;
}
