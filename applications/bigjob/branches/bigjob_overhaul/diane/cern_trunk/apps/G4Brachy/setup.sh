

#################################################
#
# GEANT4 APPLICATION - ENVIRONMENT SETUP
#
# Description:
#   Shell script for setting up the
#   environment to run and compile Geant4 
#   applications on EGEE worker nodes
#
# Creation date: Oct 2008
#
# Modifications:
#   26-11-08     
#
#
#################################################

echo "### Geant4 Application Environment:"


#################################################
# 
# A. BASIC DEFINITIONS
#
#################################################

# Geant4 top level directory
export G4INSTALLATION_DIR=${VO_GEANT4_SW_DIR}/dirInstallations


# Geant4 version
export G4GRIDVERSION="9.1.p02"


# Subdirectories of top level directory containing AIDA, CLHEP,
# CERNLIB and Geant4
export G4AIDA_DIR=${G4INSTALLATION_DIR}/diriAIDA
export G4CLHEP_DIR=${G4INSTALLATION_DIR}/dirCLHEP
export G4CERNLIB_DIR=${G4INSTALLATION_DIR}/dirCERNLIB
export G4_DIR=${G4INSTALLATION_DIR}/dirGeant4-${G4GRIDVERSION}
export G4DATA_DIR=$G4_DIR/data


#########################################

# Checks if directory exist
if [ ! -d $G4INSTALLATION_DIR ] ; then
    echo "Error: Geant4 grid installation not found"
    echo "Error: Setup incomplete"
    return
fi

if [ ! -d $G4AIDA_DIR ] ; then
    echo "Error: AIDA installation not found"
    echo "Error: Setup incomplete"
    return
fi

if [ ! -d $G4CLHEP_DIR ] ; then
    echo "Error: CLHEP installation not found"
    echo "Error: Setup incomplete"
    return
fi

if [ ! -d $G4CERNLIB_DIR ] ; then
    echo "Error: CERNLIB installation not found"
    echo "Error: Setup incomplete"
    return
fi

if [ ! -d $G4_DIR ] ; then
    echo "Error: Geant4 installation not found"
    echo "Error: Setup incomplete"
    return
fi

if [ ! -d $G4DATA_DIR ] ; then
    echo "Error: Physics data directory not found"
    echo "Error: Setup incomplete"
    return
fi


#################################################
# 
# B. GEANT4 ENVIRONMENT
#
#################################################


######################################
# Clear existing environment
#
######################################

unset  CLHEP_BASE_DIR
unset  CLHEP_INCLUDE_DIR
unset  CLHEP_LIB
unset  CLHEP_LIB_DIR

unset  G4ANALYSIS_USE
unset  G4DEBUG
unset  G4INCLUDE
unset  G4INSTALL

unset  G4LEDATA
unset  G4LEVELGAMMADATA
unset  G4NEUTRONHPDATA
unset  G4RADIOACTIVEDATA
unset  G4ABLADATA

unset  G4LIB
unset  G4LIB_BUILD_G3TOG4
unset  G4LIB_BUILD_SHARED
unset  G4LIB_BUILD_STATIC
unset  G4LIB_BUILD_ZLIB
unset  G4LIB_BUILD_GDML
unset  G4LIB_USE_G3TOG4
unset  G4LIB_USE_GRANULAR
unset  G4LIB_USE_ZLIB

unset  G4SYSTEM

unset  G4UI_BUILD_WIN32_SESSION
unset  G4UI_BUILD_XAW_SESSION
unset  G4UI_BUILD_XM_SESSION
unset  G4UI_USE_TCSH
unset  G4UI_USE_WIN32
unset  G4UI_USE_XAW
unset  G4UI_USE_XM
unset  G4UI_USE_QT

unset  G4VIS_BUILD_DAWN_DRIVER
unset  G4VIS_BUILD_OIWIN32_DRIVER
unset  G4VIS_BUILD_OIX_DRIVER
unset  G4VIS_BUILD_OPENGLWIN32_DRIVER
unset  G4VIS_BUILD_OPENGLXM_DRIVER
unset  G4VIS_BUILD_OPENGLX_DRIVER
unset  G4VIS_BUILD_RAYTRACERX_DRIVER
unset  G4VIS_BUILD_VRML_DRIVER
unset  G4VIS_BUILD_OPENGLQT_DRIVER

unset  G4VIS_USE_DAWN
unset  G4VIS_USE_OIWIN32
unset  G4VIS_USE_OIX
unset  G4VIS_USE_OPENGLWIN32
unset  G4VIS_USE_OPENGLX
unset  G4VIS_USE_OPENGLXM
unset  G4VIS_USE_RAYTRACERX
unset  G4VIS_USE_VRML
unset  G4VIS_USE_OPENGLQT


######################################
# Libraries
#
######################################
#
# *** g4shared
#
export G4LIB_BUILD_SHARED=1
echo "   G4LIB_BUILD_SHARED=$G4LIB_BUILD_SHARED"

export G4LIB_BUILD_STATIC=1
echo "   G4LIB_BUILD_STATIC=$G4LIB_BUILD_STATIC"

#
# *** g4granular
#
export G4LIB_USE_GRANULAR=1
echo "   G4LIB_USE_GRANULAR=$G4LIB_USE_GRANULAR"


######################################
# Installation and working area
#
######################################
#
# *** g4system.U *** 
#
export G4SYSTEM="Linux-g++"
echo "   G4SYSTEM=$G4SYSTEM"

#
# *** g4dirs.U *** 
#
export G4INSTALL=${G4_DIR}
echo "   G4INSTALL=$G4INSTALL"

if [ ! -d $G4INSTALL ] ; then
    echo "Error: Geant4 install directory not found"
    echo "Error: Setup incomplete"
    return
fi

export G4LIB=${G4INSTALL}/lib
echo "   G4LIB=$G4LIB"

if [ ${G4LIB_BUILD_SHARED} ] ; then
   if [ ${LD_LIBRARY_PATH} ] ;  then
       export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${G4LIB}/${G4SYSTEM}
   else
       export LD_LIBRARY_PATH=${G4LIB}/${G4SYSTEM}
   fi
fi

export G4WORKDIR=${HOME}/geant4
echo "   G4WORKDIR=$G4WORKDIR"

export G4TMP=${G4WORKDIR}/tmp
echo "   G4TMP=$G4TMP"

export G4BIN=${G4WORKDIR}/bin
echo "   G4BIN=$G4BIN"

export PATH=${PATH}:${G4WORKDIR}/bin/${G4SYSTEM}


######################################
# Data
#
######################################

export G4LEVELGAMMADATA=${G4DATA_DIR}/PhotonEvaporation
echo "   G4LEVELGAMMADATA=$G4LEVELGAMMADATA"

if [ ! -d $G4LEVELGAMMADATA ] ; then
    echo "Warning: Photon evaporation data not found"
fi

export G4RADIOACTIVEDATA=${G4DATA_DIR}/RadioactiveDecay
echo "   G4RADIOACTIVEDATA=$G4RADIOACTIVEDATA"

if [ ! -d $G4RADIOACTIVEDATA ] ; then
    echo "Warning: Decay data not found"
fi

export G4LEDATA=${G4DATA_DIR}/G4EMLOW
echo "   G4LEDATA=$G4LEDATA"

if [ ! -d $G4LEDATA ] ; then
    echo "Warning: LE data not found"
fi

export G4NEUTRONHPDATA=${G4DATA_DIR}/G4NDL
echo "   G4NEUTRONHPDATA=$G4NEUTRONHPDATA"

if [ ! -d $G4NEUTRONHPDATA ] ; then
    echo "Warning: Neutron data not found"
fi

export G4ABLADATA=${G4DATA_DIR}/G4ABLA
echo "   G4ABLADATA=$G4ABLADATA"

if [ ! -d $G4ABLADATA ] ; then
    echo "Warning: ABLA data not found"
fi

#export G4ELASTICDATA ""
#echo "   G4ELASTICDATA=$G4ELASTICDATA"


######################################
# Analysis
#
######################################
#
# *** g4analysis
#
export G4ANALYSIS_USE=1
echo "   G4ANALYSIS_USE=$G4ANALYSIS_USE"


######################################
# Visualization
#
######################################
#
# *** g4vis
#
export G4VIS_NONE=1
echo "   G4VIS_NONE=$G4VIS_NONE"


######################################
# UI
#
######################################
#
# *** g4ui
#
export G4UI_USE_TCSH=1
echo "   G4UI_USE_TCSH=$G4UI_USE_TCSH"


#################################################
# 
# C. CLHEP ENVIRONMENT 
#
#################################################

export CLHEP_BASE_DIR=${G4CLHEP_DIR}
echo "   CLHEP_BASE_DIR=$CLHEP_BASE_DIR"

if [ ! -d $CLHEP_BASE_DIR ] ; then
    echo "Error: CLHEP not found"
    echo "Error: Setup incomplete"
    return
fi

export CLHEP_INCLUDE_DIR=${CLHEP_BASE_DIR}/include
echo "   CLHEP_INCLUDE_DIR=$CLHEP_INCLUDE_DIR"

export CLHEP_LIB_DIR=${CLHEP_BASE_DIR}/lib
echo "   CLHEP_LIB_DIR=$CLHEP_LIB_DIR"

export CLHEP_LIB="CLHEP"
echo "   CLHEP_LIB=$CLHEP_LIB"

export PATH=${CLHEP_BASE_DIR}/bin:${PATH}

if [ ${G4LIB_BUILD_SHARED} ] ; then
   if [ ${LD_LIBRARY_PATH} ] ;  then
       export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${CLHEP_BASE_DIR}/lib
   else
       export LD_LIBRARY_PATH=${CLHEP_BASE_DIR}/lib
   fi
fi


#################################################
# 
# D. CERNLIB ENVIRONMENT 
#
#################################################

export CERNLIB_DIR=${G4CERNLIB_DIR}


#################################################
# 
# E. AIDA ENVIRONMENT 
#
#################################################

export PATH=${G4AIDA_DIR}/bin:${PATH}

eval `aida-config --run sh`

