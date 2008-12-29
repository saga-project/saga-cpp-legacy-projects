
#For 13_LogicalFile
logicalfile_plainname = "/tmp/Logicalfile.part1"
logicalfile_name1 = "file:///tmp/Logicalfile.part1"
logicalfile_name2 = "file:///tmp/Logicalfile.part2"
logicalfile_name3 = "file:///tmp/Logicalfile.part3"
logicalfile_name4 = "file:///tmp/Logicalfile.part4"

logicalDirectory_plainname = "/tmp/LogicalDir1"
logicalDirectory_name = "file:///tmp/LogicalDir1"
logicalDirectory_dirname = "file:///tmp"


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
job_Arguments_alternative = ["--version"]
job_WorkingDirectory = "/tmp"

#For 19_StreamService
streamservice_url = "tcp://localhost"
streamservice_port = 6000

#For 20_Stream
stream_data = "abcdefghijklmnopqrstuvwxyz"

#For 22_RPC
rpc_scheme = "any" #"xmlrpc" #"http" #"gridrpc"
rpc_port = 8080
rpc_host = "127.0.0.1"
rpc_path = "/Calculator.add"

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



