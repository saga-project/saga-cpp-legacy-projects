from diane import getLogger
logger = getLogger('WorkerServlet')

import CORBA
import DIANE_CORBA 
import DIANE_CORBA__POA

import streamer

from diane.application import create_application_proxy, IApplicationProxy
import diane.application

def raiseXApplication(x):
    assert isinstance(x,(diane.application.ApplicationFailure,diane.application.ApplicationCritical))
    xapp = DIANE_CORBA.XApplication(None)
    xapp.source_exception = streamer.dumps(x)
    raise xapp

class WorkerServlet(DIANE_CORBA__POA.WorkerServlet,):
    """ WorkerServlet provides a way of sandboxing the application in a separate process.
    WorkerAgent acts a client to the locally-bound server (WorkerServlet).
    """
    def __init__(self,server):
        self.server = server
        self.application = None

    def create_application(self, app_boot):
        try:
            logger.debug('servlet.create_application()')
            app_boot = streamer.loads(app_boot)
            
            import sys
            sys.path.insert(0,app_boot.app_python_path)            
            from application import InprocessApplicationProxy
            ## FIXME: app_boot.environment = ...
            self.application = InprocessApplicationProxy.create_proxy(app_boot)
        except Exception,x:
            logger.exception('problem with WorkerServelet.create_application')
            raiseXApplication(x)
        
    def initialize(self, app_init):
        try:
            logger.debug('servlet.initialize()')
            return self.application.initialize(app_init)
        except Exception,x:
            raiseXApplication(x)

    def do_work(self, task_data):
        try:
            logger.debug('servlet.do_work()')
            return self.application.do_work(task_data)
        except Exception,x:
            raiseXApplication(x)
        
    def finalize(self, cookie):
        try:
            logger.debug('servlet.finalize() cookie=%s',repr(cookie))
            self.application.finalize(cookie)
        except Exception,x:
            raiseXApplication(x)

    def stop(self):
        self.server.stop()

# this function will always
#  - raise ApplicationFailure on recoverable application problems
#  - raise ApplicationCritical on unrecoverable application problems (such as crash /maybe due to system reasons/)

def handleXApplication(x):
    if isinstance(x, DIANE_CORBA.XApplication):
        xapp = streamer.loads(x.source_exception)
        if not isinstance(xapp,(diane.application.ApplicationFailure,diane.application.ApplicationCritical)):
            msg = 'internal framework error: unexpected application exception: %s %s'%(str(xapp),str(repr(xapp)))
            logger.critical(msg)
            raise AssertionError(msg)
        raise xapp
    if isinstance(x, CORBA.Exception):
        msg = 'WorkerServlet process communication problem: %s'%x
        logger.warning(msg,exc_info=True)
        raise diane.application.ApplicationCritical(msg)
    logger.exception('other exception raised: %s',x)
    raise x

import os, signal

import subprocess,time

class ServletApplicationProxy(IApplicationProxy):
    """Inprocess application proxy which handles the streaming and provides uniform error reporting."""
    def __init__(self):
        self.servlet = None
        self.popen = None
            
    def initialize(self,app_init):
        try:
            logger.debug('proxy.initialize()')
            return self.servlet.initialize(app_init)
        except Exception,x:
            handleXApplication(x)

    def do_work(self, task_data):
        try:
            logger.debug('proxy.do_work()')
            return self.servlet.do_work(task_data)
        except Exception,x:
            handleXApplication(x)

    def finalize(self, cookie):
        try:
            try:
                logger.debug('proxy.finalize()')
                self.servlet.finalize(cookie)
            except Exception,x:
                handleXApplication(x)
        finally:
            self.release_resources()
            
    def release_resources(self):
        logger.debug('attempt to release_resources()')
        #import traceback
        #traceback.print_stack()
        if self.popen is not None:
            logger.info('release_resources(pid=%d)',self.popen.pid)
            try:
                if self.servlet:
                    self.servlet.stop()
            except Exception,x: # servlet ORB may be shutting down with errors, we do not care
                logger.warning('while servlet.stop(): %s',x)
            try:
                time.sleep(1) # give the servlet process a chance to terminate by itself...
                if self.popen.poll() is None:
                    logger.info('killing worker servlet process which is still running (pid=%d)',self.popen.pid)
                    os.kill(-self.popen.pid,signal.SIGKILL)
            except OSError,x:
                logger.warning('while killing the servlet process: %s',x)
            try:
                ws = self.popen.wait() # collect pid to avoid zombies
            except OSError,x:
                logger.warning('problem while wait %s (pid=%d)',x,self.popen.pid)
        self.popen = None
        
    def create_proxy(boot):
        agent = boot.agent
        
        s = ServletApplicationProxy()
        import diane
        ior_filename = "ServletOID-%d"%agent.wid
        wrapper_filename = "ServletWrapper-%d"%agent.wid

        for fn in [ior_filename, wrapper_filename]:
            if os.path.exists(fn):
                os.remove(fn)

        wrapper_text = """
######        
# Automatically generated shell wrapper for DIANE application servlet process.
# shell command: %s
# shell prefix:
DIANE_APPLICATION_SHELL_PRE_PROCESS
# end of shell prefix
######
env ORBendPoint=giop:unix: python  -W ignore::RuntimeWarning %s %s
######
# shell suffix:
DIANE_APPLICATION_SHELL_POST_PROCESS
# end of shell suffix
######
""" % (boot.application_shell_command,os.path.join(os.path.dirname(diane.__file__),'WorkerServlet.py'), ior_filename)
        
        wrapper_text = wrapper_text.replace('DIANE_APPLICATION_SHELL_PRE_PROCESS',boot.application_shell_pre_process)
        wrapper_text = wrapper_text.replace('DIANE_APPLICATION_SHELL_POST_PROCESS',boot.application_shell_post_process)        

        file(wrapper_filename,'w').write(wrapper_text)

        # FIXME: windows unfriendly
        s.popen = subprocess.Popen([boot.application_shell_command,wrapper_filename])
        
        #s.popen = subprocess.Popen(['python',"-W ignore::RuntimeWarning",os.path.join(os.path.dirname(diane.__file__),'WorkerServlet.py'), ior_filename])
        try:
            TIMEOUT = 5
            countdown = TIMEOUT
            while not os.path.exists(ior_filename) or countdown>0:
                time.sleep(0.5) #FIXME: a better mechanism to assure that the process has indeed started...
                countdown-=0.5
            if not os.path.exists(ior_filename):
                raise Exception('%s file not created by the WorkerServlet subprocess after %d seconds',os.path.abspath(ior_filename),TIMEOUT)
            ior = file(ior_filename).read()
            ### DEBUG BEGIN
            logger.info('ServletIOR: %s',ior)
            file("2ServletOID-%d"%agent.wid,'w').write(ior)
            ### DEBUG END
            s.servlet = agent.program.orb.string_to_object(ior)
            s.servlet.create_application(streamer.dumps(boot))
            return s
        except Exception,x: # FIXME: prevent process leak here
            logger.error('Problem starting WorkerServlet: %s',x)
            s.release_resources()
            raise
    create_proxy = staticmethod(create_proxy)


def main():
    from CORBAProgram import CORBAProgram
    program = CORBAProgram(server=True)

    logger.info('Starting WorkerServlet process...')
    servlet_servant = WorkerServlet(program)
    servlet = servlet_servant._this()
    import sys
    try:
        ior_filename = sys.argv[1]
    except IndexError,x:
        logger.error('missing Servlet OID file name!')

    #FIXME: make mv on this file (to avoid race condition and passing empty ServletOID to the calling process    
    iorf = file(ior_filename,"w")
    iorf.write(program.orb.object_to_string(servlet))
    iorf.close()

    program.start()
    logger.info('Stopping WorkerServlet process...')

if __name__ == '__main__':
    main()

