using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Net;
using System.Threading;
using Microsoft.WindowsAzure;
using Microsoft.WindowsAzure.Diagnostics;
using Microsoft.WindowsAzure.ServiceRuntime;
using Microsoft.WindowsAzure.StorageClient;
using System.IO;
using System.Globalization;
using System.Runtime.Serialization.Json;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace BigjobAzureAgent
{

    public class WorkerRole : RoleEntryPoint
    {
        CloudStorageAccount storageAccount;
        String applicationName;
        String STATE = "state";

        CloudBlobClient blobClient = null;
        CloudQueueClient queueClient = null;
        CloudBlobContainer blobContainer = null;


        public override void Run()
        {
            // This is a sample worker implementation. Replace with your logic.
            Trace.WriteLine("BigjobAzureAgent entry point called", "Information");

            //GET reference to Queue Storage
            queueClient = new CloudQueueClient(storageAccount.QueueEndpoint, storageAccount.Credentials);

            CloudQueue queue = queueClient.GetQueueReference(applicationName);
            queue.CreateIfNotExist();
            while (true)
            {
                //IAsyncResult result = queue.BeginGetMessage(null, null);
                //CloudQueueMessage jobId = queue.EndGetMessage(result);
                CloudQueueMessage queueMessage = queue.GetMessage();

                if (queueMessage == null)
                {
                    Thread.Sleep(1000);
                    continue;
                }
                else
                {
                    Trace.WriteLine("BigjobAzureAgent got queue message: " + queueMessage.AsString, "Information");
                }
                if (queueMessage.AsString == "STOP")
                {
                    //put STOP message back in queue
                    queue.AddMessage(queueMessage);
                    break; //exit while loop and worker role
                }
                else
                {
                    string jobId = queueMessage.AsString; //queue message contains id of subjob to execute
                    if (ExecuteSubJob(jobId)) 
                    {
                        //success - delete message
                        updateState(jobId, "Done");
                        queue.DeleteMessage(queueMessage);
                    }
                    else
                    {
                        //failure
                        updateState(jobId, "New");
                        queue.AddMessage(queueMessage); //put message back in queue 
                    }
                    
                }
            } //end while loop
        }

        public void updateState(string jobId, string newState)
        {
            Dictionary<string, object> jobDict = getJobDictFromBlob(jobId);
            jobDict[STATE] = newState;
            uploadJobDictToBlob(jobId, jobDict);
        }

        public bool ExecuteSubJob(String jobId)
        {
            //get reference to blob client
            Dictionary<string, object> jobDict = getJobDictFromBlob(jobId);

            string state = (string) jobDict[STATE];
            if (state == "New" || state == "Unknown")
            {
                jobDict[STATE] = "New";
                Trace.WriteLine(jobDict, "Information");
                String numberOfProcesses = "1";
                if (jobDict.ContainsKey("NumberOfProcesses"))
                {
                    numberOfProcesses = (string)jobDict["NumberOfProcesses"];
                }

                String spmdvariation = "single";
                if (jobDict.ContainsKey("SPMDVariation"))
                {
                    spmdvariation = (string)jobDict["SPMDVariation"];
                }
                String arguments = "";
                if (jobDict.ContainsKey("Arguments"))
                {
                    JArray argumentArray = (JArray)jobDict["Arguments"];
                    foreach (JValue a in argumentArray)
                    {
                        arguments = arguments + " " + a.ToString();
                    }
                }
                String executable = (string)jobDict["Executable"];

                String workingdirectory = Environment.GetEnvironmentVariable("RoleRoot");
                if (jobDict.ContainsKey("WorkingDirectory"))
                {
                    workingdirectory = (string)jobDict["WorkingDirectory"];
                }
                String output = "stdout";
                if (jobDict.ContainsKey("Output"))
                {
                    output = (string)jobDict["Output"];
                }

                String error = "stderr";
                if (jobDict.ContainsKey("Error"))
                {
                    error = (string)jobDict["Error"];
                }

                try
                {
                    #region execute subjob
                    //String command = executable + " " + arguments;
                    string localPath = Path.Combine(Environment.GetEnvironmentVariable("RoleRoot")); // + @"\", @"approot\resources\namd\");
                    //string exeFileName = "namd2.exe";
                    //ProcessStartInfo info = new ProcessStartInfo(localPath + exeFileName, "+p8 NPT.conf");

                    Trace.WriteLine("Executable: " + localPath + @"\" + executable + " Arguments: " + arguments
                        + " WorkingDirectory: " + Path.Combine(localPath + @"\", workingdirectory), "Information");
                    ProcessStartInfo info = new ProcessStartInfo(localPath + @"\" + executable, arguments);
                    info.UseShellExecute = false;
                    info.ErrorDialog = false;
                    info.WorkingDirectory = Path.Combine(localPath + @"\", workingdirectory);                    
                    info.CreateNoWindow = true;
                    info.RedirectStandardOutput = true;
                    info.RedirectStandardError = true;
                    Stopwatch swComputeTime = Stopwatch.StartNew();
                    updateState(jobId, "Running");
                    Trace.WriteLine("Starting .exe in directory " + Path.Combine(localPath + @"\", workingdirectory), "Information");
                    Process applicationProcess = Process.Start(info);
                    Trace.WriteLine("Started .exe on host " + applicationProcess.MachineName, "Information");

                    #region Grap Output
                    StreamReader reader = applicationProcess.StandardOutput;
                    string CompleteOutput = string.Empty;
                    string line = string.Empty;
                    line = reader.ReadLine();
                    Trace.WriteLine(line, "Information");
                    while (line != null)
                    {
                        line = reader.ReadLine();
                        CompleteOutput += line != null ? line : string.Empty;
                        CompleteOutput += "\n";
                        Trace.WriteLine(line != null ? line : "<EOL>", "Information");
                    }
                    #endregion



                    applicationProcess.WaitForExit();
                    swComputeTime.Stop();
                    Trace.WriteLine("Runtime: " + swComputeTime.ElapsedMilliseconds + " msec");
                    #endregion

                    #region get output and store in subjob blob

                    string outputString = applicationProcess.StandardOutput.ReadToEnd();
                    string errorString = applicationProcess.StandardError.ReadToEnd();

                    DateTime date = DateTime.Now;
                    string dateString = date.ToString("yyyyMMdd_HHmm", CultureInfo.InvariantCulture);
                    CloudBlob stdoutBlob = blobContainer.GetBlobReference(output + "-" + dateString + ".txt");
                    stdoutBlob.UploadText("VMSize: " + "n/a" + "\n" + "Runtime: "
                        + swComputeTime.ElapsedMilliseconds + " ms\n\n"
                        + "******************************************************************************************"
                        + "\nOutput:\n" + outputString);
                    CloudBlob stderrBlob = blobContainer.GetBlobReference(error + "-" + dateString + ".txt");
                    stdoutBlob.UploadText(errorString);                    
                    #endregion
                }
                catch (Exception ex)
                {
                    Console.WriteLine("\nMessage ---\n{0}", ex.Message);
                    Console.WriteLine(
                        "\nHelpLink ---\n{0}", ex.HelpLink);
                    Console.WriteLine("\nSource ---\n{0}", ex.Source);
                    Console.WriteLine(
                        "\nStackTrace ---\n{0}", ex.StackTrace);
                    Console.WriteLine(
                        "\nTargetSite ---\n{0}", ex.TargetSite);
                    Console.WriteLine(
                      "\nInner Exception ---\n{0}", ex.InnerException.StackTrace);
                    return false;
                }
                return true;
            }
            return false;
        }

        private Dictionary<string, object> getJobDictFromBlob(String jobId)
        {
            CloudBlob jobBlob = getCloudBlobContainer().GetBlobReference(jobId);
            String jobString = jobBlob.DownloadText();
            Dictionary<string, object> jobDict = JsonConvert.DeserializeObject<Dictionary<string, object>>(jobString);
            return jobDict;
        }

        private void uploadJobDictToBlob(String jobId, Dictionary<string,object> jobDict)
        {
            CloudBlob jobBlob = getCloudBlobContainer().GetBlobReference(jobId);
            String jobString = JsonConvert.SerializeObject(jobDict);            
            jobBlob.UploadText(jobString);            
        }
        
        private CloudBlobClient getCloudBlobClient()
        {
            if (blobClient == null)
            {
                blobClient = new CloudBlobClient(storageAccount.BlobEndpoint, storageAccount.Credentials);
            }
            return blobClient;
        }

        private CloudBlobContainer getCloudBlobContainer()
        {
            if (blobContainer == null)
            {
                blobContainer = getCloudBlobClient().GetContainerReference(applicationName); 
            }
            return blobContainer;
        
        }

        public override bool OnStart()
        {

            // Set the maximum number of concurrent connections 
            ServicePointManager.DefaultConnectionLimit = 12;

            DiagnosticMonitor.Start("DiagnosticsConnectionString");

            // Config Change Handling
            CloudStorageAccount.SetConfigurationSettingPublisher((configName, configSetter) =>
            {
                configSetter(RoleEnvironment.GetConfigurationSettingValue(configName));

                RoleEnvironment.Changed += (sender, arg) =>
                {
                    if (arg.Changes.OfType<RoleEnvironmentConfigurationSettingChange>()
                        .Any((change) => (change.ConfigurationSettingName == configName)))
                    {
                        if (!configSetter(RoleEnvironment.GetConfigurationSettingValue(configName)))
                        {
                            RoleEnvironment.RequestRecycle();
                        }
                    }
                };
            });

            // create storage account
            storageAccount = CloudStorageAccount.FromConfigurationSetting("DataConnectionString");
            applicationName = RoleEnvironment.GetConfigurationSettingValue("ApplicationName");


            // For information on handling configuration changes
            // see the MSDN topic at http://go.microsoft.com/fwlink/?LinkId=166357.
            RoleEnvironment.Changing += RoleEnvironmentChanging;

            return base.OnStart();
        }

        private void RoleEnvironmentChanging(object sender, RoleEnvironmentChangingEventArgs e)
        {
            // If a configuration setting is changing
            if (e.Changes.Any(change => change is RoleEnvironmentConfigurationSettingChange))
            {
                // Set e.Cancel to true to restart this role instance
                e.Cancel = true;
            }
        }
    }
}
