/*
!-------------------------------------------------------------------------!
!                                                                         !
!                   N A S   G R I D   B E N C H M A R K S                 !  
!                                                                         !
!			 J A V A	 V E R S I O N  		  !
!                                                                         !
!                            B M R E Q U E S T                            !
!                                                                         !
!-------------------------------------------------------------------------!
!                                                                         !
!    BMRequest implements request class used to request an execution of   !
!    a service by a BenchmarkServer.                                      !
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
package tasks.DGraph;

import java.io.*;
import java.text.*;

public class BMRequest implements Serializable{
  public String BMName;
  public String NGBName;
  public String username;
  public String MachineName;
  public String PrLang;
  public char clss;
  public int n1,n2,n3,niter;
  public double time,acctime,ops,memsize;
  public double tmSent=0.0,tmReceived=0.0;
  public String optype;
  public int numthreads;
  public boolean serial;
  public int pid;
  public int uid;
  public int gid;
  public int ndid;
  public SparseA spa;
  public DGraph dfg;
  public int Explore=0;
  
  public BMRequest(){
  //default values
    BMName="SP";
    NGBName="ED";
    MachineName="turing.nas.nasa.gov";
    PrLang="Java";
    clss='S';
    n1=n2=n3=12;
    niter=40;
    time=12;
    ops=3000;
    memsize=2000;
    time=1000;
    optype="floating_point";
    numthreads=1;
    serial=true;
    pid=1;
    uid=0;
    gid=-1;
    ndid=-1;
  }
  public BMRequest(BMRequest req) {
  //default values
    if(req.BMName!=null)  BMName=new String(req.BMName);
    if(req.NGBName!=null) NGBName=new String(req.NGBName);
    username=req.username;
    MachineName=req.MachineName;
    PrLang=req.PrLang;
    clss=req.clss;
    n1=req.n1;
    n2=req.n2;
    n3=req.n3;
    niter=req.niter;
    time=req.time;
    ops=req.ops;
    memsize=req.memsize;
    time=req.time;
    optype=req.optype;
    numthreads=req.numthreads;
    serial=req.serial;
    pid=req.pid;  
    uid=req.uid;  
    gid=req.gid;  
    ndid=req.ndid;  
    dfg=req.dfg;
    Explore=req.Explore;
  }
 
  public void Show(){    
    DecimalFormat fmt = new DecimalFormat("0.000");
    System.out.println("***** NAS Grid Benchmark (NGB) Request *****");
    System.out.println("NGB             = "+NGBName+"."+BMName+"."+clss);
    System.out.println("Machine         = "+MachineName);
      System.out.print("Parallel mode   = ");
    if(serial) System.out.println("serial");
      else System.out.println(numthreads+" threads");
    System.out.println("Paradigm        = "+PrLang);
      System.out.print("Size            = ");
    if( n2 == 0 && n3 == 0 ) System.out.println(n1);
      else  System.out.println(n1+" X "+n2+" X "+n3);
    System.out.println("Iterations      = "+niter);
    System.out.println("Time in seconds = "+fmt.format(time));
    System.out.println("ACCTime         = "+fmt.format(acctime));
    System.out.println("Mops total      = "+fmt.format(ops));
    System.out.println("Operation type  = "+optype);
    System.out.println("Memory size     = "+fmt.format(memsize));
    System.out.println("Pid             = "+pid);
    System.out.println("Gid             = "+gid);
    System.out.println("Uid             = "+uid);
    System.out.println("Ndid            = "+ndid);
    if(dfg!=null)System.out.println("dfg            = "+dfg.name);
  }
  public int putInFile(String filename){
    BufferedWriter of=null;
    try{
      of = new BufferedWriter(new FileWriter(filename));
    }catch(Exception e){
      System.err.println("BMRequest.putInFile: filename "+e.toString());   
      return 0;
    }
    try{
      String line="1";
      of.write(line,0,line.length());	// ASCII format for FORTRAN in/out
      of.newLine();	    
      line=NGBName;
      of.write(line,0,line.length());
      of.newLine();	 
      line=String.valueOf(clss);
      of.write(line,0,line.length());
      of.newLine();	 
      line=String.valueOf(ndid);
      of.write(line,0,line.length());
      of.newLine();	 
      line=String.valueOf(uid);
      of.write(line,0,line.length());
      of.newLine();
      of.flush();
      of.close();
    }catch(Exception e){
      System.err.println("BMRequest.putInFile: "+e.toString());   
      return 0;
    }
    return 1;
  }
    public int getNGBdepth(){
    int dpt=1;
    if(dfg.name.indexOf("HC")>=0){
      switch(clss){
      case 'S':
      case 'W':
      case 'A':
   	dpt=9;
   	break;
      case 'B':
   	dpt=18;
   	break;
      case 'C':
   	dpt=27;
   	break;
      case 'D':
   	dpt=36;
   	break;
      case 'E':
   	dpt=45;
   	break;
      }
    }else if(dfg.name.indexOf("VP")>=0){
      switch(clss){
      case 'S':
      case 'W':
      case 'A':
   	dpt=3;
   	break;
      case 'B':
   	dpt=6;
   	break;
      case 'C':
   	dpt=9;
   	break;
      case 'D':
   	dpt=12;
   	break;
      case 'E':
   	dpt=15;
   	break;
      }
    }else if(dfg.name.indexOf("MB")>=0){
      switch(clss){
      case 'S':
      case 'W':
      case 'A':
   	dpt=3;
   	break;
      case 'B':
   	dpt=4;
   	break;
      case 'C':
   	dpt=5;
   	break;
      case 'D':
   	dpt=6;
   	break;
      case 'E':
   	dpt=7;
   	break;
      }
    }
    return dpt;
  }
}
