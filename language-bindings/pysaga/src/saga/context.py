# Package: saga
# Module: context 
# Description: The module which specifies the context classes used in saga
# Specification and documentation can be found in section 3.6, page 81-86 of the GFD-R-P.90 document
# Author: P.F.A. van Zoolingen, Computer Systems Section, Faculty of Exact Science (FEW), Vrije Universiteit, Amsterdam, The Netherlands.

from object import Object, ObjectType
from attributes import Attributes
from error import NotImplemented

class Context(Object, Attributes):
    """
    The saga.context.Context class provides the functionality of a security information container.

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
                - note: a typical example would be the contact information for a MyProxy server, such as
                  'myproxy.remote.net:7512', for a 'myproxy' type context.
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
                -    note: a typical example for a globus type context would be "/tmp/x509up_u<uid>".
            - B{UserCert}:
                -    name: UserCert
                -    desc: location of a user certificate to use
                -    mode: ReadWrite
                -    type: String
                -    value: -
                -    note: a typical example for a globus type context would be "$HOME/.globus/usercert.pem".
            - B{UserKey}:
                -    name: UserKey
                -    desc: location of a user key to use
                -    mode: ReadWrite
                -    type: String
                -    value: -
                -    note: a typical example for a globus type context would be "$HOME/.globus/userkey.pem".
            - B{UserID}:
                -    name:  UserID
                -    desc:  user id or user name to use
                -    mode:  ReadWrite
                -    type:  String
                -    value: -
                -    note: a typical example for a ftp type context would be "anonymous".
            - B{UserPass}:
                -    name:  UserPass
                -    desc:  password to use
                -    mode:  ReadWrite
                -    type:  String
                -    value: -
                -    note: a typical example for a ftp type context would be "anonymous@localhost".
            - B{UserVO}:
                -    name:  UserVO
                -    desc:  the VO the context belongs to
                -    mode:  ReadWrite
                -    type:  String
                -    value: -
                -    note: a typical example for a globus type context  would be "O=dutchgrid".
            - B{LifeTime}:
                -    name:  LifeTime
                -    desc:  time up to which this context is valid
                -    mode:  ReadWrite
                -    type:  Int
                -    value: -1
                -    note: time and date specified in number of seconds since epoch, 
                          a value of -1 indicates an infinite lifetime.
            - B{RemoteID}:
                -    name:  RemoteID
                -    desc:  user ID for an remote user, who is identified by this context.
                -    mode: ReadOnly
                -    type: String
                -    value: -
                -    note: a typical example for a globus type context would be
                      "/O=dutchgrid/O=users/O=vu/OU=cs/CN=Joe Doe".
            - B{RemoteHost}
                -     name: RemoteHost
                -     desc: the hostname where the connection origininates which is identified by this context.
                -     mode: ReadOnly
                -     type: String
                -     value: -
                -     name: RemotePort
                -     desc: the port used for the connection which is identified by this context.
                -     mode: ReadOnly
                -     type: String
                -     value: -
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
        @note: if name is given (i.e. non-empty), then the __init__ internally calls set_defaults().
            The notes to set_defaults apply.
        @see: the notes about lifetime management in Section 2 of the GFD-R-P.90 document

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
        @note: the method avaluates the value of the 'Type' attribute, and of all other non-empty
              attributes, and, based on that information, tries to set sensible default values for all
              previously empty attributes.
        @note: if the 'Type' attribute has an empty value, an 'IncorrectState' exception is raised.
        @note: this method can be called more than once on a context instance.
        @note: if the implementation cannot create valid default values based on the available
              information, an 'NoSuccess' exception is raised, and a detailed error message is given,
              describing why no default values could be set.

        """
        pass
