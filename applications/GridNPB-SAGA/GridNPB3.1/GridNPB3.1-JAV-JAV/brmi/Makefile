#!-------------------------------------------------------------------------!
#!									   !
#!		   N A S   G R I D   B E N C H M A R K S  		   !
#!									   !
#!			  J A V A	 V E R S I O N          	   !
#!                                                                         !
#!		               M A K E F I L E				   !
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
#!    Send bug reports to	       ngb@nas.nasa.gov		   !
#!									   !
#!	    E-mail:  ngb@nas.nasa.gov					   !
#!	    Fax:     (650) 604-3957					   !
#!									   !
#!-------------------------------------------------------------------------!
#! GridNPB3.0 Java version						   !
#!	    M. Frumkin  						   !
#!-------------------------------------------------------------------------!
JAVAC = $(JROOT)/bin/javac
JBIN = $(JROOT)/bin

all: BenchClient.class Benchmark.class\
	BenchUnion.class BenchServer.class \
	BenchServer_Stub.class BenchServer_Skel.class

BenchServer.class:
	$(JAVAC) $(JAVAFLAGS) BenchServer.java
BenchClient.class:	
	$(JAVAC) $(JAVAFLAGS) BenchClient.java
Benchmark.class:
	$(JAVAC) $(JAVAFLAGS) Benchmark.java
BenchServer_Skel.class:
BenchServer_Stub.class:
	$(JBIN)/rmic $(JAVAFLAGS) -d $(CLASSPATH) brmi.BenchServer	
BenchUnion.class:
	$(JAVAC) $(JAVAFLAGS) BenchUnion.java
	
.SUFFIXES : .java.class
.java.class :
	$(JAVAC) $(JAVAFLAGS) $<
clean:
	- rm *.class ndin.* ndout.* core \
	             ED* HC* VP* MB* \
	             BT.* SP.* LU.* MG.* FT.*
