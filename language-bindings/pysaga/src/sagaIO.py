#page 62
from sagaObject import ObjectType, Object

#use jython's JArray to create a byte[]. but needs to 
#update the application buffer then after each mutation....

##or use (immutable) string or something?

class Buffer(Object):
    CONSTRUCTOR (in array<byte> data,
                 in int           size,
                 out buffer       obj);
    CONSTRUCTOR (in int           size = -1,
                 out buffer       obj);
    DESTRUCTOR (in buffer         obj);

    set_size    (in int           size = -1)
    get_size    (out int          size);
    set_data    (in array<byte>   data,
                 in int           size);
    get_data    (out array<byte>  data);
    close       (in  float        timeout = -0.0);
  }
}
