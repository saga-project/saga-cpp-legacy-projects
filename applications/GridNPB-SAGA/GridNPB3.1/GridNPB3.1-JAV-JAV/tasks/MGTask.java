/*
!-------------------------------------------------------------------------!
!                                                                         !
!                   N A S   G R I D   B E N C H M A R K S                 !  
!                                                                         !
!			 J A V A	 V E R S I O N  		  !
!                                                                         !
!                              M G T A S K                                !
!                                                                         !
!-------------------------------------------------------------------------!
!                                                                         !
!    MGTask encapsulates MG benchmark of NPB_JAV benchmark.               !
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
package tasks;

import tasks.DGraph.*;
import NPB_JAV.MG.*;
import NPB_JAV.Timer.*;

import java.io.*;
import java.text.*;

public class MGTask extends NPB_JAV.MG{
  public int bid=-1;
  public BMResults results;
  public BMRequest request=null;
  public boolean timeron=false;
  public double rnm2, rnmu, epsilon;
  public int n1, n2, n3, nn;

  String t_names[];
  int is1, is2, is3, ie1, ie2, ie3;
  int nsizes[];

  public MGTask(char clss,int np,boolean ser ){
    super(clss,np,ser);
  }
  public MGTask(BMRequest req){ 
    super(req.clss,req.numthreads, req.serial);
    request=req;
    serial=req.serial;
    int np=req.numthreads;
    bid=req.pid;
    if(request.dfg!=null&&request.dfg.name.indexOf("ED")<0){
      nit /= req.getNGBdepth();
      if(nit<=0) nit=1;
      if(request.dfg.name.indexOf("MB")>=0){
  	int cnum=request.ndid%3;
        nit*=(1.0-0.5*cnum/(cnum+1.0));
        if(nit<=0) nit=1;
      }
    }  	
  }
  public static void main(String argv[] ){
    MGTask mgtask=null;

    BMArgs.ParseCmdLineArgs(argv,BMName);
    char CLSS=BMArgs.CLASS;
    int np=BMArgs.num_threads;
    boolean serial=BMArgs.serial;

    try{ 
      mgtask = new MGTask(CLSS,np,serial);
    }catch(OutOfMemoryError e){
      BMArgs.outOfMemoryMessage();
      System.exit(0);
    }
    mgtask.runTask();
  }
   
  public void run(){runTask();}
    
  public void runTask(){
    BMArgs.Banner(request,BMName,CLASS,serial,num_threads);

    int niter=getInputPars();
    
    nsizes=new int[3];
    setup(nsizes);
    n1=nsizes[0];
    n2=nsizes[1];
    n3=nsizes[2];
     
    setTimers();
    timer.resetAllTimers();
    timer.start(T_init);

    zero3(u,0,n1,n2,n3);
    initU(v,n1,n2,n3,nx[lt-1],ny[lt-1]);

    if(!serial) setupThreads(this);
    if(serial) resid(u,v,r,0,n1,n2,n3);
    else residMaster(u,v,r,0,n1,n2,n3);
//--------------------------------------------------------------------
//    One iteration for startup
//--------------------------------------------------------------------
    if(serial){
      mg3P(u,v,r,n1,n2,n3);
      resid(u,v,r,0,n1,n2,n3);
    }else{
      mg3Pmaster(u,v,r,n1,n2,n3);
      residMaster(u,v,r,0,n1,n2,n3);	 
    }

    zero3(u,0,n1,n2,n3);
    initU(v,n1,n2,n3,nx[lt-1],ny[lt-1]);

    timer.stop(T_init);
    timer.start(T_bench);     
 
    if (timeron) timer.start(T_resid2);
    if(serial) resid(u,v,r,0,n1,n2,n3);
    else residMaster(u,v,r,0,n1,n2,n3);
    if (timeron) timer.stop(T_resid2);
    for(int it=1;it<=nit;it++){
       if (timeron) timer.start(T_mg3P);
       if(serial) mg3P(u,v,r,n1,n2,n3);
       else mg3Pmaster(u,v,r,n1,n2,n3);
       if (timeron) timer.stop(T_mg3P);
       
       if (timeron) timer.start(T_resid2);
       if(serial) resid(u,v,r,0,n1,n2,n3);
       else residMaster(u,v,r,0,n1,n2,n3);
       if (timeron) timer.stop(T_resid2);
    }
    timer.stop(T_bench);
    
    double tinit = timer.readTimer(T_init);
    System.out.println(" Initialization time: "+tinit+" seconds");
    rnm2=norm2u3(r,n1,n2,n3,rnmu,nx[lt-1],ny[lt-1],nz[lt-1]);
    System.out.println(" L2 norm of r="+rnm2);

    double tm = timer.readTimer(T_bench);
    results=new BMResults("MG",
  			  CLASS,
  			  nx[lt-1],
  			  ny[lt-1],
  			  nz[lt-1],
  			  nit,
  			  tm,
  			  getMFLOPS(tm,nit),
  			  "floating point",
  			  -1,
  			  serial,
  			  num_threads,
  			  bid);
    results.print();			      
    if (timeron) printTimers();
  }
    
  public SparseA getField(){
    int n1=nsizes[0],
        n2=nsizes[1],
        n3=nsizes[2];
    SparseA spal=new SparseA(1,n1,n2,n3);
    for(int i=0;i<spal.len;i++){
      spal.val[i]=u[i];
    }
    String bmcl=BMName+"."+CLASS;
    SparseA spa=new SparseA(bmcl);
    spa.BlockCopy(spal,-1,-1,-1);
    return spa;
  }
  public void initU(double z[],int n1,int n2,int n3,int nx,int ny){
    if(request!=null){
      if(request.spa!=null&&request.spa.len>0){
        if(
	   request.spa.len!=(n1-2)*(n2-2)*(n3-2) 
	   //Only points INSIDE of the grid are received
	  ){ 
          System.err.println("MG.initU: incorrect length"+
	                     " of sparse array="+request.spa.len);
          System.err.println("MG.initU: sizes=("+n1+","+n2+","+n3+")");
        }else{
          SparseA spa=new SparseA(1,n1,n2,n3);
	  spa.BlockCopy(request.spa,1,1,1);
          for(int i=0;i<spa.len;i++){
            z[i]=spa.val[i];
          } 
	  comm3(z,0,n1,n2,n3);
        }
      }
    }else zran3(z,n1,n2,n3,nx,ny);
  }
  public BMResults getResults(){
    if(request!=null){
      results.tmSent=request.tmSent;
      results.tmReceived=request.tmReceived;
    }
    return results;
  }
  public void finalize() throws Throwable{
    results=null;
    request.spa=null;
    request=null;
    t_names=null;
    nsizes=null;
    super.finalize();
  }
}

