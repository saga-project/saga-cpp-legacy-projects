using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading;
using Microsoft.WindowsAzure.Diagnostics;
using Microsoft.WindowsAzure.ServiceRuntime;
using System.IO;
using Microsoft.WindowsAzure;
using Microsoft.WindowsAzure.StorageClient;
using System.Globalization;

namespace NAMDWorker
{
    public class WorkerRole : RoleEntryPoint
    {
        public override void Run()
        {
            // This is a sample worker implementation. Replace with your logic.
            Trace.WriteLine("NAMDWorker entry point called", "Information");

            string localPath = Path.Combine(Environment.GetEnvironmentVariable("RoleRoot") + @"\", @"approot\res\");
            string exeFileName = "namd2.exe";
            ProcessStartInfo info = new ProcessStartInfo(localPath + exeFileName, "+p8 NPT.conf");
            info.UseShellExecute = false;
            info.ErrorDialog = false;
            info.WorkingDirectory = localPath;
            info.CreateNoWindow = true;
            info.RedirectStandardOutput = true;

            Stopwatch swComputeTime = Stopwatch.StartNew();
          
            Trace.WriteLine("Starting .exe in directory " + Environment.CurrentDirectory, "Information");
            Process runningAlgorithm = Process.Start(info);
            Trace.WriteLine("Started .exe on host " + runningAlgorithm.MachineName, "Information");

            #region Grap Output
            StreamReader reader = runningAlgorithm.StandardOutput;
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

            runningAlgorithm.WaitForExit();
            swComputeTime.Stop();
            Trace.WriteLine("Runtime: " + swComputeTime.ElapsedMilliseconds + " ms");

            var AaK = new StorageCredentialsAccountAndKey("drelu", "f7Fzqm60UVdRtz0x7Db/WABB2xuW460oOQSlyQdVH4pnsTwrA//TC2hfJy3o870ERntccgcCDxBijCY1uI9nGg==");
            var storageAccount2 = new CloudStorageAccount(AaK, false);
            //var storageAccount2 = CloudStorageAccount.FromConfigurationSetting("MyStorage");

            try
            {
                CloudBlobClient blobClient = new CloudBlobClient(storageAccount2.BlobEndpoint, storageAccount2.Credentials);
                CloudBlobContainer blobContainer = blobClient.GetContainerReference("namd");
            
                blobContainer.CreateIfNotExist();
                
                DateTime date = DateTime.Now;
                string dateString = date.ToString("yyyyMMdd_HHmm", CultureInfo.InvariantCulture);
                CloudBlob blob = blobContainer.GetBlobReference("namd-result-"+dateString + ".txt");
                //string vmsize = RoleEnvironment.GetConfigurationSettingValue("vmsize");
                blob.UploadText("VMSize: " + "n/a" + "\n" + "Runtime: " + swComputeTime.ElapsedMilliseconds + " ms\n\nOutput:\n" + CompleteOutput);
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
