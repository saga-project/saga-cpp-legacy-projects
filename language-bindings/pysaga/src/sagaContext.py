#GFD-R-P.90 section 3.6, page 81,  package saga.context
from sagaObject import Object
from sagaAttributes import Attributes
from org.ogf.saga.context import ContextFactory, Context

class Context(Object, Attributes):
    _context

    def __init__(self, type=""):
        if type=="":
            self._context = org.ogf.saga.context.ContextFactory.createContext();
        else:
            self._context = org.ogf.saga.context.ContextFactory.createContext(type);
        
        
    def set_defaults(self):
        _context.setDefaults();

# add everything from Object
# add everything form Attributes