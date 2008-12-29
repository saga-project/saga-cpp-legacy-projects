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
from saga.logicalfile import LogicalFile, Flags, LogicalDirectory
import unittest
import test_constants
import os

class TestLogicalDirectory(unittest.TestCase):

    def setUp(self):
        url = URL(test_constants.logicalDirectory_name)
        try:
            os.mkdir(test_constants.logicalDirectory_plainname)
        except OSError:
            pass
        self.ldir =  LogicalDirectory(url)
        
 
    def test_is_file(self):
        url = URL(test_constants.logicalfile_name1)
        file = open( test_constants.logicalfile_plainname, "w")
        file.write("abcdefghijklmnopqrstuvwxyz")
        file.flush()
        file.close()
        try:
            test = self.ldir.is_file(url)       
        except NotImplemented:
            test_constants.add_NotImplemented()

    def test_open_dir(self):
        try:
            test = self.ldir.open_dir(URL(test_constants.logicalDirectory_dirname))
            self.failUnless(isinstance(test,LogicalDirectory))
        except NotImplemented:
            test_constants.add_NotImplemented()  

    def test_open(self):
        url = URL(test_constants.logicalfile_name1)
        file = open( test_constants.logicalfile_plainname, "w")
        file.write("abcdefghijklmnopqrstuvwxyz")
        file.flush()
        file.close()
        try:
            test = self.ldir.open(url)       
        except NotImplemented:
            test_constants.add_NotImplemented()

    def test_find(self):
        try:
            test = self.ldir.find("*",["*"])
            self.failUnless(type(test) == list)       
        except NotImplemented:
            test_constants.add_NotImplemented()       
 

#Inherited from namespace.NSDirectory: change_dir, copy, exists, get_entry, get_num_entries, is_dir, is_entry, is_link, link, list, make_dir, move, permissions_allow, permissions_deny, read_link, remove
#Inherited from namespace.NSEntry: __del__, close, copy_self, get_cwd, get_name, get_url, is_dir_self, is_entry_self, is_link_self, link_self, move_self, permissions_allow_self, permissions_deny_self, read_link_self, remove_self
#TODO: add Other tests


#Inherited from attributes.Attributes: attribute_exists, attribute_is_readonly, attribute_is_removable, attribute_is_vector, attribute_is_writable, find_attributes, get_attribute, get_vector_attribute, list_attributes, remove_attribute, set_attribute, set_vector_attribute
    def test_Attributes_methods(self):
        try:
            self.ldir.set_attribute("example","True")
        except NotImplemented:
            test_constants.add_NotImplemented()         
        except PermissionDenied:
            pass 
        except IncorrectState:
            pass        
        try:
            test = self.ldir.list_attributes()
            self.failUnless( type(test) == list)
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            self.failUnless(type(self.ldir.attribute_exists(test[0]) == bool))
        except NotImplemented:
            test_constants.add_NotImplemented()                              
        try:
            self.failUnless(type(self.ldir.attribute_is_readonly(test[0])==bool))
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            self.failUnless(type(self.ldir.attribute_is_removable(test[0])==bool))
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            self.failUnless(type(self.ldir.attribute_is_vector(test[0])==bool))
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            self.failUnless(type(self.ldir.attribute_is_writable(test[0])==bool))
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            self.failUnless( type(self.ldir.find_attributes([test[0]]) == list) )
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            self.failUnless( type(self.ldir.get_attribute(test[0]) == str) )
        except NotImplemented:
            test_constants.add_NotImplemented()   
        except IncorrectState:
            pass
        try:
            self.failUnless(type(self.ldir.get_vector_attribute(test[0])==list))
        except NotImplemented:
            test_constants.add_NotImplemented()   
        except IncorrectState:
            pass
        try:
            self.failUnless(type(self.ldir.remove_attribute(test[0])==None))
        except NotImplemented:
            test_constants.add_NotImplemented()         
        except PermissionDenied:
            pass
        try:
            self.failUnless(type(self.ldir.remove_attribute(test[0])==None))
        except NotImplemented:
            test_constants.add_NotImplemented()         
        except PermissionDenied:
            pass
        except IncorrectState:
            pass
        try:
            self.failUnless(type(self.ldir.set_attribute(test[0],"")==None))
        except NotImplemented:
            test_constants.add_NotImplemented()         
        except PermissionDenied:
            pass 
        except IncorrectState:
            pass        
        try:
          self.failUnless(type(self.ldir.set_vector_attribute(test[0],[])==None))
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
            self.failUnless( type(self.ldir.attributes) == dict)
        except NotImplemented:
            test_constants.add_NotImplemented()
        test_constants.add_method_tested(1)


#Inherited from object.Object: id, session, type
    def test_Object_properties(self):
        try:        
            self.failUnless( type(self.ldir.id) == str)
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:        
            self.failUnless( isinstance(self.ldir.session, Session) )
        except NotImplemented:
            test_constants.add_NotImplemented()
        except DoesNotExist:
            pass
        try:        
            self.failUnless( type(self.ldir.type) == str )
        except NotImplemented:
            test_constants.add_NotImplemented()  
        test_constants.add_method_tested(3)

       

#Inherited from object.Object: clone, get_id, get_session, get_type
    def test_Object_methods(self):
        o = self.ldir

        try:
            test = o.get_id() 
            self.failUnless( (type(test) == str)) 
        except NotImplemented, e: 
            print str(e),"... ",
            test_constants.add_NotImplemented()
        
        try:
            test = o.get_type()
            self.failUnless( (type(test) == int) )
            self.failUnless( test == ObjectType.LOGICALDIRECTORY )
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
            temp = self.ldir.get_group()
            self.failUnless( type(test) == str)
        except NotImplemented:
            test_constants.add_NotImplemented()
 
        try:
            temp = self.ldir.get_owner()
            self.failUnless( type(test) == str)
        except NotImplemented:
            test_constants.add_NotImplemented()  
        
        try:
            url = URL(test_constants.logicalfile_name1)
            self.ldir.permissions_allow(url, "*",Permission.QUERY)
        except NotImplemented:
            test_constants.add_NotImplemented() 

        try:
            url = URL(test_constants.logicalfile_name1)
            self.ldir.permissions_deny(url,"*",Permission.QUERY)
        except NotImplemented:
            test_constants.add_NotImplemented() 
            
        try:
            self.ldir.permissions_check("*",Permission.QUERY)
        except NotImplemented:
            test_constants.add_NotImplemented() 
        test_constants.add_method_tested(5)
 
#Inherited from permissions.Permissions: group, owner  
    def test_Permissions_properties(self):
        try:        
            self.failUnless( type(self.ldir.group) == str )
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:        
            self.failUnless( type(self.ldir.owner) == str )
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
    suite = unittest.TestLoader().loadTestsFromTestCase(TestLogicalDirectory)
    unittest.TextTestRunner(verbosity=2).run(suite)