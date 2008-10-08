/*
!-------------------------------------------------------------------------!
!                                                                         !
!                   N A S   G R I D   B E N C H M A R K S                 !  
!                                                                         !
!			 J A V A	 V E R S I O N  		  !
!                                                                         !
!                           B E N C H C L I E N T                         !
!                                                                         !
!-------------------------------------------------------------------------!
!                                                                         !
!    BenchClient implements Benchmark Client Class.                       !
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
package brmi;

import tasks.DGraph.*;
import brun.*;
import NPB_JAV.Timer;

import java.rmi.Naming; 
import java.rmi.Remote;
import java.rmi.RemoteException; 
import java.rmi.RMISecurityManager;
import java.lang.Object;
import java.text.*;

public class BenchClient extends Thread{
  static int pid=-1;
  public void BenchClient(){}
  public BMResults[] ExecuteRemotely(BMRequest req,DGraph dfg){
    BMResults[] res=null;
    if (System.getSecurityManager() == null) { 
  	System.setSecurityManager(new RMISecurityManager()); 
    }try{    
      int t_total=0;
      Timer timer = new Timer();  
      timer.resetAllTimers();
      timer.start(t_total);
      req.dfg=dfg;
      res=LaunchNodes(req);
      timer.stop(t_total); 
      res[dfg.numNodes]=new BMResults();
      res[dfg.numNodes].time = timer.readTimer(t_total);
    }catch (Exception e) { 
      System.out.println("BenchClient exception: " + e.getMessage()); 
      e.printStackTrace(); 
    }
    return res;
  }
  public BMResults[] LaunchNodes(BMRequest req){
    DGraph dfg=req.dfg;
    Executor exec[]=new Executor[dfg.numNodes];
    BMRequest reqst[]=new BMRequest[dfg.numNodes];
    System.err.println("\n launching "+NGBArgs.getBanner(dfg.name));
    Timer timer = new Timer();  
    timer.resetAllTimers();
    for(int i=0;i<dfg.numNodes;i++){
      DGNode nd=dfg.node[i];
      reqst[i]=new BMRequest(req);
      FormRequest(reqst[i],nd,req);
      reqst[i].dfg=dfg;

      System.err.println(" node."+i+" "+dfg.node[i].name+
                         " started at "+reqst[i].MachineName);
      nd.verified=1;
      exec[i]=new Executor();
      exec[i].Init(reqst[i]);
      timer.start(i);
      exec[i].start();
    }
    for(int i=0;i<dfg.numNodes;i++){
      synchronized(exec[i]){
  	exec[i].done=false;
        exec[i].notify();
      }
    }
    int finnum=0;
    for(int i=0;i<dfg.numNodes;i++){
      dfg.node[i].attribute=0;
    }
    System.err.println();
    BMResults res[]=new BMResults[dfg.numNodes+1];
    while(finnum<dfg.numNodes){
      for(int i=0;i<dfg.numNodes;i++){
        if(dfg.node[i].attribute==1) continue;
        if(!exec[i].done){
          try{Thread.sleep(1);}
          catch(InterruptedException ie){};
          continue;
        }
        timer.stop(i);
  	exec[i].res.wctime = timer.readTimer(i);  
        try{exec[i].join();}catch(InterruptedException e){
          System.err.println("LaunchNodes: node["+i+
        		     "]. Interrupted exception "+e.getMessage());
          e.printStackTrace(); 
        }
        finnum++;
        dfg.node[i].attribute=1;
        res[i]=exec[i].res;
        res[i].name=dfg.node[i].name;
        dfg.node[i].verified=res[i].verified;
      }
    }
    System.err.println();    
    return res;
  }
  int FormRequest(BMRequest req,DGNode nd,BMRequest reqin){
    String str=nd.name;
    req.pid=nd.id;
    req.ndid=nd.id;
    req.NGBName=reqin.NGBName;
    req.PrLang=reqin.PrLang;
    req.serial=reqin.serial;
    req.numthreads=reqin.numthreads;
    
    int fromidx=0,
        idx=str.indexOf('.',fromidx);
    req.BMName=str.substring(fromidx,idx-fromidx);
    fromidx=idx+1;
    idx=str.indexOf('.',fromidx);
    char tmpclss[]=new char[str.length()];
    str.getChars(fromidx,fromidx+1,tmpclss,0);
    req.clss=tmpclss[0];
    fromidx=idx+1;
    if(0<fromidx&&fromidx<str.length()) req.MachineName=str.substring(fromidx);
    else {
      req.MachineName=reqin.MachineName; //local host
      nd.name+="."+req.MachineName;
    }
    return 1;
  }
}
