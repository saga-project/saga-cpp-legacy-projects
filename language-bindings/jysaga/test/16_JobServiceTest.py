from saga.session import Session
from saga.context import Context
from saga.error import *
from saga.object import ObjectType, Object
from saga.job import JobDescription, JobService, JobSelf, State, StdIO, Job
import unittest
import test_constants

class TestJobService(unittest.TestCase):
    
    def setUp(self):
        self.js = JobService('localhost')
        
    def test_list(self):
        try:
            test = self.js.list()
            self.failUnless( isinstance(test,list) )
        except NotImplemented, e:
            if e.saga_object != None: print e.message,"... ", 
            test_constants.add_NotImplemented()
        test_constants.add_method_tested()



    def test_create_job(self):
        output_file = "/tmp/OUTPUT_FILE"
        try:
            jd = JobDescription()
            jd.Executable = test_constants.jobservice_Executable
            jd.Arguments = test_constants.jobservice_Arguments
            jd.Output = output_file
            jd.Cleanup = test_constants.jobservice_Cleanup
            test = self.js.create_job(jd)
            self.failUnless( isinstance(test,Job) )
            test.run()
            test.wait() 
            f=file(output_file)
            del f
            import os
            os.remove(output_file)
        except NotImplemented, e:
            if e.saga_object != None: print e.message,"... ",
            test_constants.add_NotImplemented()        
        test_constants.add_method_tested()


    
    def test_run_job(self):
        try:
            test, inp, outp, err = self.js.run_job(\
                            commandline=test_constants.jobservice_commandline,
                            host=test_constants.jobservice_host)
            self.failUnless( isinstance(test,Job) )
            self.failUnless( isinstance(inp, StdIO) )
            self.failUnless( isinstance(outp,StdIO) )
            self.failUnless( isinstance(err, StdIO) )
        except NotImplemented, e:
            if e.saga_object != None: print e.message,"... ", 
            test_constants.add_NotImplemented() 
        test_constants.add_method_tested()


    
    def test_get_job(self):
        output_file = "/tmp/OUTPUT_FILE"
        try:
            jd = JobDescription()
            jd.Executable = test_constants.jobservice_Executable
            jd.Arguments = test_constants.jobservice_Arguments_2
            test = self.js.create_job(jd)
            self.failUnless( isinstance(test,Job) )
            test.run()            
            tmp = self.js.list()
            j = self.js.get_job(tmp[0])
            self.failUnless( isinstance(j,Job) )
            j.wait() 
        except NotImplemented, e: 
            if e.saga_object != None: print e.message,"... ",
            test_constants.add_NotImplemented()
        test_constants.add_method_tested()


    
    def test_get_job_self(self):
        try:
            test = self.js.get_self()
            self.failUnless( isinstance(test,JobSelf) )
            self.failUnless( type(test.get_state()) == int )
        except NotImplemented, e: 
            if e.saga_object != None: print e.message(),"... ",
            test_constants.add_NotImplemented()
        test_constants.add_method_tested()



    def test_Object_Methods(self):
        o = self.js

        try:
            test = o.get_id() 
            self.failUnless( (type(test) == str)) 
        except NotImplemented, e: 
            if e.saga_object != None: print e.message,"... ",
            test_constants.add_NotImplemented()
        
        try:
            test = o.get_type()
            self.failUnless( (type(test) == int) )
            self.failUnless( test == ObjectType.JOBSERVICE )
        except NotImplemented:
            if e.saga_object != None: print e.message,"... ",
            test_constants.add_NotImplemented()    
            
        try:  
            test =  o.get_session()
            self.failUnless( isinstance(test, Session) )
            self.failUnless( test.get_type() == ObjectType.SESSION )
        except NotImplemented: 
            if e.saga_object != None: print e.message,"... ",
            test_constants.add_NotImplemented()
        except DoesNotExist:pass
                       
        try:     
            clone = o.clone()
            self.failUnless( isinstance(o.list(),list) )
        except NotImplemented: 
            if e.saga_object != None: print e.message,"... ",            
            test_constants.add_NotImplemented()            
        test_constants.add_method_tested(4)


    def test_zzz_nr_NotImplemented(self):
        #if test_constants.result_NotImplemented() != 0:# and 
        if test_constants.print_not_implemented == True:
            base = float(test_constants.result_methods_tested())/100
            percentage = 100-float(test_constants.result_NotImplemented())//base

            print "Number of methods not implemented in JobService:",\
                test_constants.result_NotImplemented(), "of",\
                test_constants.result_methods_tested(), "methods",\
                "("+str(percentage)+"% implemented)","... ",




if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestJobService)
    result = unittest.TextTestRunner(verbosity=2).run(suite)


    
    