
#For 15_JobDescription
jobdescription_Executable = "/bin/sleep"
jobdescription_Arguments = ["--help", "--version"]
jobdescription_SPMDVariation = "None"   #None or ''
jobdescription_TotalCPUCount = "1"
jobdescription_NumberOfProcesses = '1'
jobdescription_ProcessesPerHost = '1'
jobdescription_ThreadsPerProcess = "1"
jobdescription_Environment = ["a=1", "b=2"]
jobdescription_WorkingDirectory = "/tmp/"
jobdescription_Interactive = "False"
jobdescription_Input = "/tmp/INPUT_FILE"
jobdescription_Output = "/tmp/OUTPUT_FILE"
jobdescription_Error = "/tmp/ERROR_FILE"
jobdescription_FileTransfer = ["a", "b"]
jobdescription_Cleanup = "True"
jobdescription_JobStartTime = '1'
jobdescription_TotalCPUTime = "60"
jobdescription_TotalPhysicalMemory = "10"
jobdescription_CPUArchitecture = ["x86","UltraSPARC"]
jobdescription_OperatingSystemType = ["Linux", "SunOS"]
jobdescription_CandidateHosts = ["galjas.few.vu.nl", "kits.few.vu.nl"]
jobdescription_Queue = "sge"
jobdescription_JobContact = ["fax:+123456789","mailto:joe@doe.net" ]

#For 16_JobService
jobservice_Executable = "/bin/sleep"
jobservice_Arguments = ["--help"]
jobservice_Cleanup = "False"
jobservice_commandline = "/bin/sleep"
jobservice_host = "localhost"
jobservice_Arguments_2 = ["5"]

#For 17_Job
job_hostname = "localhost"
job_Executable = "/bin/sleep"
job_Arguments = ["5"]
job_Interactive = "True"
class Result(object):
    nr_NotImplemented = 0
    nr_methods_tested = 0
    
print_not_implemented = True
def add_NotImplemented():
    Result.nr_NotImplemented += 1

def add_method_tested(value=1):
    Result.nr_methods_tested += value

def result_NotImplemented():
    return Result.nr_NotImplemented

def result_methods_tested():
    return Result.nr_methods_tested



