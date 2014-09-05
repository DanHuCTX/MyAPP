using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Mail;
using System.Reflection;
using System.Text;
using System.Threading;
using Microsoft.WindowsAzure;
using Microsoft.WindowsAzure.Diagnostics;
using Microsoft.WindowsAzure.ServiceRuntime;
using Microsoft.WindowsAzure.StorageClient;
using Ionic.Zip;

namespace WCFWorker
{
    public class WorkerRole : RoleEntryPoint
    {
        static void SendMail(string host, int port, bool ssl, string from, string to, string subject, string content, NetworkCredential credential)
        {
            traceSource.TraceEvent(TraceEventType.Information, 0, "Sending analysis email to {0}...", to);

            SmtpClient client = new SmtpClient(host, port)
            {
                EnableSsl = ssl,
            };

            if (credential != null)
            {
                client.Credentials = credential;
            }

            try
            {
                MailMessage msg = new MailMessage(from, to, subject, content);
                client.Send(msg);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }


        public override void Run()
        {
            var account = CloudStorageAccount.Parse(RoleEnvironment.GetConfigurationSettingValue("DataConnectionString"));
            var queue = account.CreateCloudQueueClient().GetQueueReference("package-incoming");
            queue.CreateIfNotExist();
            while (true)
            {
                var msg = queue.GetMessage();
                if (msg != null)
                {
                    traceSource.TraceEvent(TraceEventType.Information, 0, "New upload arrived, blog uri is {0}", msg.AsString);
                    try
                    {
                        var ctx = account.CreateCloudBlobClient();
                        var cloudBlobContainer = ctx.GetContainerReference("package");
                        var blob = cloudBlobContainer.GetBlobReference(msg.AsString);

                        string[] parts = msg.AsString.Split('/');
                        if (parts.Length > 0)
                        {
                            string downloadedFile = Path.GetTempFileName() + "-" + parts.Last();

                            traceSource.TraceEvent(TraceEventType.Information, 0, "Blob file will be downloaded at {0}", downloadedFile);
                            blob.DownloadToFile(downloadedFile);
                            traceSource.TraceEvent(TraceEventType.Information, 0, "Blob file was downloaded successfully.");


                            string dumpFolder = Path.Combine(Path.GetTempPath(), parts.Last());
                            if (!Directory.Exists(dumpFolder))
                            {
                                Directory.CreateDirectory(dumpFolder);
                            }

                            traceSource.TraceEvent(TraceEventType.Information, 0, "Unzipping to {0}", dumpFolder);
                            using (ZipFile zipFile = new ZipFile(downloadedFile, Encoding.UTF8))
                            {
                                foreach (ZipEntry e in zipFile)
                                {
                                    e.Extract(dumpFolder);
                                }
                            }

                            traceSource.TraceEvent(TraceEventType.Information, 0, "Unzip successfully.");

                            FileInfo[] dmpFiles = new DirectoryInfo(dumpFolder).GetFiles("*.dmp");
                            if (dmpFiles == null || dmpFiles.Length == 0)
                            {
                                traceSource.TraceEvent(TraceEventType.Information, 0, "No dump file found!");
                            }
                            else
                            {
                                string cdb = Path.Combine(AssemblyDirectory, "Debugger", "CDB.exe");
                                traceSource.TraceEvent(TraceEventType.Information, 0, "CDB tool path is {0}", cdb);
                                if (!File.Exists(cdb))
                                {
                                    traceSource.TraceEvent(TraceEventType.Information, 0, "{0} does not exist!", cdb);
                                }
                                else
                                {
                                    string dmpFile = dmpFiles[0].FullName;
                                    string analyzeResultFile = Path.Combine(Path.GetDirectoryName(dmpFile), "AnalyzeResult.txt");
                                    Analyze(cdb, dmpFile, true, analyzeResultFile);
                                    if (File.Exists(analyzeResultFile) && new FileInfo(analyzeResultFile).Length > 0)
                                    {
                                        traceSource.TraceEvent(TraceEventType.Information, 0, "Analysis result generated at {0}", analyzeResultFile);
                                        using (StreamReader sr = File.OpenText(analyzeResultFile))
                                        {
                                            const string from = "dump.analyzer@cis.citrix.com";
                                            const string to = "dan.hu@citrix.com";
                                            const string subject = "You got a crash dump analyze report.";
                                            string content = sr.ReadToEnd();
                                            if (Dns.GetHostName().StartsWith("DAN-DEV-VM2", StringComparison.InvariantCultureIgnoreCase))
                                            {
                                                // using citrix mail server if running inside lab
                                                SendMail("sinpmail.citrite.net",
                                                    25,
                                                    false,
                                                    from,
                                                    to,
                                                    subject,
                                                    content,
                                                    null);
                                            }
                                            else
                                            {
                                                const string live_user = "redhu1111@hotmail.com";
                                                const string live_pass = "ZABhAGkAcwB5ACYAagBvAGwAaQBlACEAQABAACUA";

                                                SendMail("smtp-mail.outlook.com",
                                                    587,
                                                    true,
                                                    live_user,
                                                    to,
                                                    subject,
                                                    content,
                                                    new NetworkCredential(live_user, Encoding.Unicode.GetString(Convert.FromBase64String(live_pass))));
                                                return;
                                            }
                                        }
                                    }
                                }
                            }

                            Directory.Delete(dumpFolder, true);
                        }
                    }
                    catch (Exception e)
                    {
                        traceSource.TraceEvent(TraceEventType.Error, 0, e.Message);
                    }
                    finally
                    {
                        queue.DeleteMessage(msg);
                    }
                }
                else
                {
                    Thread.Sleep(TimeSpan.FromSeconds(1));
                }
            }
        }

        public override bool OnStart()
        {
            // Set the maximum number of concurrent connections 
            string customTempLocalResourcePath = RoleEnvironment.GetLocalResource("CustomTempLocalStore").RootPath;
            Environment.SetEnvironmentVariable("TMP", customTempLocalResourcePath);
            Environment.SetEnvironmentVariable("TEMP", customTempLocalResourcePath);

            ServicePointManager.DefaultConnectionLimit = 12;

            DiagnosticMonitorConfiguration diagnosticConfig = DiagnosticMonitor.GetDefaultInitialConfiguration();
            diagnosticConfig.Directories.ScheduledTransferPeriod = TimeSpan.FromMinutes(1);
            diagnosticConfig.Logs.ScheduledTransferLogLevelFilter = LogLevel.Verbose;

            DiagnosticMonitor.Start("Microsoft.WindowsAzure.Plugins.Diagnostics.ConnectionString", diagnosticConfig);

            // For information on handling configuration changes
            // see the MSDN topic at http://go.microsoft.com/fwlink/?LinkId=166357.

            return base.OnStart();
        }

        static public string AssemblyDirectory
        {
            get
            {
                string codeBase = Assembly.GetExecutingAssembly().CodeBase;
                UriBuilder uri = new UriBuilder(codeBase);
                string path = Uri.UnescapeDataString(uri.Path);
                return Path.GetDirectoryName(path);
            }
        }

        static void Analyze(string cdb, string dmpFile, bool IsManaged, string outputFile)
        {
            string dmpPath = Path.GetDirectoryName(dmpFile);
            if (IsManaged)
            {
                string sosFile = Path.Combine(dmpPath, "SOS.dll");
                string arguments = string.Format("-z {0} -y {1} -logo {2} -c \".load {3};.cordll -ve -se -u -lp {4};!analyze -v;q\"",
                    dmpFile, dmpPath, outputFile, sosFile, dmpPath);

                traceSource.TraceEvent(TraceEventType.Information, 0, "Will launch command {0} {1} for analysis.", cdb, arguments);

                ProcessStartInfo startInfo = new ProcessStartInfo();
                startInfo.FileName = cdb;
                startInfo.Arguments = arguments;
                startInfo.UseShellExecute = false;
                startInfo.CreateNoWindow = true;
                startInfo.RedirectStandardOutput = false;

                Process pr = Process.Start(startInfo);
                pr.WaitForExit();

                traceSource.TraceEvent(TraceEventType.Information, 0, "Analysis finished.");
            }
        }

        private static TraceSource traceSource = new TraceSource("CustomTrace", SourceLevels.All);
    }
}
