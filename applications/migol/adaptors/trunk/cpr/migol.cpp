extern "C" {
//#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

}

#include <string>
#include "config.hpp"
//#include <saga/util.hpp>
#include "monitor.nsmap"

#if defined(BOOST_HAS_UNISTD_H)
#include <unistd.h>
#endif

#include <boost/assert.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/scoped_array.hpp>
#include <boost/tokenizer.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>

//#include <saga/types.hpp>
//#include <saga/exception.hpp>
//#include <impl/exception.hpp>
#include "common_helpers.hpp"
#include "migol.hpp"
#include <saga/impl/config.hpp>
#include <saga/saga/exception.hpp>
#include <saga/saga/adaptors/task.hpp>
#include <saga/saga/adaptors/attribute.hpp>
#include <saga/saga/url.hpp>
#include <saga/saga/packages/job/job_description.hpp>



#define DEBUG 1
#define JAR_DIR "exe/lib/:lib/"
#define CLIENT_CONFIG "client-config.wsdd"

#define MAX_CLASSPATH 20000

namespace cpr {

    //singleton instance
    boost::shared_ptr<migol> migol::migol_instance; 
    
    /** Constructor **/    
    migol::migol():
    ais_url(""),
    globus_location(""),
    jvm(NULL),
    ais_global_cls(NULL),
    terminate(false),
    initialized(false),
    external_monitoring_host("") ,
    monitorable_thread(TR1::bind(&migol::monitorable_server, this))
    {
        SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
        {
            std::cout<<"Migol Constructor" <<std::endl;
        }
        //scoped_lock lock(mutex); //locks made problem on debian sarge, gcc-3.4
        while(initialized==false){
            SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
            {
                std::cout<<"migol(): Wait for monitoring thread ..." <<std::endl;
            }
            boost::thread::yield();
            //cond.wait(lock);
        }
        initIni();
        if (ini.has_section("preferences")) {
            saga::ini::ini prefs = ini.get_section ("preferences");
            ais_url = prefs.has_entry("ais_url") ? prefs.get_entry("ais_url") : "";
            globus_location = prefs.has_entry("globus_location") ? prefs.get_entry("globus_location") : "";
            external_monitoring_host =(prefs.has_entry("external_monitoring_host") ? prefs.get_entry("external_monitoring_host") : "");
            // boost::trim(external_monitoring_host);
            //        boost::trim(ais_url);
            //        boost::trim(globus_location);
            SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
            {
                std::cout<<"AIS URL: " << ais_url <<std::endl;
                std::cout<<"GLOBUS Location: " << globus_location <<std::endl;
                std::cout<<"SOAP Monitorable URL: " << getUrl()<<std::endl;
                std::cout<<"Reverse Proxy: " << external_monitoring_host<<std::endl;
            }  
        } 
        initJVM();
        reverse_proxy_thread = new boost::thread(TR1::bind(&migol::init_external_monitoring, this));
        //init_external_monitoring();
    }
    
     /** Constructor **/ 
    migol::migol(saga::ini::ini adaptor_ini):
    ais_url(""),
    globus_location(""),
    jvm(NULL),
    ais_global_cls(NULL),
    terminate(false),
    initialized(false),
    external_monitoring_host(""),
    monitorable_thread(TR1::bind(&migol::monitorable_server, this))
{
    SAGA_LOG_INFO("Migol Constructor");
    while(initialized==false){
        SAGA_LOG_BLURB("migol(): Wait for monitoring thread ...");
        boost::thread::yield();
    }
    ini = adaptor_ini;
    if (ini.has_section("preferences")) {
        saga::ini::ini prefs = ini.get_section ("preferences");
        ais_url = prefs.has_entry("ais_url") ? prefs.get_entry("ais_url") : "";
        globus_location = prefs.has_entry("globus_location") ? prefs.get_entry("globus_location") : "";
        external_monitoring_host =(prefs.has_entry("external_monitoring_host") ? prefs.get_entry("external_monitoring_host") : "");
        SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
        {
            std::cout<<"AIS URL: " << ais_url <<std::endl;
            std::cout<<"GLOBUS Location: " << globus_location <<std::endl;
            std::cout<<"SOAP Monitorable URL: " << getUrl()<<std::endl;
            std::cout<<"Reverse Proxy: " << external_monitoring_host<<std::endl;
        }  
    } 
    initJVM();
//    reverse_proxy_thread = new boost::thread(TR1::bind(&migol::init_external_monitoring, this));
    init_external_monitoring();
}
    
/** Destructor **/
migol::~migol(){
    SAGA_LOG_INFO("D'tor ~migol begin: Terminate Migol ...");
    terminate=true;
    //destroyJVM();
    SAGA_LOG_BLURB("Wait for monitorable thread ...");
    monitorable_thread.join();    
    //kill(reverse_proxy_pid, SIGKILL);
    //delete reverse_proxy_thread;
    SAGA_LOG_INFO("D'tor ~migol end: Termination successfull.");        
}
    
void migol::finalize_external_monitoring(){
    JNIEnv *env;
    jclass ssh_proxy_jclass;
    jmethodID mid;
    if(external_monitoring_host!=""){
        env = initJVM();
        if (jvm != NULL)   {  
            ssh_proxy_jclass = env->FindClass("org/globus/ogsa/migol/SshReverseProxy");               
            if(ssh_proxy_jclass !=0)    {                
                mid = env->GetMethodID(ssh_proxy_jclass, "cancelMonitoring",  "()V");
                env->CallVoidMethod(ssh_proxy, mid);
                env->DeleteGlobalRef(ssh_proxy);
            } else {
                printFault(env, "Error finding AisJniClient\n");
            }  
        } else {
            printFault(env, "Error finding AisJniClient\n");
        }  
    }

}  
    
void migol::init_external_monitoring()
{
    while(initialized==false){
            SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
            {
                std::cout<<"init_external_monitoring(): Wait for monitoring thread ..." <<std::endl;
            }
            //boost::thread::yield();
    }
    std::cout<< "init reverse proxy on: " << external_monitoring_host <<std::endl;
    std::cout<< "local web server port: " << soap_port <<std::endl;
    if(external_monitoring_host!=""){
        jmethodID mid;
        //jstring jguid;    
        jstring jexternal_monitoring_host; 
        jclass ssh_proxy_jclass;
        jint port;
        JNIEnv *env;
        SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
        {
            std::cout<<"init SshReverseProxy (JNI) " << std::endl;
        }
        std::map<saga::url, std::map<std::string, std::string> > result_map;
        env = initJVM();
        if (jvm != NULL)   {  
            ssh_proxy_jclass = env->FindClass("org/globus/ogsa/migol/SshReverseProxy");               
            if(ssh_proxy_jclass !=0)    {                
                mid = env->GetMethodID(ssh_proxy_jclass, "<init>",  "(Ljava/lang/String;I)V");
                if(mid !=0)   {
                    jexternal_monitoring_host=env->NewStringUTF(external_monitoring_host.c_str());
                    if (jexternal_monitoring_host == NULL) {
                        printFault(env, "error creating string\n");
                    }
                    port = soap_port;                    
                    jobject ssh_proxy_local = env->NewObject(ssh_proxy_jclass, mid, jexternal_monitoring_host, port);
                    jmethodID mid_get_remote_port = env->GetMethodID(ssh_proxy_jclass, "getRemotePort",  "()I");
                    remote_port = (int) env->CallIntMethod(ssh_proxy_local, mid_get_remote_port);
                    std::cout << "remote port: " << remote_port << std::endl;
                    ssh_proxy = env->NewGlobalRef(ssh_proxy_local); 
                    env->DeleteLocalRef(ssh_proxy_local);
                    env->ReleaseStringUTFChars(jexternal_monitoring_host, NULL);
                    SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
                    {
                        std::cout<<"created SshReverseProxy\n";
                    }
                   
                } else {
                    printFault(env, "Error finding AisJniClient\n");
                }  
            } else {
                printFault(env, "Error finding AisJniClient\n");
            }  
        }
                
        //old deprecated C++
        //std::string ip=get_ip();
//        //std::ostringstream command_stream;
//        //command_stream <<"-g -N -R " << soap_port << ":" << ip << ":" << soap_port << " "<< external_monitoring_host << " \n"; 
//        //std::string command = command_stream.str();
//        
//        std::ostringstream host_stream;
//        host_stream<< soap_port << ":" << ip << ":" << soap_port;
//        std::string host = host_stream.str();
//        
//        reverse_proxy_pid = fork();
//        //std::cout<<"spawn monitoring process: " <<reverse_proxy_pid<<std::endl;
//        if (reverse_proxy_pid == -1) {
//            std::cerr<<"Error creating monitoring process"<<std::endl;
//        } else if (reverse_proxy_pid == 0) {//child process
//   
//            SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
//            {
//                std::cout << "PID: " << reverse_proxy_pid << " - activated external monitoring on: " << external_monitoring_host <<std::endl;
//            }
//            
//            execl("/usr/bin/ssh", "migol_mon", "-g", "-N", "-R", host.c_str(), external_monitoring_host.c_str(), NULL);
//            //int rc = system(command.c_str());
//            //if (rc!=0){
////                SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_ERROR)
////                {    
////                    std::cerr<< "Error executing: "<< command <<std::endl;
////                }
////            }
//        } 
    }
}

void migol::monitorable_server(){
    SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
    {
            std::cout<<"start monitorable server" <<std::endl;
    }    
    //scoped_lock lk(mutex);
    int m, s;      // master and slave sockets
    // init soap environment
    struct soap *soap = soap_new();
    soap_set_namespaces (soap, monitor_namespaces);
    do
    {
        //soap_port = (int) (rand () * 65535.0 / RAND_MAX); //
        soap_port = (int) (rand () % 65535);    //
        //soap_port = randInt(65535);
        //printf ("try host %s:%d\n", get_ip ().c_str(), soap_port);
        m = soap_bind (soap, get_ip ().c_str(), soap_port, 100);
        if (m < 0)
        {
            // soap_print_fault(soap, stderr);
        } 
    }
    while (m < 0);
    SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
    {
        std::cout<<"Socket creation successful: port "<<  soap_port <<std::endl;
        std::cout<<"Wait for request ...\n";
    }  
    std::cout<<"monitorable server started"<<std::endl;
    initialized=true;
    //lk.unlock();
    //cond.notify_all();
    //casoap->accept_timeout = 1000 mSec; 
    soap->accept_timeout = 1;
    while (!terminate)
    {
        s = soap_accept (soap);
        if (s < 0)
        {
            //soap_print_fault (soap, stderr);
            continue;
        }   
        SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
        {
            std::cout<< "Connection from: " << ((soap->ip >> 24) & 0xFF) << "." 
            << ((soap->ip >> 16) & 0xFF) <<"."<< ((soap->ip >> 8) & 0xFF)
            <<"." <<  ((soap->ip) & 0xFF) <<std::endl;
        }
        monitor_serve (soap);  // process RPC request
        SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
        {
            std::cout<< "Request served\n";
        }
        soap_end (soap);  // clean up everything and close socket
    }  
    SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
    {
        std::cout<<"call soap done" <<std::endl;
    }
    soap_done (soap);
    SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
    {
        std::cout<<"call soap free" <<std::endl;
    }
    soap_free(soap);
}

/* Methods */
std::string
migol::register_service(std::string url, std::string service_name, std::string state){
    jmethodID mid;
    jstring jguid, jurl, jservice_name, jstate;
    jstring jais_url;
    JNIEnv *env;
    SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
    {
        std::cout<<"call JNI for registerService()" <<std::endl;
    }
    env = initJVM();
    if (jvm != NULL)   {
        if(ais_global_cls !=0)    {
            mid = env->GetStaticMethodID(ais_global_cls, "registerService",
                    "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
            if(mid !=0)   {
                jais_url=env->NewStringUTF(ais_url.c_str());
                if (jais_url == NULL) {
                    printFault(env, "error creating string\n");
                }
                jurl = env->NewStringUTF(url.c_str());
                if (jurl == NULL) {
                	printFault(env, "error creating string\n");
                }
                jservice_name =  env->NewStringUTF(service_name.c_str());
                if (jservice_name == NULL) {
                	printFault(env, "error creating string\n");
                }
                jstate =  env->NewStringUTF(state.c_str());
                if (jstate == NULL) {
                	printFault(env, "error creating string\n");
                }
                //env->SetObjectArrayElement(jargs,i,env->NewStringUTF(message[i]));
                jguid = (jstring) env->CallStaticObjectMethod(ais_global_cls, mid, jais_url, jurl, jservice_name, jstate);
                if(jguid!=NULL){
                    guid = std::string((env->GetStringUTFChars(jguid, NULL)));
                    env->GetStringUTFChars(jguid, NULL);
                    env->ReleaseStringUTFChars(jguid, NULL);
                    env->ReleaseStringUTFChars(jservice_name, NULL);
                    env->ReleaseStringUTFChars(jstate, NULL);
                    env->ReleaseStringUTFChars(jais_url, NULL);
                    std::cout<<"[MIGOL_ADAPTER] Registered service at AIS with GUID: " << guid << std::endl;           
                }
            }    else{
                printFault(env, "Error finding AisJniClient method: registerService\n");
                
            }
        } else {
            printFault(env, "Error finding AisJniClient\n");
        }      
        return guid;
    }
    else {
        printFault(env, "Error creating JVM\n");
        return NULL;
    }
}


/******************************************************************************/
  
    
bool migol::update_machine(std::string guid, std::string url) {
    jmethodID mid;
    jstring jguid, jurl, jais_url;    
    JNIEnv *env;
    jint result;        
    env = initJVM();
    std::cout<<"update machine "<<  guid<< " url: " << url <<std::endl;

    if (jvm != NULL)   {
        if(ais_global_cls !=0)    {      
            
            if(ais_global_cls !=0)    {
                mid = env->GetStaticMethodID(ais_global_cls, "updateMachine",
                                                 "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I");
                if(mid !=0)   {
                    jais_url=env->NewStringUTF(ais_url.c_str());
                    if (jais_url == NULL) {
                        printFault(env, "error creating string\n");
                    }
                     jurl = env->NewStringUTF(url.c_str());
                    if (jurl == NULL) {
                        printf("error creating string\n");
                    }
                    jguid = env->NewStringUTF(guid.c_str());
                    if (jguid == NULL) {
                        printf("error creating string\n");
                    }
                    if (DEBUG) printf("call update machine\n");
                    result = env->CallStaticIntMethod(ais_global_cls, mid, jais_url,
                                                          jguid, jurl);
                    std::cout<<"Updated machine " <<url << " guid: " << guid << " result:  " <<result <<std::endl;                
                    env->ReleaseStringUTFChars(jais_url, NULL);
                    env->ReleaseStringUTFChars(jurl, NULL);
                    env->ReleaseStringUTFChars(jguid, NULL);
                    return true;
                } else {
                    printFault(env, "Error finding updateMachine method \n");
                }
                
            } else {            
                printFault(env, "Error finding AisJniClient\n");                        
            }           
        }
        else {
            printFault(env, "Error creating JVM\n");
        }
       
  }    
  return false;
}
/******************************************************************************/
bool
    migol::register_checkpoint(std::string guid, std::string fileName){
        jmethodID mid;
        jstring jguid, jfileName;    
        jstring jais_url;  
        JNIEnv *env;
        SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
        {
            std::cout<<"call JNI register_checkpoint: " << guid << " file: " << fileName <<std::endl;
        }
        env = initJVM();
        if (jvm != NULL)   {  
            if(ais_global_cls==NULL){
                ais_global_cls = env->FindClass("org/globus/ogsa/migol/AisJniClient");               
            }              
            if(ais_global_cls !=0)    {
                mid = env->GetStaticMethodID(ais_global_cls, "registerCheckPoint",
                                                 "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z");
                if(mid !=0)   {
                    SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
                    {
                        std::cout<<"found registerCheckPoint method\n";
                    }
                    jais_url=env->NewStringUTF(ais_url.c_str());
                    if (jais_url == NULL) {
                        printFault(env, "error creating string\n");
                    }
                    jfileName = env->NewStringUTF(fileName.c_str());
                    if (jfileName == NULL) {
                        printFault(env, "error creating string\n");
                    }
                    jguid = env->NewStringUTF(guid.c_str());
                    if (jguid == NULL) {
                         printFault(env, "error creating string\n");
                    }
                    SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
                    {
                        std::cout<<"call register checkpoint\n";
                    }
                    env->CallStaticVoidMethod(ais_global_cls, mid, 
                                                  jais_url, jguid, jfileName);
                    SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
                    {
                        std::cout<<"Success register checkpoint " << guid << " file: " << fileName <<std::endl;                
                    }
                    env->ReleaseStringUTFChars(jfileName, NULL);
                    env->ReleaseStringUTFChars(jguid, NULL);   
                    env->ReleaseStringUTFChars(jais_url, NULL);
                    return true;
                } else {
                    fprintf(stderr, "Error finding registerCheckpoint method \n");
                }
                
            } else {            
                std::cerr<< "Error finding AisJniClient\n";                        
                env->ExceptionDescribe();
                env->ExceptionClear();
            }           
        }
        else {
            std::cerr<<  "Error creating JVM\n"; 
        }
        return false;
}

/******************************************************************************/
bool migol::change_service_state(std::string guid, std::string newState) {
    jmethodID mid;
    jstring jguid, jstate, jais_url;    
    JNIEnv *env;
    jboolean result;
    //make sure only one thread accesses JVM
    SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
    {
        std::cout<<"AIS URL: " << ais_url << std::endl;
        std::cout<<"change service state for " << guid << " to " << newState << std::endl;
    }
    env = initJVM();
    if (jvm != NULL)   {  
        if(ais_global_cls==NULL){
            ais_global_cls = env->FindClass("org/globus/ogsa/migol/AisJniClient");               
        }
        if(ais_global_cls !=NULL)    {
                mid = env->GetStaticMethodID(ais_global_cls, "changeServiceState",
                                                "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z");
                if(mid !=0)   {
                    jais_url=env->NewStringUTF(ais_url.c_str());
                    if (jais_url == NULL) {
                        printFault(env, "error creating string\n");
                    }
                    jstate = env->NewStringUTF(newState.c_str());
                    if (jstate == NULL) {
                        printFault(env,"error creating string\n");
                    }
                    jguid = env->NewStringUTF(guid.c_str());
                    if (jguid == NULL) {
                        printFault(env,"error creating string\n");
                    }
                    SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
                    {
                        std::cout<<"Call JNI"<< std::endl;
                    }
                    result = env->CallStaticBooleanMethod(ais_global_cls, mid, 
                                                             jais_url, jguid, jstate);
                    //env->CallStaticVoidMethod(ais_global_cls, mid, 
                    //                                      jais_url, jguid, jstate);
                    SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
                    {
                        std::cout<<"Updated state for " << guid << " to " <<  newState << std::endl;
                    }
                    env->ReleaseStringUTFChars(jstate, NULL);
                    env->ReleaseStringUTFChars(jguid, NULL);   
                    env->ReleaseStringUTFChars(jais_url, NULL);
                    return true;
                } else {
                   printFault(env, "Error finding changeServiceState method \n");
                }
                
            } else {      
                printFault(env,  "Error finding AisJniClient\n");
            }           
        }
        else {
            printFault(env, "Error creating JVM\n");
        }
        return false;
        
    }
    
    
    
/******************************************************************************/
std::vector<saga::url> migol::get_files (std::string guid){
    jmethodID mid;
    jstring jguid;    
    jstring jais_url;
    JNIEnv *env;
    SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
    {
        std::cout<<"call JNI get files: " << guid<< std::endl;
    }
    std::vector<saga::url> result_urls;
    env = initJVM();
    if (jvm != NULL)   {  
        if(ais_global_cls==NULL){
            ais_global_cls = env->FindClass("org/globus/ogsa/migol/AisJniClient");               
        }    
        if(ais_global_cls !=0)    {
            mid = env->GetStaticMethodID(ais_global_cls, "findFileInfo",
                                    "(Ljava/lang/String;Ljava/lang/String;)[Ljava/lang/String;"); 

            if(mid !=0)   {
                SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
                {
                    std::cout<<"found getFileProfiles method\n";
                }
                jais_url=env->NewStringUTF(ais_url.c_str());
                if (jais_url == NULL) {
                    printFault(env, "error creating string\n");
                }
                jguid = env->NewStringUTF(guid.c_str());
                if (jguid == NULL) {
                    printFault(env, "error creating string\n");
                }
                SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
                {
                    std::cout<<"call get files\n";
                }
                //jobject fileprofiles= 
               jobjectArray results  = (jobjectArray) env->CallStaticObjectMethod(ais_global_cls, mid, jais_url,
                                          jguid);
                
               jint len = env->GetArrayLength(results);
             
               for (int i = 0; i < len; i++) {
                    jstring file = (jstring) env->GetObjectArrayElement(results, i);
                    saga::url filename = saga::url((env->GetStringUTFChars(file, NULL)));
                    SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
                    {
                        std::cout<< "Found file: " << filename << std::endl;                
                    }
                    result_urls.push_back(filename);
               }
               env->ReleaseStringUTFChars(jguid, NULL);  
               env->ReleaseStringUTFChars(jais_url, NULL);  
               return result_urls;
            } else {
                std::cerr<< "Error finding getFileProfiles method \n";
                env->ExceptionDescribe();
                env->ExceptionClear();
            }
            
        } else {            
            std::cerr<< "Error finding AisJniClient\n";                        
            env->ExceptionDescribe();
            env->ExceptionClear();
        }           
    }
    else {
        std::cerr<<  "Error creating JVM\n";
    }
    return result_urls;

}
    
    
/******************************************************************************/
    
    std::map<saga::url, std::map<std::string, std::string> > migol::get_files_map (std::string guid){
        jmethodID mid;
        jstring jguid;    
        jstring jais_url;   
        JNIEnv *env;
        SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
        {
            std::cout<<"call JNI get files: " << guid<< std::endl;
        }
         std::map<saga::url, std::map<std::string, std::string> > result_map;
        env = initJVM();
        if (jvm != NULL)   {  
            if(ais_global_cls==NULL){
                ais_global_cls = env->FindClass("org/globus/ogsa/migol/AisJniClient");               
            }
            if(ais_global_cls !=0)    {
                mid = env->GetStaticMethodID(ais_global_cls, "findFileInfoMap",
                                             "(Ljava/lang/String;Ljava/lang/String;)Ljava/util/HashMap;"); 
                
                if(mid !=0)   {
                    SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
                    {
                        std::cout<<"found getFileProfiles method\n";
                    }
                    jais_url=env->NewStringUTF(ais_url.c_str());
                    if (jais_url == NULL) {
                        printFault(env, "error creating string\n");
                    }
                    jguid = env->NewStringUTF(guid.c_str());
                    if (jguid == NULL) {
                        printFault(env, "error creating string\n");
                    }
                    SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
                    {
                        std::cout<<"call get files\n";
                    }
                    jobject jresult_map  = (jobject) env->CallStaticObjectMethod(ais_global_cls, mid, jais_url,
                                                                                       jguid);
                    
                    jclass hashmapClass = env->FindClass("java/util/HashMap");
                    jclass setitfclass = env->FindClass("java/util/Set");
                    
                    jmethodID key_set = env->GetMethodID(hashmapClass, "keySet", "()Ljava/util/Set;");
                    jmethodID get = env->GetMethodID(hashmapClass,"get", "(Ljava/lang/Object;)Ljava/lang/Object;");
                    jmethodID size = env->GetMethodID(hashmapClass,"size", "()I");
                    jmethodID set_toArray = env->GetMethodID(setitfclass,"toArray", "()[Ljava/lang/Object;");
                    
                    //jint jsize = (jint) env->CallObjectMethod(jresult_map,size);
                    jint jsize = (jint) env->CallIntMethod(jresult_map,size);
                    jobject jkeys = (jobject) env->CallObjectMethod(jresult_map,key_set);
                    jobjectArray jkeys_array  = (jobjectArray) env->CallObjectMethod(jkeys,set_toArray);
                    for (int i=0;i<jsize; i++){
                        //get file key
                        jstring jfile = (jstring) env->GetObjectArrayElement(jkeys_array, i);
                        saga::url file_url = saga::url((env->GetStringUTFChars(jfile, NULL)));
                        std::cout << (env->GetStringUTFChars(jfile, NULL)) << std::endl;
                        //get attribute map
                        jobject jattribute_map = (jobject) env->CallObjectMethod(jresult_map, get, jfile);
                        //iterate over attribute map
                        jobject jattribute_keys = (jobject) env->CallObjectMethod(jattribute_map, key_set);
                        jobjectArray jattribute_keys_array  = (jobjectArray) env->CallObjectMethod(jattribute_keys, set_toArray);
                        //jint jattr_size = (jint) env->CallObjectMethod(jattribute_map, size);
                        jint jattr_size = (jint) env->CallIntMethod(jattribute_map, size);
                        std::map<std::string, std::string> attribut_map;
                        for(int k=0; k<jattr_size; k++){
                            jstring jattr = (jstring) env->GetObjectArrayElement(jattribute_keys_array, k);
                            jstring jattr_value = (jstring) env->CallObjectMethod(jattribute_map, get, jattr);
                             std::string attr =  (env->GetStringUTFChars(jattr, NULL));
                            std::string attr_value =  (env->GetStringUTFChars(jattr_value, NULL));
                            std::cout << "Add " << attr << ":" << attr_value  << std::endl;
                            attribut_map[attr]=attr_value;
                            env->ReleaseStringUTFChars(jattr, NULL);  
                            env->ReleaseStringUTFChars(jattr_value, NULL);  
                        }
                        result_map[file_url]=attribut_map;
                        env->ReleaseStringUTFChars(jfile, NULL);  
                    }
                    env->ReleaseStringUTFChars(jguid, NULL);  
                    env->ReleaseStringUTFChars(jais_url, NULL);  
                    return result_map;
                } else {
                    std::cerr<< "Error finding getFileProfiles method \n";
                    env->ExceptionDescribe();
                    env->ExceptionClear();
                }
                
            } else {            
                std::cerr<< "Error finding AisJniClient\n";                        
                env->ExceptionDescribe();
                env->ExceptionClear();
            }           
        }
        else {
            std::cerr<<  "Error creating JVM\n";
        }
        return result_map;
    }
    
/******************************************************************************/
    
bool migol::replicate(std::string guid, int automatic_replication){
    jmethodID mid;
    jstring jguid, jais_url; 
    JNIEnv *env;
    env = initJVM();
    if (jvm != NULL)   {
        if(crs_global_cls==NULL){
           crs_global_cls = env->FindClass("org/globus/ogsa/migol/CrsJniClient");  
        }
        if(crs_global_cls !=0)    {
            mid = env->GetStaticMethodID(crs_global_cls, "replicateCheckpointsAisUrl",
                                            "(Ljava/lang/String;Ljava/lang/String;I)V");
            if(mid !=0)   {
                jguid = env->NewStringUTF(guid.c_str());
                if (jguid == NULL) {
                     printFault(env, "error creating string\n");
                }
                //sprintf(aisconfig, "");
                jais_url = env->NewStringUTF(ais_url.c_str());
                if (jais_url == NULL) {
                     printFault(env, "error creating string\n");
                }
                env->CallStaticObjectMethod(crs_global_cls, mid, jais_url, jguid, (jint) automatic_replication);
                env->ReleaseStringUTFChars(jguid, NULL);
                env->ReleaseStringUTFChars(jais_url, NULL);
            }                                  
        } else {
            printFault(env, "Error finding CrsJniClient\n");
            return false;
        }      
    }
    else {
        printFault(env, "Error creating JVM\n");
        return false;
    }
    return true;   
}
    
/******************************************************************************/
    
bool migol::update_jobdescription(std::string guid, saga::job::description jd){
    jmethodID mid;
    jstring jguid, jais_url, japplication_name(NULL), jexecution_directory(NULL), jarguments, jrestart_arguments; 
    JNIEnv *env;
    env = initJVM();
    
    if (jvm != NULL)   {
        if(ais_global_cls==NULL){
            ais_global_cls = env->FindClass("org/globus/ogsa/migol/AisJniClient");  
        }
        if(ais_global_cls !=0)    {
            //public static void updateJobDescription(String aisUrl, String guid,
            //                                        String applicationName, String executionDirectory,
            //                                        String arguments, String restartArguments) 
            mid = env->GetStaticMethodID(ais_global_cls, "updateJobDescription",
                                         "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
            if(mid !=0)   {
                jguid = env->NewStringUTF(guid.c_str());
                if (jguid == NULL) {
                    printFault(env, "error creating string\n");
                }
                //sprintf(aisconfig, "");
                jais_url = env->NewStringUTF(ais_url.c_str());
                if (jais_url == NULL) {
                    printFault(env, "error creating string\n");
                }
                try {
                    japplication_name = env->NewStringUTF(jd.get_attribute(saga::job::attributes::description_executable).c_str());
                    if (japplication_name == NULL) {
                        printFault(env, "error creating string\n");
                    }
                }
                catch ( saga::exception const & e ) 
                {
                    std::cerr << "saga exception caught: " << e.what () << std::endl;
                }
                try {
                    jexecution_directory = env->NewStringUTF(jd.get_attribute(saga::job::attributes::description_workingdirectory).c_str());
                    if (jexecution_directory == NULL) {
                        printFault(env, "error creating string\n");
                    }
                }
                catch ( saga::exception const & e ) 
                {
                    std::cerr << "saga exception caught: " << e.what () << std::endl;
                }
                std::string args("");
                try{
                    std::vector<std::string> args_vec = jd.get_vector_attribute(saga::job::attributes::description_arguments);
                   
                    for (unsigned int i = 0; i < args_vec.size(); i++)
                    {
                        args.append(args_vec[i]);
                        args.append(" ");
                    }   
                }
                catch ( saga::exception const & e ) 
                {
                    std::cerr << "saga exception caught: " << e.what () << std::endl;
                }
                SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
                {
                    std::cout<<"Arguments: " << args << std::endl;
                }
                jarguments = env->NewStringUTF(args.c_str());
                if (jarguments == NULL) {
                    printFault(env, "error creating string\n");
                }
                jrestart_arguments = env->NewStringUTF(args.c_str());
                if (jrestart_arguments == NULL) {
                    printFault(env, "error creating string\n");
                }
                env->CallStaticObjectMethod(ais_global_cls, mid, jais_url, jguid, japplication_name, jexecution_directory, jarguments, jrestart_arguments);
                env->ReleaseStringUTFChars(jguid, NULL);
                env->ReleaseStringUTFChars(jais_url, NULL);
                env->ReleaseStringUTFChars(japplication_name, NULL);
                env->ReleaseStringUTFChars(jexecution_directory, NULL);
                env->ReleaseStringUTFChars(jarguments, NULL);
                env->ReleaseStringUTFChars(jrestart_arguments, NULL);
            }                                  
        } else {
            printFault(env, "Error finding AisJniClient\n");
            return false;
        }      
    }
    else {
        printFault(env, "Error creating JVM\n");
        return false;
    }
    return true;    
}

/******************************************************************************/
bool migol::submit_job(std::string guid, 
                       std::string contact,
                       std::string executable_start,
                       std::string execution_directory_start,
                       std::string arguments_start,
                       std::string stdin,
                       std::string stdout,
                       std::string stderr,
                       std::string arguments_restart,
                       std::string job_type,
                       std::string number_nodes,
                       std::string number_procs_per_node,
                       std::string number_procs,
                       std::string queue                     
                       ){
       
        jstring jais_url, jguid, jcontact, jexecutable_start, jexecution_directory_start, jarguments_start,
        jstdin, jstdout, jstderr, jarguments_restart, jjob_type, jnumber_nodes, jnumber_procs_per_node, jnumber_procs, jqueue;
        jclass gram2client;
        jmethodID jmid;
        JNIEnv *env;
        env = initJVM();        
        if (jvm != NULL)   {
            gram2client = env->FindClass("org/globus/ogsa/migol/GRAM2JniClient");  
            if(gram2client !=0)    {
                jmid = env->GetStaticMethodID(gram2client, "submitJob",
                                             "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
                if(jmid != 0)   {
                    init_jstring(env, ais_url, jais_url);
                    init_jstring(env, guid, jguid);
                    init_jstring(env, contact, jcontact);
                    init_jstring(env, executable_start, jexecutable_start);
                    init_jstring(env, execution_directory_start, jexecution_directory_start);
                    init_jstring(env, arguments_start, jarguments_start);
                    init_jstring(env, stdin, jstdin);
                    init_jstring(env, stdout, jstdout);
                    init_jstring(env, stderr, jstderr);
                    init_jstring(env, arguments_restart, jarguments_restart);       
                    init_jstring(env, job_type, jjob_type);
                    init_jstring(env, number_nodes, jnumber_nodes);
                    init_jstring(env, number_procs_per_node, jnumber_procs_per_node);
                    init_jstring(env, queue, jqueue);
                    init_jstring(env, number_procs, jnumber_procs);                    
                    
                    env->CallStaticObjectMethod(gram2client, jmid, jais_url, jguid, jcontact, jexecutable_start, jexecution_directory_start, jarguments_start,
                                                jstdin, jstdout, jstderr, jarguments_restart, jjob_type, jnumber_nodes, jnumber_procs_per_node, jnumber_procs, jqueue);
                    env->ReleaseStringUTFChars(jguid, NULL);
                    env->ReleaseStringUTFChars(jexecutable_start, NULL);
                    env->ReleaseStringUTFChars(jexecution_directory_start, NULL);
                    env->ReleaseStringUTFChars(jarguments_start, NULL);
                    env->ReleaseStringUTFChars(jstdin, NULL);
                    env->ReleaseStringUTFChars(jstdout, NULL);
                    env->ReleaseStringUTFChars(jstderr, NULL);
                    env->ReleaseStringUTFChars(jarguments_restart, NULL);
                    env->ReleaseStringUTFChars(jjob_type, NULL);
                    env->ReleaseStringUTFChars(jnumber_nodes, NULL);
                    env->ReleaseStringUTFChars(jnumber_procs_per_node, NULL);
                    env->ReleaseStringUTFChars(jnumber_procs, NULL);
                    env->ReleaseStringUTFChars(jqueue, NULL);
                } else {
                    printFault(env, "Error finding GRAM2JniClient\n");
                    return false;
                }   
            } else {
                printFault(env, "Error finding GRAM2JniClient\n");
                return false;
            }      
        }
        else {
            printFault(env, "Error creating JVM\n");
            return false;
        }
        return true;    
    }

/******************************************************************************/
    /** get job state from GRAM2/AIS **/
    void migol::cancel_job(std::string guid){
        jstring jais_url, jguid;
        jclass gram2client;
        jmethodID jmid;
        JNIEnv *env;
        env = initJVM();        
        if (jvm != NULL)   {
            gram2client = env->FindClass("org/globus/ogsa/migol/GRAM2JniClient");  
            if(gram2client !=0)    {
                jmid = env->GetStaticMethodID(gram2client, "cancelJob",
                                              "(Ljava/lang/String;Ljava/lang/String;)V");
                if(jmid != 0)   {
                    init_jstring(env, ais_url, jais_url);
                    init_jstring(env, guid, jguid);
                    
                    //call java method
                    env->CallStaticVoidMethod(gram2client, jmid, jais_url, jguid);
                    env->ReleaseStringUTFChars(jguid, NULL);
                    env->ReleaseStringUTFChars(jais_url, NULL);  
                } else {
                    printFault(env, "Error finding GRAM2JniClient\n");
                }   
            } else {
                printFault(env, "Error finding GRAM2JniClient\n");
            }      
        }
        else {
            printFault(env, "Error creating JVM\n");
        }
    }

    
    
/******************************************************************************/
    /** get job state from GRAM2/AIS **/
    std::string migol::get_job_state(std::string guid){        
        std::string state;
        jstring jais_url, jguid, jstate;
        jclass gram2client;
        jmethodID jmid;
        JNIEnv *env;
        env = initJVM();        
        if (jvm != NULL)   {
            gram2client = env->FindClass("org/globus/ogsa/migol/GRAM2JniClient");  
            if(gram2client !=0)    {
                jmid = env->GetStaticMethodID(gram2client, "getState",
                                              "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
                if(jmid != 0)   {
                    init_jstring(env, ais_url, jais_url);
                    init_jstring(env, guid, jguid);
                    
                    //call java method
                    jstate = (jstring)env->CallStaticObjectMethod(gram2client, jmid, jais_url, jguid);
                    if(jstate!=NULL){
                        state = std::string((env->GetStringUTFChars(jstate, NULL)));
                    }
                    env->ReleaseStringUTFChars(jstate, NULL);
                    env->ReleaseStringUTFChars(jguid, NULL);
                    env->ReleaseStringUTFChars(jais_url, NULL);  
                    return state;
                } else {
                    printFault(env, "Error finding GRAM2JniClient\n");
                    return NULL;
                }   
            } else {
                printFault(env, "Error finding GRAM2JniClient\n");
                return NULL;
            }      
        }
        else {
            printFault(env, "Error creating JVM\n");
            return NULL;
        }
        return NULL;    
    }
    
/******************************************************************************/

    void migol::init_jstring(JNIEnv *env, std::string input_string, jstring &output_string){
        //if (input_string.length()>0) {
            output_string = env->NewStringUTF(input_string.c_str());        
            if (output_string == NULL) {
                printFault(env, "Error creating string\n");
            }
        //}        
    }
    
/******************************************************************************/

    void
    migol::printFault(JNIEnv* env, std::string message){
      std::cerr<< message; 
      env->ExceptionDescribe();
      env->ExceptionClear();
    }

/******************************************************************************/
/** Init JVM **/
JNIEnv* migol::initJVM(){
	JavaVMOption options[3];
	JavaVMInitArgs vm_args;
	long status;
    jint res;
    JNIEnv *env;
    char* jvmOptionChar = NULL;
    char* clientConfigOptionChar=NULL;
    if (jvm==NULL){
        jvmOptionString = initJVMOptions();
        clientConfigOption = discoverClientConfig();
        
        //JavaVMOption does not accept const char* => thus a new char* array is required
        jvmOptionChar = (char*) malloc(sizeof(char)*jvmOptionString.length());
        strcpy(jvmOptionChar, jvmOptionString.c_str());
        options[0].optionString=jvmOptionChar;
        
        clientConfigOptionChar = (char*) malloc(sizeof(char)*clientConfigOption.length());
        strcpy(clientConfigOptionChar, clientConfigOption.c_str());

        options[1].optionString=clientConfigOptionChar;
        options[2].optionString="-verbose:jni";   
        SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
        {        	
            std::cout<<"JVM Option String: " << jvmOptionChar << std::endl;
        	std::cout<<"JVM Axis Option String: "<< clientConfigOptionChar<<std::endl;
        }
        memset(&vm_args, 0, sizeof(vm_args));
        vm_args.version = JNI_VERSION_1_4;
        if (DEBUG){
        	vm_args.nOptions = 2;
        } else {
        	vm_args.nOptions = 2;
        }
        vm_args.options = options;
        status = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
        if (status != JNI_OK)   {
            std::cerr<< "Error creating Java VM with error code: " << status <<std::endl;
        }
    } else {
//        SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
//        {  
//            std::cout<<"Attach current thread to JVM\n";
//        }
        res = jvm->AttachCurrentThread((void **)&env, NULL);
        if (res < 0)   {
          printFault(env, "FAILURE while attaching thread to JVM.\n");
          return NULL;
        } 
        SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
        {  
            std::cout<<"Attached current thread to JVM\n";
        }
    }
    if (ais_global_cls== NULL){
        ais_global_cls = env->FindClass("org/globus/ogsa/migol/AisJniClient");
    }
    if (ais_global_cls == NULL){
       printFault(env, "Error finding AisJniClient.\n");
    }
    //free memory
    if(jvmOptionChar){
            free(jvmOptionChar);
    }
    if(clientConfigOptionChar){
            free(clientConfigOptionChar);
    }
    return env;    
}
        

/**
 * discover client configuration file in globus home or lib directories
 */
std::string migol::discoverClientConfig(){
    std::string globus_loc;
	std::string filename;
	std::string optionString;
	std::string jarDirs;
    std::string globus_env;
    int found=0;
	optionString.assign("-Daxis.ClientConfigFile=");	
	
    if (globus_location!=""){
        filename=globus_location+"/"+CLIENT_CONFIG;
		if (fileExists(filename)==0){
            optionString.append(filename);
			found=1;
		}        
    }else {
        SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_WARNING)
        {
            std::cout<< "No globus_location in migol_logicalfile_adaptor.ini set!";
        }
    }
    
if (found==0){
    globus_env= (saga::safe_getenv("GLOBUS_LOCATION")!=NULL) ? (saga::safe_getenv("GLOBUS_LOCATION")) :"";    
    if (globus_env!=""){
        filename=globus_env+"/"+CLIENT_CONFIG;
		if (fileExists(filename)==0){
            optionString.append(filename);
			found=1;
		}        
    } else {
        std::cout<< "No globus location set. " << std::endl;
    }
}
	if (found==0){
        SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
        {  
            std::cout<<"search for client-config.xml - cycle through jar dirs\n";
        }
	    jarDirs.assign(JAR_DIR);
        typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
        boost::char_separator <char> sep (SAGA_INI_PATH_DELIMITER);
        tokenizer           tok (jarDirs, sep);
        tokenizer::iterator tok_end = tok.end ();
        std::string cwd =  boost::filesystem::current_path().string();
        for ( tokenizer::iterator it = tok.begin (); it != tok_end; ++it )
        {
            std::string dir = *it;
            std::cout<<"check: " << dir << std::endl;
            filename=cwd+"/"+dir+"/"+CLIENT_CONFIG;
            if (fileExists(filename)==0){
                optionString.append(filename);
                break;
            }
        }
	}
    return optionString;
}

int migol::fileExists(std::string filename){
	FILE *client_config=NULL;
	client_config=fopen(filename.c_str(), "r");
	if (client_config!=NULL){
		fclose(client_config);
		return 0;
	} else {
		return 1;
	}
	
}

/******************************************************************************/
/** Destroy JVM **/
void migol::destroyJVM(){
    if (jvm){
        jvm->DestroyJavaVM();
        jvm=NULL;
    }
}

/**
 * initialize classpath from globus location and lib directory
 */
std::string migol::initJVMOptions(){
    std::string classpath("");
    std::string globus_env("");
    std::string saga_loc_env("");
    std::string jvmOptionString("");
    std::string jarDirs; 
    SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
    {
        std::cout<<"Init JVM\n";
    }
//  if ( getenv("CLASSPATH") != NULL){
//        classpath= (saga::safe_getenv("CLASSPATH")!=NULL) ? (saga::safe_getenv("CLASSPATH")) :"";    
//    }
    //if globus location set add jars from globus location
    if (globus_location!=""){
        std::cout<<"search for jars in globus_location (saga.ini): " <<globus_location <<std::endl;
        addJarsFromDir(globus_location + "/lib", classpath);           
    }else {
        SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_WARNING)
        {
            std::cout<< "No globus_location in migol_logicalfile_adaptor.ini set!";
        }
    }
    
    globus_env= (saga::safe_getenv("GLOBUS_LOCATION")!=NULL) ? (saga::safe_getenv("GLOBUS_LOCATION")) :"";    
    if (globus_env!=""){
        std::cout<<"search for jars in ${GLOBUS_LOCATION}/lib: " <<globus_env <<"/lib"<<std::endl;
        addJarsFromDir(globus_env + "/lib", classpath);           
    }else {
        SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_WARNING)
        {
            std::cout<< "No GLOBUS_LOCATION environment set!";
        }
    }
    
    saga_loc_env= (saga::safe_getenv("SAGA_LOCATION")!=NULL) ? (saga::safe_getenv("SAGA_LOCATION")) :"";    
    if (saga_loc_env!=""){
        std::cout<<"search for jars in ${SAGA_LOCATION}/lib: " <<saga_loc_env <<"/lib"<<std::endl;
        addJarsFromDir(saga_loc_env + "/lib", classpath);           
    }else {
        SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_WARNING)
        {
            std::cout<< "No SAGA_LOCATION environment set!";
        }
    }
    
    //iterate through a give list of directories
    jarDirs.assign(JAR_DIR);
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator <char> sep (SAGA_INI_PATH_DELIMITER);
    tokenizer           tok (jarDirs, sep);
    tokenizer::iterator tok_end = tok.end ();
    for ( tokenizer::iterator it = tok.begin (); it != tok_end; ++it )
    {
        std::string dir = *it;
        SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
        {
            std::cout<<"cycle through jar dirs "<<jarDirs;
        }
        addJarsFromDir(dir, classpath);
        *it++;        
    }
    //build classpath option string
    jvmOptionString.assign("-Djava.class.path=");
    jvmOptionString.append(classpath);
    return jvmOptionString;
}

/**
 * Add all jars in dirname to classpath
 */
void migol::addJarsFromDir(std::string dirname, std::string &classpath){
    DIR *dp;
    struct dirent *ep;
    std::string fulldirname;    
    std::string fullfilename;
    std::string filename;
    if (strstr(dirname.c_str(), "/")==NULL){ //relative path given
    	fulldirname = boost::filesystem::current_path().string();;
    	fulldirname.append("/");
    	fulldirname.append(dirname);
    } else { //absolute path given
    	fulldirname.assign(dirname);
    }
    SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
    {
        std::cout << "add jars from " << fulldirname <<std::endl;
    }
    dp = opendir(fulldirname.c_str());
    if (dp != NULL) {
        int counter=0;
        while ((ep = readdir(dp))){
            filename= ep->d_name;
            if (filename.find(".jar")!= std::string::npos){
                if(classpath!=""){ //print separator
                    fullfilename.assign(SAGA_INI_PATH_DELIMITER);
                }
                fullfilename.append(dirname);
                fullfilename.append("/");
                fullfilename.append(filename);
                classpath=classpath+fullfilename;
                counter++;
            }
        }
        (void) closedir(dp);
        SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
        {
            std::cout<<"Found " << counter << " jar files." <<std::endl;
        }
        
        
    } else {
        SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
        {
             std::cout<<"No jars found.";
        }
    }	
}

std::string
migol::get_ip ()
{
    char hostname[256];
    struct hostent *hostinfo;
    int n = 0;
    struct in_addr addr;
    char *ip = NULL;
    gethostname (hostname, sizeof (hostname));
    hostinfo = gethostbyname (hostname);
    while (hostinfo->h_addr_list[n])
    {
	    memcpy (&addr, hostinfo->h_addr_list[n], hostinfo->h_length);
	    ip = inet_ntoa (addr);
	    n++;
    }
    std::string ipString(ip);
    //free(ip);
    return ipString;
}

/*
 * builds URL from PORT and local ip address
 */
std::string
migol::getUrl ()
{
    std::string url;
    if(external_monitoring_host!=""){//TODO
        url.assign("http://");
        url.append(external_monitoring_host);
        url.append(":");
        std::ostringstream port;
        port << remote_port;
        url.append(port.str());
    } else {
        url.assign("http://");
        url.append(get_ip());
        url.append(":");
        std::ostringstream port;
        port << soap_port;
        url.append(port.str());
    }
    return url;
}


void migol::initIni (void)
{
    // look in the current directory first
    namespace fs = boost::filesystem;
    //std::string cwd = fs::current_path().string() + "/.saga.ini";
    //detail::handle_ini_file     (ini, cwd);
    
    // afterwards in the standard locations
#if !defined(BOOST_WINDOWS)   // /etc/saga.ini doesn't make sense for Windows
    detail::handle_ini_file     (ini, std::string ("/etc/saga.ini"));
#endif
    detail::handle_ini_file_env (ini, "SAGA_LOCATION", "/share/saga/saga.ini");
    detail::handle_ini_file_env (ini, "HOME",          "/.saga.ini");
    detail::handle_ini_file_env (ini, "PWD",           "/.saga.ini");
    detail::handle_ini_file_env (ini, "SAGA_INI");
}

//private functions
std::string  migol::init_migol_context(saga::ini::ini ini)
{
        SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
        {
            std::cout<<"init migol context" <<std::endl;
        }
        char* guid_env = NULL;
        guid_env =saga::safe_getenv("MIGOL_GUID");
        std::string migol_guid("");    
        boost::shared_ptr<cpr::migol> mig= cpr::migol::instance(ini);
        if (guid_env==NULL){
            SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
            {
                std::cout<<"New service - register at AIS" <<std::endl;
            }
            migol_guid = mig->register_service(mig->getUrl(), "SAGA App", "active");    
            std::cout<<"Migol GUID: " << migol_guid << std::endl;
        } else {
            SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
            {
                std::cout<<"Existing service - set state to active" <<std::endl;
            }
            migol_guid = guid_env;
            mig->change_service_state(migol_guid, "active"); 
        }
        SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
        {
            std::cout<<"Update machine" <<std::endl;
        }
        mig->update_machine(migol_guid, mig->getUrl());
        return migol_guid;
} 
    
    
} //end namespace

/** gSOAP functions in flat namespace **/

int __migol__checkPoint(struct soap*, char *migoldef__checkPoint, bool &migoldef__checkPointResponse){
    std::cout<<"checkPoint"<<std::endl;
    migoldef__checkPointResponse=true;
    return SOAP_OK;
}

int __migol__checkService(struct soap*, char *migoldef__checkService, bool &migoldef__checkServiceResponse){
    std::cout<<"checkService"<<std::endl;
    migoldef__checkServiceResponse=true;
    return SOAP_OK;
}

int __migol__Destroy(struct soap*, _lifetime__Destroy *lifetime__Destroy, _lifetime__DestroyResponse *lifetime__DestroyResponse){
	std::cout<<"destroy"<<std::endl;
    exit(0);
    return SOAP_OK;
}

