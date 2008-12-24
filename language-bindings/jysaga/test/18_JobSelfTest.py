from saga.session import Session
from saga.context import Context
from saga.error import *
from saga.object import ObjectType, Object
from saga.job import JobDescription, JobService, JobSelf, State, StdIO, Job
import unittest
import test_constants

class TestJob(unittest.TestCase):
    js = JobService(test_constants.job_hostname)
    jd = JobDescription()
    jd.Executable = test_constants.job_Executable
    jd.Arguments = test_constants.job_Arguments
    jd.WorkingDirectory = test_constants.job_WorkingDirectory
        
    def setUp(self):
        try:
            self.job = self.js.create_job(self.jd)
        except NotImplemented, e: 
            if e.saga_object != None: print e.message,"... ",
            test_constants.add_NotImplemented()
            test_constants.add_method_tested()          

    def tearDown(self):
        try:
            self.job.cancel(0)
        except IncorrectState,e:
            pass

        
    def test_get_job_description(self):
        try:
            test = self.job.get_job_description()
            self.failUnless( isinstance(test, JobDescription) )
        except NotImplemented, e:
            if e.saga_object != None: print e.message,"... ", 
            test_constants.add_NotImplemented()
        test_constants.add_method_tested()



    def test_get_stdin(self):
        try:
            jd = JobDescription()
            jd.Executable = test_constants.job_Executable
            jd.Arguments = test_constants.job_Arguments
            jd.Interactive = test_constants.job_Interactive
            job = self.js.create_job(jd)
            test = job.get_stdin()
            self.failUnless( isinstance(test, StdIO) )            
        except NotImplemented, e:
            if e.saga_object != None: print e.message,"... ",
            test_constants.add_NotImplemented()        
        test_constants.add_method_tested()

    def test_get_stdout(self):
        try:
            jd = JobDescription()
            jd.Executable = test_constants.job_Executable
            jd.Arguments = test_constants.job_Arguments
            jd.Interactive = test_constants.job_Interactive
            job = self.js.create_job(jd)
            test = job.get_stdout()
            self.failUnless( isinstance(test, StdIO) ) 
        except NotImplemented, e:
            if e.saga_object != None: print e.message,"... ",
            test_constants.add_NotImplemented()        
        test_constants.add_method_tested()
        
    def test_get_stderr(self):
        try:
            jd = JobDescription()
            jd.Executable = test_constants.job_Executable
            jd.Arguments = test_constants.job_Arguments
            jd.Interactive = test_constants.job_Interactive
            job = self.js.create_job(jd)
            test = job.get_stderr()
            self.failUnless( isinstance(test, StdIO) ) 
        except NotImplemented, e:
            if e.saga_object != None: print e.message,"... ",
            test_constants.add_NotImplemented()        
        test_constants.add_method_tested()

    def test_suspend(self):
        try:
            jd = JobDescription()
            jd.Executable = test_constants.job_Executable
            jd.Arguments = test_constants.job_Arguments
            job = self.js.create_job(jd)
            job.run()
            job.suspend()
            self.failUnless( job.state()==State.SUSPENDED, str(job.state()) )
        except NotImplemented, e:
            if e.saga_object != None: print e.message,"... ",
            test_constants.add_NotImplemented()        
        test_constants.add_method_tested()
 
 
    def test_resume(self):
        try:
            jd = JobDescription()
            jd.Executable = test_constants.job_Executable
            jd.Arguments = test_constants.job_Arguments
            job = self.js.create_job(jd)
            job.run()
            job.suspend()
            job.resume()
            self.failUnless( job.state()==State.RUNNING, str(job.state()) )
        except NotImplemented, e:
            if e.saga_object != None: print e.message,"... ",
            test_constants.add_NotImplemented()        
        test_constants.add_method_tested()
        

    def test_checkpoint(self):
        try:
            jd = JobDescription()
            jd.Executable = test_constants.job_Executable
            jd.Arguments = test_constants.job_Arguments
            job = self.js.create_job(jd)
            job.run()
            job.checkpoint()
        except NotImplemented, e:
            if e.saga_object != None: print e.message,"... ",
            test_constants.add_NotImplemented()        
        test_constants.add_method_tested()        
   
    def test_migrate(self):
        try:
            jd = JobDescription()
            jd.Executable = test_constants.job_Executable
            jd.Arguments = test_constants.job_Arguments
            job = self.js.create_job(jd)
            job.run()
            jd.Arguments = test_constants.job_Arguments_alternative
            job.migrate(jd)
        except NotImplemented, e:
            if e.saga_object != None: print e.message,"... ", 
            test_constants.add_NotImplemented() 
        test_constants.add_method_tested()


    def test_signal(self):
        try:
            jd = JobDescription()
            jd.Executable = test_constants.job_Executable
            jd.Arguments = test_constants.job_Arguments
            job = self.js.create_job(jd)
            job.run()
            job.signal(9)
        except NotImplemented, e:
            if e.saga_object != None: print e.message,"... ", 
            test_constants.add_NotImplemented() 
        test_constants.add_method_tested()


    def test_Object_Methods(self):
        o = self.job
    
        try:
            test = o.get_id() 
            self.failUnless( (type(test) == str)) 
        except NotImplemented, e: 
            if e.saga_object != None: print e.message,"... ",
            test_constants.add_NotImplemented()
        
        try:
            test = o.get_type()
            self.failUnless( (type(test) == int) )
            self.failUnless( test == ObjectType.JOB )
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
            self.failUnless(isinstance(o.get_job_description(),JobDescription))
        except NotImplemented: 
            if e.saga_object != None: print e.message,"... ",            
            test_constants.add_NotImplemented()            
        test_constants.add_method_tested(4)

    def test_properties(self):
        test_constants.add_method_tested(8)
        print self.job.get_job_description().attributes
        self.job.run()
        print self.job.attributes
        try:
            print self.job.JobID
            self.failUnless( type(self.job.JobID) == str)
        except NotImplemented:
            test_constants.add_NotImplemented()
        
        try:            
            self.failUnless( type(self.job.ExecutionHosts) == list)
        except NotImplemented:
            test_constants.add_NotImplemented()            
        
        try:            
            self.failUnless( type(self.job.Created) == str)
        except NotImplemented:
            test_constants.add_NotImplemented()
        
        try:                
            self.failUnless( type(self.job.Started) == str)
        except NotImplemented:
            test_constants.add_NotImplemented()
        
        try:       
            self.failUnless( type(self.job.Finished) == str)
        except NotImplemented:
            test_constants.add_NotImplemented()
        self.job.wait()
        
        try:        
            self.failUnless( type(self.job.ExitCode) == str)
        except NotImplemented:
            test_constants.add_NotImplemented()        
        
        try:        
            self.failUnless( type(self.job.Termsig) == str)
        except NotImplemented:
            test_constants.add_NotImplemented()

        try:
            self.failUnless( type(self.job.WorkingDirectory) == str)
        except NotImplemented:
            test_constants.add_NotImplemented()
        

    def __get_Termsig(self):
        return self.get_attribute("Termsig")  

    Termsig = property(__get_Termsig, doc="""The Termsig attribute.\n
                          @type:int""")
        


    def test_zzz_nr_NotImplemented(self):
        if test_constants.print_not_implemented == True:
            base = float(test_constants.result_methods_tested())/100
            percentage = 100-float(test_constants.result_NotImplemented())//base

            print "Number of methods not implemented in Job:",\
                test_constants.result_NotImplemented(), "of",\
                test_constants.result_methods_tested(), "methods",\
                "("+str(percentage)+"% implemented)","... ",




if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestJob)
    result = unittest.TextTestRunner(verbosity=2).run(suite)


    
    