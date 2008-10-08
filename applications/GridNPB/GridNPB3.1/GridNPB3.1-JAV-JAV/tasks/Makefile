#!-------------------------------------------------------------------------!
#!									   !
#!		   N A S   G R I D   B E N C H M A R K S  		   !
#!									   !
#!			 J A V A	 V E R S I O N  		   !
#!                                                                         !
#!		           M A K E F I L E				   !
#!									   !
#!-------------------------------------------------------------------------!
#!									   !
#!    Permission to use, copy, distribute and modify this software	   !
#!    for any purpose with or without fee is hereby granted.		   !
#!    We request, however, that all derived work reference the  	   !
#!    NAS Grid Benchmarks 3.0 or GridNPB3.0. This software is provided     !
#!    "as is" without expressed or implied warranty.			   !
#!									   !
#!    Information on GridNPB3.0, including the concept of		   !
#!    the NAS Grid Benchmarks, the specifications, source code, 	   !
#!    results and information on how to submit new results,		   !
#!    is available at:  						   !
#!									   !
#!	   http://www.nas.nasa.gov/Software/NPB 			   !
#!									   !
#!    Send comments or suggestions to  ngb@nas.nasa.gov 		   !
#!    Send bug reports to	       ngb@nas.nasa.gov		           !
#!									   !
#!	    E-mail:  ngb@nas.nasa.gov					   !
#!	    Fax:     (650) 604-3957					   !
#!									   !
#!-------------------------------------------------------------------------!
#! GridNPB3.0 Java version						   !
#!	    M. Frumkin  						   !
#!-------------------------------------------------------------------------!
JAVAC = $(JROOT)/bin/javac
NPBROOT= ../../NPB_JAV

all: BTTask.class CGTask.class FTTask.class ISTask.class \
	LUTask.class MGTask.class SPTask.class
		
bttask: BTTask.class \
	$(NPBROOT)/BT.class \
	$(NPBROOT)/BTThreads/BTBase.class \
	$(NPBROOT)/BTThreads/RHSCompute.class  \
	$(NPBROOT)/BTThreads/YSolver.class \
	$(NPBROOT)/BTThreads/RHSAdder.class  \
	$(NPBROOT)/BTThreads/XSolver.class \
	$(NPBROOT)/BTThreads/ZSolver.class \
	/DGraph/BMArgs.class \
	/DGraph/BMResults.class \
	$(NPBROOT)/Timer.class
	
cgtask: CGTask.class \
	$(NPBROOT)/CG.class \
	$(NPBROOT)/CGThreads/CGBase.class \
	$(NPBROOT)/CGThreads/CGWorker.class \
	DGraph/BMArgs.class \
	DGraph/BMResults.class \
	$(NPBROOT)/Random.class  \
	$(NPBROOT)/Timer.class
	
fttask: FTTask.class \
	$(NPBROOT)/FT.class \
	$(NPBROOT)/FTThreads/FTBase.class \
	$(NPBROOT)/FTThreads/FFTThread.class \
	$(NPBROOT)/FTThreads/EvolveThread.class \
	DGraph/BMArgs.class \
	DGraph/BMResults.class \
	$(NPBROOT)/Random.class  \
	$(NPBROOT)/Timer.class

istask: ISTask.class \
	$(NPBROOT)/IS.class \
	$(NPBROOT)/ISThreads/ISBase.class \
	$(NPBROOT)/ISThreads/RankThread.class \
	DGraph/BMArgs.class \
	DGraph/BMResults.class \
	$(NPBROOT)/Random.class  \
	$(NPBROOT)/Timer.class
		
lutask: LUTask.class \
	$(NPBROOT)/LU.class \
	$(NPBROOT)/LUThreads/LUBase.class \
	$(NPBROOT)/LUThreads/Adder.class \
	$(NPBROOT)/LUThreads/RHSCompute.class \
	$(NPBROOT)/LUThreads/UpperJac.class \
	$(NPBROOT)/LUThreads/LowerJac.class \
	$(NPBROOT)/LUThreads/Scale.class \
	$(NPBROOT)/DGraph/BMArgs.class \
	$(NPBROOT)/DGraph/BMResults.class \
	$(NPBROOT)/Timer.class
	
mgtask: MGTask.class \
	$(NPBROOT)/MG.class \
	$(NPBROOT)/MGThreads/MGBase.class MGThreads/Interp.class \
	$(NPBROOT)/MGThreads/Psinv.class \
	$(NPBROOT)/MGThreads/Resid.class MGThreads/Rprj.class \
	DGraph/BMArgs.class \
	DGraph/BMResults.class \
	$(NPBROOT)/Random.class \
	$(NPBROOT)/Timer.class

sptask: SPTask.class \
	$(NPBROOT)/SP.class \
	$(NPBROOT)/SPThreads/SPBase.class \
	$(NPBROOT)/SPThreads/RHSAdder.class \
	$(NPBROOT)/SPThreads/RHSCompute.class \
	$(NPBROOT)/SPThreads/TXInverse.class \
	$(NPBROOT)/SPThreads/XSolver.class \
	$(NPBROOT)/SPThreads/YSolver.class \
	$(NPBROOT)/SPThreads/ZSolver.class \
	DGraph/BMArgs.class \
	$(NPBROOT)/DGraph/BMResults.class \
	$(NPBROOT)/Timer.class
	
BTTask.class:
	$(JAVAC) $(JAVAFLAGS) BTTask.java
	
CGTask.class:
	$(JAVAC) $(JAVAFLAGS) CGTask.java
	
FTTask.class:
	$(JAVAC) $(JAVAFLAGS) FTTask.java
	
ISTask.class:
	$(JAVAC) $(JAVAFLAGS) ISTask.java
	
LUTask.class:
	$(JAVAC) $(JAVAFLAGS) LUTask.java

MGTask.class:
	$(JAVAC) $(JAVAFLAGS) MGTask.java
	
SPTask.class:
	$(JAVAC) $(JAVAFLAGS) SPTask.java
	
FT.class:
	$(JAVAC) $(JAVAFLAGS) FT.java

LU.class:
	$(JAVAC) $(JAVAFLAGS) LU.java

MG.class:
	$(JAVAC) $(JAVAFLAGS) MG.java

SP.class:
	$(JAVAC) $(JAVAFLAGS) SP.java

.SUFFIXES : .java.class
.java.class :
	$(JAVAC) $(JAVAFLAGS) $<
	
clean:
		- rm *.class
		- rm DGraph/*.class
