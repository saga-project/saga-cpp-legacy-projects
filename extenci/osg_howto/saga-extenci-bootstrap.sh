#!/bin/bash

SAGA_ROOT_DIR=$HOME/saga/

POSTGRESQL_VERSION=9.0.4
SAGA_CORE_VERSION=saga-core-1.6-pre1
SAGA_PYTHON_VERSION=saga-bindings-python-1.0-pre1
SAGA_X509_VERSION=saga-adaptors-x509-1.0-pre1
SAGA_GLOBUS_VERSION=saga-adaptors-globus-1.0-pre1
SAGA_CONDOR_VERSION=trunk

mkdir $SAGA_ROOT_DIR
cd $SAGA_ROOT_DIR

mkdir src
cd src

if [ ! -d "$SAGA_ROOT_DIR/src/postgresql-$POSTGRESQL_VERSION" ]; then
  wget http://wwwmaster.postgresql.org/redir/198/h/source/v$POSTGRESQL_VERSION/postgresql-$POSTGRESQL_VERSION.tar.bz2
  tar xjf postgresql-$POSTGRESQL_VERSION.tar.bz2
fi

#cd $SAGA_ROOT_DIR/src/postgresql-$POSTGRESQL_VERSION
#./configure --prefix=$SAGA_ROOT_DIR
#make -j4 && make install

## Download / Update & Install SAGA Core
cd $SAGA_ROOT_DIR/src/
if [ ! -d "$SAGA_ROOT_DIR/src/$SAGA_CORE_VERSION" ]; then
  svn co https://svn.cct.lsu.edu/repos/saga/core/tags/releases/$SAGA_CORE_VERSION
else
  cd $SAGA_ROOT_DIR/src/$SAGA_CORE_VERSION
  svn update
fi

cd $SAGA_ROOT_DIR/src/$SAGA_CORE_VERSION
./configure --prefix=$SAGA_ROOT_DIR --with-postgresql=$SAGA_ROOT_DIR
make -j4 && make install

## Download / Update & Install SAGA PYTHON
cd $SAGA_ROOT_DIR/src/
if [ ! -d "$SAGA_ROOT_DIR/src/$SAGA_PYTHON_VERSION" ]; then
  svn co https://svn.cct.lsu.edu/repos/saga/bindings/python/tags/releases/$SAGA_PYTHON_VERSION
else
  cd $SAGA_ROOT_DIR/src/$SAGA_PYTHON_VERSION
  svn update
fi

cd $SAGA_ROOT_DIR/src/$SAGA_PYTHON_VERSION
SAGA_LOCATION=$SAGA_ROOT_DIR ./configure 
make -j4 && make install


## Download / Update & Install SAGA X509
##
cd $SAGA_ROOT_DIR/src/
if [ ! -d "$SAGA_ROOT_DIR/src/$SAGA_X509_VERSION" ]; then
  svn co https://svn.cct.lsu.edu/repos/saga-adaptors/x509/tags/releases/$SAGA_X509_VERSION
else
  cd $SAGA_ROOT_DIR/src/$SAGA_X509_VERSION
  svn update
fi

cd $SAGA_ROOT_DIR/src/$SAGA_X509_VERSION
SAGA_LOCATION=$SAGA_ROOT_DIR ./configure
make -j4 && make install

## Download / Update & Install SAGA GLOBUS
##
cd $SAGA_ROOT_DIR/src/
if [ ! -d "$SAGA_ROOT_DIR/src/$SAGA_GLOBUS_VERSION" ]; then
  svn co https://svn.cct.lsu.edu/repos/saga-adaptors/globus/tags/releases/$SAGA_GLOBUS_VERSION
else
  cd $SAGA_ROOT_DIR/src/$SAGA_GLOBUS_VERSION
  svn update
fi

cd $SAGA_ROOT_DIR/src/$SAGA_GLOBUS_VERSION
SAGA_LOCATION=$SAGA_ROOT_DIR ./configure
make -j4 && make install


## Download / Update & Install SAGA Condor
##
cd $SAGA_ROOT_DIR/src/
if [ ! -d "$SAGA_ROOT_DIR/src/condor-$SAGA_CONDOR_VERSION" ]; then
  svn co https://svn.cct.lsu.edu/repos/saga-adaptors/condor/$SAGA_CONDOR_VERSION condor-$SAGA_CONDOR_VERSION
else
  cd $SAGA_ROOT_DIR/src/condor-$SAGA_CONDOR_VERSION
  svn update
fi

cd $SAGA_ROOT_DIR/src/condor-$SAGA_CONDOR_VERSION
SAGA_LOCATION=$SAGA_ROOT_DIR ./configure
make -j4 && make install


## Write env script
if [ -f "$SAGA_ROOT_DIR/saga-env.sh" ]; then
  rm $SAGA_ROOT_DIR/saga-env.sh
fi

echo "#!/bin/bash" >> $SAGA_ROOT_DIR/saga-env.sh
echo "export SAGA_LOCATION=$SAGA_ROOT_DIR" >> $SAGA_ROOT_DIR/saga-env.sh
echo 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SAGA_LOCATION/lib' >> $SAGA_ROOT_DIR/saga-env.sh
echo 'export PATH=$PATH:$SAGA_LOCATION/bin' >> $SAGA_ROOT_DIR/saga-env.sh
echo 'export PYTHONPATH=$PYTHONPATH:$SAGA_LOCATION/lib/python2.4.3/site-packages' >> $SAGA_ROOT_DIR/saga-env.sh
