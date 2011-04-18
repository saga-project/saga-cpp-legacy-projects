
# this makefile supportes the CSA deployment procedure on the TeraGrid.  It
# requires the envirinment variable CSA_LOCATION to be set (will complain
# otherwise), and needs 'wget', 'svn co', and the usual basic SAGA compilation
# requirements (compiler, linker etc).  Boost, sqlite3 and postgresql are
# installed in external/.  We also expect the SAGA version to be set as
# CSA_SAGA_VERSION, which should be available as release tag in svn.  Otherwise,
# we are going to install trunk

ifndef CSA_LOCATION
 $(error CSA_LOCATION not set - should point to the CSA space allocated on this TG machine)
endif

# get rid of symlinks in CSA_LOCATION
CSA_TMP_LOCATION  = $(shell cd $(CSA_LOCATION) && pwd -P)
CSA_LOCATION     := $(CSA_TMP_LOCATION)

ifndef CSA_SAGA_VERSION
 $(warning CSA_SAGA_VERSION not defined - installing from SVN trunk)
 SAGA_VERSION = trunk
else
  SAGA_VERSION=$(CSA_SAGA_VERSION)
endif

# never ever build parallel
.NOTPARALLEL:

SRCDIR         = $(CSA_LOCATION)/src/
EXTDIR         = $(CSA_LOCATION)/external/

HOSTNAME       = $(shell hostname)

ifdef CSA_HOST
  HOSTNAME     = $(CSA_HOST)
endif

########################################################################
#
# compiler to be used for *everything*
#
CC         = gcc
CXX        = g++


########################################################################
# 
# find out gcc version
#
# gcc --version is stupidly formatted.  Worse, that format is inconsistent over
# different distrubution channels.  Thus this detour to get the version directly
# via gcc compiler macros:
#
CC_VERSION = $(shell (make cpp_version ; ./cpp_version) | tail -n 1)
CC_NAME    = $(notdir $(CC))-$(CC_VERSION)


########################################################################
# 
# report setup
#
$(shell echo "make info: csa      location: $(CSA_LOCATION)" 1>&2 )
$(shell echo "make info: saga     version : $(SAGA_VERSION)" 1>&2 )
$(shell echo "make info: compiler version : $(CC_NAME)"      1>&2 )


########################################################################
#
# basic tools
#
SED        = sed
ENV        = env
WGET       = wget --no-check-certificate
CHMOD      = chmod


SVN        = $(shell which svn 2>/dev/null || echo '$(CSA_LOCATION)/external/subversion/1.6.16/$(CC_NAME)/bin/svn')
SVNCO      = $(SVN) co
SVNUP      = $(SVN) up


########################################################################
#
# target dependencies
#
.PHONY: all
all::           saga-core saga-adaptors saga-bindings saga-clients readme

.PHONY: externals
externals::     boost postgresql sqlite3

.PHONY: saga-core
saga-core::     externals

.PHONY: saga-adaptors
saga-adaptors:: saga-adaptor-x509
saga-adaptors:: saga-adaptor-globus 
saga-adaptors:: saga-adaptor-ssh 

# some adaptors are only build for trunk
ifeq "$(SAGA_VERSION)" "trunk"
  saga-adaptors:: saga-adaptor-bes 
  saga-adaptors:: saga-adaptor-aws 
# saga-adaptors:: saga-adaptor-drmaa
  saga-adaptors:: saga-adaptor-torque
  saga-adaptors:: saga-adaptor-pbspro
  saga-adaptors:: saga-adaptor-condor
endif

.PHONY: saga-bindings
saga-bindings:: saga-bindings-python

.PHONY: saga-clients
saga-clients::  saga-client-mandelbrot


########################################################################
#
# base
#
# create the basic directory infrastructure, documentation, etc
#
.PHONY: base
base:: svn $(CSA_LOCATION)/src/ $(CSA_LOCATION)/external/
	@echo "basic setup               ok" 

$(CSA_LOCATION)/src/:
	@mkdir $@

$(CSA_LOCATION)/external/:
	@mkdir $@


########################################################################
#
# externals
#

########################################################################
# boost
BOOST_LOCATION = $(CSA_LOCATION)/external/boost/1.44.0/$(CC_NAME)/
BOOST_CHECK    = $(BOOST_LOCATION)/include/boost/version.hpp
BOOST_SRC      = http://garr.dl.sourceforge.net/project/boost/boost/1.44.0/boost_1_44_0.tar.bz2

.PHONY: boost
boost:: base $(BOOST_CHECK)
	@echo "boost                     ok"

$(BOOST_CHECK):
	@echo "boost                     installing"
	@cd $(SRCDIR) ; $(WGET) $(BOOST_SRC)
	@cd $(SRCDIR) ; tar jxvf boost_1_44_0.tar.bz2
	@cd $(SRCDIR)/boost_1_44_0 ; ./bootstrap.sh \
                               --with-libraries=test,thread,system,iostreams,filesystem,program_options,python,regex,serialization \
                               --prefix=$(BOOST_LOCATION) && ./bjam && ./bjam install


########################################################################
#
# svn
#
# alas, some systems come without svn client, so we install it locally.  
# The way to fetch svn sources is, unbeleavably, to use svn checkout.  
# Brrr...  So, we rather fetch a prepacked version from cyder.
#
.PHONY: svn
svn: $(SVN)

SVN_SRC  = http://cyder.cct.lsu.edu/saga-interop/mandelbrot/csa/repos/subversion-1.6.16.tgz
SVNLOC   = $(CSA_LOCATION)/external/subversion/1.6.16/$(CC_NAME)/

$(SVN):
	@echo "svn                       installing"
	cd $(CSA_LOCATION)/src/                   && $(WGET) $(SVN_SRC)
	cd $(CSA_LOCATION)/src/                   && tar zxvf subversion-1.6.16.tgz
	cd $(CSA_LOCATION)/src/subversion-1.6.16/ && cd apr/         && ./configure --prefix=$(SVNLOC)
	cd $(CSA_LOCATION)/src/subversion-1.6.16/ && cd apr/         && make clean && make && make install
	cd $(CSA_LOCATION)/src/subversion-1.6.16/ && cd apr-util/    && ./configure --prefix=$(SVNLOC) --with-apr=../apr
	cd $(CSA_LOCATION)/src/subversion-1.6.16/ && cd apr-util/    && make clean && make && make install
	cd $(CSA_LOCATION)/src/subversion-1.6.16/ && cd expat-2.0.1/ && ./configure --prefix=$(SVNLOC) --with-apr=$(SVNLOC) --with-apr-util=$(SVNLOC)
	cd $(CSA_LOCATION)/src/subversion-1.6.16/ && cd expat-2.0.1/ && make clean && make && make install
	cd $(CSA_LOCATION)/src/subversion-1.6.16/ && cd serf-0.7.2/  && ./configure --prefix=$(SVNLOC) --with-apr=$(SVNLOC) --with-apr-util=$(SVNLOC)
	cd $(CSA_LOCATION)/src/subversion-1.6.16/ && cd serf-0.7.2/  && make clean && make && make install
	cd $(CSA_LOCATION)/src/subversion-1.6.16/ && ./configure --with-serf=$(SVNLOC) --with-ssl --prefix=$(SVNLOC) && make && make install


########################################################################
# postgresql
POSTGRESQL_LOCATION = $(CSA_LOCATION)/external/postgresql/9.0.2/$(CC_NAME)/
POSTGRESQL_CHECK    = $(POSTGRESQL_LOCATION)/include/pg_config.h
POSTGRESQL_SRC      = http://ftp9.us.postgresql.org/pub/mirrors/postgresql/source/v9.0.2/postgresql-9.0.2.tar.bz2

.PHONY: postgresql
postgresql:: base $(POSTGRESQL_CHECK)
	@echo "postgresql                ok"

$(POSTGRESQL_CHECK):
	@echo "postgresql                installing"
	@cd $(SRCDIR) ; $(WGET) $(POSTGRESQL_SRC)
	@cd $(SRCDIR) ; tar jxvf postgresql-9.0.2.tar.bz2
	@cd $(SRCDIR)/postgresql-9.0.2/ ; ./configure --prefix=$(POSTGRESQL_LOCATION) --without-readline && make && make install


########################################################################
# sqlite3
SQLITE3_LOCATION = $(CSA_LOCATION)/external/sqlite3/3.6.13/$(CC_NAME)/
SQLITE3_CHECK    = $(SQLITE3_LOCATION)/include/sqlite3.h
SQLITE3_SRC      = http://www.sqlite.org/sqlite-amalgamation-3.6.13.tar.gz

.PHONY: sqlite3
sqlite3:: base $(SQLITE3_CHECK)
	@echo "sqlite3                   ok"

$(SQLITE3_CHECK):
	@echo "sqlite3                   installing"
	@cd $(SRCDIR) ; $(WGET) $(SQLITE3_SRC)
	@cd $(SRCDIR) ; tar zxvf sqlite-amalgamation-3.6.13.tar.gz
	@cd $(SRCDIR)/sqlite-3.6.13/ ; ./configure --prefix=$(SQLITE3_LOCATION) && make && make install


########################################################################
#
# saga-core
#
SAGA_LOCATION = $(CSA_LOCATION)/saga/$(SAGA_VERSION)/$(CC_NAME)/
SAGA_CHECK    = $(SAGA_LOCATION)/include/saga/saga.hpp

SAGA_LDLIBPATH=$(SAGA_LOCATION)/lib:$(BOOST_LOCATION)/lib:$(POSTGRESQL_LOCATION)/lib:$(SQLITE3_LOCATION)/lib:$(LD_LIBRARY_PATH)

SAGA_ENV     += SAGA_LOCATION=$(SAGA_LOCATION)
SAGA_ENV     += BOOST_LOCATION=$(BOOST_LOCATION)
SAGA_ENV     += POSTGRESQL_LOCATION=$(POSTGRESQL_LOCATION)
SAGA_ENV     += SQLITE3_LOCATION=$(SQLITE3_LOCATION)
SAGA_ENV     += LD_LIBRARY_PATH=$(SAGA_LDLIBPATH)
SAGA_ENV     += CPPFLAGS="-D__NR_eventfd=323"

ifeq "$(SAGA_VERSION)" "trunk"
  SAGA_SRC    = https://svn.cct.lsu.edu/repos/saga/core/trunk/  saga-core-trunk
else
  SAGA_SRC    = https://svn.cct.lsu.edu/repos/saga/core/tags/releases/saga-core-$(SAGA_VERSION)
endif

.PHONY: saga-core
saga-core:: base $(SAGA_CHECK)
	@echo "saga-core                 ok"

$(SAGA_CHECK):
	@echo "saga-core                 installing"
	@cd $(SRCDIR) ; test -d saga-core-$(SAGA_VERSION) && $(SVNUP) saga-core-$(SAGA_VERSION) ; true
	@cd $(SRCDIR) ; test -d saga-core-$(SAGA_VERSION) || $(SVNCO) $(SAGA_SRC)
	@cd $(SRCDIR)/saga-core-$(SAGA_VERSION)/ ; $(ENV) $(SAGA_ENV) ./configure --prefix=$(SAGA_LOCATION) && make clean && make && make install

########################################################################
#
# saga-adaptors
#

########################################################################
# saga-adaptor-x509
SA_X509_CHECK    = $(SAGA_LOCATION)/share/saga/saga_adaptor_x509_context.ini
SA_X509_SRC      = https://svn.cct.lsu.edu/repos/saga-adaptors/x509/trunk saga-adaptor-x509-trunk

.PHONY: saga-adaptor-x509
saga-adaptor-x509:: base $(SA_X509_CHECK)
	@echo "saga-adaptor-x509         ok"

$(SA_X509_CHECK):
	@echo "saga-adaptor-x509         installing"
	@cd $(SRCDIR) ; test -d saga-adaptor-x509-trunk && $(SVNUP) saga-adaptor-x509-trunk ; true
	@cd $(SRCDIR) ; test -d saga-adaptor-x509-trunk || $(SVNCO) $(SA_X509_SRC)
	@cd $(SRCDIR)/saga-adaptor-x509-trunk/ ; $(ENV) $(SAGA_ENV) ./configure  && make clean && make && make install


########################################################################
# saga-adaptor-globus
SA_GLOBUS_CHECK    = $(SAGA_LOCATION)/share/saga/saga_adaptor_globus_gram_job.ini
SA_GLOBUS_SRC      = https://svn.cct.lsu.edu/repos/saga-adaptors/globus/trunk saga-adaptor-globus-trunk

.PHONY: saga-adaptor-globus
saga-adaptor-globus:: base $(SA_GLOBUS_CHECK)
	@echo "saga-adaptor-globus       ok"

$(SA_GLOBUS_CHECK):
	@echo "saga-adaptor-globus       installing"
	@cd $(SRCDIR) ; test -d saga-adaptor-globus-trunk && $(SVNUP) saga-adaptor-globus-trunk ; true
	@cd $(SRCDIR) ; test -d saga-adaptor-globus-trunk || $(SVNCO) $(SA_GLOBUS_SRC)
	@cd $(SRCDIR)/saga-adaptor-globus-trunk/ ; $(ENV) $(SAGA_ENV) ./configure  && make clean && make && make install


########################################################################
# saga-adaptor-ssh
SA_SSH_CHECK    = $(SAGA_LOCATION)/share/saga/saga_adaptor_ssh_job.ini
SA_SSH_SRC      = https://svn.cct.lsu.edu/repos/saga-adaptors/ssh/trunk saga-adaptor-ssh-trunk

.PHONY: saga-adaptor-ssh
saga-adaptor-ssh:: base $(SA_SSH_CHECK)
	@echo "saga-adaptor-ssh          ok"

$(SA_SSH_CHECK):
	@echo "saga-adaptor-ssh          installing"
	@cd $(SRCDIR) ; test -d saga-adaptor-ssh-trunk && $(SVNUP) saga-adaptor-ssh-trunk ; true
	@cd $(SRCDIR) ; test -d saga-adaptor-ssh-trunk || $(SVNCO) $(SA_SSH_SRC)
	@cd $(SRCDIR)/saga-adaptor-ssh-trunk/ ; $(ENV) $(SAGA_ENV) ./configure  && make clean && make && make install


########################################################################
# saga-adaptor-aws
SA_AWS_CHECK    = $(SAGA_LOCATION)/share/saga/saga_adaptor_aws_context.ini
SA_AWS_SRC      = https://svn.cct.lsu.edu/repos/saga-adaptors/aws/trunk saga-adaptor-aws-trunk

.PHONY: saga-adaptor-aws
saga-adaptor-aws:: base $(SA_AWS_CHECK)
	@echo "saga-adaptor-aws          ok"

$(SA_AWS_CHECK):
	@echo "saga-adaptor-aws          installing"
	@cd $(SRCDIR) ; test -d saga-adaptor-aws-trunk && $(SVNUP) saga-adaptor-aws-trunk ; true
	@cd $(SRCDIR) ; test -d saga-adaptor-aws-trunk || $(SVNCO) $(SA_AWS_SRC)
	@cd $(SRCDIR)/saga-adaptor-aws-trunk/ ; $(ENV) $(SAGA_ENV) ./configure  && make clean && make && make install


########################################################################
# saga-adaptor-drmaa
SA_DRMAA_CHECK  = $(SAGA_LOCATION)/share/saga/saga_adaptor_ogf_drmaa_job.ini
SA_DRMAA_SRC      = https://svn.cct.lsu.edu/repos/saga-adaptors/drmaa/trunk saga-adaptor-drmaa-trunk

.PHONY: saga-adaptor-drmaa
saga-adaptor-drmaa:: base $(SA_DRMAA_CHECK)
	@echo "saga-adaptor-drmaa        ok"

$(SA_DRMAA_CHECK):
	@echo "saga-adaptor-drmaa        installing"
	@cd $(SRCDIR) ; test -d saga-adaptor-drmaa-trunk && $(SVNUP) saga-adaptor-drmaa-trunk ; true
	@cd $(SRCDIR) ; test -d saga-adaptor-drmaa-trunk || $(SVNCO) $(SA_DRMAA_SRC)
	@cd $(SRCDIR)/saga-adaptor-drmaa-trunk/ ; $(ENV) $(SAGA_ENV) ./configure  && make clean && make && make install


########################################################################
# saga-adaptor-condor
SA_CONDOR_CHECK  = $(SAGA_LOCATION)/share/saga/saga_adaptor_ogf_condor_job.ini
SA_CONDOR_SRC      = https://svn.cct.lsu.edu/repos/saga-adaptors/condor/trunk saga-adaptor-condor-trunk

.PHONY: saga-adaptor-condor
saga-adaptor-condor:: base $(SA_CONDOR_CHECK)
	@echo "saga-adaptor-condor        ok"

$(SA_CONDOR_CHECK):
	@echo "saga-adaptor-condor        installing"
	@cd $(SRCDIR) ; test -d saga-adaptor-condor-trunk && $(SVNUP) saga-adaptor-condor-trunk ; true
	@cd $(SRCDIR) ; test -d saga-adaptor-condor-trunk || $(SVNCO) $(SA_CONDOR_SRC)
	@cd $(SRCDIR)/saga-adaptor-condor-trunk/ ; $(ENV) $(SAGA_ENV) ./configure  && make clean && make && make install


########################################################################
# saga-adaptor-pbspro
SA_PBSPRO_CHECK  = $(SAGA_LOCATION)/share/saga/saga_adaptor_ogf_pbspro_job.ini
SA_PBSPRO_SRC      = https://svn.cct.lsu.edu/repos/saga-adaptors/pbspro/trunk saga-adaptor-pbspro-trunk

.PHONY: saga-adaptor-pbspro
saga-adaptor-pbspro:: base $(SA_PBSPRO_CHECK)
	@echo "saga-adaptor-pbspro        ok"

$(SA_PBSPRO_CHECK):
	@echo "saga-adaptor-pbspro        installing"
	@cd $(SRCDIR) ; test -d saga-adaptor-pbspro-trunk && $(SVNUP) saga-adaptor-pbspro-trunk ; true
	@cd $(SRCDIR) ; test -d saga-adaptor-pbspro-trunk || $(SVNCO) $(SA_PBSPRO_SRC)
	@cd $(SRCDIR)/saga-adaptor-pbspro-trunk/ ; $(ENV) $(SAGA_ENV) ./configure  && make clean && make && make install


########################################################################
# saga-adaptor-torque
SA_TORQUE_CHECK  = $(SAGA_LOCATION)/share/saga/saga_adaptor_ogf_torque.ini
SA_TORQUE_SRC      = https://svn.cct.lsu.edu/repos/saga-adaptors/torque/trunk saga-adaptor-torque-trunk

.PHONY: saga-adaptor-torque
saga-adaptor-torque:: base $(SA_TORQUE_CHECK)
	@echo "saga-adaptor-torque        ok"

$(SA_TORQUE_CHECK):
	@echo "saga-adaptor-torque        installing"
	@cd $(SRCDIR) ; test -d saga-adaptor-torque-trunk && $(SVNUP) saga-adaptor-torque-trunk ; true
	@cd $(SRCDIR) ; test -d saga-adaptor-torque-trunk || $(SVNCO) $(SA_TORQUE_SRC)
	@cd $(SRCDIR)/saga-adaptor-torque-trunk/ ; $(ENV) $(SAGA_ENV) ./configure  && make clean && make && make install


########################################################################
# saga-adaptor-bes
SA_BES_CHECK    = $(SAGA_LOCATION)/share/saga/saga_adaptor_ogf_hpcbp_job.ini
SA_BES_SRC      = https://svn.cct.lsu.edu/repos/saga-adaptors/ogf/trunk saga-adaptor-ogf-trunk

.PHONY: saga-adaptor-bes
saga-adaptor-bes:: base $(SA_BES_CHECK)
	@echo "saga-adaptor-bes          ok"

$(SA_BES_CHECK):
	@echo "saga-adaptor-bes          installing"
	@cd $(SRCDIR) ; test -d saga-adaptor-ogf-trunk && $(SVNUP) saga-adaptor-ogf-trunk ; true
	@cd $(SRCDIR) ; test -d saga-adaptor-ogf-trunk || $(SVNCO) $(SA_BES_SRC)
	@cd $(SRCDIR)/saga-adaptor-ogf-trunk/ ; $(ENV) $(SAGA_ENV) ./configure  && make clean && make && make install


########################################################################
#
# python bindings
#
SAGA_PYTHON_LOCATION = 
SAGA_PYTHON_CHECK    = $(SAGA_LOCATION)/share/saga/config/python.m4 
SAGA_PYTHON_SRC      = https://svn.cct.lsu.edu/repos/saga/bindings/python/tags/releases/saga-bindings-python-0.9.0
SAGA_PYTHON_PSRC     = http://python.org/ftp/python/2.7.1/Python-2.7.1.tar.bz2
SAGA_PYTHON_VERSION  = $(shell python -c "import sys; print(sys.version)"  | head -1 | cut -f 1 -d ' ')
SAGA_PYTHON_MODPATH  = $(SAGA_LOCATION)lib/python$(SAGA_PYTHON_VERSION)/site-packages

.PHONY: saga-bindings-python
saga-bindings-python:: base $(SAGA_PYTHON_CHECK)
	@echo "saga-bindings-python      ok"

$(SAGA_PYTHON_CHECK):
	@echo "saga-bindings-python      installing"
	@cd $(SRCDIR) ; test -d saga-bindings-python-0.9.0 && $(SVNUP) saga-bindings-python-0.9.0 ; true
	@cd $(SRCDIR) ; test -d saga-bindings-python-0.9.0 || $(SVNCO) $(SAGA_PYTHON_SRC)
	@cd $(SRCDIR)/saga-bindings-python-0.9.0/ ; $(ENV) $(SAGA_ENV) ./configure --prefix=$(SAGA_LOCATION) | tee configure.log
	@cd $(SRCDIR)/saga-bindings-python-0.9.0/ ; grep -e 'Python Found .* yes' configure.log || ( \
    export PYTHON_LOCATION=$(CSA_LOCATION)/external/python/2.7.1/$(CC_NAME)/ ; \
    export LD_LIBRARY_PATH=$(PYTHON_LOCATION)/lib:$(LD_LIBRARY_PATH) ; \
    cd $(CSA_LOCATION)/external/ ; \
    $(WGET) $(SAGA_PYTHON_PSRC) ; \
    tar jxvf Python-2.7.1.tar.bz2 ; \
    cd Python-2.7.1 ; \
    ./configure --enable-shared --prefix=$(PYTHON_LOCATION) | tee configure.log && make && make install ; \
    cd $(SRCDIR)/saga-bindings-python-0.9.0/ ; \
	  $(ENV) $(SAGA_ENV) ./configure --prefix=$(SAGA_LOCATION) ; \
  )	
	@cd $(SRCDIR)/saga-bindings-python-0.9.0/ ; make clean && make && make install


########################################################################
#
# mandelbrot client
#
SC_MANDELBROT_CHECK    = $(SAGA_LOCATION)/bin/mandelbrot_client
SC_MANDELBROT_SRC      = https://svn.cct.lsu.edu/repos/saga-projects/applications/mandelbrot saga-client-mandelbrot

.PHONY: saga-client-mandelbrot
saga-client-mandelbrot:: base $(SC_MANDELBROT_CHECK)
	@echo "saga-client-mandelbrot    ok"

$(SC_MANDELBROT_CHECK):
	@echo "saga-client-mandelbrot    installing"
	@cd $(SRCDIR) ; test -d saga-client-mandelbrot && $(SVNUP) saga-client-mandelbrot ; true
	@cd $(SRCDIR) ; test -d saga-client-mandelbrot || $(SVNCO) $(SC_MANDELBROT_SRC)
	@cd $(SRCDIR)/saga-client-mandelbrot/ ; $(ENV) $(SAGA_ENV) ./configure && make clean && make && make install


########################################################################
#
# readme
#
# create some basic documentation about the installed software packages
#
CSA_README_SRC   = https://svn.cct.lsu.edu/repos/saga-projects/deployment/tg-csa/README.stub
CSA_README_CHECK = $(CSA_LOCATION)/README.saga-$(SAGA_VERSION).$(CC_NAME).$(HOSTNAME)

PYTHONPATH=$(SAGA_LOCATION)/$(shell cd $(SRCDIR)/saga-bindings-python-0.9.0/ ; grep -e 'Python Package Path' configure.log | cut -f 2 -d ':' | cut -f 2 -d ' ')

.PHONY: readme
readme:: saga-core $(CSA_README_CHECK)
	@echo "README                    ok"

$(CSA_README_CHECK):
	@echo "README                    creating"
	@$(WGET) $(CSA_README_SRC) -O $(CSA_README_CHECK)
	@$(SED) -i -e 's|###SAGA_VERSION###|$(SAGA_VERSION)|ig;'       $(CSA_README_CHECK)
	@$(SED) -i -e 's|###SAGA_LOCATION###|$(SAGA_LOCATION)|ig;'     $(CSA_README_CHECK)
	@$(SED) -i -e 's|###SAGA_LDLIBPATH###|$(SAGA_LDLIBPATH)|ig;'   $(CSA_README_CHECK)
	@$(SED) -i -e 's|###SAGA_PYPATH###|$(SAGA_PYTHON_MODPATH)|ig;' $(CSA_README_CHECK)
	@$(SED) -i -e 's|###CSA_LOCATION###|$(CSA_LOCATION)|ig;'       $(CSA_README_CHECK)
	@$(CHMOD) -R a+rX $(SAGA_LOCATION)
	@$(CHMOD) -R a+rX $(EXTDIR)
	-@$(CHMOD)   a+rX $(CSA_LOCATION)
	

