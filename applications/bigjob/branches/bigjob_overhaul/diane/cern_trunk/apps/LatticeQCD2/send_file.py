#!/usr/bin/env python

def main():
    import sys,os

    if len(sys.argv)<2:
        print 'arguments: [up|down] list of files to transfer'
        print
        print 'default server URL: http://lcgui003.cern.ch:4088 (controlled by XML_FILE_SERVER_URL environment variable)'
        return 1

    try:
        url = os.environ['XML_FILE_SERVER_URL']
    except KeyError:
        url = 'http://lcgui003.cern.ch:4088'

    direction = sys.argv[1]

    flist = sys.argv[2:]

    print '%sloading files %s to server %s'%(direction,flist,url)

    c = FileTransferClient(url)
    do_transfer = getattr(c,"%sload_file"%direction)
    
    for name in flist:
        print 'transferring file:',name
        do_transfer(name)
        print 'transfer completed:',name

def createFileTransferClient(url,program=None):
        http_tag = 'http://'
        if url.find(http_tag) == 0:
            return XMLRPCClientTransferClient(url)
        return DIANEFileTransferClient(url,program)
            
class DIANEFileTransferClient:
    def __init__(self,ior,program):
        #from diane.CORBAProgram import CORBAProgram
        #program = CORBAProgram() #config_file=omniorb_config_filename,enable_GSI=enable_GSI)
        file_server=program.orb.string_to_object(ior)
        import DIANE_CORBA, CORBA
        from diane.FileTransfer import FileTransferClient   
        file_server=file_server._narrow(DIANE_CORBA.FileTransferServer)
        self.client = FileTransferClient(file_server)
        self.opts=None
        ## MD5OPTS - BEGIN
        from diane.FileTransfer import FileTransferOptions
        self.opts = FileTransferOptions(md5_lazy=True)      
        ## MD5OPTS - END
        
    def upload_file(self,name,dest=None,opts=None):
        if not opts:
            opts=self.opts
        return self.client.upload(name,dest,opts=opts) ## MD5OPTS
    def download_file(self,name,dest=None):
        if not dest:
            dest=name
        return self.client.download(dest,name,opts=self.opts) ## MD5OPTS

class XMLRPCFileTransferClient:

    def __init__(self,url):
        self.url = url

    def md5sum(self,name):
        import md5
        m = md5.new()
        f = file(name,'r')
        rawbuf = f.read()
        m.update(rawbuf)
        return m,rawbuf
    
    def upload_file(self,name,dest=None):
        import os

        if dest is None:
            dest = os.path.basename(name)

        print 'upload... %s <- %s'%(dest,name)
        
        import xmlrpclib
        s = xmlrpclib.ServerProxy(self.url)

        md5,rawbuf = self.md5sum(name)
        buffer = xmlrpclib.Binary(rawbuf)
        s.send_output(dest,'.',buffer)
        s.check_transfer(dest,'.',md5.hexdigest())

    def download_file(self,name,dest=None,**kwds):
        import os

        if dest is None:
            dest = os.path.basename(name)

        print 'download... %s -> %s'%(name,dest)

        import xmlrpclib
        s = xmlrpclib.ServerProxy(self.url)

        try:
            local_md5 = self.md5sum(name)[0].hexdigest()
        except IOError:
            local_md5 = ""
            
        md5,x = s.download_file(name,local_md5)

        if md5: #if the file was really transfered, then write a new file
            f = file(dest,'w')
            f.write(x.data)
            f.close()
            del x
        else:
            #else just copy the file from source to destination
            print 'md5 checksum of the remote file identical, download skipped, file copied locally. filename=%s md5=%s'%(name,local_md5)
            import shutil
            if name != dest:
                shutil.copy(name,dest)


if __name__ == '__main__':
    main()

####################################################
## # unused, server does not support it currently
## CHUNK_SIZE = 1000000
##
## def send_file_in_chunks(name,url,uuid):
##
##     import xmlrpclib
##     s = xmlrpclib.ServerProxy(url)
##
##     import md5
##
##     m = md5.new()
##
##     f = file(name,'r')
##
##     while 1:
##         print 'transferring file:',name
##         rawbuf = f.read(CHUNK_SIZE)
##         m.update(rawbuf)
##         buffer = xmlrpclib.Binary(rawbuf)
##         s.send_output(uuid,name,buffer,'')
##         if not rawbuf:
##             print 'transfer completed:',name
##             s.send_output(uuid,name,'',m.hexdigest())
##             break


