from sagaObject import Object
#for URL
import org.ogf.saga.url.URLFactory;
import org.ogf.saga.url.URL;

class URL(Object):
    urlObject = None

    def __init__(self, url=""):
        """The URL object"""
        Object.__init__(self)
        self.urlObject =  org.ogf.saga.url.URLFactory.createURL(url);
        print "sagaJavaLB.URL object created"
        
    def set_string (self, url = ""):
        pass
    
    def get_string (self):
        pass

    def set_scheme (self, scheme = ""):
        pass

    def get_scheme (self):
        pass
    
    def set_host (self, host = ""):
        pass

    def get_host (self):
        pass

    def set_port (self, port=-1):
        pass
    
    def get_port (self):
        pass
    
    def set_fragment (self, fragment = ""):
        pass
    
    def get_fragment (self):
        pass
     
    def set_path (self, path = ""):
        pass
    
    def get_path (self):
        pass

    def set_query (self, query = ""):
        pass
    
    def get_query (self):
        pass
    
    def set_userinfo (self, userinfo = ""):
        pass
    
    def get_userinfo (self):
        pass
        
    def translate (self, scheme):
        #return url
        pass