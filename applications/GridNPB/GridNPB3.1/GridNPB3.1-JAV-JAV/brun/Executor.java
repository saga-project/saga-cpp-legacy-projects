/*
!-------------------------------------------------------------------------!
!                                                                         !
!                   N A S   G R I D   B E N C H M A R K S                 !  
!                                                                         !
!			 J A V A	 V E R S I O N  		  !
!                                                                         !
!                            E X E C U T O R                              !
!                                                                         !
!-------------------------------------------------------------------------!
!                                                                         !
!    Executor implements Executor class for launching asynchronous jobs.  !
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

import java.rmi.*; 

public class Executor extends Thread{
  public BMRequest req=null;
  public BMResults res=null;
  public boolean done=true;
  public int id=-1;
  public int JobOrder=0;
  public ArcHead ahead[];
  public NGBRun ngbrunner=null;
  
  public void Executor(){
    setPriority(Thread.MAX_PRIORITY);
    setDaemon(true);
  }
  public void Init(BMRequest reqst){
    req=new BMRequest(reqst);
    id=reqst.pid;
    res=new BMResults(req.pid);
    setPriority(Thread.MAX_PRIORITY);
    setDaemon(true);      
  }      
  public void run(){    
    synchronized(this){
      while(done==true){
        try{
	  Thread.sleep(10);
	  wait();
        }catch(InterruptedException ie){}
      }
      step();
      done=true;
      notifyAll();
    }
  }
  public void step(){
    if(JobOrder==0){ //Execute request on remote machine
      try{
        Benchmark RemBench= 
        (Benchmark) Naming.lookup("//"+req.MachineName+"/BenchmarkServer");
        RemBench.startBenchmark(req,res);
        res=RemBench.getResults(req);
      }catch (Exception e) { 
	System.out.println("Executor "+id+" exception: " + e.getMessage()); 
	e.printStackTrace(); 
      }
    }else if(JobOrder==1){ //Check arc head pointer
      while(ahead==null||ahead[id]==null){
        try{
	  Thread.sleep(10);
	  wait();
        }catch(InterruptedException ie){}
      }    
    }else if(JobOrder==2){ //Execute RunNGB
      ngbrunner.RunNGB();
    }
  }
}
