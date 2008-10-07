/*
!-------------------------------------------------------------------------!
!                                                                         !
!                   N A S   G R I D   B E N C H M A R K S                 !  
!                                                                         !
!			 J A V A	 V E R S I O N  		  !
!                                                                         !
!                           B E N C H U N I O N                           !
!                                                                         !
!-------------------------------------------------------------------------!
!                                                                         !
!    BenchUnion implements Benchmark Union Class.                         !
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

import tasks.*;
import tasks.DGraph.*;
import NPB_JAV.*;

import java.io.Serializable;

public class BenchUnion{
  BTTask bttask;    
  SPTask sptask;
  LUTask lutask;
  FTTask fttask;
  CGTask cgtask;
  MGTask mgtask;
  ISTask istask;
  BMRequest req;
  BMResults res;
  String benchtype;
  public BenchUnion(BMRequest extreq){
      bttask=null;
      sptask=null;
      lutask=null;
      fttask=null;
      cgtask=null;
      mgtask=null;
      istask=null;

      req=extreq;
      benchtype=req.BMName;
      if( req.BMName.equals("BTTask")){
        bttask = new BTTask(req);
      }else if(req.BMName.equals("SPTask")){
  	sptask = new SPTask(req);
      }else if(req.BMName.equals("LUTask")){
        lutask = new LUTask(req);
      }else if(req.BMName.equals("FTTask")){
        fttask = new FTTask(req);
      }else if(req.BMName.equals("CG")||req.BMName.equals("CGTask")){
        cgtask = new CGTask(req.clss, req.numthreads, req.serial);
      }else if(req.BMName.equals("MGTask")){
        mgtask = new MGTask(req);
      }else if(req.BMName.equals("IS")||req.BMName.equals("ISTask")){
        istask = new ISTask(req.clss, req.numthreads, req.serial);
      }
  }
  public void startBenchmark(){
      if( benchtype.equals("BTTask")){
  	  if(req.serial) bttask.runTask();
          else bttask.start();
      }else if(benchtype.equals("SPTask")){
  	  if(req.serial) sptask.runTask(); 
          else sptask.start();
      }else if(benchtype.equals("LUTask")){
  	  if(req.serial) lutask.runTask();
          else lutask.start();
      }else if(benchtype.equals("FTTask")){
  	  if(req.serial) fttask.runTask();
          else fttask.start();
      }else if(benchtype.equals("CG")||benchtype.equals("CGTask")){
  	  if(req.serial) cgtask.runBenchMark();
          else cgtask.start();
      }else if(benchtype.equals("MGTask")){
  	  if(req.serial) mgtask.runTask();
          else mgtask.start();
      }else if(benchtype.equals("IS")||benchtype.equals("ISTask")){
  	  if(req.serial) istask.runBenchMark();
          else istask.start();
      }
  }
  public void benchWait() throws InterruptedException{
      if( benchtype.equals("BTTask") ){
          bttask.join();
      }else if(benchtype.equals("SPTask")){
          sptask.join();
      }else if(benchtype.equals("LUTask")){
          lutask.join();
      }else if(benchtype.equals("FTTask")){
          fttask.join();
      }else if(benchtype.equals("CG")||benchtype.equals("CGTask")){
          cgtask.join();
      }else if(benchtype.equals("MGTask")){
          mgtask.join();
      }else if(benchtype.equals("IS")||benchtype.equals("ISTask")){
          istask.join();
      }
      System.runFinalization();
      System.gc();
  }
  
  public double getTime(){
    double tm=0.0;
      if( benchtype.equals("BTTask") ){
          tm=bttask.getTime();
      }else if(benchtype.equals("SPTask")){
          tm=sptask.getTime();
      }else if(benchtype.equals("LUTask")){
          tm=lutask.getTime();
      }else if(benchtype.equals("FTTask")){
          tm=fttask.getTime();
      }else if(benchtype.equals("CG")||benchtype.equals("CGTask")){
          tm=cgtask.getTime();
      }else if(benchtype.equals("MGTask")){
          tm=mgtask.getTime();
      }else if(benchtype.equals("IS")||benchtype.equals("ISTask")){
          tm=istask.getTime();
      }    
      return tm;
  }
  public BMResults getResults(){
    res=new BMResults(-1);
    if( benchtype.equals("BTTask") ){
  	res=bttask.getResults();
    }else if(benchtype.equals("SPTask")){
  	res=sptask.getResults();
    }else if(benchtype.equals("LUTask")){
  	res=lutask.getResults();
    }else if(benchtype.equals("FTTask")){
  	res=fttask.getResults();
    }else if(benchtype.equals("CG")||benchtype.equals("CGTask")){
  	res=cgtask.getResults();
    }else if(benchtype.equals("MGTask")){
  	res=mgtask.getResults();
    }else if(benchtype.equals("IS")||benchtype.equals("ISTask")){
  	res=istask.getResults();
    }	 
    res.acctime=res.time+req.acctime;
    return res;
  }
  
  public SparseA getField() {
    SparseA spa=new SparseA(req.BMName+"."+String.valueOf(req.clss));
    if( benchtype.equals("BTTask") ){
  	spa=bttask.getField();
    }else if(benchtype.equals("SPTask")){
  	spa=sptask.getField();
    }else if(benchtype.equals("LUTask")){
  	spa=lutask.getField();
    }else if(benchtype.equals("FTTask")){
  	spa=fttask.getField();
    }else if(benchtype.equals("CG")||benchtype.equals("CGTask")){
  	spa=cgtask.getField();
    }else if(benchtype.equals("MGTask")){
  	spa=mgtask.getField();
    }else if(benchtype.equals("IS")||benchtype.equals("ISTask")){
  	spa=istask.getField();
    }	 
    return spa;
  }
  public SparseA getMachNum() {
    SparseA spa=new SparseA(req.BMName+"."+String.valueOf(req.clss));
    if( benchtype.equals("BTTask") ){
  	spa=bttask.getMachNum();
    }else if(benchtype.equals("SPTask")){
  	spa=sptask.getMachNum();
    }else if(benchtype.equals("LUTask")){
  	spa=lutask.getMachNum();
    }	 
    return spa;
  }
  public void finalize() throws Throwable{
    if(req!=null){
      req.spa=null;
      req.dfg=null;
    }
    req=null;
    res=null;
    if( benchtype.equals("BTTask") ){
      bttask.finalize();
    }else if(benchtype.equals("SPTask")){
      sptask.finalize();
    }else if(benchtype.equals("LUTask")){
      lutask.finalize();
    }else if(benchtype.equals("FTTask")){
      fttask.finalize();
    }else if(benchtype.equals("CG")||benchtype.equals("CGTask")){
      cgtask.finalize();
    }else if(benchtype.equals("MGTask")){
      mgtask.finalize();
    }else if(benchtype.equals("IS")||benchtype.equals("ISTask")){
      istask.finalize();
    }	 
  }
}
