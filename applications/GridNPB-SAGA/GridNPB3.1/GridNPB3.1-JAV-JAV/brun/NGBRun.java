/*
!-------------------------------------------------------------------------!
!		 							  !
!	         N  A  S     G R I D      B E N C H M A R K S             !
!									  !
!			J A V A     V E R S I O N			  !
!									  !
!                              R U N N G B                                !
!                                                                         !
!-------------------------------------------------------------------------!
!                                                                         !
!    RunNGB implements RunNGB Class for running benchmarks remotely.      !
!                                                                         !
!    Permission to use, copy, distribute and modify this software         !
!    for any purpose with or without fee is hereby granted.               !
!    We request, however, that all derived work reference the             !
!    NAS Grid Benchmarks 3.0 or GridNPB3.0. This software is provided     !
!    "as is" without expressed or implied warranty.                       ! 
!                                                                         !
!    Information on GridNPB3.0, including the concept of                  !
!    the NAS Grid Benchmarks, the specifications, source code,            !
!    results and information on how to submit new results,                !
!    is available at:                                                     !
!                                                                         !
!         http://www.nas.nasa.gov/Software/NPB                            !
!                                                                         !
!    Send comments or suggestions to  ngb@nas.nasa.gov                    !
!    Send bug reports to              ngb@nas.nasa.gov                    !
!                                                                         !
!          E-mail:  ngb@nas.nasa.gov                                      !
!          Fax:     (650) 604-3957                                        !
!                                                                         !
!-------------------------------------------------------------------------!
! GridNPB3.0 Java version                   				  !
!	   M. Frumkin							  !
!-------------------------------------------------------------------------!
*/
package brun;

import tasks.DGraph.*;
import brmi.*;
import NPB_JAV.*;
import NPB_JAV.Timer;

import java.io.*;
import java.text.*;
import java.util.Date;
import java.net.*;

public class NGBRun extends Thread{
  static int pid=0;
  BufferedWriter out=null;
  public DGraph dfg;
  String dfgPath="./dfg/";
  String PrLang="Java";
  BMRequest ngbreq=null;
  NGBResults ngbres=null;

  public NGBRun(){
    ngbreq=new BMRequest();
    setDaemon(true);
  }   
  public NGBRun(BMRequest req){
    ngbreq=req;
    setDaemon(true);
  }   
  public static void main(String argv[]){
    BMRequest req=NGBArgs.ParseCmdLineArgs(argv);
    NGBRun runner= null;
    try{
      runner = new NGBRun(req);
    }catch(OutOfMemoryError e){
      NGBArgs.outOfMemoryMessage();
      System.exit(0);
    }         
    runner.dfg=req.dfg;
    NGBArgs.Banner(req);
    runner.launch();
  }
  public void launch(){
    ngbreq.MachineName=GetLaunchMachine();
    Executor exec = new Executor();
    exec.ngbrunner=this;
    exec.JobOrder=2;  //RunNGB
    exec.start();
    synchronized(exec){
      exec.done=false;
      exec.notify();
    }
  }
  public void RunNGB(){
    BenchClient client=new BenchClient();
    Date currdate=new Date(); 
    BMResults[] res=client.ExecuteRemotely(ngbreq,dfg);
    NGBResults ngbres=AnalyseResults(res,dfg);
    NGBArgs.getBMNameAndClass(ngbreq.NGBName.toUpperCase(),null,ngbreq);
    ngbres.name=ngbreq.NGBName.toUpperCase();
    ngbres.clss=ngbreq.clss;
    ngbres.date=currdate;
    ngbres.print();
 }
 public NGBResults AnalyseResults(BMResults[] res,DGraph dfg){
   NGBResults ngbres=new NGBResults(dfg.id,dfg.name);
   BMRequest req=new BMRequest();
   NGBArgs.getBMNameAndClass(dfg.name.toUpperCase(),null,req);
   ngbres.NGBName=req.NGBName;
   ngbres.clss=req.clss;
   ngbres.verified=1;
   for(int i=0;i<dfg.numNodes;i++){
     if(res[i]==null){
       System.err.println("**AnalyseResults: task "+i+
 			  " have not finished");
       dfg.node[i].verified=0;
     }else{
       dfg.node[i].verified=res[i].verified;
       ngbres.acctime+=res[i].time;
     }
     if(ngbres.verified==1){
       if(dfg.node[i].verified==0){
 	 ngbres.verified=0;
       }else if(dfg.node[i].verified==-1&&dfg.node[i].outDegree==0){
 	 ngbres.verified=-1;
       }
     }
     long mlsec=-1;
     if(mlsec<0||res[i].date.getTime()<mlsec){
       mlsec=res[i].date.getTime();
       ngbres.date=res[i].date;
     }
   }
   ngbres.taTime = res[dfg.numNodes].time;
   return ngbres;
 }
 public String GetLaunchMachine(){
   String lmachine="UNKNOWN";
   try{
     InetAddress addr=InetAddress.getLocalHost();
     if(addr!=null) lmachine=addr.getHostName();    
   }catch(java.net.UnknownHostException e){
     System.out.println(e.toString());   
   }
   return lmachine;
 }
}
