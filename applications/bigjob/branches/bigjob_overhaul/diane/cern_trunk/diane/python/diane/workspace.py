
import os,os.path

try:
    _top = os.environ['DIANE_USER_WORKSPACE']
except KeyError:
    _top = os.path.expanduser('~/diane')

def _getpath(path):
    return os.path.join(_top,path)

def _makedir(path):
    p = _getpath(path)
    if not os.path.exists(p):
        os.makedirs(p)
    return p

import pickle

run_index_fn = os.path.join(_makedir('runs'),'index')

_makedir('apps')
_makedir('submitters')

# set via PYTHONPATH by PACKAGE.py 
# set path for user-defined application modules
#import sys
#sys.path.insert(0,_getpath('apps'))

# FIXME: obviously there is a race condition in this code if two processes try
# to concurrently get the same index but for the moment we do not bother with
# that
def _new_index():
    if not os.path.exists(run_index_fn):
        pickle.dump(1,file(run_index_fn,'w'))
        return 1
    else:
        run_index = pickle.load(file(run_index_fn))  + 1
        pickle.dump(run_index,file(run_index_fn,'w'))
        return run_index

def getPath(path):
    return _getpath(path)

def currentRunIndex():
    if not os.path.exists(run_index_fn):
        return 0
    return pickle.load(file(run_index_fn))

def makeRundir():
    return _makedir('runs/%04d'%_new_index())

def getRundir(path="",runid=None):
    if runid is None:
        runid = currentRunIndex()
    return _getpath('runs/%04d/%s'%(runid,path))

def workerSubdir(wid):
    return "%05d"%wid

def makeSubdir(path="",runid=None):
    return _makedir(getRundir(path,runid))


#TEST
#for i in range(100):
#    print makeRundir()
    
