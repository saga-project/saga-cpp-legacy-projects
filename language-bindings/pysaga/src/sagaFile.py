#For File

from sagaInterfaces import NSEntry, NSDirectory, Async, Permissions, Buffer
from sagaObject import Object
from sagaAttributes import Attributes

import org.ogf.saga.url.URLFactory;
import org.ogf.saga.url.URL;
import org.ogf.saga.namespace.Flags;
import org.ogf.saga.file.File;
import org.ogf.saga.file.FileFactory;
import java.lang.Exception;

class Flags(object):
    none = 0 
    overwrite = 1
    recursive = 2
    dereference = 4
    create = 8
    exclusive = 16
    lock = 32,
    createParents = 64
    truncate = 128
    append = 256
    read = 512
    write = 1024
    readWrite = 1536 
    binary = 2048

class SeekMode(object):
    start = 1
    current = 2
    end = 3

class Iovec(Buffer, Object):
    pass
#CONSTRUCTOR (in array<byte> data = "",
#in int size = 0,
#in int offset = 0,
#in int len_in = size,
#out buffer obj);
#set_offset (in int offset);
#get_offset (out int offset);
#set_len_in (in int len_in);
#get_len_in (out int len_in);
#get_len_out (out int len_out);

class File(Object, NSEntry, Async, Permissions):
    fileObject = None
    
    def __init__(self, session, url_name, flags=Flags.read):
        Object.__init__(self)
        NSEntry.__init__(self)
        Async.__init__(self)
        Permissions.__init__(self)
        # todo: check types
        self.fileObject = org.ogf.saga.file.FileFactory.createFile( url_name.urlObject, org.ogf.saga.namespace.Flags.READ.getValue() );
        #self.fileObject = org.ogf.saga.file.FileFactory.createFile(url2, Flags.read );
        print "sagaFile.File object created"
 
    def __convertException(self, e):
        if isinstance(e, org.ogf.saga.error.AlreadyExistsException):
            print "org.ogf.saga.error.AlreadyExistsException: " + e.getMessage()
        elif isinstance(e, org.ogf.saga.error.AuthenticationFailedException):
            print "org.ogf.saga.error.AuthenticationFailedException: " + e.getMessage()
        elif isinstance(e, org.ogf.saga.error.AuthorizationFailedException):
            print "org.ogf.saga.error.AuthorizationFailedException: " + e.getMessage()
        elif isinstance(e, org.ogf.saga.error.BadParameterException):
            print "org.ogf.saga.error.BadParameterException: " + e.getMessage()
        elif isinstance(e, org.ogf.saga.error.DoesNotExistException):
            print "org.ogf.saga.error.DoesNotExistException: " + e.getMessage()
        elif isinstance(e, org.ogf.saga.error.IncorrectStateException):
            print "org.ogf.saga.error.IncorrectStateException: " + e.getMessage()
        elif isinstance(e, org.ogf.saga.error.IncorrectURLException):
            print "org.ogf.saga.error.IncorrectURLException: " + e.getMessage()
        elif isinstance(e, org.ogf.saga.error.NoSuccessException):
            print "org.ogf.saga.error.NoSuccessException: \n" + e.getMessage()
        elif isinstance(e, org.ogf.saga.error.NotImplementedException):
            print "org.ogf.saga.error.NotImplementedException: " + e.getMessage()
        elif isinstance(e, org.ogf.saga.error.PermissionDeniedException):
            print "org.ogf.saga.error.PermissionDeniedException: " + e.getMessage()
        elif isinstance(e, org.ogf.saga.error.SagaIOException):
            print "org.ogf.saga.error.SagaIOException: " + e.getMessage()
        elif isinstance(e, org.ogf.saga.error.TimeoutException):
            print "org.ogf.saga.error.TimeoutException: " + e.getMessage()
        else:
            print "Unknown other java.exception " + e.getMessage() 
     
    def copy(self, url_target, flags=0):
        """override from NSEntry"""
        print type(url_target.urlObject) 
        try:
            self.fileObject.copy(url_target.urlObject, flags);
        except java.lang.Exception, exception:
            self.__convertException(e=exception)
        print "file.copy finished"
        # org.ogf.saga.namespace.Flags.OVERWRITE.getValue());
 


    def get_size(self):
        pass
    
#    def read (inout buffer buf, in int len_in = -1, out int len_out ):
#    def write (in buffer buf, in int len_in = -1, out int len_out ):

    def seek (self, offset, seek_mode):
        #return out int position
        pass
    
#    def read_v (inout array<iovec> iovecs ):
#    def write_v (inout array<iovec> iovecs ):
    
    def size_p (self, pattern):
        #return out int size
        pass
    
#    def read_p (in string pattern, inout buffer buf, out int len_out ):
#    def write_p (in string pattern, in buffer buf, out int len_out ):

#    def modes_e (out array<string> emodes ):

    def size_e (self, emode, spec):
        #return out int size
        pass

#    def read_e (in string emode, in string spec, inout buffer buf, out int len_out );

    def write_e (self, emode, spec, buf):
        #return out int len_out
        pass

class Directory(NSDirectory, NSEntry, Object, Async, Permissions):
    def __init__(self, name, session, flags=Flags.read):
        NSDirectory.__init(self)
        Object.__init__(self)
        NSEntry.__init__(self)
        Async.__init__(self)
        Permissions.__init__(self)
        print "sagaFile.Directory object created"
        
    def get_size(self, name, flags = None):
        #return size
        pass
    
    def is_file (self, name, flags = None):
        #return boolean test
        pass

    def open_dir (self, name, flags = Flags.read):
        #return out directory dir
        pass

    def open (self, name, flags = Flags.read):
        #return out file file
        pass