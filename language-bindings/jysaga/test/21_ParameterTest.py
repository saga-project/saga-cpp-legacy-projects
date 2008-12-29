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



class TestParameter(unittest.TestCase):
    parameter = None

    def setUp(self):
        self.parameter = Parameter()

    def tearDown(self):
        pass
 
    def test_1_init(self):
      self.parameter = Parameter()
      self.parameter = Parameter("string")
      self.parameter = Parameter(1)
      self.parameter = Parameter(True)
      self.parameter = Parameter([1,2,3,4,5])
      self.parameter = Parameter(0.0)
         

    def test_2_set_io_mode(self):
        try:        
            self.parameter.set_io_mode(IOMode.IN)
            self.parameter.set_io_mode(IOMode.INOUT)
            self.parameter.set_io_mode(IOMode.OUT)
        except NotImplemented:
            test_constants.add_NotImplemented() 
        test_constants.add_method_tested(1)


    def test_3_get_io_mode(self):
        try:        
            self.failUnless(type(self.parameter.get_io_mode()) == int )
        except NotImplemented:
            test_constants.add_NotImplemented() 
        test_constants.add_method_tested(1) 


    def test_Buffer_methods(self):
        import array.array
        a = array.zeros('c', 10)
        try:
            self.parameter.set_data(a)
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            test = self.parameter.get_data()
            self.failUnless(isinstance(test,list), str(test.__class__))
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            self.parameter.set_size(5)
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            test = self.parameter.get_size()
            self.failUnless( type(test) != 5)
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            self.parameter.close()
        except NotImplemented:
            test_constants.add_NotImplemented()           
        test_constants.add_method_tested(5)




#Inherited from buffer.Buffer: data, size
    def test_Buffer_properties(self):
        import array.array
        try:
            self.parameter.data = [1,2,3]
        except NotImplemented:
            test_constants.add_NotImplemented()
        try:        
            self.failUnless(type(self.parameter.data) == str or\
                            type(self.parameter.data) == list or \
                            type(self.parameter.data) == array.array, \
                            str(self.parameter.data.__class__))
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:        
            self.parameter.size = 10
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:        
            self.failUnless( type(self.parameter.size) == int )
        except NotImplemented:
            test_constants.add_NotImplemented() 
        test_constants.add_method_tested(4)


    def test_Parameter_properties(self):
        try:        
            self.failUnless( type(self.parameter.mode) == int )
        except NotImplemented:
            test_constants.add_NotImplemented() 
        test_constants.add_method_tested(1)




#Inherited from object.Object: id, session, type
    def test_Object_properties(self):
        try:        
            self.failUnless( type(self.parameter.id) == str)
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:        
            self.failUnless( isinstance(self.parameter.session, Session) )
        except NotImplemented:
            test_constants.add_NotImplemented()
        except DoesNotExist:
            pass
        try:        
            self.failUnless( type(self.parameter.type) == str )
        except NotImplemented:
            test_constants.add_NotImplemented()  
        test_constants.add_method_tested(3)

       

#Inherited from object.Object: clone, get_id, get_session, get_type
    def test_Object_methods(self):
        o = self.parameter

        try:
            test = o.get_id() 
            self.failUnless( (type(test) == str)) 
        except NotImplemented: test_constants.add_NotImplemented()
        
        try:
            test = o.get_type()
            self.failUnless( (type(test) == int) )
            self.failUnless( test == ObjectType.PARAMETER )
        except NotImplemented: test_constants.add_NotImplemented()    
            
        try:  
            test =  o.get_session()
            self.failUnless( isinstance(test, Session) )
            self.failUnless( test.get_type() == ObjectType.SESSION )
        except NotImplemented: test_constants.add_NotImplemented()
        except DoesNotExist:pass
                       
        try:     
            clone = o.clone()
            self.failUnless( type(o.get_io_mode) != int )
        except NotImplemented: test_constants.add_NotImplemented()            
        test_constants.add_method_tested(4)


    def test_zzz_nr_NotImplemented(self):
        if test_constants.result_NotImplemented() != 0\
        and test_constants.print_not_implemented == True:
            base = float(test_constants.result_methods_tested())/100
            percentage = 100-float(test_constants.result_NotImplemented())//base

            print "\nNumber of methods not implemented in Parameter:",\
                test_constants.result_NotImplemented(), "of",\
                test_constants.result_methods_tested(), "methods",\
                "("+str(percentage)+"% implemented) ... ",




if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestParameter)
    unittest.TextTestRunner(verbosity=2).run(suite)

