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
NGBROOT= ..

all: Executor.class NGBRun.class \
	NGBResults.class NGBArgs.class

Executor.class:  $(NGBROOT)/tasks/DGraph/ArcHead.class \
		 $(NGBROOT)/tasks/DGraph/DGArc.class \
		 $(NGBROOT)/tasks/DGraph/DGNode.class \
		 $(NGBROOT)/tasks/DGraph/DGraph.class \
		 $(NGBROOT)/tasks/DGraph/BMResults.class \
		 $(NGBROOT)/tasks/DGraph/BMRequest.class
	$(JAVAC) $(JAVAFLAGS) Executor.java

NGBRun.class: Executor.class
	$(JAVAC) $(JAVAFLAGS) NGBRun.java

NGBResults.class: $(NGBROOT)/tasks/DGraph/BMResults.class
	$(JAVAC) $(JAVAFLAGS) NGBResults.java
	
NGBArgs.class: $(NGBROOT)/NPB_JAV/BMInOut/BMArgs.class
	$(JAVAC) $(JAVAFLAGS) NGBArgs.java
	
.SUFFIXES : .java.class
.java.class :
	$(JAVAC) $(JAVAFLAGS) $<
	
clean:
		- rm *.class
