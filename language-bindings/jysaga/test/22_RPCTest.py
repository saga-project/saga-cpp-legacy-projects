from saga.session import Session
from saga.context import Context
from saga.error import *
from saga.url import URL
from saga.object import ObjectType, Object
from saga.rpc import Parameter, RPC, IOMode
from saga.task import TaskType, Task
from saga.permissions import Permission
from saga.monitoring import Monitorable, Callback, Steerable, Metric
import unittest
import test_constants



class TestRPC(unittest.TestCase):
    rpc = None

    def setUp(self):
        url = URL()
        url.host = test_constants.rpc_host
        url.scheme = test_constants.rpc_scheme
        url.port = test_constants.rpc_port
        url.path = test_constants.rpc_path
        self.rpc = RPC(url)
        #self.rpc = RPC(gridrpc://server.net:1234/my_function)

    def tearDown(self):
        pass 

    def test_1_init(self):
        pass

    def test_2_call(self):
        p1 = Parameter()
        p1.set_data(707, IOMode.IN)
        p2 = Parameter()
        p2.set_data(707, IOMode.IN)
        out = Parameter(mode=IOMode.OUT)  
        parameters = [p1, p2, out]
        try:
            self.rpc.call(parameters)
        except NotImplemented:
            test_constants.add_NotImplemented() 
        test_constants.add_method_tested(1)

    def test_3_close(self):
        try:
            self.rpc.close()
        except NotImplemented:
            test_constants.add_NotImplemented() 
        test_constants.add_method_tested(1)


#Inherited from permissions.Permissions: get_group, get_owner, permissions_allow, permissions_check, permissions_deny
    def test_Permissions_methods(self):
        try:
            temp = self.rpc.get_group()
            self.failUnless( type(test) == str)
        except NotImplemented:
            test_constants.add_NotImplemented()
 
        try:
            temp = self.rpc.get_owner()
            self.failUnless( type(test) == str)
        except NotImplemented:
            test_constants.add_NotImplemented()  
        
        try:
            self.rpc.permissions_allow("*",Permission.QUERY)
        except NotImplemented:
            test_constants.add_NotImplemented() 

        try:
            self.rpc.permissions_deny("*",Permission.QUERY)
        except NotImplemented:
            test_constants.add_NotImplemented() 
            
        try:
            self.rpc.permissions_check("*",Permission.QUERY)
        except NotImplemented:
            test_constants.add_NotImplemented() 
        test_constants.add_method_tested(5)

#Inherited from permissions.Permissions: group, owner 
    def test_Permissions_properties(self):
        try:        
            self.failUnless( type(self.rpc.group) == str )
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:        
            self.failUnless( type(self.rpc.owner) == str )
        except NotImplemented:
            test_constants.add_NotImplemented()
        test_constants.add_method_tested(2) 


#Inherited from object.Object: id, session, type
    def test_Object_properties(self):
        try:        
            self.failUnless( type(self.rpc.id) == str)
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:        
            self.failUnless( isinstance(self.rpc.session, Session) )
        except NotImplemented:
            test_constants.add_NotImplemented()
        except DoesNotExist:
            pass
        try:        
            self.failUnless( type(self.rpc.type) == str )
        except NotImplemented:
            test_constants.add_NotImplemented()  
        test_constants.add_method_tested(3)

       

#Inherited from object.Object: clone, get_id, get_session, get_type
    def test_Object_methods(self):
        o = self.rpc

        try:
            test = o.get_id() 
            self.failUnless( (type(test) == str)) 
        except NotImplemented: test_constants.add_NotImplemented()
        
        try:
            test = o.get_type()
            self.failUnless( (type(test) == int) )
            self.failUnless( test == ObjectType.RPC )
        except NotImplemented: test_constants.add_NotImplemented()    
            
        try:  
            test =  o.get_session()
            self.failUnless( isinstance(test, Session) )
            self.failUnless( test.get_type() == ObjectType.SESSION )
        except NotImplemented: test_constants.add_NotImplemented()
        except DoesNotExist:pass
                       
        try:     
            clone = o.clone()
            clone.close()
        except NotImplemented: test_constants.add_NotImplemented()            
        test_constants.add_method_tested(4)


    def test_zzz_nr_NotImplemented(self):
        if test_constants.result_NotImplemented() != 0\
        and test_constants.print_not_implemented == True:
            base = float(test_constants.result_methods_tested())/100
            percentage = 100-float(test_constants.result_NotImplemented())//base

            print "\nNumber of methods not implemented in RPC:",\
                test_constants.result_NotImplemented(), "of",\
                test_constants.result_methods_tested(), "methods",\
                "("+str(percentage)+"% implemented) ... ",




if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestRPC)
    unittest.TextTestRunner(verbosity=2).run(suite)