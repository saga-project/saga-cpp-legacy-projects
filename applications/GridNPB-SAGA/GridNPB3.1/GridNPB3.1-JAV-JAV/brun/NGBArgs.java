/*
!-------------------------------------------------------------------------!
!		 							  !
!	         N  A  S     G R I D      B E N C H M A R K S             !
!									  !
!			J A V A     V E R S I O N			  !
!									  !
!                             N G B A R G S                               !
!                                                                         !
!-------------------------------------------------------------------------!
!                                                                         !
!    BMArgs implements Command Line Benchmark Arguments class             !
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
import NPB_JAV.BMInOut.BMArgs;

import java.io.*;
import java.text.*;

public class NGBArgs extends NPB_JAV.BMInOut.BMArgs{
  public NGBArgs(){
  }
  static public BMRequest ParseCmdLineArgs(String argv[]){ 
    BMRequest req=new BMRequest();
    if(argv.length!=1) commandLineError();
    req.dfg=DGraph.ReadVCG(argv[0]);
    if(req.dfg==null){
      System.out.println("NGBArgs.ParseCmdLineArgs: can't read DFG file"+
      			  argv[0]);
      System.exit(0);
    }
    getBMNameAndClass(argv[0].toUpperCase(),".vcg",req);
    return req;
  }
  public static void commandLineError(){
    System.out.println("synopsis: java GRIDNPB.ngbrun.NGBRun"+
                       " DFGname.vcg");
    System.out.println("\t DFGname.vcg is a name of the file\n"+
		       "\t containing a VCG description of the NGB.\n");
    System.exit(1);	
  } 
  public static void Banner(BMRequest req){
    System.out.println("\n* NAS Grid Benchmarks (NGB) Java version");
    System.out.println("* Starting "+req.NGBName+"."+req.clss);
    System.out.println("* Programming language is "+req.PrLang);
  }
  public static int getBMNameAndClass(String str,String ext,BMRequest req){
    if(str==null) return 0;
    if(str.indexOf("ED.")>=0) req.NGBName="ED";
    else if(str.indexOf("HC.")>=0) req.NGBName="HC";
    else if(str.indexOf("VP.")>=0) req.NGBName="VP";
    else if(str.indexOf("MB.")>=0) req.NGBName="MB";
    else {
      if(ext!=null){
        int idx=str.indexOf("ext");
        if(idx<0) req.NGBName=str;
        else req.NGBName=str.substring(0,idx);
      }
      return 0;
    }
    req.clss='U';  //UNKNOWN
    if(str.indexOf(".S")>=0) req.clss='S';
    else if(str.indexOf(".W")>=0) req.clss='W';
    else if(str.indexOf(".A")>=0) req.clss='A';
    else if(str.indexOf(".B")>=0) req.clss='B';
    else return 0;
    return 1;
  }
  public static String getBanner(String str){
    String banner=str;
    if(str.indexOf("ED")>=0) 
      banner="Embarassingly distributed (ED) - "+str;
    else if(str.indexOf("HC")>=0) 
      banner="Helical Chain (HC) - "+str;
    else if(str.indexOf("VP")>=0)
      banner="Visualisation Pipeline (VP) - "+str;
    else if(str.indexOf("MB")>=0) 
      banner="Mixed Bag (MB) - "+str;     
    return banner;
  }
}
