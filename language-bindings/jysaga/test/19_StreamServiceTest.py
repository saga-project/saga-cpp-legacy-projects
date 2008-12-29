from saga.session import Session
from saga.context import Context
from saga.error import *
from saga.url import URL
from saga.object import ObjectType, Object
from saga.stream import StreamService, Stream
from saga.permissions import Permission
from saga.monitoring import Monitorable, Callback, Steerable, Metric
import unittest
import test_constants

class TestStreamService(unittest.TestCase):
    u = URL(test_constants.streamservice_url)
    u.port = test_constants.streamservice_port
    s = Session()
    
    
    def setUp(self):
        self.sserv = StreamService( url=self.u)


    def tearDown(self):
        pass
    
    def test_1_get_url(self):
        try:
            test = self.sserv.get_url()
            self.failUnless( isinstance(test, URL) )
        except NotImplemented:
            if e.saga_object != None: print e.message,"... ", 
            test_constants.add_NotImplemented()            
        test_constants.add_method_tested(1)
   
    def test_2_serve(self):
        try:
            test = self.sserv.serve(timeout=1)
            self.failUnless( isinstance(test, Stream) or type(test) == None)
        except NotImplemented: 
            if e.saga_object != None: print e.message,"... ",            
            test_constants.add_NotImplemented()
        except Timeout, e:
            pass
            #print e.message,"... ", 
        test_constants.add_method_tested(1)

    def test_3_close(self):
        try:
            test = self.sserv.close(timeout=1)
        except NotImplemented: 
            if e.saga_object != None: print e.message,"... ",
            test_constants.add_NotImplemented()            
        test_constants.add_method_tested(1)   



#Inherited from monitoring.Monitorable: add_callback, get_metric, list_metrics, remove_callback
    def test_Monitorable_methods(self):
        test = 0
        try:
            c = Callback()
            test = self.sserv.add_callback("stream_server.client_connect", c)
        except NotImplemented:
            test_constants.add_NotImplemented()            
        try:
            self.sserv.remove_callback("stream_server.client_connect", test)
        except NotImplemented:
            test_constants.add_NotImplemented()
        try: 
            test = self.sserv.list_metrics()
        except NotImplemented:
            test_constants.add_NotImplemented()
        try:
            test = self.sserv.get_metric("stream_server.client_connect")
            self.failUnless( isinstance(test, Metric) )
        except NotImplemented:
            test_constants.add_NotImplemented()
        test_constants.add_method_tested(4)        

#Inherited from permissions.Permissions: get_group, get_owner, permissions_allow, permissions_check, permissions_deny
    def test_Permissions_methods(self):
        try:
            temp = self.sserv.get_group()
            self.failUnless( type(test) == str)
        except NotImplemented:
            test_constants.add_NotImplemented()
 
        try:
            temp = self.sserv.get_owner()
            self.failUnless( type(test) == str)
        except NotImplemented:
            test_constants.add_NotImplemented()  
        
        try:
            self.sserv.permissions_allow("*",Permission.QUERY)
        except NotImplemented:
            test_constants.add_NotImplemented() 

        try:
            self.sserv.permissions_deny("*",Permission.QUERY)
        except NotImplemented:
            test_constants.add_NotImplemented() 
            
        try:
            self.sserv.permissions_check("*",Permission.QUERY)
        except NotImplemented:
            test_constants.add_NotImplemented() 
        test_constants.add_method_tested(5)


#Inherited from object.Object: id, session, type
    def test_Object_properties(self):
        try:        
            self.failUnless( type(self.sserv.id) == str)
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:        
            self.failUnless( isinstance(self.sserv.session, Session) )
        except NotImplemented:
            test_constants.add_NotImplemented()
        try:        
            self.failUnless( type(self.sserv.type) == str )
        except NotImplemented:
            test_constants.add_NotImplemented()  
        test_constants.add_method_tested(3)

#Inherited from monitoring.Monitorable: metrics
    def test_Monitorable_properties(self):
        try:        
            self.failUnless( type(self.sserv.metrics) == list)
        except NotImplemented:
            test_constants.add_NotImplemented()  
        test_constants.add_method_tested(1) 
        
#Inherited from permissions.Permissions: group, owner 
    def test_Permissions_properties(self):
        try:        
            self.failUnless( type(self.sserv.group) == str )
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:        
            self.failUnless( type(self.sserv.owner) == str )
        except NotImplemented:
            test_constants.add_NotImplemented()
        test_constants.add_method_tested(2)         

#Inherited from object.Object: clone, get_id, get_session, get_type
    def test_Object_methods(self):
        o = self.sserv

        try:
            test = o.get_id() 
            self.failUnless( (type(test) == str)) 
        except NotImplemented: test_constants.add_NotImplemented()
        
        try:
            test = o.get_type()
            self.failUnless( (type(test) == int) )
            self.failUnless( test == ObjectType.STREAMSERVICE )
        except NotImplemented: test_constants.add_NotImplemented()    
            
        try:  
            test =  o.get_session()
            self.failUnless( isinstance(test, Session) )
            self.failUnless( test.get_type() == ObjectType.SESSION )
        except NotImplemented: test_constants.add_NotImplemented()
        except DoesNotExist:pass
                       
        try:     
            clone = o.clone()
            self.failUnless( isinstance(o.Executable,str) )
        except NotImplemented: test_constants.add_NotImplemented()            
        test_constants.add_method_tested(4)


    def test_zzz_nr_NotImplemented(self):
        if test_constants.result_NotImplemented() != 0\
        and test_constants.print_not_implemented == True:
            base = float(test_constants.result_methods_tested())/100
            percentage = 100-float(test_constants.result_NotImplemented())//base

            print "\nNumber of methods not implemented in StreamService:",\
                test_constants.result_NotImplemented(), "of",\
                test_constants.result_methods_tested(), "methods",\
                "("+str(percentage)+"% implemented) ... ",




if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestStreamService)
    unittest.TextTestRunner(verbosity=2).run(suite)

