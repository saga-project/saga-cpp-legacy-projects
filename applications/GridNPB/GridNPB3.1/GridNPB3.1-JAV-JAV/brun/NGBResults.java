/*
!-------------------------------------------------------------------------!
!		 							  !
!	         N  A  S     G R I D      B E N C H M A R K S             !
!									  !
!			J A V A     V E R S I O N			  !
!									  !
!                           N G B R E S U L T S                           !
!                                                                         !
!-------------------------------------------------------------------------!
!                                                                         !
!    NGBResults implements NGB Results Class extending BMResults class.   !
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

import tasks.DGraph.BMResults;

import java.io.*;
import java.text.*;
import java.util.*;

public class NGBResults extends BMResults{
  public Date date;
  public String NGBName="ED";
  public double taTime=0.0;
  public double acctime=0.0;
  
  public NGBResults(){
  }
  public NGBResults(int bid){
    super(bid);
  }
  public NGBResults(int bid,String ngbname){
    super(bid);
    NGBName=ngbname;
  }
  public NGBResults(String str) {
    super(str);
  }
  public NGBResults(String bname,
		   char   CLASS,
		   int    bn1, 
		   int    bn2,
		   int    bn3,
		   int    bniter,
		   double btime,
		   double bmops,
		   String boptype,
		   int    passed_verification,
		   boolean bserial,
	           int num_threads,
	           int bid){ 
    super(bname,CLASS,bn1,bn2,bn3,
          bniter,btime,bmops,boptype,passed_verification,
	  bserial,num_threads,bid);
  }
  public void print(){
    StringBuffer outbuf=new StringBuffer( "                                "
                                         +"                               *");
    int len=outbuf.length();
    String outline=new String("****** NAS Grid Benchmarks"+
                              " * Java version (GridNPB) "+
			      name+"."+clss+" *****");
    outbuf.insert(0,outline);
    outbuf.setLength(len);
    outbuf.insert(len-2,"**");
    System.out.println(outbuf.toString());

    outbuf=new StringBuffer( "                          "
                            +"                         *");     
    if(verified==1)
      outline=new String("** Verification    = successful");
    else if(verified==0) 
      outline=new String("** Verification    = failed");
    else 
      outline=new String("** Verification    = not verified");
    outbuf.insert(0,outline);
    outbuf.setLength(len);
    outbuf.insert(len-2,"**");
    System.out.println(outbuf.toString());

    outbuf=new StringBuffer( "                          "
                            +"                         *");     
      outline=new String("** Start time      = "+date.toString());
    outbuf.insert(0,outline);
    outbuf.setLength(len);
    outbuf.insert(len-2,"**");
    System.out.println(outbuf.toString());
    
    outbuf=new StringBuffer( "                          "
                            +"                         *");     
      outline=new String("** Turnaround time = "+taTime+
        	         " sec. in Java");  
    outbuf.insert(0,outline);
    outbuf.setLength(len);
    outbuf.insert(len-2,"**");
    
    System.out.println(outbuf.toString());
    outbuf=new StringBuffer( "                          "
                            +"                         *");     
    outline=new String( "*******************************"
                       +"********************************");
    outbuf.insert(0,outline);
    outbuf.setLength(len);
    outbuf.insert(len-1,"*");
    System.out.println(outbuf.toString());
  }
}
