/*
!-------------------------------------------------------------------------!
!                                                                         !
!                   N A S   G R I D   B E N C H M A R K S                 !  
!                                                                         !
!			 J A V A	 V E R S I O N  		  !
!                                                                         !
!                              B M A R G S                                !
!                                                                         !
!-------------------------------------------------------------------------!
!                                                                         !
!    BMArgs are derived from NPB_JAV.BMInOut.BMArgs by overloaing the     !
!    Banner method.                                                       !
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

public class BMArgs extends NPB_JAV.BMInOut.BMArgs{
  BMArgs(){super();}
  public static void Banner(BMRequest req,String BMName,
                            char clss,boolean serial,int np){
    if(req!=null) 
      System.out.println(" NAS Grid Benchmarks Java version (GridNPB3.0)");
    else 
      System.out.println(" NAS Parallel Benchmarks Java version (NPB3.0-JAV)");
    
    if(serial) System.out.println(" Serial Version "+BMName+"."+clss);
    else System.out.println(" Multithreaded Version "+BMName+"."+clss+
                            " np="+np);
  }
}
