from sagaErrors import NotImplemented

class ObjectType(object):
  exception         =  1
  url               =  2
  buffer            =  3
  session           =  4
  context           =  5
  task              =  6
  task_container    =  7
  metric            =  8
  nsentry           =  9
  nsdirectory       = 10
  iovec             = 11
  file              = 12
  directory         = 13
  logical_file      = 14
  logical_directory = 15
  job_description   = 16
  Job_service       = 17
  job               = 18
  job_self          = 19
  stream_service    = 20
  stream            = 21
  parameter         = 22
  rpc               = 23
  
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
    