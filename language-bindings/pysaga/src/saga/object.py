# package saga.object
from error import NotImplemented

class ObjectType(object):
  EXCEPTION         =  1
  URL               =  2
  BUFFER            =  3
  SESSION           =  4
  CONTEXT           =  5
  TASK              =  6
  TASKCONTAINER     =  7
  METRIC            =  8
  NSENTRY           =  9
  NSDIRECTORY       = 10
  IOVEC             = 11
  FILE              = 12
  DIRECTORY         = 13
  LOGICALFILE       = 14
  LOGICALDIRECTORY  = 15
  JOBDESCRIPTION    = 16
  JOBSERVICE        = 17
  JOB               = 18
  JOBSELF           = 19
  STREAMSERVICE     = 20
  STREAM            = 21
  PARAMETER         = 22
  RPC               = 23
  
class Object(object):
        
    def get_id(self):
        #return the id
        raise NotImplemented, "get_id() is not implemented in this object"
      
    def get_type(self):
        #return appropriate ObjectType entry
        raise NotImplemented, "get_type() is not implemented in this object"
      
    def get_session(self):
        #return the  appropriate session object
        raise NotImplemented, "get_session() is not implemented in this object"
    
    def clone(self):
        #return clone
        raise NotImplemented, "clone() is not implemented in this object"
    