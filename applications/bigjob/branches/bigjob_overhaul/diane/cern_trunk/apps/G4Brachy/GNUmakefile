# $Id: GNUmakefile,v 1.1 2008/10/03 16:39:42 moscicki Exp $
# --------------------------------------------------------------

name := Brachy
G4TARGET := $(name)
G4EXLIB := true

#ifndef G4INSTALL22
#assert "G4INSTALL VARIABLE NOT DEFINED IN THE ENVIRONMENT"
#endif

.PHONY: all

#### NOTE 32 bit compatibility mode
#CXXFLAGS += -m32

ifdef G4DIANE_USE

# do not change this
SIM_MODULE = $(name)Simulation

G4LIB_BUILD_SHARED = 1
all: lib bin
	@echo "Making and linking DIANE libraries..."
	@echo "Generating swig wrappers..."
	@swig -c++ -python -o ${G4TMP}/${SIM_MODULE}_wrap.cpp include/${SIM_MODULE}.i
	@echo "Creating local directory"
	@mkdir -p python
	@echo "from ${SIM_MODULE} import *" > python/__init__.py
	@touch __init__.py
	@mv ${G4TMP}/${SIM_MODULE}.py python
	# -fPIC
	@echo "Compiling wrappers..."
	@${CXX} -I${G4INCLUDE} ${CXXFLAGS} ${CPPFLAGS} -I include -c ${G4TMP}/${SIM_MODULE}_wrap.cpp -o ${G4TMP}/${G4SYSTEM}/${name}/${SIM_MODULE}_wrap.o -I/usr/include/python2.3
	@echo "Building python extension module..."
	${CXX} -shared  ${G4TMP}/${G4SYSTEM}/${name}/${SIM_MODULE}_wrap.o  -L${G4TMP}/${G4SYSTEM}/${name} -l${name} ${LDFLAGS} ${LDLIBS} -o python/_${SIM_MODULE}.so

#FIXME: integrate with global clean
clean-diane: 
	rm -rf __init__.py python 

else
all: lib bin
endif

include $(G4INSTALL)/config/binmake.gmk

#### NOTE 32 bit compatibility mode
#CXXFLAGS += -m32
#
#ifdef G4ANALYSIS_USE  
# CPPFLAGS += `linux32 aida-config --include`
# LDFLAGS  += `linux32 aida-config --lib`
# LOADLIBS += `linux32 aida-config --lib` 
#endif

ifdef G4ANALYSIS_USE
 CPPFLAGS += `aida-config --include`
 LDFLAGS  += `aida-config --lib`
 LOADLIBS += `aida-config --lib`
endif



