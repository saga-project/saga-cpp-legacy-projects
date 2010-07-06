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

        CloudBlobClient blobClient;
        CloudQueueClient queueClient;
                

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
                IAsyncResult result = queue.BeginGetMessage(null, null);
                CloudQueueMessage jobId = queue.EndGetMessage(result);
                if (jobId == null)
                {
                    Thread.Sleep(1000);
                    continue;
                }
                if (jobId.AsString == "STOP")
                {
                    break; //exit while loop and worker role
                }
                else
                {
                    ExecuteSubJob(jobId.AsString);
                }
            } //end while loop
        }

        public void ExecuteSubJob(String jobId)
        {
            //get reference to blob client
            blobClient = new CloudBlobClient(storageAccount.BlobEndpoint, storageAccount.Credentials);
            CloudBlobContainer blobContainer = blobClient.GetContainerReference(applicationName);
            CloudBlob jobBlob = blobContainer.GetBlobReference(jobId);

            String jobString = jobBlob.DownloadText();
            Dictionary<string, object> jobDict = JsonConvert.DeserializeObject<Dictionary<string, object>>(jobString);

            string state = (string)jobDict[STATE];
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
                    JArray argumentArray = (JArray) jobDict["Arguments"];
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

                Trace.WriteLine("Starting .exe in directory " + Environment.CurrentDirectory, "Information");
                Process applicationProcess = Process.Start(info);
                Trace.WriteLine("Started .exe on host " + applicationProcess.MachineName, "Information");

                applicationProcess.WaitForExit();
                swComputeTime.Stop();
                Trace.WriteLine("Runtime: " + swComputeTime.ElapsedMilliseconds + " ms");

                #region get output and store in subjob blbo
                try
                {
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
                }
                #endregion
            }
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
