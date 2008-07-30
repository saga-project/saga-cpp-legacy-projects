import os
import sys
import imp

library = None

try:
    library = os.environ["SAGA_IMPLEMENTATION_NAME"]
except:
    message =  "No \"SAGA_IMPLEMENTATION_NAME\" evironment variable available. Cannot import the language binding implementation"
    raise ImportError, message    

try:
    exec "from " + library + " import URL"
    exec "from " + library + " import File, Flags, Directory"
    exec "from " + library + " import Session"
    exec "from " + library + " import Context"
    exec "from " + library + " import SagaException, NotImplemented, IncorrectURL, BadParameter, " +\
                               "AlreadyExists, DoesNotExist, IncorrectState, PermissionDenied, " +\
                               "AuthorizationFailed, AuthenticationFailed, Timeout, NoSuccess"
    exec "from " + library + " import Object, Async, Attributes, NSEntry, NSDirectory, Permissions, Buffer"
    exec "from " + library + " import Attribute "
except (ImportError, TypeError), e:
    print e
    #message = "Library \""+library+"\", which should contain the language binding implementation, cannot be found and imported. Check the \"SAGA_IMPLEMENTATION_NAME\" evironment variable or the paths"
    #raise ImportError, message  
    raise ImportError, e











#print library
#execcode  = "from sagaFile import File"
#exec execcode
#exec "from sagaURL import URL"
#sagaFile = __import__('sagaFile' , globals(), locals(), ['File'], -1)
#globals()["Bestand"] = sagaFile.Bestand
#globals()["File"] = sagaFile.File

#file, path, desc =  imp.find_module("sagaFile")
#print file,path,desc
#s = imp.load_module("sagaFile", file, path, desc)

#file, path, desc =  imp.find_module("File")
#print file,path,desc
#s = imp.load_module("sagaFile", file, path, desc)
#for param in os.environ.keys():
#    print "%20s %s" % (param,os.environ[param])

#from spam.ham import eggs
#__import__('spam.ham', globals(), locals(), ['eggs'], -1)

#import sagaURL
#from sagaFile import File
#from sagaURL import URL
#print sys.modules["sagaURL"]


#import_string = "from sagaFile import File"
#my_import("sagaFile.File")


#name = "sagaFile"
#fp, pathname, description = imp.find_module(name)
#try:
#    imp.load_module(name, fp, pathname, description)
#finally:
#    # Since we may exit via an exception, close fp explicitly.
#        fp.close()


#from sagaFile import File
#from sagaURL import URL
#print sys.modules["sagaFile"]
#print sys.modules["sagaURL"]

#import_string = "from sagaFile import File"
#my_import("sagaFile.File")




