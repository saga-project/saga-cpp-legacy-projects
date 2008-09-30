# Page 266
#package saga.logical_file

from object import Object, ObjectType
from attributes import Attributes
from namespace import NSEntry, NSDirectory

class Flags(object):
    """
    The Flags class is inherited from the namespace package. A number of replica specific flags are added to it.
    All added flags are used for the opening of logical file and logical directory instances, and are not 
    applicable to the operations inherited from the namespace package.

    """
    NONE              =    0
    OVERWRITE         =    1
    RECURSIVE         =    2
    DEREFERENCE       =    4
    CREATE            =    8
    EXCLUSIVE         =   16
    LOCK              =   32
    CREATEPARENTS     =   64
    #                     128    #reserved for Truncate
    #                     256    #reserved for Append  
    READ              =   512
    WRITE             =   1024
    READWRITE         =   1536 
    #                   2048  #   reserved for Binary

class LogicalFile(NSEntry, Attributes):
    """
    B{To Be Added!}::
      
      Check Indentation!
      CONSTRUCTOR     (in session                 s,
                       in saga::url               name,
                       in int                     flags = Read,
                       out logical_file           obj);
      DESTRUCTOR      (in logical_file            obj);
      // manage the set of  associated replicas
      add_location    (in   saga::url             name);
      remove_location (in   saga::url             name);
      update_location (in   saga::url             name_old,
                       in   saga::url             name_new);
      list_locations (out   array<saga::url>      names);
      // create a new physical replica
      replicate       (in saga::url               name,
                       in int                     flags = None);
      // Attributes (extensible):
      //
      // no attributes pre-defined
    """
    
class LogicalDirectory(NSDirectory, Attributes):
    """
    B{To Be Added!}
    CONSTRUCTOR (in session s, in saga::url name, in int flags = Read, out logical_directory obj);
    DESTRUCTOR      (in   logical_directory     obj);
 
    // inspection methods
    is_file (in saga::url name, out bool test);
    // open methods
    open_dir (in saga::url name, in int flags = Read, out logical_directory   dir);
    open (in saga::url name, in int flags = Read, out logical_file file);
    // find logical files based on name and meta data
    find (in string name_pattern, in array<string> attr_pattern, in int flags = Recursive, out array<saga::url> names    );
    """