from error import NotImplemented

class Attributes(object):
    """
    There are various places in the SAGA API where attributes need to be associated with objects, 
    for instance for job descriptions and metrics. The Attributes
    class provides a common interface for storing and retrieving attributes.
    """
    
    def set_attribute(self, key, value):
        """
        Set an attribute to a value
        @summary: set an attribute to a value
        @param key: attribute key
        @param value: value to set the attribute to
        @type key: string
        @type value: string
        @permission:    Write
        @raise NotImplemented:
        @raise BadParameter:
        @raise DoesNotExist:
        @raise  IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: an empty string means to set an empty value (the attribute is not removed).
        @note: the attribute is created, if it does not exist
        @note: a 'PermissionDenied' exception is thrown if the
             attribute to be changed is ReadOnly.
        @note: only some SAGA objects allow to create new
             attributes - others allow only access to
             predefined attributes. If a non-existing
             attribute is queried on such objects, a
             'DoesNotExist' exception is raised
        @note: changes of attributes may reflect changes of
             endpoint entity properties. As such,
             authorization and/or authentication may fail
             for settings such attributes, for some
             backends. In that case, the respective
             'AuthenticationFailed', 'AuthorizationFailed',
             and 'PermissionDenied' exceptions are thrown.
             For example, an implementation may forbid to
             change the saga.stream.Stream 'Bufsize' attribute.
        @note: if an attribute is not well formatted, or
             outside of some allowed range, a 'BadParameter'
             exception with a descriptive error message is
             thrown.
        @note: if the operation is attempted on a vector
             attribute, an 'IncorrectState' exception is
             thrown.
        @note: setting of attributes may time out, or may fail
             for other reasons - which causes a 'Timeout' or
             'NoSuccess' exception, respectively.

        """
        raise NotImplemented, "set_attribute() is not implemented in this object"
    
    def get_attribute(self, key):
        #return value
        """
        Get an attribute value
        @summary: get an attribute value
        @param key: attribute key
        @type key: string
        @return: value: value of the attribute
        @rtype: string
        @permission:    Query
        @raise NotImplemented:
        @raise  DoesNotExist:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: queries of attributes may imply queries of endpoint entity properties. As such,
              authorization and/or authentication may fail for querying such attributes, for some
              backends. In that case, the respective 'AuthenticationFailed', 'AuthorizationFailed',
              and 'PermissionDenied' exceptions are thrown. For example, an implementation may forbid to
              read the saga.stream.Stream 'Bufsize' attribute.
        @note: reading an attribute value for an attribute which is not in the current set of attributes
              causes a 'DoesNotExist' exception.
        @note: if the operation is attempted on a vector attribute, an 'IncorrectState' exception is thrown.
        @note: getting attribute values may time out, or may fail for other reasons - which causes a
              'Timeout' or 'NoSuccess' exception, respectively.

        """
        raise NotImplemented, "get_attribute() is not implemented in this object"
            
    def set_vector_attribute(self, key, values):
        """
        Set an attribute to an list of values.
        @summary: set an attribute to an array of values
        @param key:                  attribute key
        @param values:               array of attribute values
        @type key: string
        @type values: list
        @permission: Write
        @raise NotImplemented:
        @raise BadParameter:
        @raise DoesNotExist:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: the notes to the set_attribute() method apply.
        @note: if the operation is attempted on a scalar attribute, an 'IncorrectState' exception is thrown.

        """
        raise NotImplemented, "set_vector_attribute() is not implemented in this object"
    
    def get_vector_attribute(self, key):
        #return a list of values
        """
        Get the tuple of values associated with an attribute
        @summary: get the tuple of values associated with an attribute
        @param key: attribute key
        @type key: string
        @return: tuple of values of the attribute.
        @rtype: tuple
        @permission: Query
        @raise NotImplemented:
        @raise DoesNotExist:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: the notes to the get_attribute() method apply.
        @note: if the operation is attempted on a scalar attribute, an 'IncorrectState' exception is thrown.

        """
        raise NotImplemented, "get_vector_attribute() is not implemented in this object"
    
    def remove_attribute(self, key):
        """
        Removes an attribute
        @summary: removes an attribute
        @param    key:                  attribute to be removed
        @type key: string
        @postcondition: - the attribute is not available anymore.
        @permission:    Write
        @raise NotImplemented:
        @raise DoesNotExist:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise  AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: a vector attribute can also be removed with this method
        @note: only some SAGA objects allow to remove attributes.
        @note: a ReadOnly attribute cannot be removed - any attempt to do so throws a 'PermissionDenied' exception.
        @note: if a non-existing attribute is removed, a 'DoesNotExist' exception is raised.
        @note: exceptions have the same semantics as defined for the set_attribute() method description.
        """

        raise NotImplemented, "remove_attribute() is not implemented in this object"
    
    def list_attributes(self):
        #return list of keys out
        """
        Get the list of attribute keys
        @summary: get the list of attribute keys
        @return: existing attribute keys
        @rtype: tuple
        @permission: Query
        @raise NotImplemented:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: exceptions have the same semantics as defined
            for the get_attribute() method description.
        @note: if no attributes are defined for the object, an empty tuple is returned.

        """
        raise NotImplemented, "list_attributes() is not implemented in this object"
    
    def find_attributes(self,  pattern):
        #return keys_list
        """
        Find matching attributes
        @summary: find matching attributes
        @param pattern: tuple or list of string search patterns
        @type pattern: tuple or list
        @return: matching attribute keys
        @rtype: tuple
        @permission: Query
        @raise NotImplemented:
        @raise BadParameter:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: the pattern must be formatted as described earlier, otherwise a 'BadParameter' exception is thrown.
        @note: exceptions have the same semantics as defined for the get_attribute() method description.

        """
        raise NotImplemented, "find_attributes() is not implemented in this object"
    
    def attribute_exists(self, key):
        """
        @summary: check the attribute's existence.
        @param key: attribute key
        @type key: string
        @return: does the attribute exist
        @rtype: bool
        @permission:    Query
        @raise NotImplemented:
        @raise PermissionDenied:
        @raise  AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: This method returns True if the attribute identified by the key exists.
        @note:exceptions have the same semantics as defined for the get_attribute() method description,
            apart from the fact that a 'DoesNotExist' exception is never thrown.

        """
        raise NotImplemented, "attribute_exists() is not implemented in this object"
    
    def attribute_is_readonly(self, key):
        """
        Check if the attribute is read only
        @summary: check if the attribute is read only
        @param    key:                  attribute key
        @type key: string
        @return: indicator if the attribute read only
        @rtype: bool
        @permission:    Query
        @raise NotImplemented:
        @raise  DoesNotExist:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: This method returns True if the attribute identified by the key exists, and can be read
              by get_attribute() or get_vector attribute(), but cannot be changed by set_attribute() and
              set_vector_attribute().
        @note: exceptions have the same semantics as defined for the get_attribute() method description.

        """
        raise NotImplemented, "attribute_is_readonly() is not implemented in this object"
        
    def attribute_is_writable(self, key):
        """
        Check if the attribute is writable
        @summary: check if the attribute is writable
        @param key: attribute key
        @type key: string
        @return: indicator if the attribute is writable
        @rtype: bool
        @permission:    Query
        @raise NotImplemented:
        @raise DoesNotExist:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: This method returns True if the attribute identified by the key exists, and can be
            changed by set_attribute() or set_vector_attribute().
        @note: exceptions have the same semantics as defined for the get_attribute() method description.

        """
        raise NotImplemented, "attribute_is_writeable() is not implemented in this object"
    
    def attribute_is_removable (self, key):
        """
        Check if the attribute is removable
        @summary: check if the attribute is removable
        @param key: attribute key
        @type key: string
        @return: indicator if the attribute is removable
        @rtype: bool
        @permission:    Query
        @raise NotImplemented:
        @raise  DoesNotExist:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: This method returns True if the attribute identified by the key exists, and can be
                 removed by remove_attribute().
        @note: exceptions have the same semantics as defined for the get_attribute() method description.

        """
        raise NotImplemented, "attribute_is_removeable() is not implemented in this object"
    
    def attribute_is_vector(self, key):
        """
        Check whether the attribute is a vector or a scalar
        @summary: check whether the attribute is a vector or a scalar
        @param key: attribute key
        @type key: string
        @return: indicator if the  attribute is scalar (False) or vector (True)
        @rtype: bool
        @permission:    Query
        @raise NotImplemented:
        @raise DoesNotExist:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: This method returns True if the attribute identified by key is a vector attribute.
        @note: exceptions have the same semantics as defined for the get_attribute() method description.

        """
        raise NotImplemented, "attribute_is_vector() is not implemented in this object"