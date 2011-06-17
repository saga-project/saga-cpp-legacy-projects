
# this makefile supportes the CSA deployment procedure on the TeraGrid.  It
# requires the environment variable CSA_LOCATION to be set (will complain
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
# make helper to remove white space from vars
#
empty   = 
space   = $(empty) $(empty)
nospace = $(subst $(space),$(empty),$1)

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
CC_VERSION   = $(shell (make cpp_version ; ./cpp_version) | tail -n 1)
CC_NAME      = $(notdir $(CC))-$(CC_VERSION)
MAKE_VERSION = $(shell make --version | head -1)


########################################################################
# 
# report setup
#
$(shell echo "make info: csa      location: $(CSA_LOCATION)" 1>&2 )
$(shell echo "make info: saga     version : $(SAGA_VERSION)" 1>&2 )
$(shell echo "make info: compiler version : $(CC_NAME)"      1>&2 )
$(shell echo "make info: make     version : $(MAKE_VERSION)" 1>&2 )


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
all::                      saga-core saga-adaptors saga-bindings saga-clients readme

.PHONY: externals
externals::                boost postgresql sqlite3
boost::                    python

.PHONY: saga-core
saga-core::                externals

.PHONY: saga-adaptors
saga-adaptors::            saga-adaptor-x509
saga-adaptors::            saga-adaptor-globus 
saga-adaptors::            saga-adaptor-ssh 

# some adaptors are only build for trunk
ifeq "$(SAGA_VERSION)" "trunk"
  saga-adaptors::          saga-adaptor-bes 
  saga-adaptors::          saga-adaptor-aws 
  saga-adaptors::          saga-adaptor-drmaa
ifneq "$(CSA_HOST)" "abe"
ifneq "$(CSA_HOST)" "queenbee"
  saga-adaptors::          saga-adaptor-torque
endif
endif
  saga-adaptors::          saga-adaptor-pbspro
ifneq "$(CSA_HOST)" "pople"
  saga-adaptors::          saga-adaptor-condor
endif
endif


.PHONY: saga-bindings
saga-bindings::            saga-core
saga-bindings::            saga-bindings-python

.PHONY: saga-bindings-python
saga-bindings-python::     python

.PHONY: saga-clients saga-client-mandelbrot saga-client-bigjob
saga-clients::             saga-client-mandelbrot saga-client-bigjob
saga-client-mandelbrot::   saga-core
saga-client-bigjob::       saga-core saga-bindings-python            


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
# python
PYTHON_VERSION  = 2.7.1
PYTHON_SVERSION = 2.7
PYTHON_LOCATION = $(CSA_LOCATION)/external/python/$(PYTHON_VERSION)/gcc-$(CC_VERSION)/
PYTHON_CHECK    = $(PYTHON_LOCATION)/bin/python
PYTHON_SRC      = http://python.org/ftp/python/$(PYTHON_VERSION)/Python-$(PYTHON_VERSION).tar.bz2
SAGA_ENV_VARS  += PYTHON_LOCATION=$(PYTHON_LOCATION)
SAGA_ENV_LIBS  += $(PYTHON_LOCATION)/lib/

.PHONY: python
python:: base $(PYTHON_CHECK)
	@echo "python                    ok"

$(PYTHON_CHECK):
	@echo "python                    installing"
	@cd $(SRCDIR) ; $(WGET) $(PYTHON_SRC)
	@cd $(SRCDIR) ; tar jxvf Python-$(PYTHON_VERSION).tar.bz2
	@cd $(SRCDIR)/Python-$(PYTHON_VERSION)/ ; ./configure --prefix=$(PYTHON_LOCATION) --enable-shared --enable-unicode=ucs4 && make && make install


########################################################################
# boost
BOOST_LOCATION  = $(CSA_LOCATION)/external/boost/1.44.0/$(CC_NAME)/
BOOST_CHECK     = $(BOOST_LOCATION)/include/boost/version.hpp
BOOST_SRC       = http://garr.dl.sourceforge.net/project/boost/boost/1.44.0/boost_1_44_0.tar.bz2
SAGA_ENV_VARS  += BOOST_LOCATION=$(BOOST_LOCATION)
SAGA_ENV_LIBS  += $(BOOST_LOCATION)/lib/

SAGA_ENV_LDPATH = LD_LIBRARY_PATH=$(call nospace, $(foreach d,$(SAGA_ENV_LIBS),:$(d))):$$(LD_LIBRARY_PATH)
SAGA_ENV_PATH  += PATH=$(PYTHON_LOCATION)/bin/:$(PATH)

.PHONY: boost
boost:: base $(BOOST_CHECK)
	@echo "boost                     ok"

$(BOOST_CHECK):
	@echo "boost                     installing"
	@cd $(SRCDIR) ; $(WGET) $(BOOST_SRC)
	@cd $(SRCDIR) ; tar jxvf boost_1_44_0.tar.bz2
	@cd $(SRCDIR)/boost_1_44_0 ; $(ENV) $(SAGA_ENV_PATH) $(SAGA_ENV_LDPATH) $(SAGA_ENV_VARS) ./bootstrap.sh \
                               --with-libraries=test,thread,system,iostreams,filesystem,program_options,python,regex,serialization \
															 --with-python=$(PYTHON_LOCATION)/bin/python \
															 --with-python-root=$(PYTHON_LOCATION) \
															 --with-python-version=2.7 \
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
SAGA_ENV_VARS      += POSTGRESQL_LOCATION=$(POSTGRESQL_LOCATION)
SAGA_ENV_LIBS      += :$(POSTGRESQL_LOCATION)/lib/

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
SAGA_ENV_VARS   += SQLITE3_LOCATION=$(SQLITE3_LOCATION)
SAGA_ENV_LIBS   += :$(SQLITE3_LOCATION)/lib/

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
SAGA_LOCATION   = $(CSA_LOCATION)/saga/$(SAGA_VERSION)/$(CC_NAME)/
SAGA_CHECK      = $(SAGA_LOCATION)/include/saga/saga.hpp
SAGA_ENV_VARS  += SAGA_LOCATION=$(SAGA_LOCATION)
SAGA_ENV_LIBS  += :$(SAGA_LOCATION)/lib/

SAGA_ENV_LDPATH = LD_LIBRARY_PATH=$(call nospace, $(foreach d,$(SAGA_ENV_LIBS),:$(d))):$$(LD_LIBRARY_PATH)

ifeq "$(CSA_HOST)" "abe"
  # boost assumes that all linux hosts know this define.  Well, abe does not.
  SAGA_ENV_VARS += CPPFLAGS="-D__NR_eventfd=323"
endif

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
	@cd $(SRCDIR) ; test -d saga-core-$(SAGA_VERSION) || $(SVNCO) $(SAGA_SRC)/
	@cd $(SRCDIR)/saga-core-$(SAGA_VERSION)/ ; $(ENV) $(SAGA_ENV_PATH) $(SAGA_ENV_LDPATH) $(SAGA_ENV_VARS) \
		              ./configure --prefix=$(SAGA_LOCATION) && make clean && make && make install


########################################################################
#
# python bindings
#
SAGA_PYTHON_CHECK    = $(SAGA_LOCATION)/share/saga/config/python.m4 
SAGA_PYTHON_SRC      = https://svn.cct.lsu.edu/repos/saga/bindings/python/tags/releases/saga-bindings-python-0.9.0/
SAGA_PYTHON_MODPATH  = $(SAGA_LOCATION)lib/python$(PYTHON_VERSION)/site-packages/

SAGA_ENV_LDPATH      = LD_LIBRARY_PATH=$(call nospace, $(foreach d,$(SAGA_ENV_LIBS),:$(d))):$$(LD_LIBRARY_PATH)

.PHONY: saga-bindings-python
saga-bindings-python:: base python $(SAGA_PYTHON_CHECK)
	@echo "saga-bindings-python      ok"

$(SAGA_PYTHON_CHECK):
	@echo "saga-bindings-python      installing"
	@cd $(SRCDIR) ; test -d saga-bindings-python-0.9.0 && $(SVNUP) saga-bindings-python-0.9.0 ; true
	@cd $(SRCDIR) ; test -d saga-bindings-python-0.9.0 || $(SVNCO) $(SAGA_PYTHON_SRC)
	@cd $(SRCDIR)/saga-bindings-python-0.9.0/ ; $(ENV) $(SAGA_ENV_PATH) $(SAGA_ENV_LDPATH) $(SAGA_ENV_VARS) ./configure && make && make install


########################################################################
#
# saga-adaptors
#

SAGA_ENV = $(SAGA_ENV_PATH) $(SAGA_ENV_LDPATH) $(SAGA_ENV_VARS)

########################################################################
# saga-adaptor-x509
SA_X509_CHECK    = $(SAGA_LOCATION)/share/saga/saga_adaptor_x509_context.ini
SA_X509_SRC      = https://svn.cct.lsu.edu/repos/saga-adaptors/x509/trunk

.PHONY: saga-adaptor-x509
saga-adaptor-x509:: base $(SA_X509_CHECK)
	@echo "saga-adaptor-x509         ok"

$(SA_X509_CHECK):
	@echo "saga-adaptor-x509         installing"
	@cd $(SRCDIR) ; test -d saga-adaptor-x509-trunk && $(SVNUP)                saga-adaptor-x509-trunk ; true
	@cd $(SRCDIR) ; test -d saga-adaptor-x509-trunk || $(SVNCO) $(SA_X509_SRC) saga-adaptor-x509-trunk
	@cd $(SRCDIR)/saga-adaptor-x509-trunk/ ; $(ENV) $(SAGA_ENV) ./configure  && make clean && make && make install


########################################################################
# saga-adaptor-globus
SA_GLOBUS_CHECK    = $(SAGA_LOCATION)/share/saga/saga_adaptor_globus_gram_job.ini
SA_GLOBUS_SRC      = https://svn.cct.lsu.edu/repos/saga-adaptors/globus/trunk

.PHONY: saga-adaptor-globus
saga-adaptor-globus:: base $(SA_GLOBUS_CHECK)
	@echo "saga-adaptor-globus       ok"

$(SA_GLOBUS_CHECK):
	@echo "saga-adaptor-globus       installing"
	@cd $(SRCDIR) ; test -d saga-adaptor-globus-trunk && $(SVNUP)                  saga-adaptor-globus-trunk ; true
	@cd $(SRCDIR) ; test -d saga-adaptor-globus-trunk || $(SVNCO) $(SA_GLOBUS_SRC) saga-adaptor-globus-trunk
	@cd $(SRCDIR)/saga-adaptor-globus-trunk/ ; $(ENV) $(SAGA_ENV) ./configure  && make clean && make && make install


########################################################################
# saga-adaptor-ssh
SA_SSH_CHECK    = $(SAGA_LOCATION)/share/saga/saga_adaptor_ssh_job.ini
SA_SSH_SRC      = https://svn.cct.lsu.edu/repos/saga-adaptors/ssh/trunk

.PHONY: saga-adaptor-ssh
saga-adaptor-ssh:: base $(SA_SSH_CHECK)
	@echo "saga-adaptor-ssh          ok"

$(SA_SSH_CHECK):
	@echo "saga-adaptor-ssh          installing"
	@cd $(SRCDIR) ; test -d saga-adaptor-ssh-trunk && $(SVNUP)               saga-adaptor-ssh-trunk ; true
	@cd $(SRCDIR) ; test -d saga-adaptor-ssh-trunk || $(SVNCO) $(SA_SSH_SRC) saga-adaptor-ssh-trunk
	@cd $(SRCDIR)/saga-adaptor-ssh-trunk/ ; $(ENV) $(SAGA_ENV) ./configure  && make clean && make && make install


########################################################################
# saga-adaptor-aws
SA_AWS_CHECK    = $(SAGA_LOCATION)/share/saga/saga_adaptor_aws_context.ini
SA_AWS_SRC      = https://svn.cct.lsu.edu/repos/saga-adaptors/aws/trunk

.PHONY: saga-adaptor-aws
saga-adaptor-aws:: base $(SA_AWS_CHECK)
	@echo "saga-adaptor-aws          ok"

$(SA_AWS_CHECK):
	@echo "saga-adaptor-aws          installing"
	@cd $(SRCDIR) ; test -d saga-adaptor-aws-trunk && $(SVNUP)               saga-adaptor-aws-trunk ; true
	@cd $(SRCDIR) ; test -d saga-adaptor-aws-trunk || $(SVNCO) $(SA_AWS_SRC) saga-adaptor-aws-trunk
	@cd $(SRCDIR)/saga-adaptor-aws-trunk/ ; $(ENV) $(SAGA_ENV) ./configure  && make clean && make && make install


########################################################################
# saga-adaptor-drmaa
SA_DRMAA_CHECK  = $(SAGA_LOCATION)/share/saga/saga_adaptor_ogf_drmaa_job.ini
SA_DRMAA_SRC      = https://svn.cct.lsu.edu/repos/saga-adaptors/drmaa/trunk

.PHONY: saga-adaptor-drmaa
saga-adaptor-drmaa:: base $(SA_DRMAA_CHECK)
	@echo "saga-adaptor-drmaa        ok"

$(SA_DRMAA_CHECK):
	@echo "saga-adaptor-drmaa        installing"
	@cd $(SRCDIR) ; test -d saga-adaptor-drmaa-trunk && $(SVNUP)                 saga-adaptor-drmaa-trunk ; true
	@cd $(SRCDIR) ; test -d saga-adaptor-drmaa-trunk || $(SVNCO) $(SA_DRMAA_SRC) saga-adaptor-drmaa-trunk
	@cd $(SRCDIR)/saga-adaptor-drmaa-trunk/ ; $(ENV) $(SAGA_ENV) ./configure  && make clean && make && make install


########################################################################
# saga-adaptor-condor
SA_CONDOR_CHECK  = $(SAGA_LOCATION)/share/saga/saga_adaptor_condor_job.ini
SA_CONDOR_SRC      = https://svn.cct.lsu.edu/repos/saga-adaptors/condor/trunk

.PHONY: saga-adaptor-condor
saga-adaptor-condor:: base $(SA_CONDOR_CHECK)
	@echo "saga-adaptor-condor       ok"

$(SA_CONDOR_CHECK):
	@echo "saga-adaptor-condor       installing"
	@cd $(SRCDIR) ; test -d saga-adaptor-condor-trunk && $(SVNUP)                  saga-adaptor-condor-trunk ; true
	@cd $(SRCDIR) ; test -d saga-adaptor-condor-trunk || $(SVNCO) $(SA_CONDOR_SRC) saga-adaptor-condor-trunk
	@cd $(SRCDIR)/saga-adaptor-condor-trunk/ ; $(ENV) $(SAGA_ENV) ./configure  && make clean && make && make install


########################################################################
# saga-adaptor-pbspro
SA_PBSPRO_CHECK  = $(SAGA_LOCATION)/share/saga/saga_adaptor_pbspro_job.ini
SA_PBSPRO_SRC      = https://svn.cct.lsu.edu/repos/saga-adaptors/pbspro/trunk

.PHONY: saga-adaptor-pbspro
saga-adaptor-pbspro:: base $(SA_PBSPRO_CHECK)
	@echo "saga-adaptor-pbspro       ok"

$(SA_PBSPRO_CHECK):
	@echo "saga-adaptor-pbspro       installing"
	@cd $(SRCDIR) ; test -d saga-adaptor-pbspro-trunk && $(SVNUP)                  saga-adaptor-pbspro-trunk ; true
	@cd $(SRCDIR) ; test -d saga-adaptor-pbspro-trunk || $(SVNCO) $(SA_PBSPRO_SRC) saga-adaptor-pbspro-trunk
	@cd $(SRCDIR)/saga-adaptor-pbspro-trunk/ ; $(ENV) $(SAGA_ENV) ./configure  && make clean && make && make install


########################################################################
# saga-adaptor-torque
SA_TORQUE_CHECK  = $(SAGA_LOCATION)/share/saga/saga_adaptor_torque_job.ini
SA_TORQUE_SRC      = https://svn.cct.lsu.edu/repos/saga-adaptors/torque/trunk

.PHONY: saga-adaptor-torque
saga-adaptor-torque:: base $(SA_TORQUE_CHECK)
	@echo "saga-adaptor-torque       ok"

$(SA_TORQUE_CHECK):
	@echo "saga-adaptor-torque       installing"
	@cd $(SRCDIR) ; test -d saga-adaptor-torque-trunk && $(SVNUP)                  saga-adaptor-torque-trunk ; true
	@cd $(SRCDIR) ; test -d saga-adaptor-torque-trunk || $(SVNCO) $(SA_TORQUE_SRC) saga-adaptor-torque-trunk
	@cd $(SRCDIR)/saga-adaptor-torque-trunk/ ; $(ENV) $(SAGA_ENV) ./configure  && make clean && make && make install


########################################################################
# saga-adaptor-bes
SA_BES_CHECK    = $(SAGA_LOCATION)/share/saga/saga_adaptor_ogf_hpcbp_job.ini
SA_BES_SRC      = https://svn.cct.lsu.edu/repos/saga-adaptors/ogf/trunk

.PHONY: saga-adaptor-bes
saga-adaptor-bes:: base $(SA_BES_CHECK)
	@echo "saga-adaptor-bes          ok"

$(SA_BES_CHECK):
	@echo "saga-adaptor-bes          installing"
	@cd $(SRCDIR) ; test -d saga-adaptor-ogf-trunk && $(SVNUP)               saga-adaptor-ogf-trunk ; true
	@cd $(SRCDIR) ; test -d saga-adaptor-ogf-trunk || $(SVNCO) $(SA_BES_SRC) saga-adaptor-ogf-trunk
	@cd $(SRCDIR)/saga-adaptor-ogf-trunk/ ; $(ENV) $(SAGA_ENV) ./configure  && make clean && make && make install


########################################################################
#
# mandelbrot client
#
SC_MANDELBROT_CHECK    = $(SAGA_LOCATION)/bin/mandelbrot_client
SC_MANDELBROT_SRC      = https://svn.cct.lsu.edu/repos/saga-projects/applications/mandelbrot

.PHONY: saga-client-mandelbrot
saga-client-mandelbrot:: base $(SC_MANDELBROT_CHECK)
	@echo "saga-client-mandelbrot    ok"

$(SC_MANDELBROT_CHECK):
	@echo "saga-client-mandelbrot    installing"
	@cd $(SRCDIR) ; test -d saga-client-mandelbrot && $(SVNUP)                      saga-client-mandelbrot ; true
	@cd $(SRCDIR) ; test -d saga-client-mandelbrot || $(SVNCO) $(SC_MANDELBROT_SRC) saga-client-mandelbrot
	@cd $(SRCDIR)/saga-client-mandelbrot/ ; $(ENV) $(SAGA_ENV) ./configure --disable-master && make clean && make && make install


########################################################################
#
# bigjob client
#
SC_BIGJOB_CHECK    = $(SAGA_PYTHON_MODPATH)/bigjob
SC_BIGJOB_SRC      = https://svn.cct.lsu.edu/repos/saga-projects/applications/bigjob/trunk/

.PHONY: saga-client-bigjob
saga-client-bigjob:: base $(SC_BIGJOB_CHECK)
	@echo "saga-client-bigjob        ok"

$(SC_BIGJOB_CHECK):
	@echo "saga-client-bigjob        installing"
	@cd $(SRCDIR) ; test -d saga-client-bigjob && $(SVNUP)                  saga-client-bigjob ; true
	@cd $(SRCDIR) ; test -d saga-client-bigjob || $(SVNCO) $(SC_BIGJOB_SRC) saga-client-bigjob
	@rm -rf $(SC_BIGJOB_CHECK)
	@cp -R $(SRCDIR)/saga-client-bigjob/ $(SC_BIGJOB_CHECK)


########################################################################
#
# readme
#
# create some basic documentation about the installed software packages
#
CSA_README_SRC   = $(CSA_LOCATION)/tg-csa/README.stub
CSA_README_CHECK = $(CSA_LOCATION)/README.saga-$(SAGA_VERSION).$(CC_NAME).$(HOSTNAME)

.PHONY: readme
readme:: saga-core $(CSA_README_CHECK)
	@echo "README                    ok"

$(CSA_README_CHECK): $(CSA_README_SRC)
	@echo "README                    creating"
	@cp -fv $(CSA_README_SRC) $(CSA_README_CHECK)
	@$(SED) -i -e 's|###SAGA_VERSION###|$(SAGA_VERSION)|ig;'              $(CSA_README_CHECK)
	@$(SED) -i -e 's|###SAGA_LOCATION###|$(SAGA_LOCATION)|ig;'            $(CSA_README_CHECK)
	@$(SED) -i -e 's|###SAGA_LDLIBPATH###|$(SAGA_ENV_LDPATH)|ig;'         $(CSA_README_CHECK)
	@$(SED) -i -e 's|###SAGA_PYTHON###|$(PYTHON_LOCATION)/bin/python|ig;' $(CSA_README_CHECK)
	@$(SED) -i -e 's|###SAGA_PYLOCATION###|$(PYTHON_LOCATION)|ig;'        $(CSA_README_CHECK)
	@$(SED) -i -e 's|###SAGA_PYPATH###|$(SAGA_PYTHON_MODPATH)|ig;'        $(CSA_README_CHECK)
	@$(SED) -i -e 's|###SAGA_PYVERSION###|$(PYTHON_VERSION)|ig;'          $(CSA_README_CHECK)
	@$(SED) -i -e 's|###SAGA_PYSVERSION###|$(PYTHON_SVERSION)|ig;'        $(CSA_README_CHECK)
	@$(SED) -i -e 's|###CSA_LOCATION###|$(CSA_LOCATION)|ig;'              $(CSA_README_CHECK)
	@echo "fixing permissions"
	@$(CHMOD) -R a+rX $(SAGA_LOCATION)
	@$(CHMOD) -R a+rX $(EXTDIR)
	-@$(CHMOD)   a+rX $(CSA_LOCATION)
	

