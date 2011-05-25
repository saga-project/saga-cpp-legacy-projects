from omniORB import CORBA

import diane
logger = diane.getLogger('CORBAProgram')

import time

# CORBAProgram singleton.
theProgramInstance = None

class CORBAProgram:
    """
    A generic CORBA program (client or server). Initializes the CORBA runtime and provides methods
    to start and stop the program (the ORB event loop).

    Use start() to start the event loop (blocking until stop() is called from a different thread).
    Use stop() to stop the event loop and return from start() which typically means
    terminating the process. The threads attribute is a list of threads for which the program should wait at exit.

    Public attributes:
      self.orb
      self.root_poa
      self.root_poa_manager
    """
    
    def __init__(self,server=False,config_file=None,enable_GSI=False,threads=[],args=None):
        """ Create ORB. If server == True then create the ROOT POA objects, activate the ROOT POA manager (listening sockets).
        The threads list specifies threads which should be joined at exit (each join with THREAD_EXIT_TIMEOUT seconds). The
        thread objects should provide a stop() method in the same way as diane.BaseThread class.
        Optional args are passed to ORB_init (default: sys.argv).
        """

        global theProgramInstance
        assert theProgramInstance is None, 'CORBAProgram() created more than once'
        theProgramInstance = self
        
        self.SLEEP_TIME = 0.01
        self.THREAD_EXIT_TIMEOUT = 10
        self.__should_stop_flag = False

        if args is None:
            import sys
            args = sys.argv

        self.server = server

        # omniORB's configuration
        if config_file:
            import os
            if os.environ.has_key('OMNIORB_CONFIG'):
                logger.warning('OMNIORB_CONFIG is already defined (%s), I am resetting it now'%os.environ['OMNIORB_CONFIG'])
            os.environ['OMNIORB_CONFIG'] = os.path.abspath(config_file)
            
        if enable_GSI:
            self.enable_GSI()

        logger.debug('Initializing ORB.')
        self.orb = CORBA.ORB_init(args, CORBA.ORB_ID)

        if server:
            self.root_poa = self.orb.resolve_initial_references("RootPOA")
            self.root_poa_manager = self.root_poa._get_the_POAManager()
            self.root_poa_manager.activate()

        self.__threads = []
        self.__atexit = []

        self.finalizing = False
        
    def start(self,doEvents=None):
        """ Start the program: enter infinite event loop until stop() method is called by someone.
        Optional doEvents is a non-blocking callable object (e.g. a function) taking this CORBA program instance as an argument.
        On exit from the event loop the program the finalize() is called automatically.
        """

        logger.debug('Starting event loop.')

        try:
            while not self.__should_stop_flag:
                self.orb.perform_work()
                if doEvents:
                    try:
                        doEvents(self)
                    except Exception,x:
                        logger.exception("doEvents() raised an exception")
                        
                if self.SLEEP_TIME is not None:
                    time.sleep(self.SLEEP_TIME)
        except KeyboardInterrupt:
            logger.info("KeyboardInterrupt, quitting event loop")
            #if interrupt occured while perform_work(), do it one again to avoid "None not callable" exceptions
            self.orb.perform_work()
        self.finalize()
        
    def stop(self):
        """ Stop the event loop. """
        if not self.__should_stop_flag:
            logger.debug("Stopping event loop.")
            self.__should_stop_flag = True

    def registerAtExitHandler(self,f,args=[],kwds={}):
        """ Add an exit handler which is called before the ORB is destroyed and
        after all service threads have been given a chance to stop in the finalize() method. 
        """
        self.__atexit.append((f,args,kwds))

    def addServiceThread(self,t):
        logger.debug('service thread "%s" added to the CORBAProgram',t.getName())
        self.__threads.append(t)

    def finalize(self):
        """ Cleanup ORB resources. If you use the event loop with start() and stop() methods you do not need to call this explicitly."""

        if self.finalizing:
            logger.warning('CORBAProgram already finalizing')
            return
        self.finalizing = True

        logger.debug('CORBAProgram.finalize() started')
        
        import threading
        t = threading.Thread(target=self._finalize) #FIXME: use deamon thread?
        t.start()

        # FIXME: add timeout
        while t.isAlive():
            self.orb.perform_work()
            t.join(self.SLEEP_TIME)
            #if self.SLEEP_TIME is not None:
            #    time.sleep(self.SLEEP_TIME)

        self.orb.perform_work()
        
        logger.debug('sanity checking of other threads created by the threading module')
        for t in threading.enumerate():
            if 'diane' in t.getName():
                if t.isAlive():
                    logger.warning('Thread %s is still running (it may use ORB). Proceeding with ORB shutdown anyway',t.getName())
                if t not in self.__threads:
                    logger.warning('Thread %s is not in the CORBAProgram.threads list.')
                
        logger.debug('Destroying ORB.')

        if self.server:
            self.root_poa.destroy(True,True)
            self.root_poa_manager.deactivate(True,True)
        self.orb.destroy()
        

    def _finalize(self):
        """Internal finalization sequence: stop and wait for termination of service threads and run atexit handlers.
        The ORB is active while this method is running.
        """
        logger.debug('CORBAProgram._finalize() started')
        #ask helper server threads to stop
        for t in self.__threads:
            t.stop()

        for t in self.__threads:
            if t.isAlive():
                logger.debug('Waiting for CORBAProgram service thread to finish %s',t)
                t.join(self.THREAD_EXIT_TIMEOUT)
            if t.isAlive():
                logger.warning('Helper thread %s not terminated yet, exiting anyway',str(t))
            else:
                logger.debug('Service thread %s finished OK',t)

        for f,args,kwds in self.__atexit:
            try:
                logger.debug('Running CORBAProgram.atexit handler: %s(*args=%s,**kwds=%s):',f,args,kwds)
                f(*args,**kwds)
            except Exception,x:
                logger.exception('CORBAProgram.atexit %s(*args=%s,**kwds=%s):',f,args,kwds)
        logger.debug('CORBAProgram._finalize() finished')

    def enable_GSI(self):
        import os
        logger.debug('Enabling GSI support.')
        # Currently we need extra setup only for GSI-mode
        # Please note that this setup procedure requires you already have a Grid environment loaded
        # before starting DIANE (i.e: X509_* vars are used)

        #1: (GLITE workaround) some WN does not set X509_CERT_DIR anymore, so we rely on default location
        os.environ.setdefault('X509_CERT_DIR','/etc/grid-security/certificates')	
        if not os.environ.has_key('X509_USER_PROXY') or not os.environ.has_key('X509_CERT_DIR'):
            logger.warning('''Grid environment not available (X509_USER_PROXY
            and X509_CERT_DIR required but not found in env)...Disabling
            security support''')
            return
        import omniORB.sslTP
        #2: set OPENSSL_ALLOW_GRIDPROXY_CERTS to enable Grid proxy certificate handlers - already set by DIANE setup script
        os.environ.setdefault('OPENSSL_ALLOW_GRIDPROXY_CERTS','1')
        #3: set OPENSSL_ENABLE_CRL to enable revocation list cheking - already set by DIANE setup script
        os.environ.setdefault('OPENSSL_ALLOW_GRIDPROXY_CERTS','1')

        omniORB.sslTP.key_file(os.environ.get("X509_USER_PROXY",''))
        #DO NOT pass your private key password here:
        #In case of proxy certificate just pass an empty string
        omniORB.sslTP.key_file_password("")


if __name__ == '__main__':
    program = CORBAProgram()
    #server.SLEEP_TIME = 0.1
    cnt = 0
    def doEvents(s):
        global cnt
        logger.info('shutdown %d next message in %f seconds',20-cnt,float(s.SLEEP_TIME))
        cnt += 1
        if cnt == 20:
            s.stop()
    program.start(doEvents=doEvents)
    
    
