# Package: saga
# Module: url 
# Description: The module which specifies the url class used in saga
# Specification and documentation can be found in section 3.3, page 53-58 of the GFD-R-P.90 document
# Author: P.F.A. van Zoolingen, Computer Systems Section, Faculty of Exact Science (FEW), Vrije Universiteit, Amsterdam, The Netherlands.

#DOCUMENT: second constructor delegateObject see IMPL

from object import Object, ObjectType

from org.ogf.saga.url import URLFactory, URL
import java.lang.Exception

class URL(Object):
    """
    URLs are used to reference remote entities.
    @version: 1.0
    """
    delegateObject = None

    def __init__(self, url="", **impl):
        """
        Initialize an URL instance.
        @summary: Initialize an URL instance.
        @param url: initial url to be used
        @type url: string
        @raise NotImplemented:
        @raise BadParameter:
        @raise NoSuccess:
        @Note: if the implementation cannot parse the given url, a 'BadParameter' exception is raised.
        @note: this constructor will never raise an 'IncorrectURL' exception, as the
            interpretation of the URL is not part of this class' functionality.   

        """
        if "delegateObject" in impl:
            if type(impl["delegateObject"]) is not org.ogf.saga.url.URL:
                raise BadParameter, "Parameter impl[\"delegateObject\"] is not a org.ogf.saga.url.URL. Type: " + str(type(impl["delegateObject"]))
            self.delegateObject = impl["delegateObject"]
        else:
            if type(url) is not str:
                raise BadParameter, "Parameter url is not a string. Type: " + str(type(url))
            try:
                self.delegateObject = URLFactory.createURL(url)
            except java.lang.Exception, e:
                raise self.convertException(e)
             
        
    def set_string (self, url = ""):
        """
        Set a new url.
        @summary: Set a new url.
        @param url: the new url
        @type url: string
        @raise NotImplemented:
        @raise BadParameter:
        @note: the method is semantically equivalent to destroying the url, and re-creating it with
            the given parameter.
        @note: the notes for __init__() apply.

        """
        if type(url) is not str:
            raise BadParameter, "Parameter url is not a string. Type: " + str(type(url))
        #TODO: document that if _init_ raises an error the object is not created
        try:
            self.delegateObject.setString(url)
        except java.lang.Exception, e:
            raise self.convertException(e)
    
    def get_string (self):
        """
        Retrieve the url as string.
        @summary: Retrieve the url as string.
        @return: the url
        @rtype: string
        @raise NotImplemented:
        @Note: the URL may be empty, e.g. after creating the instance with an empty url parameter.

        """
        try:
            return self.delegateObject.getString()
        except java.lang.Exception, e:
            raise self.convertException(e)

    def set_scheme (self, scheme = ""):
        """
        Set the scheme of the url.
        @summary: Set the scheme of the url.
        @param scheme: new url scheme
        @type scheme: string
        @postCondition: the scheme part of the URL is updated.
        @raise NotImplemented:
        @raise BadParameter:
        @Note: the given parameter is parsed, and if it is either not well formed (see RFC-3986), or the
            implementation cannot handle it, a 'BadParameter' exception is raised.
        @note: if the given parameter is empty, it is removed from the URL. If that results in an invalid
            URL, a 'BadParameter' exception is raised.
        
        """
        if type(scheme) is not str:
            raise BadParameter, "Parameter scheme is not a string. Type: " + str(type(scheme))
        try:
            self.delegateObject.setScheme(scheme)
        except java.lang.Exception, e:
            raise self.convertException(e)
        
    def get_scheme(self):
        """
        Get the scheme of the url.
        @summary: Get the scheme of the url.
        @return: the scheme of the url
        @rtype: string
        @raise NotImplemented:
        @Note: the returned scheme is either empty, or guaranteed to be well formed (see RFC-3986).

        """
        try:
            return self.delegateObject.getScheme()
        except java.lang.Exception, e:
            raise self.convertException(e)
    
    def set_host (self, host = ""):
        """
        Set the host of the url.
        @summary: Set the host of the url.
        @param host: new url host
        @type host: string
        @postCondition: the host part of the URL is updated.
        @raise NotImplemented:
        @raise BadParameter:
        @Note: the given parameter is parsed, and if it is either not well formed (see RFC-3986), or the
            implementation cannot handle it, a 'BadParameter' exception is raised.
        @note: if the given parameter is empty, it is removed from the URL. If that results in an invalid
            URL, a 'BadParameter' exception is raised.
        
        """
        if type(host) is not str:
            raise BadParameter, "Parameter host is not a string. Type: " + str(type(host))
        try:
            self.delegateObject.setHost(host)
        except java.lang.Exception, e:
            raise self.convertException(e)
        
    def get_host (self):
        """
        Get the host of the url.
        @summary: Get the host of the url.
        @return: the host of the url
        @rtype: string
        @raise NotImplemented:
        @Note: the returned host is either empty, or guaranteed to be well formed (see RFC-3986).
        
        """
        try:
            return self.delegateObject.getHost()
        except java.lang.Exception, e:
            raise self.convertException(e)

    def set_port (self, port=-1):
        """
        Set the port of the url.
        @summary: Set the port of the url.
        @param port: new url port
        @type port: int
        @postCondition: the port part of the URL is updated.
        @raise NotImplemented:
        @raise BadParameter:
        @Note: the given parameter is parsed, and if it is either not well formed (see RFC-3986), or the
            implementation cannot handle it, a 'BadParameter' exception is raised.
        @note: if the given parameter is empty, it is removed from the URL. If that results in an invalid
            URL, a 'BadParameter' exception is raised.
        
        """        
        if type(port) is not int:
            raise BadParameter, "Parameter port is not an int. Type: " + str(type(port))
        try:
            self.delegateObject.setPort(port)
        except java.lang.Exception, e:
            raise self.convertException(e)
    
    def get_port (self):
        """
        Get the port of the url.
        @summary: Get the port of the url.
        @return: the port of the url
        @rtype: int
        @raise NotImplemented:
        @Note: the returned port is either empty, or guaranteed to be well formed (see RFC-3986).
        
        """
        try:
            return self.delegateObject.getPort()
        except java.lang.Exception, e:
            raise self.convertException(e)
    
    def set_fragment (self, fragment = ""):
        """
        Set the fragment of the url.
        @summary: Set the fragment of the url.
        @param fragment: new url fragment
        @type fragment: string
        @postCondition: the fragment part of the URL is updated.
        @raise NotImplemented:
        @raise BadParameter:
        @Note: the given parameter is parsed, and if it is either not well formed (see RFC-3986), or the
            implementation cannot handle it, a 'BadParameter' exception is raised.
        @note: if the given parameter is empty, it is removed from the URL. If that results in an invalid
            URL, a 'BadParameter' exception is raised.
        
        """
        if type (fragment) is not str:
            raise BadParameter, "Parameter fragment is not a string. Type: " + str(type(fragment))
        try:
            self.delegateObject.setFragment(fragment)
        except java.lang.Exception, e:
            raise self.convertException(e)      
        
    def get_fragment (self):
        """
        Get the fragment of the url.
        @summary: Get the fragment of the url.
        @return: the fragment of the url
        @rtype: string
        @raise NotImplemented:
        @Note: the returned fragment is either empty, or guaranteed to be well formed (see RFC-3986).
        
        """
        try:
            return self.delegateObject.getFragment()
        except java.lang.Exception, e:
            raise self.convertException(e)       
     
    def set_path (self, path = ""):
        """
        Set the path of the url.
        @summary: Set the path of the url.
        @param path: new url path
        @type path: string
        @postCondition: the path part of the URL is updated.
        @raise NotImplemented:
        @raise BadParameter:
        @Note: the given parameter is parsed, and if it is either not well formed (see RFC-3986), or the
            implementation cannot handle it, a 'BadParameter' exception is raised.
        @note: if the given parameter is empty, it is removed from the URL. If that results in an invalid
            URL, a 'BadParameter' exception is raised.
        
        """        
        if type (path) is not str:
            raise BadParameter, "Parameter path is not a string. Type: " + str(type(path))
        try:
            self.delegateObject.setPath(path)
        except java.lang.Exception, e:
            raise self.convertException(e)
    
    def get_path (self):
        """
        Get the path of the url.
        @summary: Get the path of the url.
        @return: the path of the url
        @rtype: string
        @raise NotImplemented:
        @Note: the returned path is either empty, or guaranteed to be well formed (see RFC-3986).
        
        """
        try:
            return self.delegateObject.getPath()
        except java.lang.Exception, e:
            raise self.convertException(e)

    def set_query (self, query = ""):
        """
        Set the query of the url.
        @summary: Set the query of the url.
        @param query: new url query
        @type query: string
        @postCondition: the query part of the URL is updated.
        @raise NotImplemented:
        @raise BadParameter:
        @Note: the given parameter is parsed, and if it is either not well formed (see RFC-3986), or the
            implementation cannot handle it, a 'BadParameter' exception is raised.
        @note: if the given parameter is empty, it is removed from the URL. If that results in an invalid
            URL, a 'BadParameter' exception is raised.
        
        """        
        if type (query) is not str:
            raise BadParameter, "Parameter query is not a string. Type: " + str(type(query))
        try:
            self.delegateObject.setQuery(query)
        except java.lang.Exception, e:
            raise self.convertException(e)
    
    def get_query (self):
        """
        Get the query of the url.
        @summary: Get the query of the url.
        @return: the query of the url
        @rtype: string
        @raise NotImplemented:
        @Note: the returned query is either empty, or guaranteed to be well formed (see RFC-3986).
        
        """
        try:
            return self.delegateObject.getQuery()
        except java.lang.Exception, e:
            raise self.convertException(e)
    
    def set_userinfo (self, userinfo = ""):
        """
        Set the userinfo of the url.
        @summary: Set the userinfo of the url.
        @param userinfo: new url userinfo
        @type userinfo: string
        @postCondition: the userinfo part of the URL is updated.
        @raise NotImplemented:
        @raise BadParameter:
        @Note: the given parameter is parsed, and if it is either not well formed (see RFC-3986), or the
            implementation cannot handle it, a 'BadParameter' exception is raised.
        @note: if the given parameter is empty, it is removed from the URL. If that results in an invalid
            URL, a 'BadParameter' exception is raised.
        
        """       
        if type (userinfo) is not str:
            raise BadParameter, "Parameter userinfo is not a string. Type: " + str(type(userinfo))
        try:
            self.delegateObject.setUserInfo(userinfo)
        except java.lang.Exception, e:
            raise self.convertException(e)        
    
    def get_userinfo (self):
        """
        Get the userinfo of the url.
        @summary: Get the userinfo of the url.
        @return: the userinfo of the url
        @rtype: string
        @raise NotImplemented:
        @Note: the returned userinfo is either empty, or guaranteed to be well formed (see RFC-3986).
        
        """
        try:
            return self.delegateObject.getUserInfo()
        except java.lang.Exception, e:
            raise self.convertException(e)
        
    def translate (self, scheme):
        #return url
        """
        Translate an URL to a new scheme.
        @summary: Translate an URL to a new scheme.
        @param scheme: the new scheme to translate into
        @type scheme: string
        @return: URL representation of the translated string
        @rtype: L{URL}
        @raise NotImplemented:
        @raise BadParameter:
        @raise NoSuccess:
        @Note: if the scheme is not supported, a 'BadParameter' exception is raised.
        @Note: if the scheme is supported, but the url cannot be translated to the scheme, a
            'NoSuccess' exception is raised.
        @Note: if the url can be translated, but cannot be handled with the new scheme anymore, no
            exception is raised. That can only be detected if the returned string is again used
            in a URL contructor, or with set_string().
        @Note: the call does not change the URL represented by the class instance itself, but the
            translation is only reflected by the returned URL object.
        @Note: the notes from section 'The URL Problem' of the GFD-R-P.90 document apply.
        
        """
        if type (scheme) is not str:
            raise BadParameter, "Parameter scheme is not a string. Type: " + str(type(scheme))
        try:
            tempObject = self.delegateObject.translate(scheme)
            return URL(tempObject.getString())
        except java.lang.Exception, e:
            raise self.convertException(e)    
        
###################################
#    Inherited from Object        #
###################################

#TODO: Object methodes in URL

    def get_id(self):
        """
        Query the object ID.
        @summary: Query the object ID.
        @return: uuid for the object
        @rtype: string 
        """
        from saga.error import NotImplemented
        error = NotImplemented("get_id() is not yet implemented in this object")
        raise error
        # raise NotImplemented, "get_id() is not yet implemented in this object"
     
    def get_type(self):
        """
        Query the object type.
        @summary: Query the object type.
        @return: type of the object as an int from ObjectType
        @rtype: int
        """
        return ObjectType.URL
        
    def get_session(self):
        """
        Query the objects session.
        @summary: Query the objects session.
        @return: session of the object
        @rtype: L{Session}
        @PreCondition: the object was created in a session, either
            explicitly or implicitly.
        @PostCondition: the returned session is shallow copied.
        @raise DoesNotExist:
        @Note: if no specific session was attached to the object at creation time, 
            the default SAGA session is returned.
        @note: some objects do not have sessions attached, such as JobDescription, Task, Metric, and the
            Session object itself. For such objects, the method raises a 'DoesNotExist' exception.
        """
        from saga.error import NotImplemented
        raise NotImplemented, "get_session() is not yet implemented in this object"
    
    def clone(self):
        """
        @summary: Deep copy the object
        @return: the deep copied object
        @rtype: L{Object}
        @PostCondition: apart from session and callbacks, no other state is shared
            between the original object and it's copy.
        @raise NoSuccess:
        @Note: that method is overloaded by all classes which implement saga.object.Object, and returns
                 a deep copy of the respective class type.
        @see: section 2 of the GFD-R-P.90 document for deep copy semantics.

        """
        from saga.error import NotImplemented
        raise NotImplemented, "clone() is not yet implemented in this object"
    