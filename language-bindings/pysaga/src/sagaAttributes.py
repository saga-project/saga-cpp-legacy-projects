from sagaErrors import NotImplemented

class Attributes(object):
    
    def set_attribute(self, key, value):
        raise NotImplemented, "set_attribute() is not implemented in this object"
    
    def get_attribute(self, key):
        #return value
        raise NotImplemented, "get_attribute() is not implemented in this object"
            
    def set_vector_attribute(self, key, value_list):
        raise NotImplemented, "set_vector_attribute() is not implemented in this object"
    
    def get_vector_attribute(self, key):
        #return a list of values
        raise NotImplemented, "get_vector_attribute() is not implemented in this object"
    
    def remove_attribute(self, key):
        raise NotImplemented, "remove_attribute() is not implemented in this object"
    
    def list_attributes(self):
        #return list of keys out
        raise NotImplemented, "list_attributes() is not implemented in this object"
    
    def find_attributes(self,  pattern_list):
        #return keys_list
        raise NotImplemented, "find_attributes() is not implemented in this object"
    
    def attribute_exists(self, key):
        raise NotImplemented, "attribute_exists() is not implemented in this object"
    
    def attribute_is_readonly(self, key):
        raise NotImplemented, "attribute_is_readonly() is not implemented in this object"
        
    def attribute_is_writable(self, key):
        raise NotImplemented, "attribute_is_writeable() is not implemented in this object"
    
    def attribute_is_removable (self, key):
        raise NotImplemented, "attribute_is_removeable() is not implemented in this object"
    
    def attribute_is_vector(self, key):
        raise NotImplemented, "attribute_is_vector() is not implemented in this object"