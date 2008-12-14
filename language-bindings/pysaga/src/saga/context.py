# Package: saga
# Module: context 
# Description: The module which specifies the context classes used in saga
# Specification and documentation can be found in section 3.6, page 81-86 of 
#    the GFD-R-P.90 document
# Author: P.F.A. van Zoolingen, Computer Systems Section, Faculty of 
#    Exact Science (FEW), Vrije Universiteit, Amsterdam, The Netherlands.

from object import Object, ObjectType
from attributes import Attributes
from error import NotImplemented

class Context(Object, Attributes):
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

    def __init__(self, name=""):
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
        pass
        
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
        pass





    def __set_Type(value):
        set_attribute("Type", value)
        
    def __get_Type():
        return get_attribute("Type")  
    
    def __del_Type():
        return set_attribute("Type", "")            

    Type = property(__get_Type, __set_Type, __del_Type,
            doc="""The Type attribute. \n@type: string""")


    def __set_Server(value):
        set_attribute("Server", value)
        
    def __get_Server():
        return get_attribute("Server")   

    def __del_Server():
        return set_attribute("Server", "") 

    Server= property(__get_Server, __set_Server, __del_Server,
            doc="""The Server attribute.\n@type: string""")
  
    
    def __set_CertRepository(value):
        set_attribute("CertRepository", value)
        
    def __get_CertRepository():
        return get_attribute("CertRepository")   

    def __del_CertRepository():
        return set_attribute("CertRepository", "") 

    CertRepository = property(__get_CertRepository, __set_CertRepository, 
      __del_CertRepository, doc="The CertRepository attribute.\n@type: string")

    
    def __set_UserProxy(value):
        set_attribute("UserProxy", value)
        
    def __get_UserProxy():
        return get_attribute("UserProxy")   

    def __del_UserProxy():
        return set_attribute("UserProxy", "") 

    UserProxy = property(__get_UserProxy, __set_UserProxy, __del_UserProxy,
            doc="""The UserProxy attribute.\n@type: string""")
 
    
    def __set_UserCert(value):
        set_attribute("UserCert", value)
        
    def __get_UserCert():
        return get_attribute("UserCert")   

    def __del_UserCert():
        return set_attribute("UserCert", "") 

    UserCert = property(__get_UserCert, __set_UserCert, __del_UserCert,
            doc="""The UserCert attribute.\n@type: string""")
 
    
    def __set_UserKey(value):
        set_attribute("UserKey", value)
        
    def __get_UserKey():
        return get_attribute("UserKey")   

    def __del_UserKey():
        return set_attribute("UserKey", "") 

    UserKey = property(__get_UserKey, __set_UserKey, __del_UserKey,
            doc="""The UserKey attribute.\n@type: string""")

    
    def __set_UserID(value):
        set_attribute("UserID", value)
        
    def __get_UserID():
        return get_attribute("UserID")   

    def __del_UserID():
        return set_attribute("UserID", "") 

    UserID = property(__get_UserID, __set_UserID, __del_UserID,
            doc="""The UserID attribute.\n@type: string""")

    
    def __set_UserPass(value):
        set_attribute("UserPass", value)
        
    def __get_UserPass():
        return get_attribute("UserPass")   

    def __del_UserPass():
        return set_attribute("UserPass", "") 

    UserPass = property(__get_UserPass, __set_UserPass, __del_UserPass,
            doc="""The UserPass attribute.\n@type: string""")
    

    def __set_UserVO(value):
        set_attribute("UserVO", value)
        
    def __get_UserVO():
        return get_attribute("UserVO")   

    def __del_UserVO():
        return set_attribute("UserVO", "") 

    UserVO = property(__get_UserVO, __set_UserVO, __del_UserVO,
            doc="""The UserVO attribute.\n@type: string""")


    def __set_LifeTime(value):
        set_attribute("LifeTime", value)
        
    def __get_LifeTime():
        return get_attribute("LifeTime")   

    def __del_LifeTime():
        return set_attribute("LifeTime", "") 

    LifeTime = property(__get_LifeTime, __set_LifeTime, __del_LifeTime,
            doc="""The LifeTime attribute.\n@type: int""")


    def __set_RemoteID(value):
        set_attribute("RemoteID", value)
        
    def __get_RemoteID():
        return get_attribute("RemoteID")   

    def __del_RemoteID():
        return set_attribute("RemoteID", "") 

    RemoteID = property(__get_RemoteID, __set_RemoteID, __del_RemoteID,
            doc="""The RemoteID attribute.\n@type: string""")
    

    def __set_RemoteHost(value):
        set_attribute("RemoteHost", value)
        
    def __get_RemoteHost():
        return get_attribute("RemoteHost")   

    def __del_RemoteHost():
        return set_attribute("RemoteHost", "") 

    RemoteHost = property(__get_RemoteHost, __set_RemoteHost, __del_RemoteHost,
            doc="""The RemoteHost attribute.\n@type: string""")


    
    def __set_RemotePort(value):
        set_attribute("RemotePort", value)
        
    def __get_RemotePort():
        return get_attribute("RemotePort")   

    def __del_RemotePort():
        return set_attribute("RemotePort", "") 

    RemotePort = property(__get_RemotePort, __set_RemotePort, __del_RemotePort,
            doc="""The RemotePort attribute.\n@type: string""")

