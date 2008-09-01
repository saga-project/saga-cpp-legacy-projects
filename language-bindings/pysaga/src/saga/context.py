#GFD-R-P.90 section 3.6, page 81,  package saga.context
from object import Object, ObjectType
from attributes import Attributes
#from org.ogf.saga.context import ContextFactory, Context

class Context(Object, Attributes):
    _context = None

    def __init__(self, type=""):
        if type=="":
            #self._context = org.ogf.saga.context.ContextFactory.createContext();
            pass
        else:
            #self._context = org.ogf.saga.context.ContextFactory.createContext(type);
            pass
        
    def set_defaults(self):
        #_context.setDefaults();
        pass

# add everything from Object
# add everything form Attributes