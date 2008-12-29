from saga.error import *
from saga.url import URL
from saga.context import Context
from saga.session import Session
from saga.object import ObjectType, Object
from saga.buffer import Buffer
from saga.permissions import Permissions, Permission
import array.array
from saga.file import File, Directory
from saga.namespace import Flags, NSEntry, NSDirectory
from saga.logicalfile import LogicalFile, Flags
import unittest
import test_constants


class TestLogicalFile(unittest.TestCase):

    def setUp(self):
        url = URL(test_constants.logicalfile_name1)
        file = open( test_constants.logicalfile_plainname, "w")
        file.write("abcdefghijklmnopqrstuvwxyz")
        file.flush()
        file.close()
        self.lfile =  LogicalFile(url, flags=Flags.READWRITE)
        
    def test_add_remove_location(self):
        try:
            url = URL(test_constants.logicalfile_name2)
            self.lfile.add_location(url)
        except NotImplemented:
            test_constants.add_NotImplemented() 
        test_constants.add_method_tested(1)  
        try:
            url = URL(test_constants.logicalfile_name2)
            self.lfile.remove_location(url)
        except NotImplemented:
            test_constants.add_NotImplemented() 
        test_constants.add_method_tested(1)  

    def test_update_location(self):
        try:
            url1 = URL(test_constants.logicalfile_name2)
            self.lfile.add_location(url1)
            url2 = URL(test_constants.logicalfile_name3)
            self.lfile.update_location(url1, url2)
        except NotImplemented:
            test_constants.add_NotImplemented() 
        test_constants.add_method_tested(1)       

    def test_list_locations(self):
        try:
            url1 = URL(test_constants.logicalfile_name2)
            url2 = URL(test_constants.logicalfile_name3)
            self.lfile.add_location(url1)
            self.lfile.add_location(url2)
            test = self.lfile.list_locations()
            self.failUnless( type(test) == list )
        except NotImplemented:
            test_constants.add_NotImplemented() 
        test_constants.add_method_tested(1) 
  
    def test_replicate(self):
        try:
            url4 = URL(test_constants.logicalfile_name4)
            self.lfile.replicate(url4)
        except NotImplemented:
            test_constants.add_NotImplemented() 
        test_constants.add_method_tested(1) 


#TODO: add Other tests
#Inherited from namespace.NSEntry: __del__, close, copy_self, get_cwd, get_name, get_url, is_dir_self, is_entry_self, is_link_self, link_self, move_self, permissions_allow_self, permissions_deny_self, read_link_self, remove_self

    def test_LogicalFile_properties(self):
        try:
            test = self.lfile.locations
            self.failUnless(type(test) == list)
        except NotImplemented:
            test_constants.add_NotImplemented() 
        test_constants.add_method_tested(1) 


#Inherited from attributes.Attributes: attribute_exists, attribute_is_readonly, attribute_is_removable, attribute_is_vector, attribute_is_writable, find_attributes, get_attribute, get_vector_attribute, list_attributes, remove_attribute, set_attribute, set_vector_attribute
    def test_Attributes_methods(self):
        try:
            self.lfile.set_attribute("example","True")
        except NotImplemented:
            test_constants.add_NotImplemented()         
        except PermissionDenied:
            pass 
        except IncorrectState:
            pass        
        try:
            test = self.lfile.list_attributes()
            self.failUnless( type(test) == list)
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            self.failUnless(type(self.lfile.attribute_exists(test[0]) == bool))
        except NotImplemented:
            test_constants.add_NotImplemented()                              
        try:
            self.failUnless(type(self.lfile.attribute_is_readonly(test[0])==bool))
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            self.failUnless(type(self.lfile.attribute_is_removable(test[0])==bool))
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            self.failUnless(type(self.lfile.attribute_is_vector(test[0])==bool))
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            self.failUnless(type(self.lfile.attribute_is_writable(test[0])==bool))
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            self.failUnless( type(self.lfile.find_attributes([test[0]]) == list) )
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            self.failUnless( type(self.lfile.get_attribute(test[0]) == str) )
        except NotImplemented:
            test_constants.add_NotImplemented()   
        except IncorrectState:
            pass
        try:
            self.failUnless(type(self.lfile.get_vector_attribute(test[0])==list))
        except NotImplemented:
            test_constants.add_NotImplemented()   
        except IncorrectState:
            pass
        try:
            self.failUnless(type(self.lfile.remove_attribute(test[0])==None))
        except NotImplemented:
            test_constants.add_NotImplemented()         
        except PermissionDenied:
            pass
        try:
            self.failUnless(type(self.lfile.remove_attribute(test[0])==None))
        except NotImplemented:
            test_constants.add_NotImplemented()         
        except PermissionDenied:
            pass
        except IncorrectState:
            pass
        try:
            self.failUnless(type(self.lfile.set_attribute(test[0],"")==None))
        except NotImplemented:
            test_constants.add_NotImplemented()         
        except PermissionDenied:
            pass 
        except IncorrectState:
            pass        
        try:
          self.failUnless(type(self.lfile.set_vector_attribute(test[0],[])==None))
        except NotImplemented:
            test_constants.add_NotImplemented()         
        except PermissionDenied:
            pass 
        except IncorrectState:
            pass 
        test_constants.add_method_tested(13)


#Inherited from attributes.Attributes: attributes            
    def test_Attributes_properties(self):
        try:        
            self.failUnless( type(self.lfile.attributes) == dict)
        except NotImplemented:
            test_constants.add_NotImplemented()
        test_constants.add_method_tested(1)


#Inherited from object.Object: id, session, type
    def test_Object_properties(self):
        try:        
            self.failUnless( type(self.lfile.id) == str)
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:        
            self.failUnless( isinstance(self.lfile.session, Session) )
        except NotImplemented:
            test_constants.add_NotImplemented()
        except DoesNotExist:
            pass
        try:        
            self.failUnless( type(self.lfile.type) == str )
        except NotImplemented:
            test_constants.add_NotImplemented()  
        test_constants.add_method_tested(3)

       

#Inherited from object.Object: clone, get_id, get_session, get_type
    def test_Object_methods(self):
        o = self.lfile

        try:
            test = o.get_id() 
            self.failUnless( (type(test) == str)) 
        except NotImplemented, e: 
            print str(e),"... ",
            test_constants.add_NotImplemented()
        
        try:
            test = o.get_type()
            self.failUnless( (type(test) == int) )
            self.failUnless( test == ObjectType.LOGICALFILE )
        except NotImplemented, e:
            print str(e),"... ", 
            test_constants.add_NotImplemented()    
            
        try:  
            test =  o.get_session()
            self.failUnless( isinstance(test, Session) )
            self.failUnless( test.get_type() == ObjectType.SESSION )
        except NotImplemented, e: 
            print str(e),"... ",
            test_constants.add_NotImplemented()
        except DoesNotExist:pass
                       
        try:     
            clone = o.clone()
            clone.close()
        except NotImplemented, e: 
            print str(e),"... ",
            test_constants.add_NotImplemented()            
        test_constants.add_method_tested(4)


#Inherited from permissions.Permissions: get_group, get_owner, permissions_allow, permissions_check, permissions_deny
    def test_Permissions_methods(self):
        try:
            temp = self.lfile.get_group()
            self.failUnless( type(test) == str)
        except NotImplemented:
            test_constants.add_NotImplemented()
 
        try:
            temp = self.lfile.get_owner()
            self.failUnless( type(test) == str)
        except NotImplemented:
            test_constants.add_NotImplemented()  
        
        try:
            self.lfile.permissions_allow("*",Permission.QUERY)
        except NotImplemented:
            test_constants.add_NotImplemented() 

        try:
            self.lfile.permissions_deny("*",Permission.QUERY)
        except NotImplemented:
            test_constants.add_NotImplemented() 
            
        try:
            self.lfile.permissions_check("*",Permission.QUERY)
        except NotImplemented:
            test_constants.add_NotImplemented() 
        test_constants.add_method_tested(5)
 
#Inherited from permissions.Permissions: group, owner  
    def test_Permissions_properties(self):
        try:        
            self.failUnless( type(self.lfile.group) == str )
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:        
            self.failUnless( type(self.lfile.owner) == str )
        except NotImplemented:
            test_constants.add_NotImplemented()
        test_constants.add_method_tested(2)   
 
  
    def test_zzz_nr_NotImplemented(self):
        if test_constants.result_NotImplemented() != 0\
        and test_constants.print_not_implemented == True:
            base = float(test_constants.result_methods_tested())/100
            percentage = 100-float(test_constants.result_NotImplemented())//base

            print "\nNumber of methods not implemented in LogicalFile:",\
                test_constants.result_NotImplemented(), "of",\
                test_constants.result_methods_tested(), "methods",\
                "("+str(percentage)+"% implemented) ... ",

if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestLogicalFile)
    unittest.TextTestRunner(verbosity=2).run(suite)