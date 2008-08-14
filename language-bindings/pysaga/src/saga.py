import os
import sys
import imp

library = None

try:
    library = os.environ["SAGA_IMPLEMENTATION_NAME"]
except:
    message =  "No \"SAGA_IMPLEMENTATION_NAME\" evironment variable available. Cannot import the language binding implementation"
    raise ImportError, message    

#try:
exec "from " + library + " import ObjectType, Object"
exec "from " + library + " import URL"
exec "from " + library + " import File, Flags, Directory"
exec "from " + library + " import Session"
exec "from " + library + " import Context"
exec "from " + library + " import SagaException, NotImplemented, IncorrectURL, BadParameter, " +\
                               "AlreadyExists, DoesNotExist, IncorrectState, PermissionDenied, " +\
                               "AuthorizationFailed, AuthenticationFailed, Timeout, NoSuccess"
exec "from " + library + " import Buffer"
exec "from " + library + " import Attributes "
exec "from " + library + " import Callback, Monitorable, Steerable "
exec "from " + library + " import Permissions, Permission "
exec "from " + library + " import State, WaitMode, Async"
exec "from " + library + " import Flags, NSEntry, NSDirectory"
#except (ImportError, TypeError), e:
#    print e
#    message = "Library \""+library+"\", which should contain the language binding implementation, cannot be found and imported. Check the \"SAGA_IMPLEMENTATION_NAME\" evironment variable or the paths"
#    raise ImportError, message  
#    raise ImportError, e