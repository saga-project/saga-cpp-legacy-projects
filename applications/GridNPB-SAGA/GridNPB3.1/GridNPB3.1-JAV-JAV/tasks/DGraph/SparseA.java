/*
!-------------------------------------------------------------------------!
!                                                                         !
!                   N A S   G R I D   B E N C H M A R K S                 !  
!                                                                         !
!			 J A V A	 V E R S I O N  		  !
!                                                                         !
!                              S P A R S E A                              !
!                                                                         !
!-------------------------------------------------------------------------!
!                                                                         !
!    SparseA implements Sparse Array Class.                               !
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

public class SparseA implements Serializable{
  public int len;
  public int fielddim=0,nx=0,ny=0,nz=0;
  public int idx[];
  public double val[];
  
  public SparseA(){
    len=12*12*12;
    idx=new int[len];
    val=new double[len]; 
    for(int i=0;i<len;i++){
      idx[i]=-1;
      val[i]=0.0;   
    }
  }
  public SparseA(SparseA ar){
    len=ar.len;
    fielddim=ar.fielddim;
    nx=ar.nx;
    ny=ar.ny;
    nz=ar.nz;
    idx=new int[len];
    val=new double[len];
    for(int i=0;i<len;i++){
      idx[i]=ar.idx[i];
      val[i]=ar.val[i];   
    }
  }
  public SparseA(int length){
    len=length; 
    idx=new int[len];
    val=new double[len];
    for(int i=0;i<len;i++){
      idx[i]=-1;
      val[i]=0.0;   
    }
  }
  public SparseA(int fieldd,int n1,int n2,int n3){
    len=fieldd*n1*n2*n3;
    fielddim=fieldd;
    nx=n1;
    ny=n2;
    nz=n3;
    idx=new int[len];
    val=new double[len];
    for(int i=0;i<len;i++){
      idx[i]=-1;
      val[i]=0.0;   
    }
  }
  public SparseA(String bm){
    if(  bm.indexOf("BT2MG")==0){
      fielddim=1;
      if(bm.indexOf(".S")>0){
        nx=ny=nz=12;
      }else if(bm.indexOf(".W")>0){
	nx=ny=nz=24;
      }else if(bm.indexOf(".A")>0){
        nx=ny=nz=64;
      }else if(bm.indexOf(".B")>0){
        nx=ny=nz=102;
      }else if(bm.indexOf(".C")>0){
        nx=ny=nz=162;
      }     
    }else if(  bm.indexOf("LU2MG")==0){
      fielddim=1;
      if(bm.indexOf(".S")>0){
        nx=ny=nz=12;
      }else if(bm.indexOf(".W")>0){
        nx=ny=nz=33;
      }else if(bm.indexOf(".A")>0){
        nx=ny=nz=64;
      }else if(bm.indexOf(".B")>0){
        nx=ny=nz=102;
      }else if(bm.indexOf(".C")>0){
        nx=ny=nz=162;
      }     
    }else if(  bm.indexOf("BT")==0||bm.indexOf("bt")==0
       ||bm.indexOf("SP")==0||bm.indexOf("sp")==0
       ||bm.indexOf("LU")==0||bm.indexOf("lu")==0
    ){
      fielddim=5;
      if(bm.indexOf(".S")>0){
        nx=ny=nz=12;
      }else if(bm.indexOf(".W")>0){
        if(bm.indexOf("BT")==0||bm.indexOf("bt")==0){
	  nx=ny=nz=24;
	}else if(bm.indexOf("SP")==0||bm.indexOf("sp")==0){
          nx=ny=nz=36;	
	}else if(bm.indexOf("LU")==0||bm.indexOf("lu")==0){
          nx=ny=nz=33;
	}
      }else if(bm.indexOf(".A")>0){
        nx=ny=nz=64;
      }else if(bm.indexOf(".B")>0){
        nx=ny=nz=102;
      }else if(bm.indexOf(".C")>0){
        nx=ny=nz=162;
      } 
    }else if(  bm.indexOf("FT")==0||bm.indexOf("ft")==0){
      fielddim=1;
      if(bm.indexOf(".S")>0){
        nx=ny=nz=64;
      }else if(bm.indexOf(".W")>0){
 	nx=ny=128; nz=32;
      }else if(bm.indexOf(".A")>0){
        nx=ny=256; nz=128;
      }else if(bm.indexOf(".B")>0){
        nx=512; ny=nz=256;
      }else if(bm.indexOf(".C")>0){
        nx=ny=nz=512;
      } 
    }else if(  bm.indexOf("CG")==0||bm.indexOf("cg")==0){
      fielddim=1;
      if(bm.indexOf(".S")>0){
        nx=1400; ny=nz=1;
      }else if(bm.indexOf(".W")>0){
 	nx=7000; ny=nz=1;
      }else if(bm.indexOf(".A")>0){
        nx=14000; ny=nz=1;
      }else if(bm.indexOf(".B")>0){
        nx=75000; ny=nz=1;
      }else if(bm.indexOf(".C")>0){
        nx=150000; ny=nz=1;
      } 
    }else if(  bm.indexOf("MG")==0||bm.indexOf("mg")==0){
      fielddim=1;
      if(bm.indexOf(".S")>0){
        nx=ny=nz=32;
      }else if(bm.indexOf(".W")>0){
 	nx=ny=nz=64;
      }else if(bm.indexOf(".A")>0){
        nx=ny=nz=256;
      }else if(bm.indexOf(".B")>0){
        nx=ny=nz=256;
      }else if(bm.indexOf(".C")>0){
        nx=ny=nz=512;
      } 
    }else if(  bm.indexOf("IS")==0||bm.indexOf("is")==0){
      fielddim=1;
      if(bm.indexOf(".S")>0){
        nx=1<<16; ny=nz=1;
      }else if(bm.indexOf(".W")>0){
 	nx=1<<20; ny=nz=1;
      }else if(bm.indexOf(".A")>0){
        nx=1<<23; ny=nz=1;
      }else if(bm.indexOf(".B")>0){
        nx=1<<25; ny=nz=1;
      }else if(bm.indexOf(".C")>0){
        nx=1<<27; ny=nz=1;
      } 
    }
    len=fielddim*nx*ny*nz; 
    idx=new int[len];
    val=new double[len];
  }
  public double CheckVal(){
    double checksum=0.0;
    for(int i=0;i<len;i++) checksum+=(val[i]*val[i])/(double)len;
    return checksum;
  }
  public int CheckIdx(){
    int checksum=0;
    for(int i=0;i<len;i++) checksum+=idx[i];
    return checksum;
  }
  public void Show(){
    System.err.println("Array: "+
                       " dims=("+fielddim+","+nx+","+ny+","+nz+")"+
                       " length="+ len+
                       " checkVal="+CheckVal()+
		       " checkIdx= "+CheckIdx());  
  }
  public void BlockCopy(SparseA b,int xoff,int yoff, int zoff){
    if(  fielddim!=b.fielddim){
        System.err.println("Can't copy array");
	b.Show();
        System.err.println("To array of different first dim");
	Show();
    }
    for(int i=0;i<b.len;i++){
      int fldidx=i%b.fielddim;
      int xidx=(i/b.fielddim)%b.nx+xoff;
      if(xidx<0||xidx>=nx) continue;
      int yidx=(i/(b.fielddim*b.nx))%b.ny+yoff;
      if(yidx<0||yidx>=ny) continue;
      int zidx=(i/(b.fielddim*b.nx*b.ny))%b.nz+zoff;
      if(zidx<0||zidx>=nz) continue;
      int aoff=fldidx+fielddim*(xidx+nx*(yidx+ny*zidx));      
      val[aoff]=b.val[i];
    } 
  }
  public void InterpAdd(SparseA b){
    if(  fielddim!=b.fielddim){
      System.err.println("Can't interpolate array of incompartible filed dim:");
      b.Show();
      System.err.print("To array");Show();
      return;
    }
    if(nx==b.nx&ny==b.ny&&nz==b.nz){
      for(int i=0;i<len;i++) val[i]+=b.val[i];
      return;   
    }
    int kupsum=0,jupsum=0,iupsum=0;
    double iflsum=0.0;
    for(int k2=1;k2<=nz;k2++){
      double kfl = ((k2-1.0)/(nz-1.0))*(b.nz-1.0)+1.0;
      int kup    = Math.min((int)(kfl+1.0),b.nz);
      kupsum+=kup;
      kup--;
      int kdw    = kup-1;
      double gamma  = kfl-kdw-1;
      for(int j2=1;j2<=ny;j2++){
        double jfl = ((j2-1.0)/(ny-1.0))*(b.ny-1.0)+1.0;
        int jup    = Math.min((int)(jfl+1.0),b.ny);
        jupsum+=jup;
	jup--;
        int jdw    = jup-1;
        double beta   = jfl-jdw-1;
        for(int i2=1;i2<=nx;i2++){	
          double ifl = ((i2-1.0)/(nx-1.0))*(b.nx-1.0)+1.0;
	  iflsum+=ifl;
          int iup    = Math.min((int)(ifl+1.0),b.nx);
          iupsum+=iup;
	  iup--;
          int idw    = iup-1;
          double alpha  = ifl-idw-1;

 	  int une=fielddim*(iup+b.nx*(jup+b.ny*kup)); 
	  int unw=fielddim*(idw+b.nx*(jup+b.ny*kup)); 
	  int use=fielddim*(iup+b.nx*(jdw+b.ny*kup)); 
	  int usw=fielddim*(idw+b.nx*(jdw+b.ny*kup)); 
	  int dnw=fielddim*(idw+b.nx*(jup+b.ny*kdw)); 
	  int dse=fielddim*(iup+b.nx*(jdw+b.ny*kdw)); 
	  int dsw=fielddim*(idw+b.nx*(jdw+b.ny*kdw)); 
	  int dne=fielddim*(iup+b.nx*(jup+b.ny*kdw));

	  int aoff=fielddim*(i2-1+nx*(j2-1+ny*(k2-1))); 
          for(int m=0;m<fielddim;m++){
            val[m+aoff]+=gamma*(
	                 beta*(
			   alpha*      b.val[m+une]+
			   (1.0-alpha)*b.val[m+unw])+
	                 (1.0-beta)*(
			   alpha*      b.val[m+use]+
			   (1.0-alpha)*b.val[m+usw]))+
                       (1.0-gamma)*(
	                 beta*(
			   alpha*      b.val[m+dne]+
			   (1.0-alpha)*b.val[m+dnw])+
	                 (1.0-beta)*(
			   alpha*      b.val[m+dse]+
			   (1.0-alpha)*b.val[m+dsw]));
          } 
        } 
      } 
    } 
  }
  public int putInFile(String filename){
    BufferedWriter of=null;
    try{
      of = new BufferedWriter(new FileWriter(filename));
    }catch(Exception e){
      System.err.println("SparseA.putInFile: filename "+e.toString());   
      return 0;
    }
    try{
       String line=null;
       for(int i=0;i<len;i+=4){
         line=String.valueOf(val[i])+"  ";
	 for(int j=1;j<4;j++){
	   if(j+i==len) break;
	   line+=String.valueOf(val[i+j])+"  ";
	 }
         of.write(line,0,line.length());
         of.newLine();
       }	     
       of.flush();
       of.close();
    }catch(Exception e){
      System.err.println("SparseA.putInFile: "+e.toString());   
      return 0;
    }
    return 1;
  }
}
