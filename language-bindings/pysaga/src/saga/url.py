from object import Object, ObjectType
from error import NotImplemented
#for URL
#import org.ogf.saga.url.URLFactory;
#import org.ogf.saga.url.URL;

class URL(Object):
    """
    URLs are used to reference remote entities.
    """
    #urlObject = None

    def __init__(self, url=""):
        """
        Initialize an URL instance
        @summary: initialize an URL instance
        @param url: initial url to be used
        @type url: string
        @raise NotImplemented:
        @raise BadParameter:
        @raise NoSuccess:
        @Note: if the implementation cannot parse the given url, a 'BadParameter' exception is thrown.
        @note: this constructor will never throw an 'IncorrectURL' exception, as the
                  interpretation of the URL is not part of this class' functionality.   
        """
        Object.__init__(self)
        #self.urlObject =  org.ogf.saga.url.URLFactory.createURL(url);
        #print "sagaJavaLB.URL object created"
        
    def set_string (self, url = ""):
        """
        Set a new url
        @summary: set a new url
        @param url: the new url
        @type url: string
        @raise NotImplemented:
        @raise BadParameter:
        @note: the method is semantically equivalent to destroying the url, and re-creating it with
            the given parameter.
        @note: the notes for __init__() apply.

        """
        pass
    
    def get_string (self):
        """
        Retrieve the url as string
        @summary: retrieve the url as string
        @return: the url
        @rtype: string
        @raise NotImplemented:
        @Note: the URL may be empty, e.g. after creating the instance with an empty url parameter.

        """
        pass

    def set_scheme (self, scheme = ""):
        """
        Set the scheme of the url
        @summary: set the scheme of the url
        @param scheme: new url scheme
        @type scheme: string
        @postCondition: the scheme part of the URL is updated.
        @raise NotImplemented:
        @raise BadParameter:
        @Note: the given parameter is parsed, and if it is either not well formed (see RFC-3986), or the
            implementation cannot handle it, a 'BadParameter' exception is thrown.
        @note: if the given parameter is empty, it is removed from the URL. If that results in an invalid
            URL, a 'BadParameter' exception is thrown.
        """
        
        pass

    def get_scheme(self):
        """
        Get the scheme of the url
        @summary: get the scheme of the url
        @return: the scheme of the url
        @rtype: string
        @raise NotImplemented:
        @Note: the returned scheme is either empty, or guaranteed to be well formed (see RFC-3986).

        """
        pass
    
    def set_host (self, host = ""):
        """
        Set the host of the url
        @summary: set the host of the url
        @param host: new url host
        @type host: string
        @postCondition: the host part of the URL is updated.
        @raise NotImplemented:
        @raise BadParameter:
        @Note: the given parameter is parsed, and if it is either not well formed (see RFC-3986), or the
            implementation cannot handle it, a 'BadParameter' exception is thrown.
        @note: if the given parameter is empty, it is removed from the URL. If that results in an invalid
            URL, a 'BadParameter' exception is thrown.
        """
        pass

    def get_host (self):
        """
        Get the host of the url
        @summary: get the host of the url
        @return: the host of the url
        @rtype: string
        @raise NotImplemented:
        @Note: the returned host is either empty, or guaranteed to be well formed (see RFC-3986).
        """
        pass

    def set_port (self, port=-1):
        """
        Set the port of the url
        @summary: set the port of the url
        @param port: new url port
        @type port: int
        @postCondition: the port part of the URL is updated.
        @raise NotImplemented:
        @raise BadParameter:
        @Note: the given parameter is parsed, and if it is either not well formed (see RFC-3986), or the
            implementation cannot handle it, a 'BadParameter' exception is thrown.
        @note: if the given parameter is empty, it is removed from the URL. If that results in an invalid
            URL, a 'BadParameter' exception is thrown.
        """        
        pass
    
    def get_port (self):
        """
        Get the port of the url
        @summary: get the port of the url
        @return: the port of the url
        @rtype: int
        @raise NotImplemented:
        @Note: the returned port is either empty, or guaranteed to be well formed (see RFC-3986).
        """
        pass
    
    def set_fragment (self, fragment = ""):
        """
        Set the fragment of the url
        @summary: set the fragment of the url
        @param fragment: new url fragment
        @type fragment: string
        @postCondition: the fragment part of the URL is updated.
        @raise NotImplemented:
        @raise BadParameter:
        @Note: the given parameter is parsed, and if it is either not well formed (see RFC-3986), or the
            implementation cannot handle it, a 'BadParameter' exception is thrown.
        @note: if the given parameter is empty, it is removed from the URL. If that results in an invalid
            URL, a 'BadParameter' exception is thrown.
        """
        pass
    
    def get_fragment (self):
        """
        Get the fragment of the url
        @summary: get the fragment of the url
        @return: the fragment of the url
        @rtype: string
        @raise NotImplemented:
        @Note: the returned fragment is either empty, or guaranteed to be well formed (see RFC-3986).
        """
        pass
     
    def set_path (self, path = ""):
        """
        Set the path of the url
        @summary: set the path of the url
        @param path: new url path
        @type path: string
        @postCondition: the path part of the URL is updated.
        @raise NotImplemented:
        @raise BadParameter:
        @Note: the given parameter is parsed, and if it is either not well formed (see RFC-3986), or the
            implementation cannot handle it, a 'BadParameter' exception is thrown.
        @note: if the given parameter is empty, it is removed from the URL. If that results in an invalid
            URL, a 'BadParameter' exception is thrown.
        """        
        pass
    
    def get_path (self):
        """
        Get the path of the url
        @summary: get the path of the url
        @return: the path of the url
        @rtype: string
        @raise NotImplemented:
        @Note: the returned path is either empty, or guaranteed to be well formed (see RFC-3986).
        """
        pass

    def set_query (self, query = ""):
        """
        Set the query of the url
        @summary: set the query of the url
        @param query: new url query
        @type query: string
        @postCondition: the query part of the URL is updated.
        @raise NotImplemented:
        @raise BadParameter:
        @Note: the given parameter is parsed, and if it is either not well formed (see RFC-3986), or the
            implementation cannot handle it, a 'BadParameter' exception is thrown.
        @note: if the given parameter is empty, it is removed from the URL. If that results in an invalid
            URL, a 'BadParameter' exception is thrown.
        """        
        pass
    
    def get_query (self):
        """
        Get the query of the url
        @summary: get the query of the url
        @return: the query of the url
        @rtype: string
        @raise NotImplemented:
        @Note: the returned query is either empty, or guaranteed to be well formed (see RFC-3986).
        """
        pass
    
    def set_userinfo (self, userinfo = ""):
        """
        Set the userinfo of the url
        @summary: set the userinfo of the url
        @param userinfo: new url userinfo
        @type userinfo: string
        @postCondition: the userinfo part of the URL is updated.
        @raise NotImplemented:
        @raise BadParameter:
        @Note: the given parameter is parsed, and if it is either not well formed (see RFC-3986), or the
            implementation cannot handle it, a 'BadParameter' exception is thrown.
        @note: if the given parameter is empty, it is removed from the URL. If that results in an invalid
            URL, a 'BadParameter' exception is thrown.
        """       
        pass
    
    def get_userinfo (self):
        """
        Get the userinfo of the url
        @summary: get the userinfo of the url
        @return: the userinfo of the url
        @rtype: string
        @raise NotImplemented:
        @Note: the returned userinfo is either empty, or guaranteed to be well formed (see RFC-3986).
        """
        pass
        
    def translate (self, scheme):
        #return url
        """
        Translate an URL to a new scheme
        @summary: translate an URL to a new scheme
        @param scheme: the new scheme to translate into
        @type scheme: string
        @return: URL representation of the translated string
        @rtype: L{URL<saga.url.URL>}
        @raise NotImplemented:
        @raise BadParameter:
        @raise NoSuccess:
        @Note: if the scheme is not supported, a 'BadParameter' exception is thrown.
        @Note: if the scheme is supported, but the url cannot be translated to the scheme, a
            'NoSuccess' exception is thrown.
        @Note: if the url can be translated, but cannot be handled with the new scheme anymore, no
            exception is thrown. That can only be detected if the returned string is again used
            in a URL contructor, or with set_string().
        @Note: the call does not change the URL represented by the class instance itself, but the
            translation is only reflected by the returned URL object.
        @Note: the notes from section 'The URL Problem' of the GFD-R-P.90 document apply.
        """
        pass