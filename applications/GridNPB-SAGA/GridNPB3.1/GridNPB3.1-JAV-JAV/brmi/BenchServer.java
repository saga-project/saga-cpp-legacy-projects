/*
!-------------------------------------------------------------------------!
!                                                                         !
!                   N A S   G R I D   B E N C H M A R K S                 !  
!                                                                         !
!			 J A V A	 V E R S I O N  		  !
!                                                                         !
!                           B E N C H S E R V E R                         !
!                                                                         !
!-------------------------------------------------------------------------!
!                                                                         !
!    BenchServer implements Benchmark Server Class.                       !
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
package brmi;

import tasks.DGraph.*;

import java.util.*;
import java.net.*;
import java.rmi.Naming;
import java.rmi.Remote;
import java.rmi.RemoteException;
import java.rmi.RMISecurityManager;
import java.rmi.server.UnicastRemoteObject;

public class BenchServer extends UnicastRemoteObject 
                      implements Benchmark{
    static int maxbmnum=16;
    static int maxusrnum=16;
    static int maxarcnum=32;
    static BenchUnion bu[];
    static ArcHead ahead[];
    static Hashtable arhead;

    public BenchServer() throws RemoteException {
      super();
      bu=new BenchUnion[maxbmnum]; 
      ahead=new ArcHead[maxarcnum];
      arhead=new Hashtable(2*maxarcnum);
    }    
    public void startBenchmark(BMRequest req,BMResults res)
        			     throws RemoteException{
      if(bu[req.pid]!=null){
        try{bu[req.pid].finalize();}
        catch(Throwable e){
          System.err.println("BenchServer.startBenchmark: "+
	                   "exception in finalizing bu["+req.pid+"].");
        }
      }
      System.runFinalization();
      System.gc();
      System.err.println("startBenchmark: node."+req.pid+" "+req.BMName+
                         " started at "+req.MachineName);

      if(req.PrLang.equals("Java")){
	GetArcData(req);
        bu[req.pid] = new BenchUnion(req);
        bu[req.pid].startBenchmark();
	try{bu[req.pid].benchWait();}
	catch(InterruptedException e){e.printStackTrace();}
	PutArcData(req,res);
	System.runFinalization();
	System.gc();
      }else{
         System.out.println( "Remote execution on "+req.PrLang
	                    +" not implemented.");
      }
    }
    public int GetArcData(BMRequest req){
      DGNode nd=req.dfg.node[req.pid];
      for(int i=0;i<nd.inDegree;i++){
        int arcid=nd.inArc[i].id;
	Integer intgrn=getHVal(req.uid,req.dfg.id,arcid);
	while(!arhead.containsKey(intgrn)){
          try{
	    Thread.sleep(10);
	  }catch(InterruptedException e){
	    System.err.println("GetArcData: Arc["+arcid+
	                       "]. Interrupted exception "+e.getMessage());
	    e.printStackTrace(); 
	  }
	}
      }
      if(nd.inDegree>0)CombineArcData(req,nd);
      return 1;
    }
    public int CombineArcData(BMRequest req,DGNode nd){
      ArcHead arh[]=new ArcHead[nd.inDegree];
      for(int i=0;i<nd.inDegree;i++){
        int arcid=nd.inArc[i].id;
	Integer intgrn=getHVal(req.uid,req.dfg.id,arcid);
	arh[i]=new ArcHead((ArcHead)arhead.get(intgrn));
        arhead.remove(intgrn);
	req.tmSent+=arh[i].req.tmSent;
	req.tmReceived+=arh[i].req.tmReceived;
      }
      req.spa=nd.CombineArcData(arh);
      arh=null;
      System.runFinalization();
      System.gc();
      return 1;
    }
    public int PutArcData(BMRequest req,BMResults res){
      DGNode nd=req.dfg.node[req.pid];
      BMRequest lreq[]=new BMRequest[nd.outDegree];
      for(int i=0;i<nd.outDegree;i++){
        lreq[i]=new BMRequest(req);
	lreq[i].dfg=req.dfg;
        int arcid=nd.outArc[i].id;
	DGArc ar=req.dfg.arc[arcid];
        int valrequest=GetRequest(lreq[i],nd,ar);
	if(ar.head.name.indexOf("MG")>=0){
	  lreq[i].spa=bu[req.pid].getMachNum();
	}else {
	  lreq[i].spa=bu[req.pid].getField();
	}
        if(valrequest==1) try{    
	  Benchmark RemBench = (Benchmark) Naming.lookup("//"+
	                       lreq[i].MachineName+"/BenchmarkServer");
	  lreq[i].tmSent=System.currentTimeMillis();
	  RemBench.SendData(lreq[i],res);
	  lreq[i].spa=null;
	  lreq[i].dfg=null;
          lreq[i]=null;
        }catch (Exception e) {
	  System.out.println("BenchServer exception: " +
	                     e.getMessage()); 
	  e.printStackTrace(); 
        }
      }
      lreq=null;
      nd=null;
      System.runFinalization();
      System.gc();
      return 1;   
    }
    public int SendData(BMRequest req,BMResults res){
      Integer intgrn=getHVal(req.uid,req.dfg.id,req.pid);
      req.tmReceived=System.currentTimeMillis();
      arhead.put(intgrn,(new ArcHead(req)));
      System.err.println("BMServer: "+req.NGBName+
        		 "."+req.BMName+"."+req.clss+
        		 ".CommTime="+(req.tmReceived-req.tmSent));
      return 1;
    }
    public BMResults getResults(BMRequest req) throws RemoteException{
      BMResults lres = new BMResults(req.pid);
        try{bu[req.pid].benchWait();}catch(InterruptedException e){
          System.out.println("Interrupted Exception: " + 
	                      e.getMessage()); 
          e.printStackTrace(); 
        } 
        lres=bu[req.pid].getResults();
        if(bu[req.pid].res!=null){
          lres.tmSent=bu[req.pid].req.tmSent;
          lres.tmReceived=bu[req.pid].req.tmReceived;
	  if(bu[req.pid].req.spa!=null){
	    lres.RecArrSize=bu[req.pid].req.spa.len*(4+8);
	  }
        }
      return lres;
    }
    public static void main(String args[]) { 
      // Create and install a security manager 
      String lhost="UNKNOWN";
      try{
        InetAddress addr=InetAddress.getLocalHost();
        if(addr!=null) lhost=addr.getHostName();    
      }catch(java.net.UnknownHostException e){
	System.out.println("BenchServer err: "+e.getMessage()); 
	e.printStackTrace(); 
      }
      if(System.getSecurityManager()==null) { 
	System.setSecurityManager(new RMISecurityManager()); 
      }
      try{ 
	BenchServer obj = new BenchServer(); 
	Naming.rebind("BenchmarkServer", obj); 
	System.out.println("BenchmarkServer bound in registry on "+lhost); 
      }catch (Exception e) { 
	System.out.println("BenchServer err: "+e.getMessage()); 
	e.printStackTrace(); 
      } 
    }
    int GetRequest(BMRequest req,DGNode nd,DGArc ar){
      req.pid=ar.id;
      NdName2Req(ar.head.name,req);
      return 1;
    }
    public void NdName2Req(String str,BMRequest req){
      int idx,fromidx;
      fromidx=0;
      idx=str.indexOf('.',fromidx);
      req.BMName=str.substring(fromidx,idx-fromidx);
      fromidx=idx+1;
      idx=str.indexOf('.',fromidx);
      char tmpclss[]=new char[10];
      str.getChars(fromidx,fromidx+1,tmpclss,0);
      req.clss=tmpclss[0];
      fromidx=idx+1;
      if(0<fromidx&&fromidx<str.length())
        req.MachineName=str.substring(fromidx); 
    }
    Integer getHVal(int uid,int dfgid,int arcid){
      return new Integer(uid+maxusrnum*(dfgid+maxbmnum*arcid));
    }  
    public void finalize()throws RemoteException{
//      System.err.println("===== BenchServer.finalize start");
//      System.err.println("===== BenchServer.finalize OK");
    } 
}
