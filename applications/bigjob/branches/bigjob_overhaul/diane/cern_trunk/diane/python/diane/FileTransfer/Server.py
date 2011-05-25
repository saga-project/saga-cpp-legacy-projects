import os,sys
import time
import pickle

from diane.util.compatibility import uuid, format_exc

import diane
logger = diane.getLogger('FileTransfer.Server')

from config import config

import DIANE_CORBA 
import DIANE_CORBA__POA

from Client import unique_md5sum_hexdigest

class FileTransferSession:
   def __init__(self):
      self.file = None
      self.filename = None
      self.peer_uuid = None
      self.opts = None
      self.start_time = time.time()
      self.last_update = self.start_time
      self.finish_time = None
      self.direction = None #upload/download
      self.bytes = 0
      self.session_uuid = str(uuid())
      self.status = "ongoing"

   def __getstate__(self):
      state = self.__dict__.copy()
      state['file'] = None
      return state


import threading

#TODO: cleanup of orphaned sessions...

from diane.BaseThread import BaseThread
class FileServerCleanupThread(BaseThread):
    """ Cleanup orphaned sessions and shutdown ongoing sessions.
    """
    def __init__(self,file_server):
        BaseThread.__init__(self,'FileServerCleanupThread')
        self.file_server = file_server

    def run(self):
       while not self.should_stop():
          time.sleep(0.1)          
          for s in self.file_server.sessions.values():
             if time.time() - s.last_update > config.ORPHANED_SESSION_TIMEOUT:
                logger.warning('session timeout (%d s): %s',config.ORPHANED_SESSION_TIMEOUT,s.session_uuid)
                self.file_server._close_session(s.session_uuid)


       # do not accept any new sessions and wait to finish ongoing ones...
       self.file_server.active = False

       while len(self.file_server.sessions) > 0:
          time.sleep(0.1)
          

from diane.Peer import Peer

class FileTransferServer(DIANE_CORBA__POA.FileTransferServer,Peer):
   """ A simple file transfer service. The server resolves the file names literally as provided by the client.
   Therefore, a relative file name will be resolved within the current working directory of the server. 
   An absolute file name will be resolved as expected: as an absolute path on the server.
   However, a security mechanism prevents to transfer files which are physically located outside of
   the directory specified by basedir.
   """
   def __init__(self,server):
      Peer.__init__(self,server,logger)
      self.sessions = {}
      # relative filenames and paths are resolved wrt to the basedir 
      self.basedir = os.getcwd()
      # list of authorized server-side directories which may be accessed, basedir is authorized implicitly
      # use setAuthorizedDirs() method to set this attribute
      self.authorized_dirs = []
      self.uuid_lock = threading.Lock()
      self.active = True # when not active then do not accept any new sessions
      self.cleanup_thread =  FileServerCleanupThread(self)

   def setAuthorizedDirs(self,authorized_dirs):
      # authorized dirs should be absolute (use os.path.realpath())
      self.authorized_dirs = [os.path.realpath(d) for d in authorized_dirs]


   # NOT USED YET: hook which allows customization of session handling in the derived classes
   def start_session_hook(self,filename,peer_uuid,opts,direction):
      return filename,opts
   
   # NOT USED YET: hook which allows customization of session handling in the derived classes
   def close_session_hook(self,session_id):
      return None
      
   def _start_session(self,filename, peer_uuid, opts,direction):
      logger.debug('attempt to start a new session, filename=%s, direction=%s, peer_uuid=%s, opts=...',filename,direction,peer_uuid)

      # get the real absolute path to the requested file
      # resolve relative paths wrt to the basedir 
      fn = os.path.realpath(os.path.normpath(os.path.join(self.basedir, filename)))

      # security check with authorized dirs: the requested file must be in one of authorized dirs
      check_passed = False

      authorized_dirs = self.authorized_dirs + [self.basedir]
      for authd in authorized_dirs:
         if os.path.commonprefix([authd,fn]) == authd:
            check_passed = True
            break

      if not check_passed:
            message = 'access denied: cannot access %s outside of the authorized dirs (%s)!'%(fn,authorized_dirs)
            logger.warning(message)
            raise DIANE_CORBA.XFileTransferError(message)

      # end of security check

      assert direction in ['download','upload']

      opts = pickle.loads(opts) # maybe unsecure...

      if self.start_session_hook:
         filename,opts = self.start_session_hook(filename,peer_uuid,opts,direction)

      #==== start of direction specific code ====
      if direction == 'upload':
         if not opts.overwrite and os.path.exists(fn):
             msg = 'ignoring transfer: forbiden to overwrite file %s'%fn
             logger.info(msg)
             raise DIANE_CORBA.XFileTransferError(msg)
          #TODO: upload file to a temporary unique name and then mv when session is closed...

      if direction == 'download':
         if opts.md5_lazy:
            if not opts._md5_hexdigest:
               msg = 'md5_lazy check ignored:_md5_hexdigest checkum is not provided'
               logger.debug(msg)
            if opts._md5_hexdigest == unique_md5sum_hexdigest(fn,opts):
               logger.info('md5_lazy: skipping download of file %s',fn)
               return ""
      #==== end of direction specific code ====
         
      flags = {'download':'r','upload':'w'}
      try:
         f = file(fn,flags[direction])
      except IOError,x:
         logger.warning(str(x))
         raise DIANE_CORBA.XFileTransferError(str(x))

      try:
          self.uuid_lock.acquire()
          s = FileTransferSession()
      finally:
          self.uuid_lock.release()          
      s.file = f
      s.peer_uuid = peer_uuid
      s.opts = opts
      s.direction = direction
      s.filename = fn

      logger.info('starting %s session: %s, session_uuid: %s, peer_uuid: %s',s.direction,s.file.name,s.session_uuid,peer_uuid)
      self.sessions[s.session_uuid] = s
      return s.session_uuid

   def start_file_upload(self,filename, peer_uuid, opts):
      return self._start_session(filename,peer_uuid,opts,'upload')
   
   def start_file_download(self,filename, peer_uuid, opts):
      return self._start_session(filename,peer_uuid,opts,'download')      

   def upload(self,session_id, data, more):
      s = self._get_session(session_id,'upload')
      try:
         s.file.write(data)
         logger.debug('written %d bytes to file %s, session_uuid: %s',len(data),s.file.name,s.session_uuid)
         s.bytes += len(data)
      except IOError,x:
         logger.warning(str(x))        
         raise DIANE_CORBA.XFileTransferError(str(x))
      if not more:
         s.file.close()
         s.status = "OK"
         self._close_session(s.session_uuid)

   def download(self,session_id):
      s = self._get_session(session_id,'download')
      try:
         data = s.file.read(s.opts.CHUNK_SIZE)
         s.bytes += len(data)
         logger.debug('read %d bytes from file %s, session_uuid: %s',len(data),s.file.name,s.session_uuid)         
      except IOError,x:
         logger.warning(str(x))        
         raise DIANE_CORBA.XFileTransferError(str(x))
      if not data:
         s.file.close()
         s.status = "OK"         
         self._close_session(s.session_uuid)         
      return data

   def _get_session(self,session_id,direction):
      try:
         s = self.sessions[session_id]
         if s.direction != direction:
             msg = 'wrong transfer mode, session is open for %s, session_uuid=%s'%(s.direction,session_id)
             logger.error(msg)
             raise DIANE_CORBA.XFileTransferError(msg)
         s.last_update=time.time()
         return s
      except KeyError:
          msg='non-existing session id: %s'%session_id
          logger.error(msg)
          logger.error(format_exc())
          raise DIANE_CORBA.XFileTransferError(msg)

   def _close_session(self,session_id):
      try:
         s = self.sessions[session_id]
         s.finish_time = time.time()
         logger.info('closing %s session, status=%s: file=%s, bytes=%d, elapsed_time=%.3fs, session_uuid=%s ',s.direction,s.status,s.file.name,s.bytes,s.finish_time-s.start_time, s.session_uuid )
         del self.sessions[session_id]
         self.close_session_hook(session_id)
      except KeyError:
         logger.debug('while closing session: %s already closed or non-existing',session_id)

   def ping(self,peer_uuid,cookie):
      logger.info('they ping me...')
      return True

   def kill(self):
      logger.info('they kill me...')
      self.server.stop()
      return True

   def getStatusPickle(self):
      logger.info('getStatusPickle...')      
      import pickle
      return pickle.dumps(self.sessions)

   def createServant(server):
      from omniORB.BiDirPolicy import BIDIRECTIONAL_POLICY_TYPE, BOTH
      from omniORB.PortableServer import PERSISTENT, USER_ID, RETAIN
      policy = [server.root_poa.create_lifespan_policy(PERSISTENT),
                server.root_poa.create_id_assignment_policy(USER_ID),
                server.root_poa.create_servant_retention_policy(RETAIN),
                server.orb.create_policy(BIDIRECTIONAL_POLICY_TYPE,BOTH)
                ]
      poa = server.root_poa.create_POA("FileTransferServerPOA",server.root_poa_manager,policy)
      poa._get_the_POAManager().activate()
      file_servant = FileTransferServer(server)
      oid = "FileTransferServer"
      poa.activate_object_with_id(oid,file_servant)        
      fs = file_servant._this()
      return file_servant
   createServant = staticmethod(createServant)
  
def main(ior_filename,program=None):
   autostart = False
   if program is None:
      from diane.CORBAProgram import CORBAProgram
      program = CORBAProgram(server=True) #config_file=omniorb_config_filename,enable_GSI=enable_GSI)
      autostart = True

   file_servant = FileTransferServer.createServant(program)
   file_servant.cleanup_thread.start()
   file_server = file_servant._this()
   file(ior_filename,"w").write(program.orb.object_to_string(file_server))
   logger.info("FileTransferServer activated, IOR in file %s",ior_filename)

   if autostart:
      program.start()

   # add a servant reference for local use
   file_server.servant = file_servant

   return file_server
