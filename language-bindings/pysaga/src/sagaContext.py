#sagaContext.py
from sagaInterfaces import Attributes, Object, NSEntry, NSDirectory, Async, Permissions
from org.ogf.saga.context import ContextFactory, Context

class Context(object):

    def __init__(self, type="", clone=False):
        if type=="" and not clone:
            self.context = ContextFactory.createContext()
        elif type != "" and clone==False:
            self.context = ContextFactory.createContext(type)
        else:
            pass
        
    def set_defaults():
        context.setDefaults()
        
    #Attributes interface
    #Object Interface
    #Checking of types of objects
    #Checking all the exceptions    
        
        
        
        
