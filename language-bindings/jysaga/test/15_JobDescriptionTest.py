from saga.session import Session
from saga.context import Context
from saga.error import *
from saga.object import ObjectType, Object
from saga.job import JobDescription
import unittest
import test_constants

class TestJobDescription(unittest.TestCase):
    
    def setUp(self):
        self.jd = JobDescription()
        
    def test_Excecutable_attribute(self):
        try:
            self.jd.Executable = test_constants.jobdescription_Executable
        except NotImplemented: test_constants.add_NotImplemented()

        try:
            test = self.jd.Executable
        except NotImplemented: test_constants.add_NotImplemented()
        self.failUnlessEqual(test, test_constants.jobdescription_Executable)
        
        try:
            del self.jd.Executable
            self.assertRaises(AttributeError, self.jd.Executable)
        except NotImplemented: test_constants.add_NotImplemented()            
        test_constants.add_method_tested(3)

           
    def test_Arguments_attribute(self):
        try:
            self.jd.Arguments = test_constants.jobdescription_Arguments
        except NotImplemented: test_constants.add_NotImplemented()

        try:
            test = self.jd.Arguments
        except NotImplemented: test_constants.add_NotImplemented()
        self.failUnlessEqual(test, test_constants.jobdescription_Arguments)
        
        try:
            del self.jd.Arguments
            self.assertRaises(AttributeError, self.jd.Arguments)
        except NotImplemented: test_constants.add_NotImplemented()            
        test_constants.add_method_tested(3)
        

    def test_SPMDVariation_attribute(self):
        try:
            self.jd.SPMDVariation = test_constants.jobdescription_SPMDVariation
        except NotImplemented: test_constants.add_NotImplemented()

        try:
            test = self.jd.SPMDVariation
        except NotImplemented: test_constants.add_NotImplemented()
        self.failUnlessEqual(test, test_constants.jobdescription_SPMDVariation)
        
        try:
            del self.jd.SPMDVariation
            self.assertRaises(AttributeError, self.jd.SPMDVariation)
        except NotImplemented: test_constants.add_NotImplemented()            
        test_constants.add_method_tested(3)

 

    def test_TotalCPUCount_attribute(self):    
        try:
            self.jd.TotalCPUCount = test_constants.jobdescription_TotalCPUCount
        except NotImplemented: test_constants.add_NotImplemented()

        try:
            test = self.jd.TotalCPUCount
        except NotImplemented: test_constants.add_NotImplemented()
        self.failUnlessEqual(test, test_constants.jobdescription_TotalCPUCount)
        
        try:
            del self.jd.TotalCPUCount
            self.assertRaises(AttributeError, self.jd.TotalCPUCount)
        except NotImplemented: test_constants.add_NotImplemented()            
        test_constants.add_method_tested(3)
        
      
    def test_NumberOfProcesses_attribute(self):    
        try:
            self.jd.NumberOfProcesses = \
                                test_constants.jobdescription_NumberOfProcesses
        except NotImplemented: test_constants.add_NotImplemented()

        try:
            test = self.jd.NumberOfProcesses
        except NotImplemented: test_constants.add_NotImplemented()
        self.failUnlessEqual(test, 
                             test_constants.jobdescription_NumberOfProcesses)
        
        try:
            del self.jd.NumberOfProcesses
            self.assertRaises(AttributeError, self.jd.NumberOfProcesses)
        except NotImplemented: test_constants.add_NotImplemented()            
        test_constants.add_method_tested(3)
             

    def test_ProcessesPerHost_attribute(self):    
        try:
            self.jd.ProcessesPerHost = \
                                test_constants.jobdescription_ProcessesPerHost
        except NotImplemented: test_constants.add_NotImplemented()

        try:
            test = self.jd.ProcessesPerHost
        except NotImplemented: test_constants.add_NotImplemented()
        self.failUnlessEqual(test, 
                             test_constants.jobdescription_ProcessesPerHost)
        
        try:
            del self.jd.ProcessesPerHost
            self.assertRaises(AttributeError, self.jd.ProcessesPerHost)
        except NotImplemented: test_constants.add_NotImplemented()            
        test_constants.add_method_tested(3)


    def test_ThreadsPerProcess_attribute(self):    
        try:
            self.jd.ThreadsPerProcess = \
                                test_constants.jobdescription_ThreadsPerProcess
        except NotImplemented: test_constants.add_NotImplemented()

        try:
            test = self.jd.ThreadsPerProcess
        except NotImplemented: test_constants.add_NotImplemented()
        self.failUnlessEqual(test, 
                             test_constants.jobdescription_ThreadsPerProcess)
        
        try:
            del self.jd.ThreadsPerProcess
            self.assertRaises(AttributeError, self.jd.ThreadsPerProcess)
        except NotImplemented: test_constants.add_NotImplemented()            
        test_constants.add_method_tested(3)
             

    def test_Environment_attribute(self):
        try:
            self.jd.Environment = test_constants.jobdescription_Environment
        except NotImplemented: test_constants.add_NotImplemented()

        try:
            test = self.jd.Environment
        except NotImplemented: test_constants.add_NotImplemented()
        self.failUnlessEqual(test, test_constants.jobdescription_Environment)
        
        try:
            del self.jd.Environment
            self.assertRaises(AttributeError, self.jd.Environment)
        except NotImplemented: test_constants.add_NotImplemented()            
        test_constants.add_method_tested(3)


    def test_WorkingDirectory_attribute(self):    
        try:
            self.jd.WorkingDirectory = \
                                test_constants.jobdescription_WorkingDirectory
        except NotImplemented: test_constants.add_NotImplemented()

        try:
            test = self.jd.WorkingDirectory
        except NotImplemented: test_constants.add_NotImplemented()
        self.failUnlessEqual(test, 
                             test_constants.jobdescription_WorkingDirectory)
        
        try:
            del self.jd.WorkingDirectory
            self.assertRaises(AttributeError, self.jd.WorkingDirectory)
        except NotImplemented: test_constants.add_NotImplemented()           
        test_constants.add_method_tested(3)
        
 
    def test_Interactive_attribute(self):    
        try:
            self.jd.Interactive = test_constants.jobdescription_Interactive
        except NotImplemented: test_constants.add_NotImplemented()

        try:
            test = self.jd.Interactive
        except NotImplemented: test_constants.add_NotImplemented()
        self.failUnlessEqual(test, test_constants.jobdescription_Interactive)
        
        try:
            del self.jd.Interactive
            self.assertRaises(AttributeError, self.jd.Interactive)
        except NotImplemented: test_constants.add_NotImplemented()            
        test_constants.add_method_tested(3)
        

    def test_Input_attribute(self):    
        try:
            self.jd.Input = test_constants.jobdescription_Input
        except NotImplemented: test_constants.add_NotImplemented()

        try:
            test = self.jd.Input
        except NotImplemented: test_constants.add_NotImplemented()
        self.failUnlessEqual(test, test_constants.jobdescription_Input)
        
        try:
            del self.jd.Input
            self.assertRaises(AttributeError, self.jd.Input)
        except NotImplemented: test_constants.add_NotImplemented()            
        test_constants.add_method_tested(3)
     

    def test_Output_attribute(self):    
        try:
            self.jd.Output = test_constants.jobdescription_Output
        except NotImplemented: test_constants.add_NotImplemented()

        try:
            test = self.jd.Output
        except NotImplemented: test_constants.add_NotImplemented()
        self.failUnlessEqual(test, test_constants.jobdescription_Output)
        
        try:
            del self.jd.Output
            self.assertRaises(AttributeError, self.jd.Output)
        except NotImplemented: test_constants.add_NotImplemented()            
        test_constants.add_method_tested(3)
          

    def test_Error_attribute(self):    
        try:
            self.jd.Error = test_constants.jobdescription_Error
        except NotImplemented: test_constants.add_NotImplemented()

        try:
            test = self.jd.Error
        except NotImplemented: test_constants.add_NotImplemented()
        self.failUnlessEqual(test, test_constants.jobdescription_Error)
        
        try:
            del self.jd.Error
            self.assertRaises(AttributeError, self.jd.Error)
        except NotImplemented: test_constants.add_NotImplemented()            
        test_constants.add_method_tested(3)


    def test_FileTransfer_attribute(self):    
        try:
            self.jd.FileTransfer = test_constants.jobdescription_FileTransfer
        except NotImplemented: test_constants.add_NotImplemented()

        try:
            test = self.jd.FileTransfer
        except NotImplemented: test_constants.add_NotImplemented()
        self.failUnlessEqual(test, test_constants.jobdescription_FileTransfer)
        
        try:
            del self.jd.FileTransfer
            self.assertRaises(AttributeError, self.jd.FileTransfer)
        except NotImplemented: test_constants.add_NotImplemented()            
        test_constants.add_method_tested(3)
 
    
    def test_Cleanup_attribute(self):    
        try:
            self.jd.Cleanup = test_constants.jobdescription_Cleanup
        except NotImplemented: test_constants.add_NotImplemented()

        try:
            test = self.jd.Cleanup
        except NotImplemented: test_constants.add_NotImplemented()
        self.failUnlessEqual(test, test_constants.jobdescription_Cleanup)
        
        try:
            del self.jd.Cleanup
            self.assertRaises(AttributeError, self.jd.Cleanup)
        except NotImplemented: test_constants.add_NotImplemented()            
        test_constants.add_method_tested(3)

           

    def test_JobStartTime_attribute(self):    
        try:
            self.jd.JobStartTime = test_constants.jobdescription_JobStartTime
        except NotImplemented: test_constants.add_NotImplemented()

        try:
            test = self.jd.JobStartTime
        except NotImplemented: test_constants.add_NotImplemented()
        self.failUnlessEqual(test, test_constants.jobdescription_JobStartTime)
        
        try:
            del self.jd.JobStartTime
            self.assertRaises(AttributeError, self.jd.JobStartTime)
        except NotImplemented: test_constants.add_NotImplemented()            
        test_constants.add_method_tested(3)


    def test_TotalCPUTime_attribute(self):    
        try:
            self.jd.TotalCPUTime = test_constants.jobdescription_TotalCPUTime
        except NotImplemented: test_constants.add_NotImplemented()

        try:
            test = self.jd.TotalCPUTime
        except NotImplemented: test_constants.add_NotImplemented()
        self.failUnlessEqual(test, test_constants.jobdescription_TotalCPUTime)
        
        try:
            del self.jd.TotalCPUTime
            self.assertRaises(AttributeError, self.jd.TotalCPUTime)
        except NotImplemented: test_constants.add_NotImplemented()            
        test_constants.add_method_tested(3)

     
    def test_TotalPhysicalMemory_attribute(self):
        try:
            self.jd.TotalPhysicalMemory = \
                            test_constants.jobdescription_TotalPhysicalMemory
        except NotImplemented: test_constants.add_NotImplemented()

        try:
            test = self.jd.TotalPhysicalMemory
        except NotImplemented: test_constants.add_NotImplemented()
        self.failUnlessEqual(test, 
                             test_constants.jobdescription_TotalPhysicalMemory)
        
        try:
            del self.jd.TotalPhysicalMemory
            self.assertRaises(AttributeError, self.jd.TotalPhysicalMemory)
        except NotImplemented: test_constants.add_NotImplemented()            
        test_constants.add_method_tested(3)
    
    
    def test_CPUArchitecture_attribute(self):    
        try:
            self.jd.CPUArchitecture = \
                                test_constants.jobdescription_CPUArchitecture
        except NotImplemented: test_constants.add_NotImplemented()

        try:
            test = self.jd.CPUArchitecture
        except NotImplemented: test_constants.add_NotImplemented()
        self.failUnlessEqual(test, 
                             test_constants.jobdescription_CPUArchitecture)
        
        try:
            del self.jd.CPUArchitecture
            self.assertRaises(AttributeError, self.jd.CPUArchitecture)
        except NotImplemented: test_constants.add_NotImplemented()            
        test_constants.add_method_tested(3)                      
     

    def test_OperatingSystemType_attribute(self):
        try:
            self.jd.OperatingSystemType = \
                            test_constants.jobdescription_OperatingSystemType
        except NotImplemented: test_constants.add_NotImplemented()

        try:
            test = self.jd.OperatingSystemType
        except NotImplemented: test_constants.add_NotImplemented()
        self.failUnlessEqual(test, 
                             test_constants.jobdescription_OperatingSystemType)
        
        try:
            del self.jd.OperatingSystemType
            self.assertRaises(AttributeError, self.jd.OperatingSystemType)
        except NotImplemented: test_constants.add_NotImplemented()            
        test_constants.add_method_tested(3)
             

    def test_CandidateHosts_attribute(self):
        try:
            self.jd.CandidateHosts = test_constants.jobdescription_CandidateHosts
        except NotImplemented: test_constants.add_NotImplemented()

        try:
            test = self.jd.CandidateHosts
        except NotImplemented: test_constants.add_NotImplemented()
        self.failUnlessEqual(test, test_constants.jobdescription_CandidateHosts)
        
        try:
            del self.jd.CandidateHosts
            self.assertRaises(AttributeError, self.jd.CandidateHosts)
        except NotImplemented: test_constants.add_NotImplemented()            
        test_constants.add_method_tested(3)
        

    def test_Queue_attribute(self):    
        try:
            self.jd.Queue = test_constants.jobdescription_Queue
        except NotImplemented: test_constants.add_NotImplemented()

        try:
            test = self.jd.Queue
        except NotImplemented: test_constants.add_NotImplemented()
        self.failUnlessEqual(test, test_constants.jobdescription_Queue)
        
        try:
            del self.jd.Queue
            self.assertRaises(AttributeError, self.jd.Queue)
        except NotImplemented: test_constants.add_NotImplemented()            
        test_constants.add_method_tested(3)


    def test_JobContact_attribute(self):

        try:
            self.jd.JobContact = test_constants.jobdescription_JobContact
        except NotImplemented: test_constants.add_NotImplemented()

        try:
            test = self.jd.JobContact
        except NotImplemented: test_constants.add_NotImplemented()
        self.failUnlessEqual(test, test_constants.jobdescription_JobContact)
        
        try:
            del self.jd.JobContact
            self.assertRaises(AttributeError, self.jd.JobContact)
        except NotImplemented: test_constants.add_NotImplemented()            
        test_constants.add_method_tested(3)
    

    def test_Object_Methods(self):
        o = self.jd

        try:
            test = o.get_id() 
            self.failUnless( (type(test) == str)) 
        except NotImplemented: test_constants.add_NotImplemented()
        
        try:
            test = o.get_type()
            self.failUnless( (type(test) == int) )
            self.failUnless( test == ObjectType.JOBDESCRIPTION )
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


    def test_nr_NotImplemented(self):
        if test_constants.result_NotImplemented() != 0\
        and test_constants.print_not_implemented == True:
            base = float(test_constants.result_methods_tested())/100
            percentage = 100-float(test_constants.result_NotImplemented())//base

            print "\nNumber of methods not implemented in JobDescription:",\
                test_constants.result_NotImplemented(), "of",\
                test_constants.result_methods_tested(), "methods",\
                "("+str(percentage)+"% implemented)",




if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestJobDescription)
    result = unittest.TextTestRunner().run(suite)


    
    