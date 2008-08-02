#include <saga/saga.hpp>
#include <saga/saga/cpr.hpp>    
#include <boost/thread/xtime.hpp>
#include <boost/thread.hpp>

#define CHECKPOINT_NAME "remd_checkpoint"

int main (int argc, char* argv[])
{
    // Init Migol/Monitoring
    // uses per default Application Information Service (AIS) configured in 
    // the $SAGA_LOCATON/share/saga/saga_adaptor_migol_cpr.ini
    // required for all subsequent CPR calls (otherwise exception is thrown)
    saga::cpr::service js;
    
    //look for restart files
    //Checkpoint Registration
    saga::url url(CHECKPOINT_NAME);
    saga::cpr::checkpoint chkpt(url);
    chkpt.add_file(saga::url("gsiftp://~/remd/chkpt_it1.dat"));
    chkpt.add_file(saga::url("gsiftp://~/remd/chkpt_it2.dat"));
    
    std::vector<saga::url> files;
    files = chkpt.list_files();
    std::cout << "Received files: " <<std::endl;
    for (unsigned int i = 0; i < files.size(); i++)
    {
        std::cout << files[i] << std::endl;
    }
    
    int sleeptime= 0;
    if(argc==2){
        int input = (int) strtol(argv[1], (char **)NULL, 10);  
        if(input>0){
            sleeptime=input; 
        }
    } 
    
    //std::cout<<"go to sleep for " << sleeptime <<" s"<<std::endl;   
    boost::xtime xt;
    boost::xtime_get(&xt, boost::TIME_UTC);
    xt.sec += sleeptime;
    boost::thread::sleep(xt);  
}
