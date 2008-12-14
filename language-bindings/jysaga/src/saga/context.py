# Package: saga
# Module: context 
# Description: The module which specifies the context classes used in saga
# Specification and documentation can be found in section 3.6, page 81-86 of 
#    the GFD-R-P.90 document
# Author: P.F.A. van Zoolingen, Computer Systems Section, Faculty of 
#    Exact Science (FEW), Vrije Universiteit, Amsterdam, The Netherlands.

from saga.object import Object, ObjectType
from saga.attributes import Attributes
from saga.error import NotImplemented
#from saga.session import Session

from org.ogf.saga.context import ContextFactory, Context
import org.ogf.saga.error.DoesNotExistException
from java.lang import String



class Context(Object, Attributes):
    delegateObject = None
    """
    The saga.context.Context class provides the functionality of a security 
    information container.

        - B{Attributes:} 
            - B{Type}:
                - name: Type
                - desc: type of context
                - mode: ReadWrite
                - type: String
                - value: naming conventions as described in GFD-R-P.90 apply
            - B{Server}:
                - name: Server
                - desc: server which manages the context
                - mode: ReadWrite
                - type: String
                - value: -
                - note: a typical example would be the contact information for 
                    a MyProxy server, such as 'myproxy.remote.net:7512', for 
                    a 'myproxy' type context.
            - B{CertRepository}:
                - name: CertRepository
                -    desc: location of certificates and CA signatures
                -    mode: ReadWrite
                -    type: String
                -    value: -
                -    note: a typical example for a globus type context
                       would be "/etc/grid-security/certificates/".
            - B{UserProxy}:
                -    name: UserProxy
                -    desc: location of an existing certificate proxy to be used
                -    mode: ReadWrite
                -    type: String
                -    value: -
                -    note: a typical example for a globus type context would be 
                    "/tmp/x509up_u<uid>".
            - B{UserCert}:
                -    name: UserCert
                -    desc: location of a user certificate to use
                -    mode: ReadWrite
                -    type: String
                -    value: -
                -    note: a typical example for a globus type context would be 
                    "$HOME/.globus/usercert.pem".
            - B{UserKey}:
                -    name: UserKey
                -    desc: location of a user key to use
                -    mode: ReadWrite
                -    type: String
                -    value: -
                -    note: a typical example for a globus type context would be 
                    "$HOME/.globus/userkey.pem".
            - B{UserID}:
                -    name:  UserID
                -    desc:  user id or user name to use
                -    mode:  ReadWrite
                -    type:  String
                -    value: -
                -    note: a typical example for a ftp type context would be 
                    "anonymous".
            - B{UserPass}:
                -    name:  UserPass
                -    desc:  password to use
                -    mode:  ReadWrite
                -    type:  String
                -    value: -
                -    note: a typical example for a ftp type context would be 
                    "anonymous@localhost".
            - B{UserVO}:
                -    name:  UserVO
                -    desc:  the VO the context belongs to
                -    mode:  ReadWrite
                -    type:  String
                -    value: -
                -    note: a typical example for a globus type context  would 
                    be "O=dutchgrid".
            - B{LifeTime}:
                -    name:  LifeTime
                -    desc:  time up to which this context is valid
                -    mode:  ReadWrite
                -    type:  Int
                -    value: -1
                -    note: time and date specified in number of seconds since 
                    epoch, a value of -1 indicates an infinite lifetime.
            - B{RemoteID}:
                -    name:  RemoteID
                -    desc:  user ID for an remote user, who is identified by 
                    this context.
                -    mode: ReadOnly
                -    type: String
                -    value: -
                -    note: a typical example for a globus type context would be
                      "/O=dutchgrid/O=users/O=vu/OU=cs/CN=Joe Doe".
            - B{RemoteHost}
                -     name: RemoteHost
                -     desc: the hostname where the connection origininates which 
                    is identified by this context.
                -     mode: ReadOnly
                -     type: String
                -     value: -
                -     name: RemotePort
                -     desc: the port used for the connection which is identified 
                    by this context.
                -     mode: ReadOnly
                -     type: String
                -     value: -
    
    @undocumented: get_Type
    @undocumented: set_Type
    @undocumented: del_Type
    @undocumented: get_Server
    @undocumented: set_Server
    @undocumented: del_Server
    @undocumented: get_CertRepository
    @undocumented: set_CertRepository
    @undocumented: del_CertRepository
    @undocumented: get_UserProxy
    @undocumented: set_UserProxy
    @undocumented: del_UserProxy
    @undocumented: get_UserCert
    @undocumented: set_UserCert
    @undocumented: del_UserCert
    @undocumented: get_UserKey
    @undocumented: set_UserKey
    @undocumented: del_UserKey
    @undocumented: get_UserID
    @undocumented: set_UserID
    @undocumented: del_UserID
    @undocumented: get_UserPass
    @undocumented: set_UserPass
    @undocumented: del_UserPass
    @undocumented: get_UserVO
    @undocumented: set_UserVO
    @undocumented: del_UserVO
    @undocumented: get_LifeTime
    @undocumented: set_LifeTime
    @undocumented: del_LifeTime
    @undocumented: get_RemoteID
    @undocumented: set_RemoteID
    @undocumented: del_RemoteID
    @undocumented: get_RemoteHost
    @undocumented: set_RemoteHost
    @undocumented: del_RemoteHost
    @undocumented: get_RemotePort
    @undocumented: set_RemotePort
    @undocumented: del_RemotePort    
    """

    def __init__(self, name="", **impl):
        """
        Initialize a security context.
        @summary: Initialize a security context.
        @param name: initial type of context
        @type name: string
        @raise NotImplemented:
        @raise IncorrectState:
        @raise Timeout:
        @raise NoSuccess:
        @note: if name is given (i.e. non-empty), then the __init__ internally 
            calls set_defaults(). The notes to set_defaults apply.
        @see: the notes about lifetime management in Section 2 of the GFD-R-P.90 
            document

        """
        if "delegateObject" in impl:
            if not isinstance(impl["delegateObject"], org.ogf.saga.impl.context.ContextImpl):
                from saga.error import BadParameter
                raise BadParameter("Parameter impl[\"delegateObject\"] is not a org.ogf.saga.impl.context.ContextImpl. Type: " + str( impl["delegateObject"].__class__))
            self.delegateObject = impl["delegateObject"]
        else:    
            try:
                if name is "" or type(name) is not str:
                    self.delegateObject = ContextFactory.createContext();
                else:
                    self.delegateObject = ContextFactory.createContext(name);
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)

#TODO: Redo all BadParameter throws.... :(

    def set_defaults(self):
        """
        Set default values for specified context type.
        @summary: Set default values for specified context type.
        @PostCondition: the context is valid, and can be used for authorization.
        @raise NotImplemented:
        @raise IncorrectState:
        @raise Timeout:
        @raise NoSuccess:
        @note: the method avaluates the value of the 'Type' attribute, and of 
            all other non-empty attributes, and, based on that information, 
            tries to set sensible default values for all previously empty 
            attributes.
        @note: if the 'Type' attribute has an empty value, an 'IncorrectState' 
            exception is raised.
        @note: this method can be called more than once on a context instance.
        @note: if the implementation cannot create valid default values based 
            on the available information, an 'NoSuccess' exception is raised, 
            and a detailed error message is given, describing why no default 
            values could be set.

        """
        try:
            self.delegateObject.setDefaults()
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)
        
    def get_type(self):
        """
        Query the object type.
        @summary: Query the object type.
        @return: type of the object as an int from ObjectType
        @rtype: int
        """
        return ObjectType.CONTEXT
 
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
        try:
            delegateClone = self.delegateObject.clone()
            tempClone = Context(delegateObject=delegateClone)
            return tempClone
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)
 




    def __set_Type(self, value):
        self.set_attribute("Type", value)
        
    def __get_Type(self):
        return self.get_attribute("Type")  
    
    def __del_Type(self):
        return self.set_attribute("Type", "")            

    Type = property(__get_Type, __set_Type, __del_Type,
            doc="""The Type attribute. \n@type: string""")


    def __set_Server(self, value):
        self.set_attribute("Server", value)
        
    def __get_Server(self):
        return self.get_attribute("Server")   

    def __del_Server(self):
        return self.set_attribute("Server", "") 

    Server= property(__get_Server, __set_Server, __del_Server,
            doc="""The Server attribute.\n@type: string""")
  
    
    def __set_CertRepository(self,value):
        self.set_attribute("CertRepository", value)
        
    def __get_CertRepository(self):
        return self.get_attribute("CertRepository")   

    def __del_CertRepository(self):
        return self.set_attribute("CertRepository", "") 

    CertRepository = property(__get_CertRepository, __set_CertRepository, 
      __del_CertRepository, doc="The CertRepository attribute.\n@type: string")

    
    def __set_UserProxy(self,value):
        self.set_attribute("UserProxy", value)
        
    def __get_UserProxy(self):
        return self.get_attribute("UserProxy")   

    def __del_UserProxy(self):
        return self.set_attribute("UserProxy", "") 

    UserProxy = property(__get_UserProxy, __set_UserProxy, __del_UserProxy,
            doc="""The UserProxy attribute.\n@type: string""")
 
    
    def __set_UserCert(self, value):
        self.set_attribute("UserCert", value)
        
    def __get_UserCert(self):
        return self.get_attribute("UserCert")   

    def __del_UserCert(self):
        return self.set_attribute("UserCert", "") 

    UserCert = property(__get_UserCert, __set_UserCert, __del_UserCert,
            doc="""The UserCert attribute.\n@type: string""")
 
    
    def __set_UserKey(self, value):
        self.set_attribute("UserKey", value)
        
    def __get_UserKey(self):
        return self.get_attribute("UserKey")   

    def __del_UserKey(self):
        return self.set_attribute("UserKey", "") 

    UserKey = property(__get_UserKey, __set_UserKey, __del_UserKey,
            doc="""The UserKey attribute.\n@type: string""")

    
    def __set_UserID(self, value):
        self.set_attribute("UserID", value)
        
    def __get_UserID(self):
        return self.get_attribute("UserID")   

    def __del_UserID(self):
        return self.set_attribute("UserID", "") 

    UserID = property(__get_UserID, __set_UserID, __del_UserID,
            doc="""The UserID attribute.\n@type: string""")

    
    def __set_UserPass(self, value):
        self.set_attribute("UserPass", value)
        
    def __get_UserPass(self):
        return self.get_attribute("UserPass")   

    def __del_UserPass(self):
        return self.set_attribute("UserPass", "") 

    UserPass = property(__get_UserPass, __set_UserPass, __del_UserPass,
            doc="""The UserPass attribute.\n@type: string""")
    

    def __set_UserVO(self, value):
        self.set_attribute("UserVO", value)
        
    def __get_UserVO(self):
        return self.get_attribute("UserVO")   

    def __del_UserVO(self):
        return self.set_attribute("UserVO", "") 

    UserVO = property(__get_UserVO, __set_UserVO, __del_UserVO,
            doc="""The UserVO attribute.\n@type: string""")


    def __set_LifeTime(self, value):
        self.set_attribute("LifeTime", value)
        
    def __get_LifeTime(self):
        return self.get_attribute("LifeTime")   

    def __del_LifeTime(self):
        return self.set_attribute("LifeTime", "") 

    LifeTime = property(__get_LifeTime, __set_LifeTime, __del_LifeTime,
            doc="""The LifeTime attribute.\n@type: int""")


    def __set_RemoteID(self, value):
        self.set_attribute("RemoteID", value)
        
    def __get_RemoteID(self):
        return self.get_attribute("RemoteID")   

    def __del_RemoteID(self):
        return self.set_attribute("RemoteID", "") 

    RemoteID = property(__get_RemoteID, __set_RemoteID, __del_RemoteID,
            doc="""The RemoteID attribute.\n@type: string""")
    

    def __set_RemoteHost(self, value):
        self.set_attribute("RemoteHost", value)
        
    def __get_RemoteHost(self):
        return self.get_attribute("RemoteHost")   

    def __del_RemoteHost(self):
        return self.set_attribute("RemoteHost", "") 

    RemoteHost = property(__get_RemoteHost, __set_RemoteHost, __del_RemoteHost,
            doc="""The RemoteHost attribute.\n@type: string""")


    
    def __set_RemotePort(self, value):
        self.set_attribute("RemotePort", value)
        
    def __get_RemotePort(self):
        return self.get_attribute("RemotePort")   

    def __del_RemotePort(self):
        return self.set_attribute("RemotePort", "") 

    RemotePort = property(__get_RemotePort, __set_RemotePort, __del_RemotePort,
            doc="""The RemotePort attribute.\n@type: string""")

