from sagaObject import Object, ObjectType

from org.ogf.saga.session import Session, SessionFactory
from org.ogf.saga.context import ContextFactory, Context

class Session(Object):
    """Doc string"""
    _session
        
    def __init__(self, default=True):
        if default == True:
            self._session = SessionFactory.createSession()
        else:
            self._session = SessionFactory.createSession(False)
        
        
    def add_context(self, context):
        #add checking of type
        self._session.addContext(c.context)

    def remove_context(self, context):
        self._session.removeContext(context)
        
    def list_contexts(self):
        #check if it returns python objects and not java ones
        results = []
        temp = self.session.listContexts()
        for contexts in temp:
            results.append(contexts)
        return results
 
 
    # Methods inherited from Object
    def get_id(self):
        return self._session.getID();
    
    def get_type(self):
        return ObjectType.session
    
    def get_session(self):
        return self
    
    def clone(self):
        s = Session(default=False, clone=True)
        s.session = session.clone()
        return s
    
    # Method inherited from Attributes
    
    

