
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

ifndef CSA_SAGA_VERSION
 $(warning CSA_SAGA_VERSION not defined - installing from SVN trunk)
 SAGA_VERSION = trunk
else
  SAGA_VERSION=$(CSA_SAGA_VERSION)
endif

SRCDIR = $(CSA_LOCATION)/src/
EXTDIR = $(CSA_LOCATION)/external/

########################################################################
#
# basic tools
#
CC         = gcc
CXX        = g++
CC_VERSION = $(shell gcc --version | head -n 1 | rev | cut -f 1 -d ' ' | rev)
 
ENV        = env
WGET       = wget
SVNCO      = svn co
 

########################################################################
#
# target dependencies
#
all::           saga-core saga-adaptors saga-bindings saga-clients

externals::     boost postgresql sqlite3

saga-core::     externals

saga-adaptors:: saga-adaptors-x509
saga-adaptors:: saga-adaptors-globus 
saga-adaptors:: saga-adaptors-bes 
saga-adaptors:: saga-adaptors-ssh 
saga-adaptors:: saga-adaptors-aws 
# saga-adaptors:: saga-adaptors-drmaa

saga-bindings:: saga-bindings-python

saga-clients::  saga-clients-mandelbrot

########################################################################
#
# base
#
# create the basic directory infrastructure, documentation, etc
#
base::
	@echo "check basic setup" 
	@test -d $(CSA_LOCATION)/src/      || mkdir -p $(CSA_LOCATION)/src/
	@test -d $(CSA_LOCATION)/external/ || mkdir -p $(CSA_LOCATION)/external/


########################################################################
#
# externals
#

########################################################################
# boost
BOOST_LOCATION = $(CSA_LOCATION)/external/boost/1.44.0/gcc-$(CC_VERSION)/
BOOST_CHECK    = $(BOOST_LOCATION)/include/boost/version.hpp
BOOST_SRC      = http://garr.dl.sourceforge.net/project/boost/boost/1.44.0/boost_1_44_0.tar.bz2

boost:: base $(BOOST_CHECK)

$(BOOST_CHECK):
	@echo "installing boost $(BOOST_CHECK)"
	@cd $(SRCDIR) ; $(WGET) $(BOOST_SRC)
	@cd $(SRCDIR) ; tar jxvf boost_1_44_0.tar.bz2
	@cd $(SRCDIR)/boost_1_44_0 ; ./bootstrap.sh \
                               --with-libraries=test,thread,system,iostreams,filesystem,program_options,python,regex,serialization \
                               --prefix=$(BOOST_LOCATION)
	@cd $(SRCDIR)/boost_1_44_0 ; ./bjam
	@cd $(SRCDIR)/boost_1_44_0 ; ./bjam install


########################################################################
# postgresql
POSTGRESQL_LOCATION = $(CSA_LOCATION)/external/postgresql/9.0.2/gcc-$(CC_VERSION)/
POSTGRESQL_CHECK    = $(POSTGRESQL_LOCATION)/include/pg_config.h
POSTGRESQL_SRC      = http://ftp9.us.postgresql.org/pub/mirrors/postgresql/source/v9.0.2/postgresql-9.0.2.tar.bz2

postgresql:: base $(POSTGRESQL_CHECK)

$(POSTGRESQL_CHECK):
	@echo "installing postgresql"
	@cd $(SRCDIR) ; $(WGET) $(POSTGRESQL_SRC)
	@cd $(SRCDIR) ; tar jxvf postgresql-9.0.2.tar.bz2
	@cd $(SRCDIR)/postgresql-9.0.2/ ; ./configure --prefix=$(POSTGRESQL_LOCATION) --without-readline
	@cd $(SRCDIR)/postgresql-9.0.2/ ; make
	@cd $(SRCDIR)/postgresql-9.0.2/ ; make install


########################################################################
# sqlite3
SQLITE3_LOCATION = $(CSA_LOCATION)/external/sqlite3/9.0.2/gcc-$(CC_VERSION)/
SQLITE3_CHECK    = $(SQLITE3_LOCATION)/include/sqlite3.h
SQLITE3_SRC      = http://www.sqlite.org/sqlite-autoconf-3070500.tar.gz

sqlite3:: base $(SQLITE3_CHECK)

$(SQLITE3_CHECK):
	@echo "installing sqlite3"
	@cd $(SRCDIR) ; $(WGET) $(SQLITE3_SRC)
	@cd $(SRCDIR) ; tar zxvf sqlite-autoconf-3070500.tar.gz
	@cd $(SRCDIR)/sqlite-autoconf-3070500/ ; ./configure --prefix=$(SQLITE3_LOCATION)
	@cd $(SRCDIR)/sqlite-autoconf-3070500/ ; make
	@cd $(SRCDIR)/sqlite-autoconf-3070500/ ; make install


########################################################################
#
# saga-core
#
SAGA_LOCATION = $(CSA_LOCATION)/saga/$(SAGA_VERSION)/gcc-$(CC_VERSION)/
SAGA_CHECK    = $(SAGA_LOCATION)/include/saga/saga.hpp

SAGA_ENV     += SAGA_LOCATION=$(SAGA_LOCATION)
SAGA_ENV     += BOOST_LOCATION=$(BOOST_LOCATION)
SAGA_ENV     += POSTGRESQL_LOCATION=$(POSTGRESQL_LOCATION)
SAGA_ENV     += SQLITE3_LOCATION=$(SQLITE3_LOCATION)

ifeq "$(SAGA_VERSION)" "trunk"
  SAGA_SRC    = https://svn.cct.lsu.edu/repos/saga/core/trunk/  saga-core-trunk
else
  SAGA_SRC    = https://svn.cct.lsu.edu/repos/saga/core/tags/releases/saga-core-$(SAGA_VERSION)/
endif

saga-core:: base $(SAGA_CHECK)

$(SAGA_CHECK):
	@echo "installing saga-core"
	@cd $(SRCDIR) ; $(SVNCO) $(SAGA_SRC)
	@cd $(SRCDIR)/saga-core-$(SAGA_VERSION)/ ; $(ENV) $(SAGA_ENV) ./configure --prefix=$(SAGA_LOCATION)
	@cd $(SRCDIR)/saga-core-$(SAGA_VERSION)/ ; make
	@cd $(SRCDIR)/saga-core-$(SAGA_VERSION)/ ; make install


########################################################################
#
# saga-adaptors
#

########################################################################
# saga-adaptors-x509
SA_X509_CHECK    = $(SAGA_LOCATION)/share/saga/saga_adaptor_x509_context.ini)
SA_X509_SRC      = https://svn.cct.lsu.edu/repos/saga-adaptors/x509/trunk saga-adaptors-x509-trunk

saga-adaptors-x509:: base $(SA_X509_CHECK)

$(SA_X509_CHECK):
	@echo "installing saga-adaptors-x509 ($(SA_X509_CHECK))"
	@cd $(SRCDIR) ; $(SVNCO) $(SA_X509_SRC)
	@cd $(SRCDIR)/saga-adaptors-x509-trunk/ ; $(ENV) $(SAGA_ENV) ./configure 
	@cd $(SRCDIR)/saga-adaptors-x509-trunk/ ; make
	@cd $(SRCDIR)/saga-adaptors-x509-trunk/ ; make check
	@cd $(SRCDIR)/saga-adaptors-x509-trunk/ ; make install


########################################################################
# saga-adaptors-globus
SA_GLOBUS_CHECK    = $(SAGA_LOCATION)/share/saga/saga_adaptor_globus_gram_job.ini)
SA_GLOBUS_SRC      = https://svn.cct.lsu.edu/repos/saga-adaptors/globus/trunk saga-adaptors-globus-trunk

saga-adaptors-globus:: base $(SA_GLOBUS_CHECK)

$(SA_GLOBUS_CHECK):
	@echo "installing saga-adaptors-globus"
	@cd $(SRCDIR) ; $(SVNCO) $(SA_GLOBUS_SRC)
	@cd $(SRCDIR)/saga-adaptors-globus-trunk/ ; $(ENV) $(SAGA_ENV) ./configure 
	@cd $(SRCDIR)/saga-adaptors-globus-trunk/ ; make
	@cd $(SRCDIR)/saga-adaptors-globus-trunk/ ; make check
	@cd $(SRCDIR)/saga-adaptors-globus-trunk/ ; make install


########################################################################
# saga-adaptors-ssh
SA_SSH_CHECK    = $(SAGA_LOCATION)/share/saga/saga_adaptor_ssh_job.ini)
SA_SSH_SRC      = https://svn.cct.lsu.edu/repos/saga-adaptors/ssh/trunk saga-adaptors-ssh-trunk

saga-adaptors-ssh:: base $(SA_SSH_CHECK)

$(SA_SSH_CHECK):
	@echo "installing saga-adaptors-ssh"
	@cd $(SRCDIR) ; $(SVNCO) $(SA_SSH_SRC)
	@cd $(SRCDIR)/saga-adaptors-ssh-trunk/ ; $(ENV) $(SAGA_ENV) ./configure 
	@cd $(SRCDIR)/saga-adaptors-ssh-trunk/ ; make
	@cd $(SRCDIR)/saga-adaptors-ssh-trunk/ ; make check
	@cd $(SRCDIR)/saga-adaptors-ssh-trunk/ ; make install


########################################################################
# saga-adaptors-aws
SA_AWS_CHECK    = $(SAGA_LOCATION)/share/saga/saga_adaptor_aws_job.ini)
SA_AWS_SRC      = https://svn.cct.lsu.edu/repos/saga-adaptors/aws/trunk saga-adaptors-aws-trunk

saga-adaptors-aws:: base $(SA_AWS_CHECK)

$(SA_AWS_CHECK):
	@echo "installing saga-adaptors-aws"
	@cd $(SRCDIR) ; $(SVNCO) $(SA_AWS_SRC)
	@cd $(SRCDIR)/saga-adaptors-aws-trunk/ ; $(ENV) $(SAGA_ENV) ./configure 
	@cd $(SRCDIR)/saga-adaptors-aws-trunk/ ; make
	@cd $(SRCDIR)/saga-adaptors-aws-trunk/ ; make check
	@cd $(SRCDIR)/saga-adaptors-aws-trunk/ ; make install


########################################################################
# saga-adaptors-bes
SA_BES_CHECK    = $(SAGA_LOCATION)/share/saga/saga_adaptor_ogf_bes_job.ini)
SA_BES_SRC      = https://svn.cct.lsu.edu/repos/saga-adaptors/ogf/trunk saga-adaptors-ogf-trunk

saga-adaptors-bes:: base $(SA_BES_CHECK)

$(SA_BES_CHECK):
	@echo "installing saga-adaptors-bes"
	@cd $(SRCDIR) ; $(SVNCO) $(SA_BES_SRC)
	@cd $(SRCDIR)/saga-adaptors-ogf-trunk/ ; $(ENV) $(SAGA_ENV) ./configure 
	@cd $(SRCDIR)/saga-adaptors-ogf-trunk/ ; make
	@cd $(SRCDIR)/saga-adaptors-ogf-trunk/ ; make check
	@cd $(SRCDIR)/saga-adaptors-ogf-trunk/ ; make install



########################################################################
#
# python bindings
#
SAGA_PYTHON_LOCATION = 
SAGA_PYTHON_CHECK    = $(SAGA_LOCATION)/share/saga/config/python.m4 
SAGA_PYTHON_SRC      = https://svn.cct.lsu.edu/repos/saga/bindings/python/tags/releases/saga-bindings-python-0.9.0
SAGA_PYTHON_PSRC     = http://python.org/ftp/python/2.7.1/Python-2.7.1.tar.bz2

saga-bindings-python:: base $(SAGA_PYTHON_CHECK)

$(SAGA_PYTHON_CHECK):
	@echo "installing saga-bindings-python"
	@cd $(SRCDIR) ; $(SVNCO) $(SAGA_PYTHON_SRC)
	@cd $(SRCDIR)/saga-bindings-python-0.9.0/ ; $(ENV) $(SAGA_ENV) ./configure --prefix=$(SAGA_LOCATION) | tee configure.log
	@cd $(SRCDIR)/saga-bindings-python-0.9.0/ ; grep -e 'Python Found .* yes' configure.log || ( \
    export PYTHON_LOCATION=$(CSA_LOCATION)/external/python/2.7.1/gcc-$(CC_VERSION)/ \
    export LD_LIBRARY_PATH=$(PYTHON_LOCATION)/lib:$(LD_LIBRARY_PATH) \
    cd $(CSA_LOCATION)/external/ \
    $(WGET) $(SAGA_PYTHON_PSRC) \
    tar jxvf Python-2.7.1.tar.bz2 \
    cd Python-2.7.1 \
    ./configure --enable-shared --prefix=$(PYTHON_LOCATION) \
    make \
    make install \
    cd $(SRCDIR)/saga-bindings-python-0.9.0/ \
	  $(ENV) $(SAGA_ENV) ./configure --prefix=$(SAGA_LOCATION) \
  )
	@cd $(SRCDIR)/saga-bindings-python-0.9.0/ ; make
	@cd $(SRCDIR)/saga-bindings-python-0.9.0/ ; make install

