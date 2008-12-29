from saga.error import *
from saga.context import Context
import unittest
import test_constants

class TestException(unittest.TestCase):

    def setUp(self):
        self.excep =  SagaException("message", Context())

    def test_get_object(self):
        try:
            self.excep.get_object()
        except NotImplemented:
            test_constants.add_NotImplemented() 
        test_constants.add_method_tested(1)            

    def test_get_message(self):
        try:
            self.excep.get_message()
        except NotImplemented:
            test_constants.add_NotImplemented() 
        test_constants.add_method_tested(1)    

    def test_Exception_properties(self):
        try:
            test = self.excep.saga_object
        except NotImplemented:
            test_constants.add_NotImplemented() 
        test_constants.add_method_tested(1) 
        try:
            test = self.excep.message
        except NotImplemented:
            test_constants.add_NotImplemented() 
        test_constants.add_method_tested(1)

    def test_Exception_subclasses(self):
        e = NotImplemented("message", Context())
        e = IncorrectURL("message", Context())
        e = BadParameter("message", Context())
        e = AlreadyExists("message", Context())
        e = DoesNotExist("message", Context())
        e = IncorrectState("message", Context())
        e = PermissionDenied("message", Context())
        e = AuthorizationFailed("message", Context())
        e = AuthenticationFailed("message", Context())
        e = Timeout("message", Context())
        e = NoSuccess("message", Context())
        
    def test_zzz_nr_NotImplemented(self):
        if test_constants.result_NotImplemented() != 0\
        and test_constants.print_not_implemented == True:
            base = float(test_constants.result_methods_tested())/100
            percentage = 100-float(test_constants.result_NotImplemented())//base

            print "\nNumber of methods not implemented in Exception:",\
                test_constants.result_NotImplemented(), "of",\
                test_constants.result_methods_tested(), "methods",\
                "("+str(percentage)+"% implemented) ... ",

if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestException)
    unittest.TextTestRunner(verbosity=2).run(suite)