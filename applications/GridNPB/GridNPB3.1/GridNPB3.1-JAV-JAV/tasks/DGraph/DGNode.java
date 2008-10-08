/*
!-------------------------------------------------------------------------!
!                                                                         !
!                   N A S   G R I D   B E N C H M A R K S                 !  
!                                                                         !
!			 J A V A	 V E R S I O N  		  !
!                                                                         !
!                               D G N O D E                               !
!                                                                         !
!-------------------------------------------------------------------------!
!                                                                         !
!    DGNode implements Directed Graph Node.                               !
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

public class DGNode implements Serializable{
  public int	maxInDegree;
  public int	maxOutDegree;
  public int	id;
  public String	name;
  public DGArc 	inArc[];
  public int	inDegree;
  public DGArc 	outArc[];
  public int	outDegree;
  public DGraph	graph;
  public int attribute;
  public int color;
  public int verified=1;
	 
  public DGNode(String nm){
    attribute=0;
    color=0;
    inDegree=0;
    outDegree=0;
    maxInDegree=16;
    maxOutDegree=16;
    inArc=new DGArc[maxInDegree];
    outArc=new DGArc[maxOutDegree];
    name=nm;
  }
  public DGNode(DGNode nd){
    attribute=nd.attribute;
    name=nd.name;
    color=nd.color;
    verified=nd.verified; 
  }
  public void Show(){
    if(verified==1) System.out.println(id+"."+name+"\t: usable.");
    else if(verified==0) System.out.println(id+"."+name+"\t: unusable.");
    else System.out.println(id+"."+name+"\t: notverified.");   
  }
  public SparseA CombineArcData(ArcHead arh[]){
    SparseA arr=new SparseA(name);
    if(arh.length!=inDegree){
      System.err.println("**CombineArcData: mismatch between arh.length="
                          +arh.length+
			  "and inDegree="+inDegree);
      Show();
    }
    for(int i=inDegree-1;i>=0;i--){
      if(arh[i].spa==null){
        System.err.println("**CombineArcData: missing data on arc "+i);
	continue;
      }
      arr.InterpAdd(arh[i].spa); 
      arh[i].spa=null;
      arh[i]=null;
    }
    System.runFinalization();
    System.gc();
    if(inDegree>0) {
      for(int i=0;i<arr.len;i++) arr.val[i]/=inDegree;
      return arr;
    }else return null;
  }
  public void PutArcData(BMRequest req){
    int arcid=req.pid;
    req.spa=new SparseA();
  }
}
