import os,sys
import time
import pickle

from config import config

from diane.util.compatibility import uuid

import diane
logger = diane.getLogger('FileTransfer.Client')

def unique_md5sum_hexdigest(name,opts=None):
   """ Return md5 checksum of a file. If a file does not exist or there is a read error
   return a globally unique identifier.
   """
   if opts is None:
      opts = FileTransferOptions()
   try:
      f = file(name,'r')
      import md5
      m = md5.new()
      data = f.read(opts.CHUNK_SIZE)
      while bool(data):
         m.update(data)
         data = f.read(opts.CHUNK_SIZE)
      m.update(data)
      return m.hexdigest()
   except IOError,x:
      logger.debug('md5sum: %s',str(x))
      return uuid()


class FileTransferOptions:
   """ Currently md5 related options and overwrite flag are applied on the server side only:
        - md5_lazy avoids unnecessary downloads (but not uploads),
        - overwrite will protect the destination file at the server (so for file upload) but not at the client for file download.
   """
   def __init__(self,**kwds):
      self.CHUNK_SIZE = config.DEFAULT_CHUNK_SIZE
      self.md5_correct = False #FIXME: NOT IMPLEMENTED YET
      self.md5_lazy = False
      self.overwrite = False
      self._md5_hexdigest = "" # FIXME: this should be moved to explicit remote call argument list
      for o in kwds:
         setattr(self,o,kwds[o])


class FileTransferClient:

   # a shortcut which allows to create new options object simply by using the client obj reference
   # without a need to explicitly import the FileTransferOptions class from this module
   Options = FileTransferOptions

   def __init__(self,file_server,_uuid=None):
      if _uuid is None:
         self.uuid = uuid()
      else:
         self.uuid = _uuid
      self.file_server = file_server
      
   def upload(self,local_name,remote_name="",opts=None):
      """ Upload a file to the server. If remote_name is not specified it will be equal to the local file name.
      """
      if opts is None:
         opts = FileTransferOptions()
      if not remote_name:
         remote_name = local_name
      logger.info('uploading %s->%s',local_name,remote_name)
      if opts.md5_lazy or opts.md5_correct:
         opts._md5_hexdigest = unique_md5sum_hexdigest(local_name,opts)
      logger.debug('attempt to start a upload session, filename=%s, peer_uuid=%s', remote_name, self.uuid)         
      session_uuid = self.file_server.start_file_upload(remote_name,self.uuid,pickle.dumps(opts))
      f = file(local_name,'r')
      data = f.read(opts.CHUNK_SIZE)
      while bool(data):
         self.file_server.upload(session_uuid,data,True)
         logger.debug('uploaded %d bytes, session_uuid=%s',len(data),session_uuid)
         data = f.read(opts.CHUNK_SIZE)
      self.file_server.upload(session_uuid,data,False)
      logger.debug('uploaded %d bytes, session_uuid=%s',len(data),session_uuid)
      logger.info('upload OK')
      
   def download(self,local_name,remote_name="",opts=None):
      """ Download a file from the server. If remote_name is not specified it will be equal to the local file name.
      """      
      if opts is None:
         opts = FileTransferOptions()
      if not remote_name:
         remote_name = local_name
      logger.info('downloading %s<-%s',local_name,remote_name)
      if opts.md5_lazy or opts.md5_correct:
         opts._md5_hexdigest = unique_md5sum_hexdigest(local_name,opts)
      logger.debug('attempt to start a download session, filename=%s, peer_uuid=%s', remote_name, self.uuid)
      session_uuid = self.file_server.start_file_download(remote_name,self.uuid,pickle.dumps(opts))
      if not session_uuid:
         logger.info('transfer not needed')
         return
      f = file(local_name,'w')
      while 1:
         data = self.file_server.download(session_uuid)
         logger.debug('downloaded %d bytes,session_uuid=%s',len(data),session_uuid)
         if not data:
            break
         f.write(data)
      logger.info('download OK')
      f.close()

