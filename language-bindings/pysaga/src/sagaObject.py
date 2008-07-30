class ObjectType(object):
  Exception        =  1
  URL              =  2
  Buffer           =  3
  Session          =  4
  Context          =  5
  Task             =  6
  TaskContainer    =  7
  Metric           =  8
  NSEntry          =  9
  NSDirectory      = 10
  IOVec            = 11
  File             = 12
  Directory        = 13
  LogicalFile      = 14
  LogicalDirectory = 15
  JobDescription   = 16
  JobService       = 17
  Job              = 18
  JobSelf          = 19
  StreamService    = 20
  Stream           = 21
  Parameter        = 22
  RPC              = 23
  
  class Object(object):
        
      def get_id():
          #return the id
          raise NotImplemented, "get_id() is not implemented in this object"
      
      def get_type():
          #return appropriate ObjectType entry
          raise NotImplemented, "get_type() is not implemented in this object"
      
      def get_session():
          #return the  appropriate session object
          raise NotImplemented, "get_session() is not implemented in this object"
      def clone():
          #return clone
          raise NotImplemented, "clone() is not implemented in this object"