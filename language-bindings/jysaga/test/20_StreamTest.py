from saga.session import Session
from saga.context import Context
from saga.error import *
from saga.url import URL
from saga.object import ObjectType, Object
from saga.stream import StreamService, Stream, Activity, State 
from saga.task import TaskType, Task
from saga.permissions import Permission
from saga.monitoring import Monitorable, Callback, Steerable, Metric
import unittest
import test_constants

from threading import Thread

class Server(Thread):
    def __init__ (self):
        Thread.__init__(self)

    def run(self):
        u = URL(test_constants.streamservice_url)
        u.port = test_constants.streamservice_port
        s = Session()
        sserv = StreamService( url=u, session=s)
        incoming_stream = sserv.serve(10)
        if(incoming_stream == None):
            print "Server: Timed out"
            return
        try:
            incoming_stream.get_context()
        except NotImplemented:
            test_constants.add_NotImplemented()
        except NoSuccess, e:
            print "Server: get_context(): "+ str(e) 
        test_constants.add_method_tested(1)       
        
        try:
            sentence = incoming_stream.read()
        except NotImplemented: 
            if e.saga_object != None: print e.message,"... ",
            test_constants.add_NotImplemented()
        test_constants.add_method_tested(1)       
        try:
            incoming_stream.close()
        except NotImplemented: 
            if e.saga_object != None: print e.message,"... ",        
        if type(sentence) != str:
            print "Server: type of sentence:",str(type(sentence))
        for i in range(len(test_constants.stream_data)):
            if sentence[i] != test_constants.stream_data[i]:
                print "Server: sentence ("+sentence+") != sent data at position"+str(i) 
                break
            if i == len(test_constants.stream_data)-1:
                print "Server: sentence ("+sentence+") == "+test_constants.stream_data

class TestStream(unittest.TestCase):
    u = URL(test_constants.streamservice_url)
    u.port = test_constants.streamservice_port
    server = Server()
    server.start()
    stream = Stream(url=u)
    
    def setUp(self):
        pass

    def tearDown(self):
        pass
 

    def test_1_init(self):
        pass

    def test_2_connect(self):
        try:
            self.failUnless( self.stream != None)
            self.stream.connect()
        except NotImplemented: 
            test_constants.add_NotImplemented() 
            if e.saga_object != None: print e.message,"... ",
        except IncorrectState:
            pass             
        test_constants.add_method_tested(1)

    def test_3_wait(self):
        try:
            test = self.stream.wait(Activity.WRITE|Activity.READ|Activity.EXCEPTION,5)
            print "====="+str(test)+"======== ... ",
            self.failUnless( type(test) == int )
        except NotImplemented: 
            if e.saga_object != None: print e.message,"... ",
            test_constants.add_NotImplemented()              
        test_constants.add_method_tested(1)            
   

    def test_4_write(self):
        try:
            self.stream.write(test_constants.stream_data)
        except NotImplemented: 
            if e.saga_object != None: print e.message,"... ",
            test_constants.add_NotImplemented()            
        test_constants.add_method_tested(1)                      
     


    def test_5_read(self):
        return

    def test_6_get_url(self):
        try:
            test = self.stream.get_url()
            self.failUnless( isinstance(test, URL) )
        except NotImplemented: 
            test_constants.add_NotImplemented()            
        test_constants.add_method_tested(1)
    
    def test_7_get_context(self):
#        try:
#            test = self.stream.get_context()
#            self.failUnless( isinstance(test, Context) )
#        except NotImplemented: 
#            test_constants.add_NotImplemented()            
#        test_constants.add_method_tested(1)
        pass

    def test_8_close(self):
        try:
            test = self.stream.close(timeout=10)
        except NotImplemented: 
            test_constants.add_NotImplemented()            
        test_constants.add_method_tested(1)   


    def test_Stream_properties(self):
        try:        
            self.failUnless( type(self.stream.Bufsize) == str )
        except NotImplemented:
            test_constants.add_NotImplemented() 
        
        try:        
            self.failUnless( type(self.stream.Timeout) == str )
        except NotImplemented:
            test_constants.add_NotImplemented() 
        
        try:        
            self.failUnless( type(self.stream.Blocking) == str )
        except NotImplemented:
            test_constants.add_NotImplemented() 
            
        try:        
            self.failUnless( type(self.stream.Compression) == str )
        except NotImplemented:
            test_constants.add_NotImplemented() 
            
        try:        
            self.failUnless( type(self.stream.Reliable) == str )
        except NotImplemented:
            test_constants.add_NotImplemented() 
        test_constants.add_method_tested(5)        
        #Properties
        #int     Bufsize
        #The Bufsize attribute.
        #int     Timeout
        #The Timeout attribute.
        #bool     Blocking
        #The Blocking attribute.
        #bool     Compression
        #The Compression attribute.
        #bool     Nodelay
        #The Nodelay attribute.
        #bool     Reliable
        #The Reliable attribute.


#Inherited from attributes.Attributes: attribute_exists, attribute_is_readonly, attribute_is_removable, attribute_is_vector, attribute_is_writable, find_attributes, get_attribute, get_vector_attribute, list_attributes, remove_attribute, set_attribute, set_vector_attribute
    def test_Attributes_methods(self):
        try:
            test = self.stream.list_attributes()
            self.failUnless( type(test) == list)
        except NotImplemented:
            test_constants.add_NotImplemented() 
        self.failUnless(len(test) != 0)
        try:
            self.failUnless(type(self.stream.attribute_exists(test[0]) == bool))
        except NotImplemented:
            test_constants.add_NotImplemented()                              
        try:
            self.failUnless(type(self.stream.attribute_is_readonly(test[0])==bool))
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            self.failUnless(type(self.stream.attribute_is_removable(test[0])==bool))
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            self.failUnless(type(self.stream.attribute_is_vector(test[0])==bool))
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            self.failUnless(type(self.stream.attribute_is_writable(test[0])==bool))
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            self.failUnless( type(self.stream.find_attributes([test[0]]) == list) )
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            self.failUnless( type(self.stream.get_attribute(test[0]) == str) )
        except NotImplemented:
            test_constants.add_NotImplemented()   
        except IncorrectState:
            pass
        try:
            self.failUnless(type(self.stream.get_vector_attribute(test[0])==list))
        except NotImplemented:
            test_constants.add_NotImplemented()   
        except IncorrectState:
            pass
        try:
            self.failUnless(type(self.stream.remove_attribute(test[0])==None))
        except NotImplemented:
            test_constants.add_NotImplemented()         
        except PermissionDenied:
            pass
        try:
            self.failUnless(type(self.stream.remove_attribute(test[0])==None))
        except NotImplemented:
            test_constants.add_NotImplemented()         
        except PermissionDenied:
            pass
        except IncorrectState:
            pass
        try:
            self.failUnless(type(self.stream.set_attribute(test[0],"")==None))
        except NotImplemented:
            test_constants.add_NotImplemented()         
        except PermissionDenied:
            pass 
        except IncorrectState:
            pass        
        try:
          self.failUnless(type(self.stream.set_vector_attribute(test[0],[])==None))
        except NotImplemented:
            test_constants.add_NotImplemented()         
        except PermissionDenied:
            pass 
        except IncorrectState:
            pass 
        test_constants.add_method_tested(13)



#Inherited from monitoring.Monitorable: add_callback, get_metric, list_metrics, remove_callback
    def test_Monitorable_methods(self):
        test = 0
        try:
            c = Callback()
            test = self.stream.add_callback("stream.state", c)
        except NotImplemented:
            test_constants.add_NotImplemented()            
        try:
            self.stream.remove_callback("stream.state", test)
        except NotImplemented:
            test_constants.add_NotImplemented()
        try: 
            test = self.stream.list_metrics()
        except NotImplemented:
            test_constants.add_NotImplemented()
        try:
            test = self.stream.get_metric("stream.state")
            self.failUnless( isinstance(test, Metric) )
        except NotImplemented:
            test_constants.add_NotImplemented()
        test_constants.add_method_tested(4)        

#Inherited from permissions.Permissions: get_group, get_owner, permissions_allow, permissions_check, permissions_deny
    def test_Permissions_methods(self):
        try:
            temp = self.stream.get_group()
            self.failUnless( type(test) == str)
        except NotImplemented:
            test_constants.add_NotImplemented()
 
        try:
            temp = self.stream.get_owner()
            self.failUnless( type(test) == str)
        except NotImplemented:
            test_constants.add_NotImplemented()  
        
        try:
            self.stream.permissions_allow("*",Permission.QUERY)
        except NotImplemented:
            test_constants.add_NotImplemented() 

        try:
            self.stream.permissions_deny("*",Permission.QUERY)
        except NotImplemented:
            test_constants.add_NotImplemented() 
            
        try:
            self.stream.permissions_check("*",Permission.QUERY)
        except NotImplemented:
            test_constants.add_NotImplemented() 
        test_constants.add_method_tested(5)


#Inherited from attributes.Attributes: attributes            
    def test_Attributes_properties(self):
        try:        
            self.failUnless( type(self.stream.attributes) == dict)
        except NotImplemented:
            test_constants.add_NotImplemented()
        test_constants.add_method_tested(1)

#Inherited from object.Object: id, session, type
    def test_Object_properties(self):
        try:        
            self.failUnless( type(self.stream.id) == str)
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:        
            self.failUnless( isinstance(self.stream.session, Session) )
        except NotImplemented:
            test_constants.add_NotImplemented()
        try:        
            self.failUnless( type(self.stream.type) == str )
        except NotImplemented:
            test_constants.add_NotImplemented()  
        test_constants.add_method_tested(3)

#Inherited from monitoring.Monitorable: metrics
    def test_Monitorable_properties(self):
        try:        
            self.failUnless( type(self.stream.metrics) == list)
        except NotImplemented:
            test_constants.add_NotImplemented()  
        test_constants.add_method_tested(1) 
        
#Inherited from permissions.Permissions: group, owner 
    def test_Permissions_properties(self):
        try:        
            self.failUnless( type(self.stream.group) == str )
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:        
            self.failUnless( type(self.stream.owner) == str )
        except NotImplemented:
            test_constants.add_NotImplemented()
        test_constants.add_method_tested(2)         

#Inherited from object.Object: clone, get_id, get_session, get_type
    def test_Object_methods(self):
        o = self.stream

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
            test = clone.get_url()
            self.failUnless( isinstance(test, URL) )
            self.failUnless( isinstance(o.Executable,str) )
        except NotImplemented: test_constants.add_NotImplemented()            
        test_constants.add_method_tested(4)


    def test_zzz_nr_NotImplemented(self):
        if test_constants.result_NotImplemented() != 0\
        and test_constants.print_not_implemented == True:
            base = float(test_constants.result_methods_tested())/100
            percentage = 100-float(test_constants.result_NotImplemented())//base

            print "\nNumber of methods not implemented in Stream:",\
                test_constants.result_NotImplemented(), "of",\
                test_constants.result_methods_tested(), "methods",\
                "("+str(percentage)+"% implemented) ... ",




if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestStream)
    unittest.TextTestRunner(verbosity=2).run(suite)

