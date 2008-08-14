# Page 266

package saga.logical_file
  {
    enum flags
    {
      None            =    0, // same as in namespace::flags
      Overwrite       =    1, // same as in namespace::flags
      Recursive       =    2, // same as in namespace::flags
      Dereference     =    4, // same as in namespace::flags
      Create          =    8, // same as in namespace::flags
      Exclusive       =   16, // same as in namespace::flags
      Lock            =   32, // same as in namespace::flags
      CreateParents   =   64, // same as in namespace::flags
      //                 128,    reserved for Truncate
      //                 256,    reserved for Append
      Read            = 512,
      Write           = 1024,
      ReadWrite       = 1536, // Read | Write
      //                2048     reserved for Binary

    }
    class logical_file : extends           saga::ns_entry
                          implements      saga::attributes
                      // from ns_entry    saga::object
                      // from ns_entry    saga::async
                      // from object      saga::error_handler
    {
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
    }
    class logical_directory : extends                saga::ns_directory
                                implements           saga::attributes
                            // from ns_directory     saga::ns_entry
                            // from ns_entry         saga::object
                            // from ns_entry         saga::async
                            // from object           saga::error_handler
    {
      CONSTRUCTOR     (in   session               s,
                       in   saga::url             name,
                       in   int                   flags = Read,
                       out  logical_directory     obj);
      DESTRUCTOR      (in   logical_directory     obj);
                                                                     268
saga-core-wg@ogf.org
GFD-R-P.90          SAGA Replica Management        January 15, 2008
     // inspection methods
     is_file         (in saga::url            name,
                      out boolean             test);
     // open methods
     open_dir        (in saga::url            name,
                      in int                  flags = Read,
                      out logical_directory   dir);
     open            (in saga::url            name,
                      in int                  flags = Read,
                      out logical_file        file);
     // find logical files based on name and meta data
     find            (in string               name_pattern,
                      in array<string>        attr_pattern,
                      in int                  flags = Recursive,
                      out array<saga::url>    names    );
   }
 }
