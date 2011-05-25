#!/usr/bin/env python

# use python 2.5!

# Create server
DEFAULT_PORT = 4088 # last open port in DIANE range in lcgui003.cern.ch

import threading,time

logger = None

class BaseThread(threading.Thread):
    """
    Application thread base class.
    """

    def __init__(self,name=None,server=None):
        threading.Thread.__init__(self,name=name)
        self.setDaemon(1) # we can exit entire application at any time
        self.__should_stop_flag = False
        logger.debug("object created:%s",self.__class__.__name__)
    def should_stop(self):
        return self.__should_stop_flag
    
    def stop(self):
        if not self.__should_stop_flag:
            logger.debug("Stopping: %s",self.__class__.__name__)
            self.__should_stop_flag = True       

class AsyncCallThread(BaseThread):
    """Asynchronous function call thread.
    The result of call func(*args,**kwds) is stored in the result attribute.
    If an exception is raised the exception object is stored in the exception attribute. Otherwise the exception attribute is None.
    The caller is notified with the callback(t) where t is this thread object.
    """
    def __init__(self, callback, func, *args, **kwds):
        BaseThread.__init__(self,'async_call_%s'%func.__name__)
        self.callback = callback
        self.func = func
        self.args = args
        self.kwds = kwds
        self.result = None
        self.exception = None
        self.start_time = 0
        self.stop_time = 0
        
    def run(self):
        self.start_time = time.time()

        try:
            self.result = self.func(*self.args,**self.kwds)
        except Exception,x:
            self.exception = x

        self.stop_time = time.time()
        self.callback(self)

        
def async_call(callback,func, *args, **kwds):
    """Call func asynchronously in a separate thread. See AsyncCallThread class for details."""
    t = AsyncCallThread(callback,func,*args,**kwds)
    t.start()
    return t


def hostname():
  import socket
  try:
    return socket.gethostbyaddr(socket.gethostname())[0]
    # [bugfix #20333]: 
    # while working offline and with an improper /etc/hosts configuration	
    # the localhost cannot be resolved 
  except:
    return 'localhost'

def md5sum(file):
    import md5
    m = md5.new()
    f = open(file, 'r')
    for line in f.readlines():
        m.update(line)
    f.close()
    return m.hexdigest()

import logging

import sys,os
  
def main():

  port = DEFAULT_PORT
  host = hostname()
  #host = 'lxb1420.cern.ch'
  basedir = os.getcwd()

  print sys.argv
  
  try:
      basedir = os.path.abspath(sys.argv[1])
      port = int(sys.argv[2])
  except IndexError:
      pass

  outputdir = os.path.expanduser(basedir)

  def makedirs(path):
    try:
      os.makedirs(path)
    except OSError,x:
      import errno
      if x.errno != errno.EEXIST:
        raise

  makedirs(outputdir)

  # initialize logger
  logfilename = '%s/server.log'%basedir
  print >> sys.stderr,'logging to',logfilename
  global logger
  logger = logging.getLogger()
  hdlr = logging.FileHandler(logfilename)
  formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
  hdlr.setFormatter(formatter)
  logger.addHandler(hdlr) 
  logger.setLevel(logging.DEBUG)

  
  from SimpleXMLRPCServer import SimpleXMLRPCServer
  import SocketServer

  class FileServer(SocketServer.ThreadingMixIn, SimpleXMLRPCServer):
      def __init__(self,contup):
          SimpleXMLRPCServer.__init__(self,contup)
          self.daemon_threads = True
          
  server = FileServer((host,port))
  server.logRequests = False

  #server.register_introspection_functions()

  def prepare(name,subdir):
      destdir = os.path.join(outputdir,subdir)
      makedirs(destdir)
      fn = os.path.join(destdir,name)
      return fn

  def process_output(name,subdir,x):
    try:
      fn = prepare(name,subdir)

      if os.path.exists(fn):
        logger.warning('file already exists and will be overwritten: %s',fn)
        #import random
        #fn += "%04d"%random.randint(0,9999)
        #logger.warning('new file will be created: %s',fn)

      logger.info('transferring %s',fn)
      f = file(fn,'w')
      f.write(x.data)
      f.close()
      del x
      return 1
    except Exception,x:
      logger.exception('got exception during invocation to send_output()')
      return 2

  def callback(t):
    pass
  
  def send_output(name,subdir,x):
    async_call(callback,process_output,name,subdir,x)
    return 1

  def check_transfer(name,subdir,checksum):
      fn = prepare(name,subdir)
      if checksum:
        waitcnt = 100
        while not os.path.exists(fn):
          logger.warning('md5checksum file does not exist yet... waiting 1 second')
          time.sleep(1)
          waitcnt -= 1
          if waitcnt == 0:
            logger.error("md5checksum could not proceed: file transfer did not finish? %s",fn)
            return 1
          
        local_checksum = md5sum(fn)
        if local_checksum != checksum:
          logger.error('ERROR! the local checksum does not match: %s',local_checksum)
          return 2
        logger.info('file transfer checked OK, %s md5sum = %s '%(fn,checksum))
        return 0

  def download_file(name,local_checksum):
      logger.info('downloading the file: %s',name)
      import md5
      m = md5.new()
      f = file(name,'r')
      rawbuf = f.read()
      m.update(rawbuf)
      if m.hexdigest() == local_checksum:
          return 0,"" # do not download the file if the local and remote checksums are the same
      import xmlrpclib
      buffer = xmlrpclib.Binary(rawbuf)
      return m.hexdigest(),buffer
  
  server.register_function(process_output, 'send_output')
  server.register_function(check_transfer, 'check_transfer')
  server.register_function(download_file, 'download_file')
  
  logger.info('Starting output server: %s %d %s',host,port,outputdir)

  try:
    try:
      # Run the server's main loop
      server.serve_forever()
    except Exception,x:
      logger.exception('got exception while serve_forever()')
  finally:
    logger.info('cleaning up the socket...')
    server.socket.close()
    del server.socket
    del server

if __name__ == '__main__':
  main()
