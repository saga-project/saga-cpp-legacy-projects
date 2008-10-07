/*
!-------------------------------------------------------------------------!
!                                                                         !
!                   N A S   G R I D   B E N C H M A R K S                 !  
!                                                                         !
!			 J A V A	 V E R S I O N  		  !
!                                                                         !
!                              S P T A S K                                !
!                                                                         !
!-------------------------------------------------------------------------!
!                                                                         !
!    SPTask encapsulates SP benchmark of NPB_JAV benchmark.               !
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
import NPB_JAV.SP.*;

import java.io.*;
import java.text.*;

public class SPTask extends NPB_JAV.SP{
  public int bid=-1;
  public BMResults results;  
  public BMRequest request=null;

  public SPTask(char clss, int np, boolean ser){ 
    super(clss,np,ser);
  }
  public SPTask(BMRequest req){ 
    super(req.clss,req.numthreads,req.serial);
    request=req;
    serial=req.serial;
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
    SPTask sptask = null;

    BMArgs.ParseCmdLineArgs(argv,BMName);
    char CLSS=BMArgs.CLASS;
    int np=BMArgs.num_threads;
    boolean serial=BMArgs.serial;
    try{ 
      sptask = new SPTask(CLSS,np,serial);
    }catch(OutOfMemoryError e){
      BMArgs.outOfMemoryMessage();
      System.exit(0);
    }      
    sptask.runTask();
  }

  public void run(){runTask();}

  public void runTask(){ 
    BMArgs.Banner(request,BMName,CLASS,serial,num_threads);
    
    int numTimers=t_last+1;
    String t_names[] = new String[numTimers];
    double trecs[] = new double[numTimers];
    setTimers(t_names);
//---------------------------------------------------------------------
//      Read input file (if it exists), else take
//      defaults from parameters
//---------------------------------------------------------------------
    int niter=getInputPars();
    setConst();
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
    for(int step = 1;step<=niter;step++){
      if (step%20== 0||step==1||step==niter) {
	System.out.println("Time step " + step);
      }
      if(serial) adi_serial();
      else adi(); 
    }
    timer.stop(1);
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
    double xcrref[] = new double[5],xceref[] = new double[5],
	   xcrdif[] = new double[5],xcedif[] = new double[5],
	   xce[] = new double[5],xcr[] = new double[5],
	   dtref=0;
    int m;
    int verified=-1;
    char clss = 'U';
//---------------------------------------------------------------------
//   compute the error norm and the residual norm, and exit if not printing
//---------------------------------------------------------------------
    error_norm(xce);
    compute_rhs();
    rhs_norm(xcr);

    for(m=0;m<=4;m++) xcr[m] = xcr[m] / dt;

    for(m=0;m<=4;m++){
      xcrref[m] = 1.0;
      xceref[m] = 1.0;
    }
    if(request!=null){
      if(request.dfg!=null&&request.dfg.name.indexOf("ED")>=0){
        if(request.dfg.name.indexOf("ED.S")>=0){
          clss = 'S';
          dtref = .015;
	  if(request.ndid==0){
             xcrref[0] = 0.8676543011741937E-04;
             xcrref[1] = 0.6033493408310530E-04;
             xcrref[2] = 0.6278114032528798E-04;
             xcrref[3] = 0.6603179902638946E-04;
             xcrref[4] = 0.9122028866543740E-04;
 
             xceref[0] = 0.8719352691803724E-07;
             xceref[1] = 0.5988808458188951E-07;
             xceref[2] = 0.6270092156558460E-07;
             xceref[3] = 0.6551568383242495E-07;
             xceref[4] = 0.8553969317102369E-07;
	  }else if(request.ndid==1){
             xcrref[0] = 0.8601620225691811E-04;
             xcrref[1] = 0.5988773196543108E-04;
             xcrref[2] = 0.6157026954073211E-04;
             xcrref[3] = 0.6457044508652214E-04;
             xcrref[4] = 0.8687428401194037E-04;
	
             xceref[0] = 0.8643202500288453E-07;
             xceref[1] = 0.5943418706437600E-07;
             xceref[2] = 0.6155109448566052E-07;
             xceref[3] = 0.6413646078612827E-07;
             xceref[4] = 0.8152493003792681E-07;
	  }else if(request.ndid==2){
             xcrref[0] = 0.8528517607623426E-04;
             xcrref[1] = 0.5944509868286733E-04;
             xcrref[2] = 0.6043095623721710E-04;
             xcrref[3] = 0.6319041663855621E-04;
             xcrref[4] = 0.8273663135464699E-04;

             xceref[0] = 0.8568916795314083E-07;
             xceref[1] = 0.5898555351009214E-07;
             xceref[2] = 0.6046719704763455E-07;
             xceref[3] = 0.6283245382631176E-07;
             xceref[4] = 0.7770753603531580E-07;
	  }else if(request.ndid==3){
             xcrref[0] = 0.8457166533017866E-04;
             xcrref[1] = 0.5900696189910025E-04;
             xcrref[2] = 0.5935880462626813E-04;
             xcrref[3] = 0.6188715115064566E-04;
             xcrref[4] = 0.7879773267341849E-04;

             xceref[0] = 0.8496424291102656E-07;
             xceref[1] = 0.5854206620123146E-07;
             xceref[2] = 0.5944517278358421E-07;
             xceref[3] = 0.6159941928907614E-07;
             xceref[4] = 0.7407863788365737E-07;
	  }else if(request.ndid==4){
             xcrref[0] = 0.8387501686770963E-04;
             xcrref[1] = 0.5857325969939944E-04;
             xcrref[2] = 0.5834963218012127E-04;
             xcrref[3] = 0.6065629301996203E-04;
             xcrref[4] = 0.7504851869600651E-04;
 
             xceref[0] = 0.8425657226531804E-07;
             xceref[1] = 0.5810362059449604E-07;
             xceref[2] = 0.5848116700380958E-07;
             xceref[3] = 0.6043330938920482E-07;
             xceref[4] = 0.7062984804457614E-07;
	  }else if(request.ndid==5){
             xcrref[0] = 0.8319460928597034E-04;
             xcrref[1] = 0.5814393866091043E-04;
             xcrref[2] = 0.5739946515836649E-04;
             xcrref[3] = 0.5949368685757409E-04;
             xcrref[4] = 0.7148042299123914E-04;
 
             xceref[0] = 0.8356551209019535E-07;
             xceref[1] = 0.5767012327603912E-07;
             xceref[2] = 0.5757152229935169E-07;
             xceref[3] = 0.5933026612155630E-07;
             xceref[4] = 0.6735324051262077E-07;
	  }else if(request.ndid==6){
             xcrref[0] = 0.8252985024299141E-04;
             xcrref[1] = 0.5771895249772953E-04;
             xcrref[2] = 0.5650453308785348E-04;
             xcrref[3] = 0.5839537229112765E-04;
             xcrref[4] = 0.6808535665169321E-04;
 
             xceref[0] = 0.8289044920091543E-07;
             xceref[1] = 0.5724149042257922E-07;
             xceref[2] = 0.5671277285472057E-07;
             xceref[3] = 0.5828661632209810E-07;
             xceref[4] = 0.6424132668705135E-07;
	  }else if(request.ndid==7){
             xcrref[0] = 0.8188017551638361E-04;
             xcrref[1] = 0.5729826099434987E-04;
             xcrref[2] = 0.5566126383428085E-04;
             xcrref[3] = 0.5735757816226423E-04;
             xcrref[4] = 0.6485568447004557E-04;
 
             xceref[0] = 0.8223080020494879E-07;
             xceref[1] = 0.5681764661514553E-07;
             xceref[2] = 0.5590163933420793E-07;
             xceref[3] = 0.5729886620119791E-07;
             xceref[4] = 0.6128703228573924E-07;
	  }else if(request.ndid==8){
             xcrref[0] = 0.8124504686581368E-04;
             xcrref[1] = 0.5688182884348056E-04;
             xcrref[2] = 0.5486627790428196E-04;
             xcrref[3] = 0.5637671694675925E-04;
             xcrref[4] = 0.6178420280687236E-04;
 
             xceref[0] = 0.8158600910848114E-07;
             xceref[1] = 0.5639852350520845E-07;
             xceref[2] = 0.5513502305850328E-07;
             xceref[3] = 0.5636369592773586E-07;
             xceref[4] = 0.5848367565086695E-07;
	  }
	}else if(request.dfg.name.indexOf("ED.W")>=0){
          clss = 'W';
          dtref = .0015;
	  if(request.ndid==0){
             xcrref[0] = 0.1745133059397087E-04;
             xcrref[1] = 0.1194347497651876E-04;
             xcrref[2] = 0.1271275032480275E-04;
             xcrref[3] = 0.1150480909799353E-04;
             xcrref[4] = 0.1305114022206260E-04;
	
             xceref[0] = 0.6955341579879958E-06;
             xceref[1] = 0.4732433767510304E-06;
             xceref[2] = 0.5071720177863302E-06;
             xceref[3] = 0.4600674574080496E-06;
             xceref[4] = 0.5381322068507668E-06;
	  }else if(request.ndid==1){
             xcrref[0] = 0.1745560352290239E-04;
             xcrref[1] = 0.1193739401471295E-04;
             xcrref[2] = 0.1269942115908446E-04;
             xcrref[3] = 0.1149066787325767E-04;
             xcrref[4] = 0.1301167555462321E-04;
	
             xceref[0] = 0.6957392189784409E-06;
             xceref[1] = 0.4730069614536907E-06;
             xceref[2] = 0.5066479195335540E-06;
             xceref[3] = 0.4595086246319152E-06;
             xceref[4] = 0.5365356761184354E-06;
	  }else if(request.ndid==2){
             xcrref[0] = 0.1745986923555756E-04;
             xcrref[1] = 0.1193141412701560E-04;
             xcrref[2] = 0.1268621099471008E-04;
             xcrref[3] = 0.1147665934807368E-04;
             xcrref[4] = 0.1297237680057378E-04;

             xceref[0] = 0.6959438483249020E-06;
             xceref[1] = 0.4727745781123514E-06;
             xceref[2] = 0.5061284384576993E-06;
             xceref[3] = 0.4589549875939992E-06;
             xceref[4] = 0.5349455062024703E-06;
	  }else if(request.ndid==3){
             xcrref[0] = 0.1746412767819883E-04;
             xcrref[1] = 0.1192553395452632E-04;
             xcrref[2] = 0.1267311889347240E-04;
             xcrref[3] = 0.1146278217756985E-04;
             xcrref[4] = 0.1293324425385692E-04;

             xceref[0] = 0.6961480441358915E-06;
             xceref[1] = 0.4725461723920497E-06;
             xceref[2] = 0.5056135396850392E-06;
             xceref[3] = 0.4584064944105704E-06;
             xceref[4] = 0.5333617144459171E-06;
	  }else if(request.ndid==4){
             xcrref[0] = 0.1746837879467509E-04;
             xcrref[1] = 0.1191975215752574E-04;
             xcrref[2] = 0.1266014391456199E-04;
             xcrref[3] = 0.1144903502624621E-04;
             xcrref[4] = 0.1289427815335821E-04;
 
             xceref[0] = 0.6963518044347606E-06;
             xceref[1] = 0.4723216907465734E-06;
             xceref[2] = 0.5051031881884828E-06;
             xceref[3] = 0.4578630935544920E-06;
             xceref[4] = 0.5317843158727930E-06;
	  }else if(request.ndid==5){
             xcrref[0] = 0.1747262252970938E-04;
             xcrref[1] = 0.1191406741612622E-04;
             xcrref[2] = 0.1264728511460325E-04;
             xcrref[3] = 0.1143541657009609E-04;
             xcrref[4] = 0.1285547868348645E-04;
 
             xceref[0] = 0.6965551273059916E-06;
             xceref[1] = 0.4721010804231396E-06;
             xceref[2] = 0.5045973487811069E-06;
             xceref[3] = 0.4573247339124242E-06;
             xceref[4] = 0.5302133232448112E-06;
	  }else if(request.ndid==6){
             xcrref[0] = 0.1747685882701609E-04;
             xcrref[1] = 0.1190847842890798E-04;
             xcrref[2] = 0.1263454154958609E-04;
             xcrref[3] = 0.1142192549392219E-04;
             xcrref[4] = 0.1281684597929448E-04;
 
             xceref[0] = 0.6967580108191875E-06;
             xceref[1] = 0.4718842894206763E-06;
             xceref[2] = 0.5040959861974017E-06;
             xceref[3] = 0.4567913647034227E-06;
             xceref[4] = 0.5286487472472487E-06;
	  }else if(request.ndid==7){
             xcrref[0] = 0.1748108763020269E-04;
             xcrref[1] = 0.1190298391404811E-04;
             xcrref[2] = 0.1262191227311304E-04;
             xcrref[3] = 0.1140856049497699E-04;
             xcrref[4] = 0.1277838012802418E-04;
 
             xceref[0] = 0.6969604530568643E-06;
             xceref[1] = 0.4716712665253523E-06;
             xceref[2] = 0.5035990650298079E-06;
             xceref[3] = 0.4562629356041116E-06;
             xceref[4] = 0.5270905965546141E-06;
	  }else if(request.ndid==8){
             xcrref[0] = 0.1748530888245429E-04;
             xcrref[1] = 0.1189758260757201E-04;
             xcrref[2] = 0.1260939633998909E-04;
             xcrref[3] = 0.1139532028062394E-04;
             xcrref[4] = 0.1274008117128722E-04;
 
             xceref[0] = 0.6971624520995203E-06;
             xceref[1] = 0.4714619612488244E-06;
             xceref[2] = 0.5031065498690451E-06;
             xceref[3] = 0.4557393966743608E-06;
             xceref[4] = 0.5255388779393104E-06;
	  }	
	}else if(request.dfg.name.indexOf("ED.A")>=0){
          clss = 'A';
          dtref = .0015;
	  if(request.ndid==0){
             xcrref[0] = 0.1662388872593516E-03;
             xcrref[1] = 0.1120336284839445E-03;
             xcrref[2] = 0.1155494269554219E-03;
             xcrref[3] = 0.1144591065562746E-03;
             xcrref[4] = 0.1164120101231624E-03;
 
             xceref[0] = 0.6779616046334763E-06;
             xceref[1] = 0.4612029937003940E-06;
             xceref[2] = 0.4944689998311943E-06;
             xceref[3] = 0.4485901415827698E-06;
             xceref[4] = 0.5246454435912191E-06;
	  }else if(request.ndid==1){
             xcrref[0] = 0.1643876342654285E-03;
             xcrref[1] = 0.1107586934055227E-03;
             xcrref[2] = 0.1139204792057105E-03;
             xcrref[3] = 0.1126757070066634E-03;
             xcrref[4] = 0.1116344021422492E-03;
 
             xceref[0] = 0.6781623235228421E-06;
             xceref[1] = 0.4609720478128549E-06;
             xceref[2] = 0.4939569076487757E-06;
             xceref[3] = 0.4480440569950695E-06;
             xceref[4] = 0.5230883044555838E-06;
	  }else if(request.ndid==2){
             xcrref[0] = 0.1625918474544209E-03;
             xcrref[1] = 0.1095197898803472E-03;
             xcrref[2] = 0.1123587585230388E-03;
             xcrref[3] = 0.1109676929314279E-03;
             xcrref[4] = 0.1071044513952408E-03;
 
             xceref[0] = 0.6783626225632541E-06;
             xceref[1] = 0.4607450429708946E-06;
             xceref[2] = 0.4934493367655407E-06;
             xceref[3] = 0.4475030647124090E-06;
             xceref[4] = 0.5215374339158348E-06;
	  }else if(request.ndid==3){
             xcrref[0] = 0.1608490693651636E-03;
             xcrref[1] = 0.1083153581826390E-03;
             xcrref[2] = 0.1108604672866795E-03;
             xcrref[3] = 0.1093308651249898E-03;
             xcrref[4] = 0.1028086486843187E-03;
 
             xceref[0] = 0.6785624992312174E-06;
             xceref[1] = 0.4605219257276873E-06;
             xceref[2] = 0.4929462520671564E-06;
             xceref[3] = 0.4469671125838440E-06;
             xceref[4] = 0.5199928417464324E-06;
	  }else if(request.ndid==4){
             xcrref[0] = 0.1591569849646335E-03;
             xcrref[1] = 0.1071439334321504E-03;
             xcrref[2] = 0.1094220422343074E-03;
             xcrref[3] = 0.1077612746219508E-03;
             xcrref[4] = 0.9873431034114809E-04;
 
             xceref[0] = 0.6787619510589296E-06;
             xceref[1] = 0.4603026434516362E-06;
             xceref[2] = 0.4924476183695788E-06;
             xceref[3] = 0.4464361489565087E-06;
             xceref[4] = 0.5184545362024516E-06;
	  }else if(request.ndid==5){
             xcrref[0] = 0.1575134155289734E-03;
             xcrref[1] = 0.1060041366777818E-03;
             xcrref[2] = 0.1080401410684524E-03;
             xcrref[3] = 0.1062552073503085E-03;
             xcrref[4] = 0.9486952583324645E-04;
 
             xceref[0] = 0.6789609756324433E-06;
             xceref[1] = 0.4600871443050831E-06;
             xceref[2] = 0.4919534004407399E-06;
             xceref[3] = 0.4459101226618531E-06;
             xceref[4] = 0.5169225240005335E-06;
	  }else if(request.ndid==6){
             xcrref[0] = 0.1559163057636150E-03;
             xcrref[1] = 0.1048946693312379E-03;
             xcrref[2] = 0.1067116281816794E-03;
             xcrref[3] = 0.1048091731788827E-03;
             xcrref[4] = 0.9120310319412870E-04;
 
             xceref[0] = 0.6791595706091380E-06;
             xceref[1] = 0.4598753772286119E-06;
             xceref[2] = 0.4914635630149040E-06;
             xceref[3] = 0.4453889830283163E-06;
             xceref[4] = 0.5153968104229367E-06;
	  }else if(request.ndid==7){
             xcrref[0] = 0.1543637155168927E-03;
             xcrref[1] = 0.1038143074591630E-03;
             xcrref[2] = 0.1054335631917361E-03;
             xcrref[3] = 0.1034198910287058E-03;
             xcrref[4] = 0.8772452336450357E-04;
 
             xceref[0] = 0.6793577337061182E-06;
             xceref[1] = 0.4596672919413453E-06;
             xceref[2] = 0.4909780708165150E-06;
             xceref[3] = 0.4448726798508144E-06;
             xceref[4] = 0.5138773993154394E-06;
	  }else if(request.ndid==8){
             xcrref[0] = 0.1528538118743083E-03;
             xcrref[1] = 0.1027618953918407E-03;
             xcrref[2] = 0.1042031878741166E-03;
             xcrref[3] = 0.1020842773640444E-03;
             xcrref[4] = 0.8442389321125213E-04;
 
             xceref[0] = 0.6795554626864593E-06;
             xceref[1] = 0.4594628389051135E-06;
             xceref[2] = 0.4904968885790024E-06;
             xceref[3] = 0.4443611634404593E-06;
             xceref[4] = 0.5123642931624080E-06;
	  }	
	}else if(request.dfg.name.indexOf("ED.B")>=0){
	}else if(request.dfg.name.indexOf("ED.C")>=0){	
	}
      }
//---------------------------------------------------------------------
//    reference data for 12X12X12 grids after 100 time steps, with DT = 1.50d-02
//---------------------------------------------------------------------
    }else if (  grid_points[0]  == 12 
	      &&grid_points[1]  == 12
	      &&grid_points[2]  == 12
	      &&no_time_steps   == 100 
    ){
      
      clss = 'S';
      dtref = .015;

//---------------------------------------------------------------------
//    Reference values of RMS-norms of residual.
//---------------------------------------------------------------------
      xcrref[0] = 2.7470315451339479E-2;
      xcrref[1] = 1.0360746705285417E-2;
      xcrref[2] = 1.6235745065095532E-2;
      xcrref[3] = 1.5840557224455615E-2;
      xcrref[4] = 3.4849040609362460E-2;

//---------------------------------------------------------------------
//    Reference values of RMS-norms of solution error.
//---------------------------------------------------------------------
      xceref[0] = 2.7289258557377227E-5;
      xceref[1] = 1.0364446640837285E-5;
      xceref[2] = 1.6154798287166471E-5;
      xceref[3] = 1.5750704994480102E-5;
      xceref[4] = 3.4177666183390531E-5;


//---------------------------------------------------------------------
//    reference data for 36X36X36 grids after 400 time steps, with DT = 
//---------------------------------------------------------------------
    }else if ( (grid_points[0] == 36) && 
	       (grid_points[1] == 36) &&
	       (grid_points[2] == 36) &&
	       (no_time_steps == 400) ) {

      clss = 'W';
      dtref = .0015;

//---------------------------------------------------------------------
//    Reference values of RMS-norms of residual.
//---------------------------------------------------------------------
      xcrref[0] = 0.1893253733584E-2;
      xcrref[1] = 0.1717075447775E-3;
      xcrref[2] = 0.2778153350936E-3;
      xcrref[3] = 0.2887475409984E-3;
      xcrref[4] = 0.3143611161242E-2;

//---------------------------------------------------------------------
//    Reference values of RMS-norms of solution error.
//---------------------------------------------------------------------
      xceref[0] = 0.7542088599534E-4;
      xceref[1] = 0.6512852253086E-5;
      xceref[2] = 0.1049092285688E-4;
      xceref[3] = 0.1128838671535E-4;
      xceref[4] = 0.1212845639773E-3;

//---------------------------------------------------------------------
//    reference data for 64X64X64 grids after 400 time steps, with DT = 1.5d-03
//---------------------------------------------------------------------
    }else if ( (grid_points[0] == 64) && 
	       (grid_points[1] == 64) &&
	       (grid_points[2] == 64) &&
	       (no_time_steps  == 400) ) {

      clss = 'A';
      dtref = .0015;

//---------------------------------------------------------------------
//    Reference values of RMS-norms of residual.
//---------------------------------------------------------------------
      xcrref[0] = 2.4799822399300195;
      xcrref[1] = 1.1276337964368832;
      xcrref[2] = 1.5028977888770491;
      xcrref[3] = 1.4217816211695179;
      xcrref[4] = 2.1292113035138280;

//---------------------------------------------------------------------
//    Reference values of RMS-norms of solution error.
//---------------------------------------------------------------------
      xceref[0] = 1.0900140297820550E-4;
      xceref[1] = 3.7343951769282091E-5;
      xceref[2] = 5.0092785406541633E-5;
      xceref[3] = 4.7671093939528255E-5;
      xceref[4] = 1.3621613399213001E-4;

//---------------------------------------------------------------------
//    reference data for 102X102X102 grids after 400 time steps,
//    with DT = 1.0d-03
//---------------------------------------------------------------------
    }else if ( (grid_points[0] == 102) && 
	       (grid_points[1] == 102) &&
	       (grid_points[2] == 102) &&
	       (no_time_steps  == 400) ) {

      clss = 'B';
      dtref = .001;

//---------------------------------------------------------------------
//    Reference values of RMS-norms of residual.
//---------------------------------------------------------------------
      xcrref[0] = 0.6903293579998E+02;
      xcrref[1] = 0.3095134488084E+02;
      xcrref[2] = 0.4103336647017E+02;
      xcrref[3] = 0.3864769009604E+02;
      xcrref[4] = 0.5643482272596E+02;

//---------------------------------------------------------------------
//    Reference values of RMS-norms of solution error.
//---------------------------------------------------------------------
      xceref[0] = 0.9810006190188E-02;
      xceref[1] = 0.1022827905670E-02;
      xceref[2] = 0.1720597911692E-02;
      xceref[3] = 0.1694479428231E-02;
      xceref[4] = 0.1847456263981E-01;

//---------------------------------------------------------------------
//    reference data for 162X162X162 grids after 400 time steps,
//    with DT = 0.67d-03
//---------------------------------------------------------------------
    }else if ( (grid_points[0] == 162) && 
	       (grid_points[1] == 162) &&
	       (grid_points[2] == 162) &&
	       (no_time_steps  == 400) ) {

      clss = 'C';
      dtref = .00067;

//---------------------------------------------------------------------
//    Reference values of RMS-norms of residual.
//---------------------------------------------------------------------
      xcrref[0] = 0.5881691581829E+03;
      xcrref[1] = 0.2454417603569E+03;
      xcrref[2] = 0.3293829191851E+03;
      xcrref[3] = 0.3081924971891E+03;
      xcrref[4] = 0.4597223799176E+03;

//---------------------------------------------------------------------
//    Reference values of RMS-norms of solution error.
//---------------------------------------------------------------------
      xceref[0] = 0.2598120500183;
      xceref[1] = 0.2590888922315E-01;
      xceref[2] = 0.5132886416320E-01;
      xceref[3] = 0.4806073419454E-01;
      xceref[4] = 0.5483377491301;   
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
//---------------------------------------------------------------------
//   tolerance level
//---------------------------------------------------------------------
    double epsilon = 1.0E-8;
//---------------------------------------------------------------------
//    Output the comparison of computed results to known cases.
//---------------------------------------------------------------------
    if (clss != 'U') {
      System.out.println(" Verification being performed for class " + clss);
      System.out.println(" Accuracy setting for epsilon = " + epsilon);
      if (Math.abs(dt-dtref) <= epsilon) {  
        if(verified==-1) verified=1;
      }else{
	verified = 0;
	clss = 'U';
	System.out.println("DT does not match the reference value of " + dtref );
      }
      System.out.println(" Comparison of RMS-norms of residual");
    }else{ 
//      System.out.println(" Unknown CLASS");
      System.out.println(" RMS-norms of residual");
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
    SparseA spa=new SparseA(grid_points[0]*grid_points[1]*grid_points[2]);
    for(int k=0;k<=grid_points[2]-1;k++){
      for(int j=0;j<=grid_points[1]-1;j++){
        for(int i=0;i<=grid_points[0]-1;i++){
	  int offset=i*isize1+j*jsize1+k*ksize1;
          double rho_inv = 1.0/u[offset];
          spa.val[offset]=Math.sqrt(
	                    (u[4+offset] 
	                    -0.5*( u[1+offset]*u[1+offset]
	                          +u[2+offset]*u[2+offset]
			          +u[3+offset]*u[3+offset])
	                        *rho_inv
			    )
	                    *rho_inv*0.56);
          spa.idx[offset]=-1;
	}
      }
    }
    return spa;
  }
  public void initU(){
    if(request!=null){
      initializeNGB();
      if(request.spa!=null&&request.spa.len>0){
        if(request.spa.len!=5*grid_points[0]*grid_points[1]
	                     *grid_points[2]){
          System.err.println( "!!SP.initU: incorrect length "
	                     +"of sparse array="
	                     +request.spa.len);
          initializeNGB();
        }else{
   	  int spaidx=0;
   	  for(int k=0;k<=grid_points[2]-1;k++){
   	    for(int j=0;j<=grid_points[1]-1;j++){
   	      for(int i=0;i<=grid_points[0]-1;i++){
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
    }else  initialize();
  }
  public void initializeNGB(){
    int i, j, k, m, ix, iy, iz;
    double  xi, eta, zeta, Pface[]=new double[5*3*2], Pxi, Peta, 
      Pzeta, temp[] = new double[5];
    
//---------------------------------------------------------------------
//  Later (in compute_rhs) we compute 1/u for every element. A few of 
//  the corner elements are not used, but it convenient (and faster) 
//  to compute the whole thing with a simple loop. Make sure those 
//  values are nonzero by initializing the whole thing here. 
//---------------------------------------------------------------------
      for(k=0;k<grid_points[2];k++){
         for(j=0;j<grid_points[1];j++){
            for(i=0;i<grid_points[0];i++){
               u[0+i*isize1+j*jsize1+k*ksize1] = 1.0;
               u[1+i*isize1+j*jsize1+k*ksize1] = 0.0;
               u[2+i*isize1+j*jsize1+k*ksize1] = 0.0;
               u[3+i*isize1+j*jsize1+k*ksize1] = 0.0;
               u[4+i*isize1+j*jsize1+k*ksize1] = 1.0;
            }
         }
      }

//---------------------------------------------------------------------
// first store the exact values on the boundaries        
//---------------------------------------------------------------------

//---------------------------------------------------------------------
// west face                                                  
//---------------------------------------------------------------------
       xi = 0.0;
       i  = 0;
       for(k=0;k<grid_points[2];k++){
          zeta = k * dnzm1;
          for(j=0;j<grid_points[1];j++){
             eta = j * dnym1;
              exact_solution(xi, eta, zeta, temp,0);
             for(m=0;m<=4;m++){
                u[m+i*isize1+j*jsize1+k*ksize1] = temp[m];
             }
          }
       }

//---------------------------------------------------------------------
// east face                                                      
//---------------------------------------------------------------------

       xi = 1.0;
       i  = grid_points[0]-1;
       for(k=0;k<grid_points[2];k++){
          zeta = k * dnzm1;
          for(j=0;j<grid_points[1];j++){
             eta = j * dnym1;
              exact_solution(xi, eta, zeta, temp, 0);
             for(m=0;m<=4;m++){
                u[m+i*isize1+j*jsize1+k*ksize1] = temp[m];
             }
          }
       }

//---------------------------------------------------------------------
// south face                                                 
//---------------------------------------------------------------------

       eta = 0.0;
       j   = 0;
       for(k=0;k<grid_points[2];k++){
          zeta = k * dnzm1;
          for(i=0;i<grid_points[0];i++){
             xi = i * dnxm1;
              exact_solution(xi, eta, zeta, temp, 0);
             for(m=0;m<=4;m++){
                u[m+i*isize1+j*jsize1+k*ksize1] = temp[m];
             }
          }
       }

//---------------------------------------------------------------------
// north face                                    
//---------------------------------------------------------------------

       eta = 1.0;
       j   = grid_points[1]-1;
       for(k=0;k<grid_points[2];k++){
          zeta = k * dnzm1;
          for(i=0;i<grid_points[0];i++){
             xi = i * dnxm1;
             exact_solution(xi, eta, zeta, temp,0);
             for(m=0;m<=4;m++){
               u[m+i*isize1+j*jsize1+k*ksize1] = temp[m];
             }
          }
       }

//---------------------------------------------------------------------
// bottom face                                       
//---------------------------------------------------------------------

       zeta = 0.0;
       k    = 0;
       for(i=0;i<grid_points[0];i++){
          xi = i *dnxm1;
          for(j=0;j<grid_points[1];j++){
             eta = j * dnym1;
              exact_solution(xi, eta, zeta, temp, 0);
             for(m=0;m<=4;m++){
                u[m+i*isize1+j*jsize1+k*ksize1] = temp[m];
             }
          }
       }

//---------------------------------------------------------------------
// top face     
//---------------------------------------------------------------------

       zeta = 1.0;
       k    = grid_points[2]-1;
       for(i=0;i<grid_points[0];i++){
          xi = i * dnxm1;
          for(j=0;j<grid_points[1];j++){
             eta = j * dnym1;
              exact_solution(xi, eta, zeta, temp, 0);
             for(m=0;m<=4;m++){
                u[m+i*isize1+j*jsize1+k*ksize1] = temp[m];
             }
          }
       }
//---------------------------------------------------------------------
// Then store the "interpolated" values everywhere on the grid    
//---------------------------------------------------------------------
    int nxm1=grid_points[0]-1,
        nym1=grid_points[1]-1,
        nzm1=grid_points[2]-1;
    for(k=1;k<grid_points[2]-1;k++){
       zeta = k * dnzm1;
       for(j=1;j<grid_points[1]-1;j++){
    	  eta = j * dnym1;
    	  for(i=1;i<grid_points[0]-1;i++){
    	     xi = i * dnxm1;
    for(m=0;m<=4;m++){
      u[m+i*isize1+j*jsize1+k*ksize1] =
    	 xi*(eta    *(zeta   *u[m+nxm1*isize1+nym1*jsize1+nzm1*ksize1]+
    		     (1-zeta)*u[m+nxm1*isize1+nym1*jsize1])+
	     (1-eta)*(zeta   *u[m+nxm1*isize1+nzm1*ksize1]+
    		     (1-zeta)*u[m+nxm1*isize1])
    	    )+
     (1-xi)*(eta    *(zeta   *u[m+nym1*jsize1+nzm1*ksize1]+
    		     (1-zeta)*u[m+nym1*jsize1])+
	     (1-eta)*(zeta   *u[m+nzm1*ksize1]+
    		     (1-zeta)*u[m])
    	    );        
    }
    					   }
    	}
    }
  }
  public void setConst(){
    if(  request!=null
       &&request.dfg!=null
       &&request.dfg.name.indexOf("ED")>=0
      ) 
         set_constants(request.ndid);  
    else set_constants(0);
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






