/*
!-------------------------------------------------------------------------!
!                                                                         !
!                   N A S   G R I D   B E N C H M A R K S                 !  
!                                                                         !
!			 J A V A	 V E R S I O N  		  !
!                                                                         !
!                              B T T A S K                                !
!                                                                         !
!-------------------------------------------------------------------------!
!                                                                         !
!    BTTask encapsulates BT benchmark of NPB_JAV benchmark.               !
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
import NPB_JAV.BTThreads.*;
import NPB_JAV.BT.*;

import java.io.*;
import java.text.*;

public class BTTask extends NPB_JAV.BT{
  public int bid=-1;
  public BMResults results;
  public BMRequest request=null;
  
  public BTTask(char clss, int threads,boolean ser){            
    super(clss, threads, ser);
    serial=ser;
  }
  public BTTask(BMRequest req){ 
    super(req.clss,req.numthreads,req.serial);
    request=req;
    serial=request.serial;
    bid=req.pid;
    if(request.dfg!=null&&request.dfg.name.indexOf("ED")<0){
      niter_default /= req.getNGBdepth();
      if(niter_default<=0) niter_default=1;
      if(CLASS=='S') dt_default /= 2.0;
      if(request.dfg.name.indexOf("MB")>=0){
        int cnum=request.ndid%3;
	niter_default*=(1.0-0.5*cnum/(cnum+1.0));
	if(niter_default<0) niter_default=1;
      }
    }
  }
  public static void main(String argv[]){
    BTTask bttask = null;

    BMArgs.ParseCmdLineArgs(argv,BMName);
    char CLSS=BMArgs.CLASS;
    int np=BMArgs.num_threads;
    boolean serial=BMArgs.serial;
    try{ 
      bttask = new BTTask(CLSS,np,serial);
    }catch(OutOfMemoryError e){
      BMArgs.outOfMemoryMessage();
      System.exit(0);
    }         
    bttask.runTask();
  }
  
  public void run(){runTask();}

  public void runTask(){
    BMArgs.Banner(request,BMName,CLASS,serial,num_threads);

    int numTimers=t_last+1;
    String t_names[] = new String[numTimers];
    double trecs[] = new double[numTimers];
    setTimers(t_names);
    int niter=getInputPars();

    set_constants();
    initU();
    exact_rhs();
    
    if(!serial) setupThreads(this);    
//---------------------------------------------------------------------
//      do one time step to touch all code, and reinitialize
//---------------------------------------------------------------------
    if(serial) adi_serial();
    else adi(); 
    initU();
    
    timer.resetAllTimers();
    timer.start(t_total);

    for(int step=1;step<=niter;step++){   //niter
      if ( step % 20 == 0 || step == 1||step==niter) {   
        System.out.println("Time step "+step);
      }
      if(serial) adi_serial();
      else adi(); 
    }

    timer.stop(t_total);
    int verified = verify(niter); 

    double time = timer.readTimer(t_total);  
    results=new BMResults(BMName,
    			  CLASS,
    			  grid_points[0],
    			  grid_points[1],
    			  grid_points[2],
    			  niter,
    			  time,
    			  getMFLOPS(time,niter),
    			  "floating point",
    			  verified,
    			  serial,
    			  num_threads,
    			  bid);
    results.print();				
    if(timeron) printTimers(t_names,trecs,time);
  }

  public int verify(int no_time_steps){
    double xcrref[]=new double[5],xceref[]=new double[5],
           xcrdif[]=new double[5],xcedif[]=new double[5], 
           xce[]=new double[5], xcr[]=new double[5], 
	   dtref=0;
    int m;
    int verified=-1;
    char clss = 'U';
//---------------------------------------------------------------------
//   compute the error norm and the residual norm, and exit if not printing
//---------------------------------------------------------------------
    error_norm(xce,0);
    compute_rhs();
    rhs_norm(xcr,0);

    for(m=0;m<xcr.length;m++) xcr[m] = xcr[m] / dt;

    for(m=1;m<xcrref.length;m++){
      xcrref[m] = 1.0;
      xceref[m] = 1.0;
    }

    if(request!=null){
      if(request.dfg!=null&&request.dfg.name.indexOf("ED")<0){
        if(request.dfg.name.indexOf("ED.S")>=0){
          dtref = .001;
	} 
      }
//---------------------------------------------------------------------
//    reference data for 12X12X12 grids after 100 time steps, with DT = 1.0d-02
//---------------------------------------------------------------------
     }else if (  grid_points[0] == 12
	       &&grid_points[1] == 12
	       &&grid_points[2] == 12
	       &&no_time_steps  == 60    
     ){

       clss = 'S';
       dtref = .01;

//---------------------------------------------------------------------
//  Reference values of RMS-norms of residual.
//---------------------------------------------------------------------
       xcrref[0] = 1.7034283709541311E-01;
       xcrref[1] = 1.2975252070034097E-02;
       xcrref[2] = 3.2527926989486055E-02;
       xcrref[3] = 2.6436421275166801E-02;
       xcrref[4] = 1.9211784131744430E-01;

//---------------------------------------------------------------------
//  Reference values of RMS-norms of solution error.
//---------------------------------------------------------------------
       xceref[0] = 4.9976913345811579E-04;
       xceref[1] = 4.5195666782961927E-05;
       xceref[2] = 7.3973765172921357E-05;
       xceref[3] = 7.3821238632439731E-05;
       xceref[4] = 8.9269630987491446E-04;

//---------------------------------------------------------------------
//    reference data for 24X24X24 grids after 200 time steps, with DT = 0.8d-3
//---------------------------------------------------------------------
     }else if( (grid_points[0] == 24) &&
	       (grid_points[1] == 24) &&
	       (grid_points[2] == 24) &&
	       (no_time_steps  == 200) ) {
       
       clss = 'W';
       dtref = 0.0008;
//---------------------------------------------------------------------
//  Reference values of RMS-norms of residual.
//---------------------------------------------------------------------
       xcrref[0] = 0.1125590409344E+03;
       xcrref[1] = 0.1180007595731E+02;
       xcrref[2] = 0.2710329767846E+02;
       xcrref[3] = 0.2469174937669E+02;
       xcrref[4] = 0.2638427874317E+03;

//---------------------------------------------------------------------
//  Reference values of RMS-norms of solution error.
//---------------------------------------------------------------------
       xceref[0] = 0.4419655736008E+01;
       xceref[1] = 0.4638531260002;
       xceref[2] = 0.1011551749967E+01;
       xceref[3] = 0.9235878729944;
       xceref[4] = 0.1018045837718E+02;


//---------------------------------------------------------------------
//    reference data for 64X64X64 grids after 200 time steps, with DT = 0.8d-3
//---------------------------------------------------------------------
     }else if( (grid_points[0] == 64) &&
	       (grid_points[1] == 64) &&
	       (grid_points[2] == 64) &&
	       (no_time_steps == 200) ) {

       clss = 'A';
       dtref = 0.0008;
//---------------------------------------------------------------------
//  Reference values of RMS-norms of residual.
//---------------------------------------------------------------------
       xcrref[0] = 1.0806346714637264E+02;
       xcrref[1] = 1.1319730901220813E+01;
       xcrref[2] = 2.5974354511582465E+01;
       xcrref[3] = 2.3665622544678910E+01;
       xcrref[4] = 2.5278963211748344E+02;

//---------------------------------------------------------------------
//  Reference values of RMS-norms of solution error.
//---------------------------------------------------------------------
       xceref[0] = 4.2348416040525025;
       xceref[1] = 4.4390282496995698E-01;
       xceref[2] = 9.6692480136345650E-01;
       xceref[3] = 8.8302063039765474E-01;
       xceref[4] = 9.7379901770829278;

//---------------------------------------------------------------------
//    reference data for 102X102X102 grids after 200 time steps,
//    with DT = 3.0d-04
//---------------------------------------------------------------------
     }else if ( (grid_points[0] == 102) &&
		(grid_points[1] == 102) &&
		(grid_points[2] == 102) &&
		(no_time_steps == 200) ) {
       
       clss = 'B';
       dtref = .0003;

//---------------------------------------------------------------------
//  Reference values of RMS-norms of residual.
//---------------------------------------------------------------------
       xcrref[0] = 1.4233597229287254E+03;
       xcrref[1] = 9.9330522590150238E+01;
       xcrref[2] = 3.5646025644535285E+02;
       xcrref[3] = 3.2485447959084092E+02;
       xcrref[4] = 3.2707541254659363E+03;

//---------------------------------------------------------------------
//  Reference values of RMS-norms of solution error.
//---------------------------------------------------------------------
       xceref[0] = 5.2969847140936856E+01;
       xceref[1] = 4.4632896115670668;
       xceref[2] = 1.3122573342210174E+01;
       xceref[3] = 1.2006925323559144E+01;
       xceref[4] = 1.2459576151035986E+02;

//---------------------------------------------------------------------
//    reference data for 162X162X162 grids after 200 time steps,
//    with DT = .0001
//---------------------------------------------------------------------
     }else if( (grid_points[0] == 162) &&
	       (grid_points[1] == 162) &&
	       (grid_points[2] == 162) &&
	       (no_time_steps == 200) ) {
       
       clss = 'C';
       dtref = .0001;

//---------------------------------------------------------------------
//  Reference values of RMS-norms of residual.
//---------------------------------------------------------------------
       xcrref[0] = 0.62398116551764615E+04;
       xcrref[1] = 0.50793239190423964E+03;
       xcrref[2] = 0.15423530093013596E+04;
       xcrref[3] = 0.13302387929291190E+04;
       xcrref[4] = 0.11604087428436455E+05;

//---------------------------------------------------------------------
//  Reference values of RMS-norms of solution error.
//---------------------------------------------------------------------
       xceref[0] = 0.16462008369091265E+03;
       xceref[1] = 0.11497107903824313E+02;
       xceref[2] = 0.41207446207461508E+02;
       xceref[3] = 0.37087651059694167E+02;
       xceref[4] = 0.36211053051841265E+03;
     }
//---------------------------------------------------------------------
//    Compute the difference of solution values and the known reference values.
//---------------------------------------------------------------------
     for(m=0;m<xcr.length;m++){       
	 xcrdif[m] = Math.abs((xcr[m]-xcrref[m])/xcrref[m]);
	 xcedif[m] = Math.abs((xce[m]-xceref[m])/xceref[m]);      
     }
//---------------------------------------------------------------------
//   tolerance level
//---------------------------------------------------------------------
     double epsilon = 1.0*Math.pow(.1,8);
//---------------------------------------------------------------------
//    Output the comparison of computed results to known cases.
//---------------------------------------------------------------------
     if (clss != 'U') {
       System.out.println("Verification being performed for class "+clss);
       System.out.println("accuracy setting for epsilon = "+epsilon);
       if (Math.abs(dt-dtref) <= epsilon ) {
         verified = 1;
       }else{
	 verified = 0;
	 clss = 'U';
	 System.out.println("DT does not match the reference value of "+dtref);	 
       }
     }else{
//       System.out.println("Unknown class");  
     }
     
     if (clss != 'U') System.out.println("Comparison of RMS-norms of residual");
     else System.out.println("RMS-norms of residual"); 
     verified=BMResults.printComparisonStatus(clss,verified,epsilon,
                                              xcr,xcrref,xcrdif);

     if (clss != 'U') {
       System.out.println("Comparison of RMS-norms of solution error");
     }else{
       System.out.println("RMS-norms of solution error");
     }
     verified=BMResults.printComparisonStatus(clss,verified,epsilon,
                                              xce,xceref,xcedif);

     BMResults.printVerificationStatus(clss,verified,BMName); 
     return verified;
  }
 
  public SparseA getField(){
    String bmcl=BMName+"."+CLASS;
    SparseA spa=new SparseA(bmcl);
    int spaidx=0;
    for(int k=0;k<=grid_points[2]-1;k++){
      for(int j=0;j<=grid_points[1]-1;j++){
	for(int i=0;i<=grid_points[0]-1;i++){
	  for(int m=0;m<=4;m++){
	    int offset=m+i*isize2+j*jsize2+k*ksize2;
	    spa.val[spaidx]=u[offset];
	    spaidx++;
	  }
	}
      }
    }
    return spa;
  }
  public SparseA getMachNum(){
    SparseA spa=new SparseA(1,grid_points[0],grid_points[1],grid_points[2]);
    int idx=0;
    for(int k=0;k<=grid_points[2]-1;k++){
      for(int j=0;j<=grid_points[1]-1;j++){
        for(int i=0;i<=grid_points[0]-1;i++){
	  int offset=i*isize2+j*jsize2+k*ksize2;
          double rho_inv = 1.0/u[offset];
          spa.val[idx]=Math.sqrt(
	                    (u[4+offset] 
	                    -0.5*( u[1+offset]*u[1+offset]
	                          +u[2+offset]*u[2+offset]
			          +u[3+offset]*u[3+offset])
	                        *rho_inv
			    )
	                    *rho_inv*0.56);
          spa.idx[idx]=-1;
	  idx++;
	}
      }
    }
    return spa;
  }
  public void initU(){
    if(request!=null){
      initializeNGB();
      if(request.spa!=null&&request.spa.len>0){
        if(request.spa.len!=5*grid_points[0]*grid_points[1]*grid_points[2]){
          System.err.println("!!BT.initU: incorrect length of sparse array="+
	                      request.spa.len);
        }else{
    	  int spaidx=0;
    	  for(int k=0;k<=grid_points[2]-1;k++){
    	    for(int j=0;j<=grid_points[1]-1;j++){
    	      for(int i=0;i<=grid_points[0]-1;i++){
    		for(int m=0;m<=4;m++){
    		  int offset=m+i*isize2+j*jsize2+k*ksize2;
    		  u[offset]=request.spa.val[spaidx];
    	          spaidx++;
         	}
    	      }
    	    }
    	  }
        }
       }
    }else initialize();
  }
  public BMResults getResults(){
    if(request!=null){
      results.tmSent=request.tmSent;
      results.tmReceived=request.tmReceived;
    }
    return results;
  }
  public void initializeNGB(){
    int i, j, k, m, ix, iy, iz;
    double  xi, eta, zeta, Pface[]=new double[5*3*2]; 
    double Pxi, Peta, Pzeta, temp[]=new double[5];
//---------------------------------------------------------------------
//  Later (in compute_rhs) we compute 1/u for every element. A few of 
//  the corner elements are not used, but it convenient (and faster) 
//  to compute the whole thing with a simple loop. Make sure those 
//  values are nonzero by initializing the whole thing here. 
//---------------------------------------------------------------------
    for(k=0;k<=grid_points[2]-1;k++){
      for(j=0;j<=grid_points[1]-1;j++){
	for(i=0;i<=grid_points[0]-1;i++){
	  for(m=0;m<=4;m++){
	    u[m+i*isize2+j*jsize2+k*ksize2] = 1.0;
	  }
	}
      }
    }
  
    int nxm1=grid_points[0]-1,
        nym1=grid_points[1]-1,
        nzm1=grid_points[2]-1;
    
//---------------------------------------------------------------------
//    first store the exact values on the boundaries        
//---------------------------------------------------------------------

//---------------------------------------------------------------------
//     west face                                                  
//---------------------------------------------------------------------
    i = 0;
    xi = 0.0;
    for(k=0;k<=grid_points[2]-1;k++){
      zeta = k * dnzm1;
      for(j=0;j<=grid_points[1]-1;j++){
	eta = j * dnym1;
	exact_solution(xi, eta, zeta, temp,0);
	for(m=0;m<=4;m++){
	  u[m+i*isize2+j*jsize2+k*ksize2] = temp[m];
	}
      }
    }
  
//---------------------------------------------------------------------
//     east face                                                      
//---------------------------------------------------------------------

    i = grid_points[0]-1;
    xi = 1.0;
    for(k=0;k<=grid_points[2]-1;k++){
      zeta = k * dnzm1;
      for(j=0;j<=grid_points[1]-1;j++){
	eta = j * dnym1;
	exact_solution(xi, eta, zeta, temp,0);
	for(m=0;m<=4;m++){
	  u[m+i*isize2+j*jsize2+k*ksize2] = temp[m];
	}
      }
    }

//---------------------------------------------------------------------
//     south face                                                 
//---------------------------------------------------------------------
    j = 0;
    eta = 0.0;
    for(k=0;k<=grid_points[2]-1;k++){
      zeta = k * dnzm1;
      for(i=0;i<=grid_points[0]-1;i++){
	xi = i * dnxm1;
	exact_solution(xi, eta, zeta, temp,0);
	for(m=0;m<=4;m++){
	  u[m+i*isize2+j*jsize2+k*ksize2] = temp[m];
	}
      }
    }

//---------------------------------------------------------------------
//     north face                                    
//---------------------------------------------------------------------
    j = grid_points[1]-1;
    eta = 1.0;
    for(k=0;k<=grid_points[2]-1;k++){
      zeta = k * dnzm1;
      for(i=0;i<=grid_points[0]-1;i++){
	xi = i * dnxm1;
	exact_solution(xi, eta, zeta, temp,0);
	for(m=0;m<=4;m++){
	  u[m+i*isize2+j*jsize2+k*ksize2] = temp[m];
	}
      }
    }
    
//---------------------------------------------------------------------
//     bottom face                                       
//---------------------------------------------------------------------
    k = 0;
    zeta = 0.0;
    for(i=0;i<=grid_points[0]-1;i++){
      xi = i *dnxm1;
      for(j=0;j<=grid_points[1]-1;j++){
	eta = j * dnym1;
	exact_solution(xi, eta, zeta, temp,0);
	for(m=0;m<=4;m++){
	  u[m+i*isize2+j*jsize2+k*ksize2] = temp[m];
	}
      }
    }
    
//---------------------------------------------------------------------
//     top face     
//---------------------------------------------------------------------
    k = grid_points[2]-1;
    zeta = 1.0;
    for(i=0;i<=grid_points[0]-1;i++){
      xi = i * dnxm1;
      for(j=0;j<=grid_points[1]-1;j++){
	eta = j * dnym1;
	exact_solution(xi, eta, zeta, temp,0);
	for(m=0;m<=4;m++){
	  u[m+i*isize2+j*jsize2+k*ksize2] = temp[m];
	}
      }
    }
//---------------------------------------------------------------------
//     next store the "interpolated" values everywhere on the grid    
//---------------------------------------------------------------------
    for(k=1;k<grid_points[2]-1;k++){
      zeta = k * dnzm1;
      for(j=1;j<grid_points[1]-1;j++){
	eta = j * dnym1;
	for(i=1;i<grid_points[0]-1;i++){
	  xi = i * dnxm1;
 	  for(m=0;m<=4;m++){
	    u[m+i*isize2+j*jsize2+k*ksize2] =
	       xi*(eta    *(zeta   *u[m+nxm1*isize2+nym1*jsize2+nzm1*ksize2]+
	                   (1-zeta)*u[m+nxm1*isize2+nym1*jsize2])+
		   (1-eta)*(zeta   *u[m+nxm1*isize2+nzm1*ksize2]+
	                   (1-zeta)*u[m+nxm1*isize2])
	          )+
	   (1-xi)*(eta    *(zeta   *u[m+nym1*jsize2+nzm1*ksize2]+
	                   (1-zeta)*u[m+nym1*jsize2])+
		   (1-eta)*(zeta   *u[m+nzm1*ksize2]+
	                   (1-zeta)*u[m])
                  );	    
	  }
	}
      } 
    }
  }
  public void finalize() throws Throwable{
    results=null;
    request.spa=null;
    request=null;
    super.finalize();
  }
}




