/*
!-------------------------------------------------------------------------!
!                                                                         !
!                   N A S   G R I D   B E N C H M A R K S                 !  
!                                                                         !
!			 J A V A	 V E R S I O N  		  !
!                                                                         !
!                              L U T A S K                                !
!                                                                         !
!-------------------------------------------------------------------------!
!                                                                         !
!    LUTask encapsulates LU benchmark of NPB_JAV benchmark.               !
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
import NPB_JAV.LU.*;

import java.io.*;
import java.text.*;

public class LUTask extends NPB_JAV.LU{
  public int bid=-1;
  public BMResults results;
  public BMRequest request=null;
  boolean serial=false;
  
  public LUTask(char clss, int np, boolean ser){
    super(clss,np,ser);
  }
  public LUTask(BMRequest req){ 
    super(req.clss,req.numthreads,req.serial);
    request=req;
    serial=req.serial;
    bid=req.pid;
    if(request.dfg!=null&&request.dfg.name.indexOf("ED")<0){
      itmax_default /= req.getNGBdepth();
      if(itmax_default<=0) itmax_default=1;
      if(CLASS=='S') dt_default /= 2.0;
      if(request.dfg.name.indexOf("MB")>=0){
        int cnum=request.ndid%3;
	itmax_default*=(1.0-0.5*cnum/(cnum+1.0));
	if(itmax_default<0) itmax_default=1;
      }
    }
  }
  public static void main(String argv[]){
    LUTask lutask = null;

    BMArgs.ParseCmdLineArgs(argv,BMName);
    char CLSS=BMArgs.CLASS;
    int np=BMArgs.num_threads;
    boolean serial=BMArgs.serial;
    try{ 
      lutask = new LUTask(CLSS,np,serial);
    }catch(OutOfMemoryError e){
      BMArgs.outOfMemoryMessage();
      System.exit(0);
    }      
    lutask.runTask();
  }
  
  public void run(){runTask();}
  
  public void runTask(){
    BMArgs.Banner(request,BMName,CLASS,serial,num_threads);
    
    int numTimers=t_last+1;
    String t_names[] = new String[numTimers];
    double trecs[] = new double[numTimers];
    setTimers(t_names);

    getInputPars();

//---------------------------------------------------------------------
//   set up domain sizes
//---------------------------------------------------------------------
    domain();

//---------------------------------------------------------------------
//   set up coefficients
//---------------------------------------------------------------------
    setcoeff();
    if(!serial) setupThreads(this);
//---------------------------------------------------------------------
//   set the boundary values for dependent variables
//---------------------------------------------------------------------
    setbv();
  
//---------------------------------------------------------------------
//   set the initial values for dependent variables
//---------------------------------------------------------------------
    initU();

//---------------------------------------------------------------------
//   compute the forcing term based on prescribed exact solution
//---------------------------------------------------------------------
    erhs();

//---------------------------------------------------------------------
//   perform the SSOR iterations
//---------------------------------------------------------------------
    double tm;
    if(serial) tm=sssor();
    else tm=ssor();

//---------------------------------------------------------------------
//   compute the solution error
//---------------------------------------------------------------------
    error();

//---------------------------------------------------------------------
//   compute the surface integral
//---------------------------------------------------------------------
    pintgr();

//---------------------------------------------------------------------
//   verification test
//---------------------------------------------------------------------
    int verified = verify(rsdnm, errnm, frc);
    results=new BMResults(BMName,
    			  CLASS,
    			  nx0,
    			  ny0,
    			  nz0,
    			  itmax,
    			  tm,
    			  getMFLOPS(itmax,tm),
    			  "floating point",
    			  verified,
    			  serial,
    			  num_threads,
    			  bid);
    results.print();				
//---------------------------------------------------------------------
//      More timers
//---------------------------------------------------------------------
    if(timeron) printTimers(t_names,trecs,tm);
  }
  
  public void setivNGB(){
    int i, j, k, m;
    double  xi, eta, zeta;
    int nxm1=nx-1,
    nym1=ny-1,
    nzm1=nz-1;

    for(k=1;k<=nz-2;k++){
       zeta = (double) k / (nz-1);
       for(j=1;j<=ny-2;j++){
         eta = (double) j / (ny0-1);
    	  for(i=1;i<=nx-2;i++){
            xi = (double) i / (nx0-1);
    	     for(m=0;m<=4;m++){
          u[m+i*isize1+j*jsize1+k*ksize1] =
             xi*(eta	*(zeta   *u[m+nxm1*isize1+nym1*jsize1+nzm1*ksize1]+
        		 (1-zeta)*u[m+nxm1*isize1+nym1*jsize1])+
        	 (1-eta)*(zeta   *u[m+nxm1*isize1+nzm1*ksize1]+
        		 (1-zeta)*u[m+nxm1*isize1])
        	)+
         (1-xi)*(eta	*(zeta   *u[m+nym1*jsize1+nzm1*ksize1]+
        		 (1-zeta)*u[m+nym1*jsize1])+
        	 (1-eta)*(zeta   *u[m+nzm1*ksize1]+
        		 (1-zeta)*u[m])
    		);	  
             }
    	  }
       }
    }
  }
 
  public int verify(double xcr[], double xce[], double xci){
    double xcrref[] = new double[5], xceref[] = new double[5],xciref=0, 
           xcrdif[] = new double[5], xcedif[] = new double[5],xcidif=0,
           dtref=0;
    int m;
    int verified=-1;
    char clss = 'U';

    for(m=0;m<=4;m++){
      xcrref[m] = 1.0;
      xceref[m] = 1.0;
    }
    xciref = 1.0;
System.err.println("LU verify: request.ndid="+request.ndid);
System.err.println("LU verify: request.dfg.name="+request.dfg.name);
      if (request!=null&&request.dfg!=null&&request.dfg.name.indexOf("HC")>=0){
        if (request.dfg.name.indexOf("HC.S")>=0&&request.ndid==8) {
          clss = 'S';
          dtref = 0.25;
          xcrref[0] = 0.5218111814670799E-03;
          xcrref[1] = 0.3707865163709830E-03;
          xcrref[2] = 0.3800195018967310E-03;
          xcrref[3] = 0.3397401925927176E-03;
          xcrref[4] = 0.2759004448851586E-03;

          xceref[0] = 0.2089313120425803E-04;
          xceref[1] = 0.1476501931249467E-04;
          xceref[2] = 0.1525298768678017E-04;
          xceref[3] = 0.1366601055353010E-04;
          xceref[4] = 0.1177099812324952E-04;

	  xciref    = 0.7840627336810835E+01;
        }else if (request.dfg.name.indexOf("HC.W")>=0&&request.ndid==8) {
          clss = 'W';
          dtref = 0.0015;
          xcrref[0] = 0.2044902312107378E-01;
          xcrref[1] = 0.1588666348974413E-01;
          xcrref[2] = 0.1504465470195994E-01;
          xcrref[3] = 0.1309985413018435E-01;
          xcrref[4] = 0.8059897367087477E-02;

          xceref[0] = 0.1321514192265648E-01;
          xceref[1] = 0.9789942432742071E-02;
          xceref[2] = 0.8730718331532538E-02;
          xceref[3] = 0.7382890378185640E-02;
          xceref[4] = 0.2473115882853853E-02;

	  xciref    = 0.1116573970136815E+02;
        }else if (request.dfg.name.indexOf("HC.A")>=0&&request.ndid==8){
          clss = 'A';
          dtref = 2.0;
          xcrref[0] = 0.4568545705333646E-01;
          xcrref[1] = 0.3363154291261598E-01;
          xcrref[2] = 0.3229649387852138E-01;
          xcrref[3] = 0.2835838318897043E-01;
          xcrref[4] = 0.2033297513577555E-01;

          xceref[0] = 0.1826624824076483E-02;
          xceref[1] = 0.1336130626708917E-02;
          xceref[2] = 0.1297209081421345E-02;
          xceref[3] = 0.1142645851712347E-02;
          xceref[4] = 0.8878915256529250E-03;

          xciref    = 0.1208035142313082E+02;
        }else if (request.dfg.name.indexOf("HC.B")>=0&&request.ndid==17){
          clss = 'B';
          dtref = 2.0;
          xcrref[0] = 0.7585910699148E+00;
          xcrref[1] = 0.5696105127190E+00;
          xcrref[2] = 0.5268880230462E+00;
          xcrref[3] = 0.4574775990656E+00;
          xcrref[4] = 0.2476122970461E+00;

          xceref[0] = 0.3039692982264E-01;
          xceref[1] = 0.2267152193556E-01;
          xceref[2] = 0.2124188635570E-01;
          xceref[3] = 0.1851684149150E-01;
          xceref[4] = 0.1138970412935E-01;

          xciref    = 0.1243337273105E+02;
        }
    }else if ( (nx0 == 12) && (ny0 == 12) &&(nz0 == 12) && (itmax == 50)){
      clss = 'S';
      dtref = .5;
//---------------------------------------------------------------------
//   Reference values of RMS-norms of residual, for the (12X12X12) grid,
//   after 50 time steps, with  DT = 5.0d-01
//---------------------------------------------------------------------
      xcrref[0] = 1.6196343210976702E-2;
      xcrref[1] = 2.1976745164821318E-3;
      xcrref[2] = 1.5179927653399185E-3;
      xcrref[3] = 1.5029584435994323E-3;
      xcrref[4] = 3.4264073155896461E-2;

//---------------------------------------------------------------------
//   Reference values of RMS-norms of solution error, for the (12X12X12) grid,
//   after 50 time steps, with  DT = 5.0d-01
//---------------------------------------------------------------------
      xceref[0] = 6.4223319957960924E-4;
      xceref[1] = 8.4144342047347926E-5;
      xceref[2] = 5.8588269616485186E-5;
      xceref[3] = 5.8474222595157350E-5;
      xceref[4] = 1.3103347914111294E-3;

//---------------------------------------------------------------------
//   Reference value of surface integral, for the (12X12X12) grid,
//   after 50 time steps, with DT = 5.0d-01
//---------------------------------------------------------------------
      xciref = 7.8418928865937083;
    }else if ( (nx0 == 33) && (ny0 == 33) &&(nz0 == 33) &&(itmax == 300) ) {

      clss = 'W';
      dtref = 1.5E-3;
//---------------------------------------------------------------------
//   Reference values of RMS-norms of residual, for the (33x33x33) grid,
//   after 300 time steps, with  DT = 1.5d-3
//---------------------------------------------------------------------
      xcrref[0] =   0.1236511638192E+02;
      xcrref[1] =   0.1317228477799E+01;
      xcrref[2] =   0.2550120713095E+01;
      xcrref[3] =   0.2326187750252E+01;
      xcrref[4] =   0.2826799444189E+02;

//---------------------------------------------------------------------
//   Reference values of RMS-norms of solution error, for the (33X33X33) grid,
//---------------------------------------------------------------------
      xceref[0] =   0.4867877144216;
      xceref[1] =   0.5064652880982E-1;
      xceref[2] =   0.9281818101960E-1;
      xceref[3] =   0.8570126542733E-1;
      xceref[4] =   0.1084277417792E+01;

//---------------------------------------------------------------------
//   Reference value of surface integral, for the (33X33X33) grid,
//   after 300 time steps, with  DT = 1.5d-3
//---------------------------------------------------------------------
      xciref    =   0.1161399311023E+02;
    }else if ( (nx0 == 64) && (ny0 == 64) &&(nz0 == 64) &&(itmax == 250) ) {
      clss = 'A';
      dtref = 2.0;
//---------------------------------------------------------------------
//   Reference values of RMS-norms of residual, for the (64X64X64) grid,
//   after 250 time steps, with  DT = 2.0
//---------------------------------------------------------------------
      xcrref[0] = 7.7902107606689367E+02;
      xcrref[1] = 6.3402765259692870E+01;
      xcrref[2] = 1.9499249727292479E+02;
      xcrref[3] = 1.7845301160418537E+02;
      xcrref[4] = 1.8384760349464247E+03;

//---------------------------------------------------------------------
//   Reference values of RMS-norms of solution error, for the (64X64X64) grid,
//   after 250 time steps, with  DT = 2.0
//---------------------------------------------------------------------;
      xceref[0] = 2.9964085685471943E+01;
      xceref[1] = 2.8194576365003349;
      xceref[2] = 7.3473412698774742;
      xceref[3] = 6.7139225687777051;
      xceref[4] = 7.0715315688392578E+01;

//---------------------------------------------------------------------
//   Reference value of surface integral, for the (64X64X64) grid,
//   after 250 time steps, with DT = 2.0
//---------------------------------------------------------------------
      xciref = 2.6030925604886277E+01;
    }else if ( (nx0 == 102) && (ny0 == 102) && (nz0 == 102) && (itmax == 250) ) {
      clss = 'B';
      dtref = 2.0;

//---------------------------------------------------------------------
//   Reference values of RMS-norms of residual, for the (102X102X102) grid,
//   after 250 time steps, with  DT = 2.0
//---------------------------------------------------------------------
      xcrref[0] = 3.5532672969982736E+03;
      xcrref[1] = 2.6214750795310692E+02;
      xcrref[2] = 8.8333721850952190E+02;
      xcrref[3] = 7.7812774739425265E+02;
      xcrref[4] = 7.3087969592545314E+03;

//---------------------------------------------------------------------
//   Reference values of RMS-norms of solution error, for the (102X102X102) 
//   grid, after 250 time steps, with  DT = 2.0
//---------------------------------------------------------------------
      xceref[0] = 1.1401176380212709E+02;
      xceref[1] = 8.1098963655421574;
      xceref[2] = 2.8480597317698308E+01;
      xceref[3] = 2.5905394567832939E+01;
      xceref[4] = 2.6054907504857413E+02;

//---------------------------------------------------------------------
//   Reference value of surface integral, for the (102X102X102) grid,
//   after 250 time steps, with DT = 2.0
//---------------------------------------------------------------------
      xciref = 4.7887162703308227E+01;
    }else if ( (nx0 == 162) && (ny0 == 162) && (nz0 == 162) && (itmax == 250) ) {

      clss = 'C';
      dtref = 2.0;

//---------------------------------------------------------------------
//   Reference values of RMS-norms of residual, for the (162X162X162) grid,
//   after 250 time steps, with  DT = 2.0
//---------------------------------------------------------------------
      xcrref[0] = 1.03766980323537846E+04;
      xcrref[1] = 8.92212458801008552E+02;
      xcrref[2] = 2.56238814582660871E+03;
      xcrref[3] = 2.19194343857831427E+03;
      xcrref[4] = 1.78078057261061185E+04;

//---------------------------------------------------------------------
//   Reference values of RMS-norms of solution error, for the (162X162X162) 
//   grid, after 250 time steps, with  DT = 2.0
//---------------------------------------------------------------------
      xceref[0] = 2.15986399716949279E+02;
      xceref[1] = 1.55789559239863600E+01;
      xceref[2] = 5.41318863077207766E+01;
      xceref[3] = 4.82262643154045421E+01;
      xceref[4] = 4.55902910043250358E+02;

//---------------------------------------------------------------------
//   Reference value of surface integral, for the (162X162X162) grid,
//   after 250 time steps, with DT = 2.0
//---------------------------------------------------------------------
      xciref = 6.66404553572181300E+01;      
    }

//---------------------------------------------------------------------
//    verification test for residuals if gridsize is either 12X12X12 or 
//    64X64X64 or 102X102X102 or 162X162X162
//---------------------------------------------------------------------

//---------------------------------------------------------------------
//    Compute the difference of solution values and the known reference values.
//---------------------------------------------------------------------
    for(m=0;m<=4;m++){
      xcrdif[m] = Math.abs((xcr[m]-xcrref[m])/xcrref[m]) ;
      xcedif[m] = Math.abs((xce[m]-xceref[m])/xceref[m]);       
    }
    xcidif = Math.abs((xci - xciref)/xciref);

//---------------------------------------------------------------------
//   tolerance level
//---------------------------------------------------------------------
    double epsilon = 1.0E-8;
//---------------------------------------------------------------------
//    Output the comparison of computed results to known cases.
//---------------------------------------------------------------------

    if(clss != 'U') {
      System.out.println(" Verification being performed for class " + clss);
      System.out.println(" Accuracy setting for epsilon = " + epsilon);
      if (Math.abs(dt-dtref) <= epsilon) {  
	if(verified==-1) verified = 1;
      }else{
	verified = 0;
	clss = 'U';
	System.out.println(" DT= "+dt+
	                   " does not match the reference value of "+ dtref);
      }
    }else{ 
//      System.out.println(" Unknown class");
      verified = -1;
    }
    if (clss != 'U') {
      System.out.println(" Comparison of RMS-norms of residual");
    }else{
      System.out.println(" RMS-norms of residual");
      verified = -1;
    }
    verified=BMResults.printComparisonStatus(clss,verified,epsilon,
                                             xcr,xcrref,xcrdif);

    if (clss != 'U') {
      System.out.println(" Comparison of RMS-norms of solution error");
    }else{
      System.out.println(" RMS-norms of solution error");
    }
    verified=BMResults.printComparisonStatus(clss,verified,epsilon,
                                             xce,xceref,xcedif);

    if (clss != 'U') {
      System.out.println(" Comparison of surface integral");
    }else{
      System.out.println(" Surface integral");
    }
    verified=BMResults.printComparisonStatus(clss,verified,epsilon,
                                             xci,xciref,xcidif);
    BMResults.printVerificationStatus(clss,verified,BMName); 
    return verified;
  }
  public SparseA getField(){
    String bmcl=BMName+"."+CLASS;
    SparseA spa=new SparseA(bmcl);
    int spaidx=0;
    for(int k=0;k<=nz-1;k++){
      for(int j=0;j<=ny-1;j++){
	for(int i=0;i<=nx-1;i++){
	  for(int m=0;m<=4;m++){
	    int offset=m+i*isize1+j*jsize1+k*ksize1;
	    spa.val[spaidx]=u[offset];
	    spaidx++;
	  }
	}
      }
    }
    return spa;
  }
  public SparseA getMachNum(){
    SparseA spa=new SparseA(1,nx,ny,nz);
    for(int k=0;k<nz;k++){
      for(int j=0;j<ny;j++){
        for(int i=0;i<nx;i++){
	  int offset=i*isize1+j*jsize1+k*ksize1;
	  int off=i+nx*(j+ny*k);
          double rho_inv = 1.0/u[offset];
          spa.val[off]=Math.sqrt((u[4+offset] 
	      -0.5*(u[1+offset]*u[1+offset]
	      +u[2+offset]*u[2+offset]+u[3+offset]*u[3+offset])
	      *rho_inv)*rho_inv*0.56);
	}
      }
    }
    return spa;
  }
  public void initU(){
    if(request!=null){
      setivNGB();
      if(request.spa!=null&&request.spa.len>0){
        if(request.spa.len!=5*nx*ny*nz){
          System.err.println("!!LU.initU: incorrect length of sparse array="
	                     +request.spa.len);
        }else{
    	  int spaidx=0;
    	  for(int k=0;k<=nz-1;k++){
    	    for(int j=0;j<=ny-1;j++){
    	      for(int i=0;i<=nx-1;i++){
    		for(int m=0;m<=4;m++){
    		  int offset=m+i*isize1+j*jsize1+k*ksize1;
    		  u[offset]=request.spa.val[spaidx];
    		  spaidx++;
    		}
    	      }
    	    }
    	  }
        }
      }
    }else setiv();
    
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
    super.finalize();
  }
}
