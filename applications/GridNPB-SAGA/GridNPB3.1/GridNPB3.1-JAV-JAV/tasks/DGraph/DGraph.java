/*
!-------------------------------------------------------------------------!
!                                                                         !
!                   N A S   G R I D   B E N C H M A R K S                 !  
!                                                                         !
!			 J A V A	 V E R S I O N  		  !
!                                                                         !
!                               D G R A P H                               !
!                                                                         !
!-------------------------------------------------------------------------!
!                                                                         !
!    DGraph implements rudimentary Directed Graph.                        !
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
import java.awt.*;
import java.util.*;

public class DGraph implements Serializable{
         int	maxNodes;
         int	maxArcs;
  public int	id;
  public String name;
  public int	numNodes;
  public int	numArcs;
  public DGNode	node[];
  public DGArc arc[];
  int BLOCK_SIZE = 128;
		
  public DGraph(String nm){
    name=nm;
    maxNodes=BLOCK_SIZE;
    maxArcs=BLOCK_SIZE;
    node=new DGNode[maxNodes];
    arc=new DGArc[maxArcs];
  }
  
  public int AttachNode(DGNode nd){
    int i,j;
    if (numNodes == maxNodes ) {
      maxNodes += BLOCK_SIZE;
      DGNode tmpndar[]=new DGNode[maxNodes];
      for(i=0;i < numNodes; i++) tmpndar[i]=node[i];
      node=tmpndar;
    }
    for ( i = 0; i < numNodes; i++) {
      DGNode tmpnd = node[i];
      if (  tmpnd.name.length() != nd.name.length() 
    	  ||nd.name.indexOf(tmpnd.name)<0) continue;
      if ( nd.inDegree > 0 ) {
        if(tmpnd.inDegree+nd.inDegree>tmpnd.maxInDegree){
    	  tmpnd.maxInDegree += nd.maxInDegree;
          DGArc tmparcar[]=new DGArc[tmpnd.maxInDegree];
          for(j=0;j<tmpnd.inDegree;j++) tmparcar[j]=tmpnd.inArc[j];
          tmpnd.inArc=tmparcar;
    	}
        for ( j = 0; j < nd.inDegree; j++ ){
          nd.inArc[j].head = tmpnd;
          tmpnd.inArc[tmpnd.inDegree++]=nd.inArc[j];
        }
      } 
      if( nd.outDegree > 0 ) {
        if(tmpnd.outDegree+nd.outDegree>tmpnd.maxOutDegree){
    	  tmpnd.maxOutDegree += nd.maxOutDegree;
          DGArc tmparcar[]=new DGArc[tmpnd.maxOutDegree];
          for(j=0;j<tmpnd.outDegree;j++) tmparcar[j]=tmpnd.inArc[j];
          tmpnd.inArc=tmparcar;
    	}
    	for ( j = 0; j < nd.outDegree; j++ ){
          nd.outArc[j].tail = tmpnd;
          tmpnd.outArc[tmpnd.outDegree++]=nd.outArc[j];
        }
      } 
      return i;
    }
    nd.id = numNodes;
    node[numNodes] = nd;
    numNodes++;
    return nd.id;
  }
  public int FindByName(String name){
    int id=-1;
    for(int i = 0; i < numNodes; i++) {
      DGNode tmpnd = node[i];
      if (  tmpnd.name.length() != name.length() 
    	  ||name.indexOf(tmpnd.name)<0) continue;
      id=i;
      break;
    }
    return id;
  }
  public static DGraph ReadVCG(String filename){
    BufferedReader in;
    try{
      in = new BufferedReader(new FileReader(filename));
    }catch(Exception e){
      System.out.println("ReadVCG:Can't open file="+filename);
      return null;
    }
    DGraph gr=new DGraph(filename);
    int idx,fromidx,count;
    String str;
    try{
      while(in.ready()){
  	str=in.readLine();
  	if(str.regionMatches(0,"node:",0,5)){
  	  fromidx=5;
  	  idx=str.indexOf("label:",fromidx);
  	  fromidx=idx+1;
  	  idx=str.indexOf('\"',fromidx);
  	  fromidx=idx+1;
  	  idx=str.indexOf('\"',fromidx);   
  	  gr.node[gr.numNodes]=new DGNode(str.substring(fromidx,idx));
  	  gr.node[gr.numNodes].id=gr.numNodes;
  	  gr.numNodes++;
  	}else if(str.regionMatches(0,"edge:",0,4)){
  	  fromidx=5;
  	  idx=str.indexOf("sourcename:",fromidx);
  	  fromidx=idx+1;
  	  idx=str.indexOf('\"',fromidx);
  	  fromidx=idx+1;
  	  idx=str.indexOf('\"',fromidx);
  	  Integer tailid=new Integer(str.substring(fromidx,idx));
  	  idx=str.indexOf("targetname:",fromidx);
  	  fromidx=idx+1;
  	  idx=str.indexOf('\"',fromidx);
  	  fromidx=idx+1;
  	  idx=str.indexOf('\"',fromidx);
  	  Integer headid=new Integer(str.substring(fromidx,idx));
  	  DGNode tail=gr.node[tailid.intValue()];
  	  DGNode head=gr.node[headid.intValue()];
  	  gr.arc[gr.numArcs]=new DGArc(tail,head);
  	  gr.arc[gr.numArcs].id=gr.numArcs;
  	  tail.outArc[tail.outDegree]=gr.arc[gr.numArcs];
  	  tail.outDegree++;
  	  head.inArc[head.inDegree]=gr.arc[gr.numArcs];
  	  head.inDegree++;
  	  gr.numArcs++;
  	}
      }     
    }catch(Exception e){
      System.out.println("ReadVCG: Exception inreading file="+filename);
    }	  
    return gr;
  }

  public void Show( int DetailsLevel){
//    System.out.println(id+"."+name+": ("+numNodes+","+numArcs+")");
    System.out.println("* "+name+": ("+numNodes+","+numArcs+")");
    if( DetailsLevel < 1) return;
    for(int  i = 0; i < numNodes; i++ ) {
      DGNode focusNode = node[ i];
      if( DetailsLevel >= 2) {
  	for(int  j = 0; j < focusNode.inDegree; j++ ) {
  	    focusNode.inArc[j].tail.Show();
  	}
      }
      System.out.print("*\t");
      focusNode.Show();
      if(DetailsLevel < 2) continue;
      for(int j = 0; j < focusNode.outDegree; j++ ) {
        focusNode.outArc[ j].head.Show();
      } 
      System.out.println("---");
    }
    return;	  
  }
}
