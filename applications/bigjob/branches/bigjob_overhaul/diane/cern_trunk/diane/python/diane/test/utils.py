import subprocess
import time
import os, signal

import diane
logger = diane.getLogger('test.case')

from diane.test.utils import *

# added optional detection of false positives (e.g. NameError), case does not matter
def report_errors(fn,false_positives=[]):
    """The false_positives must contain ERROR or EXCEPT substrings (the case
    does not matter).  However the search for false_positives is
    case-sensistve.
    """
    for sub in false_positives:
        if sub.upper().find('ERROR') == -1 and sub.upper().find('EXCEPT') == -1:
            raise ValueError('False positive %s must contain ERROR or EXCEPT substring'%sub)
    for l in file(fn).readlines():
        false_positives_cnt = 0
        for sub in false_positives:
            false_positives_cnt+=l.upper().count(sub.upper())
        if false_positives_cnt != l.upper().count('ERROR') + l.upper().count('EXCEPT'):
            logger.error('%s: %s',fn,l)
            return False
    return True

def get_output_dir():
    try:
        return os.path.join('output',os.path.basename(os.environ['DIANE_CONFIG']))
    except KeyError:
        return 'output'

def _open_output_file(exename,testname,instance_number):
    outdir = get_output_dir()
    try:
        os.makedirs(outdir)
    except OSError,x:
        pass #FIXME: EEXIST
    ofn = os.path.abspath(os.path.join(outdir,'%s_%s_%d.out'%(testname,exename,instance_number)))
    ofile = file(ofn,'w')
    return ofile

def start_process(exename,args,testname,instance_number):
    ofile = _open_output_file(exename,testname,instance_number)
    process =  subprocess.Popen([exename]+args,stdout=ofile, stderr=ofile)
    logger.info('%s %d started, output: %s',exename,instance_number,ofile.name)
    return process,ofile

def wait_process_finished(process,timeout):
    """wait for a subprocess to finish with a given timeout, return None if process not finished yet or its exitcode"""
    t0 = time.time()
    while time.time()-t0 < timeout:
        if process.poll() is None:
            time.sleep(1)
        else:
            break
    return process.poll()

    
def _getmasterstatus(testname,instance_number):
    exename = 'diane-master-ping'
    ofile = _open_output_file(exename,testname,instance_number)
    import cPickle
    return cPickle.loads(subprocess.Popen([exename,'getStatusPickle','-'],stdout=subprocess.PIPE,stderr=ofile).communicate()[0])


def getcallername(pos=0):
    import traceback
    return traceback.extract_stack(limit = 3)[-2-pos][2]

import unittest

class TestBase(unittest.TestCase):
    """Generic test base: keeps track of started masters and workers and check their output for errors.
    At least one master should be started before any workers are started.
    """
    def __init__(self,*args):
        unittest.TestCase.__init__(self,*args)
        self.masters = []
        self.workers = []
        self.status_calls = 0

    def start_master_process(self,args=None,callername=None):
        newid = len(self.masters)
        if args is None:
            args = [self.defaultRunFile()]
        if callername is None:
            callername = getcallername(1)
        self.masters.append(start_process('diane-run',args,callername,newid))
        return self.masters[-1]
        
    def start_worker_process(self,args=[],callername=None):
        newid = len(self.workers)
        if callername is None:
            callername = getcallername(1)
        import diane.workspace
        
##         # save the start directory
##         startdir=os.getcwd()

##         # make a temporary workdir and cd to it
##         # this workdir is our empty "sandbox" where we can put stuff 
##         workdir = tempfile.mkdtemp(prefix="__workdir__WorkerAgent.",dir=startdir)
##         os.chdir(workdir)
        self.workers.append(start_process('diane-worker-start',['--workdir=%s'%diane.workspace.getRundir(str(newid))],callername,newid))
        return self.workers[-1]

    def getmasterstatus(self):
        n = self.status_calls
        self.status_calls+=1
        return _getmasterstatus(getcallername(1),n)


    def running_workers(self):
        """ Return indices of running workers."""
        return [i for i in range(len(self.workers)) if self.workers[i][0].poll() is None]
                
    def wait_master_finished(self,id=0,timeout=10):
        return wait_process_finished(self.masters[id][0],timeout)

    def assert_master_finished(self,id=0,timeout=10):
        if self.wait_master_finished(id,timeout) is None:
            assert False, "master %d not finished after %d seconds"%(id,timeout)

    def tearDown(self):
        errlogs = []
        def do_kill(v,errlogs):
            for w,f in v:
                if w.poll() is None:
                    os.kill(w.pid, signal.SIGINT)
                try:
                    false_positives = self.false_positives
                except AttributeError:
                    false_positives = []
                if not report_errors(f.name,false_positives):
                    errlogs.append(f.name)
        do_kill(self.workers,errlogs)
        do_kill(self.masters,errlogs)

        assert not errlogs, "errors/exceptions found in the logfiles: %s"%str(errlogs)


class TimeoutTest(TestBase):
    """ A base class for a all tests which have a stardard behaviour: 1 master process, N workers, and given timeout on master termination.
    """
    def __init__(self,*args):
        TestBase.__init__(self,*args)
        
    def defaultRunFile(self):
        raise NotImplementedError()

    def validator(self,v,x):
        return v(x)
    
    def skeleton(self,callername):
        logger.info('begin')
        self.start_master_process(callername=callername)
        time.sleep(1)
        for i in range(self.n_workers):
            self.start_worker_process(callername=callername)
        self.assert_master_finished(timeout=self.timeout)
        if self.validate:
            logger.info('validation')
            import diane.workspace
            import pickle
            self.validator(self.validate,pickle.load(file(diane.workspace.getRundir('crash.pp'))))
        else:
            logger.info('no validation')
        TIMEOUT = 10
        t = time.time()
        while self.running_workers() and time.time()-t < TIMEOUT:
            logger.debug('wating for worker processes to finish')
            time.sleep(1)
        r = self.running_workers()
        if r: logger.warning('not all workers finished: %s',r)
        logger.info('passed OK')

def make_timeout_test2(name,timeout,n_workers,validate,runfile,false_positives):
    """Create and return a timeout test 'name' using indicated run file."""
    # make a new test case class
    class T(TimeoutTest):
        def defaultRunFile(self):
            return os.path.abspath(runfile)
    # set the parameters
    setattr(T,'timeout',timeout)
    setattr(T,'n_workers',n_workers)

    # set the test method
    setattr(T,name,lambda self:self.skeleton(name))

    ##print 'test method name',name #DEBUG
    
    # set the validation method (needs to strip self from the argument list)
    if validate is None:
        _validate = None
    else:
        _validate = lambda self,x: validate(x)
        
    setattr(T,'validate',_validate)

    # set false positives for this testcase
    setattr(T,'false_positives',false_positives)

    return unittest.TestLoader().loadTestsFromTestCase(T)

def make_timeout_test(timeout, n_workers,validate=None,false_positives=[]):
    """Make the timeout test (name is based on the filename of the caller)
    using the file of the caller as the run file. Optional validation argument is a callable
    (function) which will be executed at the end of the run and it will be passed an instance of application manager.
    This function returns a unittest suite, which may be convinently run using run_test() function defined in this module.
    """
    import inspect
    #import traceback
    #traceback.print_stack(limit=3)
    #print inspect.stack()[1]
    this_file = os.path.abspath(inspect.stack()[1][0].f_code.co_filename)
    suite = make_timeout_test2(os.path.basename(this_file),timeout,n_workers,validate,this_file,false_positives)
    return suite

def run_test(suite):
    """ Convenient shortcut for executable tests.
    """
    import sys

    import logging
    level = logging.DEBUG
    
    try:
        level = getattr(logging,sys.argv[1])
        print 'user-defined logging level',sys.argv[1]
    except IndexError:
        pass
    except AttributeError,x:
        print 'cannot set the logging level:',x
    
    logging.getLogger('DIANE').setLevel(level)

    unittest.TextTestRunner().run(suite)
