#!/bin/bash

########################################################################
#
# This file has been created in the context of Geant4 running on OSG
# Author: Chris Green (FNAL)
# Date: May 2009
#
# We could be invoked via GANGA/DIANE, or manually (in which case MasterOID
# should be piped to STDIN and the config script will be generated
# internally).
#
# Globus-run invocation:
#
# globus-job-run -stdin -s <OID-file> <host:port/jobmanager> \
#                       -s <OSG_worker_wrapper.sh>
#
# Usage with Ganga/DIANE:
# Enable this file as a wrapper template for LCGSubmitter.py by 
# adding the following lines to your run file:
# 
# def initialize_submitter(s):
#    print 'using custom wrapper for globus/osg'
#    s.wrapper = file('OSG_worker_wrapper.sh').read()
#
#
# The original files in the context of Geant4 activity were located here:
# /afs/cern.ch/sw/arda/install/DIANE/Geant4/G4Prod/run/runfiles
#
########################################################################

[[ -z "$1" ]]  && use_stdin=1 # Globus, not GANGA

# Configure HTTP proxy if appropriate
if [[ -z "$http_proxy" ]] && \
  [[ -n "$OSG_SQUID_LOCATION" ]]; then # OSG
    case $OSG_SQUID_LOCATION in
        UNAVAILABLE)
        :
        ;;

        *://*:[0-9]*)
          http_proxy=$OSG_SQUID_LOCATION
        ;;

        *://*)
          http_proxy="$OSG_SQUID_LOCATION:3128"
        ;; 

        *:[0-9]*)
          http_proxy="http://$OSG_SQUID_LOCATION"
        ;;

        *)
          http_proxy="http://$OSG_SQUID_LOCATION:3128"
            
    esac
fi

# Get ourselves into a unique directory (OSG only)
idir=`/bin/pwd`
if [[ -n "$OSG_WN_TMP" ]]; then
 ! [[ -w "$OSG_WN_TMP" ]] && unset OSG_WN_TMP
  ndir=`mktemp -d "${OSG_WN_TMP:-${TMPDIR:-/tmp}}/diane_worker.XXXXXXXXXX"`
  if [[ -n "$ndir" ]]; then
    # Automatic cleanup
    trap "[[ -d \"$idir\" ]] && [[ -n \"$ndir\" ]] && { cd \"$idir\"; /bin/rm -rf \"$ndir\"; }" EXIT
    (( ${use_stdin:-0 )) || cp $* "$ndir" # Globus, not GANGA
    cd "$ndir"
  else
    echo "ERROR: unable to obtain temporary directory!" 1>&2
    exit 1
  fi
fi

if (( ${use_stdin:-0} )); then # Globus, not GANGA
  # Write to OIDFile from STDIN
  OIDFile=`mktemp "MasterOID.XXXXXXXXXX" </dev/null`
  cat > "$OIDFile"

  # Write simple worker config file.
  cfgFile=`mktemp "worker_config.XXXXXXXXXX"`
  cat >"$cfgFile" <<EOF
clientTransportRule  =  localhost               unix,tcp
clientTransportRule  =  *                       tcp
endPoint = giop:unix:
EOF
fi

# Obtain DIANE install package
wget http://cern.ch/diane/packages/diane-install

# This makes sure that a reasonable value of $diane_version regardless
# of whether this script is run standalone or interpolated by (eg)
# LCGSubmitter.py as a template.
percent=`printf '\045'`
diane_version='%(DIANE_VERSION)s'
[[ "${diane_version:-$percent}" == $percent* ]] && diane_version=2.0-beta16

# Install DIANE worker package
python ./diane-install --prefix="$PWD/diane" $diane_version

# Set up worker environment
# Seems to overwrite PATH in a not-nice way. Cope:
OLD_PATH=$PATH
$("$PWD/diane/install/$diane_version/bin/diane-env")
PATH="$PATH:$OLD_PATH"
export ORBdumpConfiguration=1
export ORBgiopMaxMsgSize=19100200
# Set (hopefully) local work space
export DIANE_USER_WORKSPACE=${PWD}

# Start DIANE
if (( ${use_stdin:-0} )); then
  # Globus
  diane-worker-start --ior-file="$OIDFile" --omniorb-config-file="$cfgFile"
else
  # GANGA
  diane-worker-start $*
fi
