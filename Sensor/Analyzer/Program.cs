using System;
using System.Collections.Generic;
using System.Configuration;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net.Mail;
using System.Text;
using Ionic.Zip;

namespace Analyzer
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length < 1)
            {
                return;
            }

            string cdb = ConfigurationManager.AppSettings["CDB"];
            if (string.IsNullOrEmpty(cdb))
            {
                return;
            }

            string zipPath = args[0];
            string dmpPath = Path.Combine(Path.GetTempPath(), Path.GetFileNameWithoutExtension(zipPath));

            Debug.WriteLine("Extract {0} to {1}...", zipPath, dmpPath);
            using (ZipFile zipFile = new ZipFile(zipPath,Encoding.UTF8))
            {
                foreach (ZipEntry e in zipFile)
                {
                    e.Extract(dmpPath);
                }
            }

            DirectoryInfo dir = new DirectoryInfo(dmpPath);
            FileInfo[] dmpFiles = dir.GetFiles("*.dmp");
            if (dmpFiles == null || dmpFiles.Length == 0)
            {
                Debug.WriteLine("No dump file found!");
                return;
            }
            string dmpFile = dmpFiles[0].FullName;

            Debug.WriteLine("Get dump file {0}, managed is {1}", dmpFile,true);

            Analyze(cdb, dmpFile, true);
        }

        static void Analyze(string cdb, string dmpFile, bool IsManaged)
        {
            string dmpPath = Path.GetDirectoryName(dmpFile);
            string outputFile = Path.Combine(dmpPath, "AnalyzeResult.txt");
            if (IsManaged)
            {
                string sosFile = Path.Combine(dmpPath, "SOS.dll");
                string arguments = string.Format("-z {0} -y {1} -logo {2} -c \".load {3};.cordll -ve -se -u -lp {4};!analyze -v;q\"",
                    dmpFile, dmpPath, outputFile, sosFile, dmpPath);

                Debug.WriteLine("Will launch command {0} {1} for analysis.", cdb, arguments);

                ProcessStartInfo startInfo = new ProcessStartInfo();
                startInfo.FileName = cdb;
                startInfo.Arguments = arguments;
                startInfo.UseShellExecute = false;
                startInfo.CreateNoWindow = true;
                startInfo.RedirectStandardOutput = false;

                Process pr = Process.Start(startInfo);
                pr.WaitForExit();
            }


            if (File.Exists(outputFile) && 
                ConfigurationManager.AppSettings["SendMail"].Equals("true", StringComparison.InvariantCultureIgnoreCase))
            {
                SendAnalyzeResult(outputFile);
            }
        }

        static void SendAnalyzeResult(string resultFile)
        {
            Debug.WriteLine("Send {0} contents in mail notification",resultFile);

            SmtpClient client = new SmtpClient(ConfigurationManager.AppSettings["SmtpServer"]);
            MailAddress from = new MailAddress(ConfigurationManager.AppSettings["MailFrom"]);
            MailAddress to = new MailAddress(ConfigurationManager.AppSettings["MailTo"]);
            MailMessage msg = new MailMessage(from, to);
            msg.Subject = ConfigurationManager.AppSettings["MailSubject"];

            using (StreamReader sr = File.OpenText(resultFile))
            {
                msg.Body = sr.ReadToEnd();
            }

            client.Send(msg);
        }
    }
}
