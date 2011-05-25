import diane
logger = diane.getLogger('application')

class Boot:
    def __init__(self):
        self.application_name = None
        self.worker_class_name = None
        self.config = None
        self.runid = None
        self.master_uuid = None
    def log(self):
        for x in self.__dict__:
            logger.info("%s = %s",x,repr(getattr(self,x)))

from diane.util import importName

# recoverable application problem
class ApplicationFailure(Exception):
    pass #FIXME: traceback_string variable

# unrecoverable application problem (e.g. crash due to abort())
class ApplicationCritical(Exception):
    pass

import traceback

from omniORB import CORBA

def handleApplicationFailure(x):
    import diane.application
    
    logger.warning('application failure detected: %s',x,exc_info=True)
    if isinstance(x,diane.application.ApplicationCritical):
        appx = x
    else:
        if isinstance(x,CORBA.UserException): # use-case: XFileTransferError
            appx = diane.application.ApplicationFailure(str(x)) # CORBA exceptions cannot be unpickled for some reason
        else:
            appx = diane.application.ApplicationFailure(x)

    from diane.util.compatibility import format_exc
    
    appx.traceback_string = format_exc()

    raise appx
    

class IApplicationProxy:
    """Application proxy interface at the worker node.
    
    Application proxy layer handles automatically the message streaming
    and provides uniform error reporting based on ApplicationFailure and ApplicationCritical exceptions.
    The application proxy delegates the method implementation to the underlying application
    component.

    All methods accept and return network message blobs (see streamer module).
    Streaming of the blobs is the responsability of the application proxy.
    """
    
    def initialize(self,app_init):
        """Initialize the application with the init data and return the init result.
        Raise ApplicationFailure exception in case of problems.
        """
        raise NotImplementedError
    
    def finalize(self,cookie):
        """Finalize the application using the cookie.
        Raise ApplicationFailure exception in case of problems.
        """        
        raise NotImplementedError

    def do_work(self,task_data):
        """ Execute the task using the task_data and return the task result.
        Raise ApplicationFailure exception in case of problems.
        """
        raise NotImplementedError

    def create_proxy(boot,**kwds):
        """ Create an instance of the 
        """
        raise NotImplementedError
    create_proxy = staticmethod(create_proxy)
    

import streamer

class InprocessApplicationProxy(IApplicationProxy):
    """Run the application component in the same process.
    Certain functionality may be limited:
      * segmentation faults in the application modules will crash the whole process (worker agent)
      * non-demonic threads may not be interrupted on finalize so the worker agent may continue
        to run until all such application threads exit
      * environment setup may not function in some cases (LD_LIBRARY_PATH)
    """
    def __init__(self,app):
        self.app = app
            
    def initialize(self,app_init):
        try:
            logger.debug('InprocessApplicationProxy.initialize()')
            app_init = streamer.loads(app_init)
            app_init_output = self.app.initialize(app_init)
            app_init_output = streamer.dumps(app_init_output)
            return app_init_output
        except Exception,x:
            handleApplicationFailure(x)

    def finalize(self,cookie):
        try:
            logger.debug('InprocessApplicationProxy.finalize()')
            cookie = streamer.loads(cookie)
            self.app.finalize(cookie)
        except Exception,x:
            handleApplicationFailure(x)

    def do_work(self,task_data):
        try:
            logger.debug('InprocessApplicationProxy.do_work()')
            task_data = streamer.loads(task_data)
            task_result = self.app.do_work(task_data)
            return streamer.dumps(task_result)
        except Exception,x:
            handleApplicationFailure(x)

    def create_proxy(boot, **kwds):
        logger.debug('InprocessApplicationProxy.create_proxy()')        
        symbols={}

        try:
            __import__(boot.name)
        except Exception,x:
            handleApplicationFailure(x)#'cannot import application package "%s": %s',boot.name, str(x))
            
        # FIXME: setup environment from boot data
        workerClass = importName(boot.name, boot.workerClassName)
        workerObj = workerClass()
        workerObj._agent = boot.agent
        return InprocessApplicationProxy(workerObj)

    create_proxy = staticmethod(create_proxy)

def print_obj(obj):
    for x in dir(obj):
        print x,getattr(obj,x)

import diane.config
from diane.diane_exceptions import DianeException

config = diane.config.getConfig('main')
config.addOption('cache','~/dianedir/cache','application tarball cache')
#config.addOption('inprocess_worker_proxy',False,'if True the worker application component is imported into the WorkerAgent directly, otherwise it is imported into a separate sandboxing servlet process, this option is used in the master only: the master sends the value of this option to the Worker as a part of application initialization process (boot data)')

def check_cache(name):
    # FIXME: to be implemented based on checksums/timestamps (?)
    return 1

def get_source_package(application):
    import os
    import inspect
    src_path = inspect.getsourcefile(application)
    dirname,basename = os.path.split(src_path)
    app_path = application.__name__.replace('.',os.sep)
    if basename == '__init__.py':
        # application is defined in a package
        loc_path = dirname
    else:
        # application is a module
        loc_path = os.path.splitext(src_path)[0]
    assert loc_path[-len(app_path):] == app_path
    return loc_path[:-len(app_path)], application.__name__.split('.')[0]

import tarfile,os

def make_cache(application):
    """Create application tarball in the cache and return the full path to the tarball.
    Or None if the application is a built-in application from diane.applications package.
    """
    assert application.__name__.find('diane.') != 0, "application packages starting with 'diane.' may clash with the standard diane package and therefore cannot be used"
        
    dir, name = get_source_package(application)
    darname = '_application.tgz'
    dar = tarfile.open(darname,'w:gz')
    dar.dereference=True # follow symlinks
    dar.posix=False # no filename restrictions
    dar.add(os.path.join(dir,name),name)
    dar.close()
    return os.path.abspath(darname) #cannot use dar.name (.tgz seems not to be recognized and .tar is returned instead)

def create_application_proxy(boot_msg,app_init,agent,**kwds):
    boot = streamer.loads(boot_msg)

    import os

    if boot.darname:
        agent.ftc.download(boot.darname)
        dar = tarfile.open(boot.darname,'r:gz')
        try:
            dar.extractall('_python')
        except AttributeError:  #python < 2.5
            os.system('mkdir -p _python')
            os.system('cd _python; tar xfzv ../%s'%boot.darname)

    import sys
    app_python_path = os.path.abspath('_python')
    sys.path.insert(0,app_python_path)
        
    diane.config.restore_config(boot.config)
    logger.info('application boot and run data received')
    boot.log()
    diane.config.log_configuration(title='updated configuration')
    boot.agent = agent
    c = diane.config.getConfig('WorkerAgent')

    boot.application_shell_command = c.APPLICATION_SHELL    
    boot.application_shell_pre_process = ''
    boot.application_shell_post_process = ''
    
    # perform a setup action of the application
    setup_application = importName(boot.name, 'setup_application')
    if setup_application:
        try:
            r = setup_application(streamer.loads(app_init),agent)
            if not r is None:
                boot.application_shell_pre_process,boot.application_shell_post_process = r
        except Exception,x:
            handleApplicationFailure(x)

    if not c.APPLICATION_SHELL:
        if boot.application_shell_pre_process or boot.application_shell_post_process:
            logger.warning('''setup_application() returns shell commands to source (%s,%s) \
- they will be ignored because config.WorkerAgent.APPLICATION_SHELL is not set \
and this application will be loaded directly in the worker agent process'''%(boot.application_shell_pre_process,boot.application_shell_post_process))
        return InprocessApplicationProxy.create_proxy(boot)
    else:
        boot.app_python_path = app_python_path
        import WorkerServlet
        return WorkerServlet.ServletApplicationProxy.create_proxy(boot)

def make_boot_data(input):
    """Create the application bootstrap data to be sent to the worker agent. """
    boot = Boot()
    boot.name = input.application.__name__
    boot.workerClassName=input.worker.__name__
    boot.config = diane.config.capture_config()
    boot.darname = os.path.basename(make_cache(input.application))
    return boot

