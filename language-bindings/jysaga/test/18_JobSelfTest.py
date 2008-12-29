from saga.session import Session
from saga.context import Context
from saga.error import *
from saga.object import ObjectType, Object
from saga.job import JobDescription, JobService, JobSelf, State, StdIO, Job
from saga.permissions import Permission
from saga.monitoring import Monitorable, Callback, Steerable, Metric
import unittest
import test_constants

class TestJobSelf(unittest.TestCase):
    js = JobService(test_constants.job_hostname)
    try:
        job = js.get_self()
    except NotImplemented, e:
        print "get_self is not implemented. Cannot execute any test."
        raise e
        
    def setUp(self):
        pass

    def tearDown(self):
        pass

#Inherited from task.Task: __del__, cancel, get_object, get_result, get_state, rethrow, run, wait
    def test_Task_methods(self):
        try:
            self.job.get_object()
        except NotImplemented:
            test_constants.add_NotImplemented()            

        try:
            self.failUnless( type(self.job.get_state()) != int)
        except NotImplemented:
            test_constants.add_NotImplemented() 

        try:
            self.job.run()
        except NotImplemented:
            test_constants.add_NotImplemented() 
        except IncorrectState:
            pass

        try:
            self.job.get_object()
        except NotImplemented:
            test_constants.add_NotImplemented()                         

        try:
            self.job.wait()
        except NotImplemented:
            test_constants.add_NotImplemented() 
        except IncorrectState:
            pass
 
        try:
            self.job.rethrow()
        except NotImplemented:
            test_constants.add_NotImplemented() 
        except IncorrectState:
            pass   
        
        try:
            self.job.get_result()
        except NotImplemented:
            test_constants.add_NotImplemented() 
        except IncorrectState:
            pass  
        
        try:
            self.job.cancel()
        except NotImplemented:
            test_constants.add_NotImplemented() 
        except IncorrectState:
            pass                       
 

#Inherited from monitoring.Steerable: add_metric, fire_metric, remove_metric
    def test_Steerable_methods(self):
        try:
            m = Metric("TotalCPUCount","total number of cpus requested for this job",\
           "ReadWrite","1","Int","1") 
            test = self.job.add_metric(m)
            self.failUnless(type(test) != bool)
        except NotImplemented:
            test_constants.add_NotImplemented()        
        try:
            self.job.remove_metric("TotalCPUCount")
        except NotImplemented:
            test_constants.add_NotImplemented()             
        try:
            self.job.fire_metric("TotalCPUCount")
        except NotImplemented:
            test_constants.add_NotImplemented()            
        test_constants.add_method_tested(3)


#Inherited from monitoring.Monitorable: add_callback, get_metric, list_metrics, remove_callback
    def test_Monitorable_methods(self):
        test = 0
        try:
            c = Callback()
            test = self.job.add_callback("Task.state", c)
        except NotImplemented:
            test_constants.add_NotImplemented()            
        try:
            self.job.remove_callback(test)
        except NotImplemented:
            test_constants.add_NotImplemented()
        try: 
            test = self.job.list_metrics()
        except NotImplemented:
            test_constants.add_NotImplemented()
        try:
            test = self.job.get_metric("Task.state")
            self.failUnless( isinstance(test, Metric) )
        except NotImplemented:
            test_constants.add_NotImplemented()
        test_constants.add_method_tested(4)        
    


#Inherited from attributes.Attributes: attribute_exists, attribute_is_readonly, attribute_is_removable, attribute_is_vector, attribute_is_writable, find_attributes, get_attribute, get_vector_attribute, list_attributes, remove_attribute, set_attribute, set_vector_attribute
    def test_Attributes_methods(self):
        try:
            test = self.job.list_attributes()
            self.failUnless( type == list)
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            self.failUnless(type(self.job.attribute_exists(test[0]) == bool))
        except NotImplemented:
            test_constants.add_NotImplemented()                              
        try:
            self.failUnless(type(self.job.attribute_is_readonly(test[0])==bool))
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            self.failUnless(type(self.job.attribute_removable(test[0])==bool))
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            self.failUnless(type(self.job.attribute_is_vector(test[0])==bool))
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            self.failUnless(type(self.job.attribute_is_writable(test[0])==bool))
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            self.failUnless( type(self.job.find_attributes(test[0]) == list) )
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            self.failUnless( type(self.job.get_attribute(test[0]) == str) )
        except NotImplemented:
            test_constants.add_NotImplemented()   
        except IncorrectState:
            pass
        try:
            self.failUnless(type(self.job.get_vector_attribute(test[0])==list))
        except NotImplemented:
            test_constants.add_NotImplemented()   
        except IncorrectState:
            pass
        try:
            self.failUnless(type(self.job.remove_attribute(test[0])==None))
        except NotImplemented:
            test_constants.add_NotImplemented()         
        except PermissionDenied:
            pass
        try:
            self.failUnless(type(self.job.remove_attribute(test[0])==None))
        except NotImplemented:
            test_constants.add_NotImplemented()         
        except PermissionDenied:
            pass
        except IncorrectState:
            pass
        try:
            self.failUnless(type(self.job.set_attribute(test[0],"")==None))
        except NotImplemented:
            test_constants.add_NotImplemented()         
        except PermissionDenied:
            pass 
        except IncorrectState:
            pass        
        try:
          self.failUnless(type(self.job.set_vector_attribute(test[0],[])==None))
        except NotImplemented:
            test_constants.add_NotImplemented()         
        except PermissionDenied:
            pass 
        except IncorrectState:
            pass 
        test_constants.add_method_tested(13)


#Inherited from Job: checkpoint, get_job_description, get_stderr, get_stdin, get_stdout, migrate, resume, signal, suspend
    def test_Job_description(self):
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

#Inherited from object.Object: clone, get_id, get_session, get_type
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
            self.failUnless( test == ObjectType.JOBSELF )
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

#Inherited from Job: Created, ExecutionHosts, ExitCode, Finished, JobID, Started, Termsig, WorkingDirectory
    def test_Job_properties(self):
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

#Inherited from permissions.Permissions: get_group, get_owner, permissions_allow, permissions_check, permissions_deny
    def test_Permissions_methods(self):
        try:
            temp = self.job.get_group()
            self.failUnless( type(test) == str)
        except NotImplemented:
            test_constants.add_NotImplemented()
 
        try:
            temp = self.job.get_owner()
            self.failUnless( type(test) == str)
        except NotImplemented:
            test_constants.add_NotImplemented()  
        
        try:
            self.job.permissions_allow("*",Permission.QUERY)
        except NotImplemented:
            test_constants.add_NotImplemented() 

        try:
            self.job.permissions_deny("*",Permission.QUERY)
        except NotImplemented:
            test_constants.add_NotImplemented() 
            
        try:
            self.job.permissions_check("*",Permission.QUERY)
        except NotImplemented:
            test_constants.add_NotImplemented() 
        test_constants.add_method_tested(5)
 
#Inherited from permissions.Permissions: group, owner  
    def test_Permissions_properties(self):
        try:        
            self.failUnless( type(self.job.group) == str )
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:        
            self.failUnless( type(self.job.owner) == str )
        except NotImplemented:
            test_constants.add_NotImplemented()
        test_constants.add_method_tested(2)         


#Inherited from attributes.Attributes: attributes            
    def test_Attributes_properties(self):
        try:        
            self.failUnless( type(self.job.attributes) == dict)
        except NotImplemented:
            test_constants.add_NotImplemented()
        test_constants.add_method_tested(1)

#Inherited from monitoring.Monitorable: metrics
    def test_Monitorable_properties(self):
        try:        
            self.failUnless( type(self.job.metrics) == list)
        except NotImplemented:
            test_constants.add_NotImplemented()  
        test_constants.add_method_tested(1) 
            
#Inherited from object.Object: id, session, type
    def test_Object_properties(self):
        try:        
            self.failUnless( type(self.job.id) == str)
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:        
            self.failUnless( isinstance(self.job.session, Session) )
        except NotImplemented:
            test_constants.add_NotImplemented()
        try:        
            self.failUnless( type(self.job.type) == str )
        except NotImplemented:
            test_constants.add_NotImplemented()  
        test_constants.add_method_tested(3)
            
#Inherited from task.Task: object, result, state
    def test_Task_properties(self):
        try: 
            test = self.job.object      
        except NotImplemented:
            test_constants.add_NotImplemented() 
        try:
            test = self.job.result        
        except NotImplemented:
            test_constants.add_NotImplemented()
        try:        
            self.failUnless( type(self.job.state) == int )
        except NotImplemented:
            test_constants.add_NotImplemented()                    
        test_constants.add_method_tested(3)                                   




 
                        
    def test_zzz_nr_NotImplemented(self):
        if test_constants.print_not_implemented == True:
            base = float(test_constants.result_methods_tested())/100
            percentage = 100-float(test_constants.result_NotImplemented())//base

            print "Number of methods not implemented in JobSelf:",\
                test_constants.result_NotImplemented(), "of",\
                test_constants.result_methods_tested(), "methods",\
                "("+str(percentage)+"% implemented)","... ",




if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestJob)
    result = unittest.TextTestRunner(verbosity=2).run(suite)


    
    