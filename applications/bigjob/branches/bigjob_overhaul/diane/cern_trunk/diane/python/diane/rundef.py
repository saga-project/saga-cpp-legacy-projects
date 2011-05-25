## functions dealing with reading user-defined run files and preparing run input

from diane import getLogger
logger = getLogger('RunMaster')

from diane.diane_exceptions import DianeException

class InputError(DianeException):
    pass

class Input:
    """Collection of user-controllable parameters of diane-run.
    The parameters starting with underscore are read-only and are initialized by DIANE automatically:
    _runfile : name (absolute path) of the runfile
    _rundir : absolute path of the run directory
    """
    def __init__(self):
        self._runfile = None
        self._rundir = None


class Runfile:
    """ Interface to runfile: resolve attribute values and call functions
    using the priority mechanism: stuff defined in runfile explicitly has
    precedence over the stuff defined at the application level.
    """
    def __init__(self,filename):
        """ Read-in the specified runfile. If None is passed as a filename a dummy runfile object.
        This may be useful to simplify the usage if the runfile is optional (as it is for submitter interface).
        """
        self.filename = filename
        self.symbols = {}
        self.application = None

        if filename is None:
            self.filename = "<dummy-runfile-object>"
            logger.debug('dummy runfile object created')
            return
        
        # exec the run file, check for the mandatory symbols (application) and return the defined symbols dict
        logger.info('runfile: %s',filename)

        try:
            execfile(filename,self.symbols)
        except Exception,x:
            _error("problem reading runfile %s: %s"%(filename,str(x)),exc=not isinstance(x,IOError))
        
        try:
            self.application = self.symbols['application']
        except KeyError:
            _error("'application' module undefined in file %s"%filename)

        if self.application.__name__.find('diane.') == 0:
            _error("""invalid application module name: '%s' - application packages starting with 'diane.' may clash with the diane core packages and therefore cannot be used"""%self.application.__name__)

        logger.info('application module: %s',self.application.__name__)
        logger.debug('application file: %s',self.application.__file__)

    def call(self,f_name,*args,**kwds):
        """ Make a chain call f_name(*args,**kwds) so that if f_name is
        defined in the application module it is called first. If f_name is
        defined in the runfile it is called next.  The result is always
        discarded so the functions may be changing the objects which are
        passed as arguments. If f_name is not defined at all then raise
        InputError.
        """
             
        fun_list = []

        try:
            fun_list.append(getattr(self.application,f_name))
        except AttributeError:
            pass

        try:
            fun_list.append(self.symbols[f_name])
        except KeyError:
            pass

        if not fun_list:
            _error('function %s not defined in runfile/application'%f_name)

        for f in fun_list:
            f(*args,**kwds)

    def get_value(self,name,direct=True):
        """ Return the value of attribute 'name' which must defined in the runfile if direct==True.

        If direct==False look up the attribute additionally in the application module.
        
        If the name is not defined then raise InputError.
        """

        try:
            return self.symbols[name]
        except KeyError:
            if direct:
                _error("'%s' not defined in runfile (it must be defined directly there)"%name)
            try:
                return getattr(self.application,name)
            except AttributeError:
                _error("'%s' not defined neither in the runfile nor in the application module"%name)

def _error(msg,exc=False):
    import diane.logger
    if exc and diane.logger.level == diane.logger.logging.DEBUG:
        logger.exception(msg)
    raise InputError(msg,exc=exc)

import diane.config
config = diane.config.getConfig(None)

import os

def makedef(fn):
    # exec the run file and process the run descriptions by running a sequence of rundef() hooks
    runfile = Runfile(fn)
    runfile.get_value('run',direct=True) # check that rundef() is indeed defined in runfile directly
    input = Input()
    input._runfile = os.path.abspath(fn)
    input._rundir = os.path.abspath(os.getcwd()) # assume that rundir is CWD
    runfile.call('run',input,config)

    def require(x):
        if not hasattr(input,x):
            msg = 'input.%s not defined'%x
            logger.error(msg)
            raise InputError(msg)

    for x in ['data','worker','scheduler','manager']: require(x)

    if hasattr(input,'application'):
        logger.warning('input.application is already defined')
    input.application = runfile.application

    # if defined run the post-processing hook
    try:
        run_post = runfile.get_value('run_post',direct=False)
    except InputError:
        pass
    else:
        logger.debug('application post_run() hook')
        run_post(input,config)
        
    return input

if __name__=='__main__':
    #import sys
    #sys.setrecursionlimit(10)
    makedef('applications/sample1/job.py')
    
