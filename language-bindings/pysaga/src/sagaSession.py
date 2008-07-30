from org.ogf.saga.session import Session, SessionFactory
from org.ogf.saga.context import ContextFactory, Context

class Session(object):
    """Doc string"""
        
    def __init__(self, default=True, clone=False):
        if default and not clone:
            self.session = SessionFactory.createSession()
        elif not default and not clone:
            self.session = SessionFactory.createSession(False)
        else:
            pass
      
    def add_context(self, c):
        #add checking of type
        self.session.addContext(c.context)

    def remove_context(context):
        self.session.removeContext(context)
        
    def list_contexts(self):
        #check if it returns python objects and not java ones
        results = []
        temp = self.session.listContexts()
        for contexts in temp:
            results.append(contexts)
        return results
      
    def get_id(self):
        return session.getID()
    
    def get_type(self):
        # 4 = 'session'
        return 4
    
    def get_session(self):
        return self
    
    def clone(self):
        s = Session(default=False, clone=True)
        s.session = session.clone()
        return s
    
    # Add the attributes interface
    
    

