/*
!-------------------------------------------------------------------------!
!                                                                         !
!               N A S   G R I D   B E N C H M A R K S                     !  
!                                                                         !
!			 J A V A	 V E R S I O N  		  !
!                                                                         !
!                              I S T A S K                                !
!                                                                         !
!-------------------------------------------------------------------------!
!                                                                         !
!    ISTask encapsulates IS benchmark of NPB_JAV benchmark.               !
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

public class ISTask extends NPB_JAV.IS{
  public int bid=-1;
  public BMResults results;
  public BMRequest request=null;

  protected static final double amult = 1220703125.0;

  public ISTask(char clss, int np, boolean ser){
    super(clss,np,ser);
  }
    
  public static void main(String argv[] ){
    ISTask istask = null;

    BMArgs.ParseCmdLineArgs(argv,BMName);
    char CLSS=BMArgs.CLASS;
    int np=BMArgs.num_threads;
    boolean serial=BMArgs.serial;

    try{ 
      istask = new ISTask(CLSS,np,serial);
    }catch(OutOfMemoryError e){
      BMArgs.outOfMemoryMessage();
      System.exit(0);
    }	 
    istask.runBenchMark();
  }
  
  public void run(){runBenchMark();}

  public BMResults getResults(){
    if(request!=null){
      results.tmSent=request.tmSent;
      results.tmReceived=request.tmReceived;
    }
    return results;
  }
        
  public SparseA getField(){
    String bmcl="IS."+CLASS;
    SparseA spa=new SparseA(bmcl);
    int i;
    for(i=0;i<spa.len;i++){
      spa.val[i]= 1/((double) (i+1));
      spa.idx[i]=i;
    }
    System.out.println(bmcl+": CheckVal="+spa.CheckVal()+
                            " CheckIdx="+spa.CheckIdx());
    return spa;
  }
  public double getTime(){return timer.readTimer(0);}
  
  public void finalize() throws Throwable{
    results=null;
    request.spa=null;
    request=null;
    super.finalize();
  }
}

