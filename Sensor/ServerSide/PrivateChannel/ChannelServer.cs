using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Runtime.Serialization;
using System.ServiceModel;
using System.Text;
using PrivateChannel.Interface;

namespace PrivateChannel
{
    enum UploadType
    {
        DumpZip = 2,
    }

    public class ChannelServer : IChannelServer
    {
        public string Echo(string msg)
        {
            return "Hello " + msg;
        }


        public void Upload(UploadEvent e)
        {
            string tempFolder = Path.Combine(Path.GetTempPath(), string.Format("Trap-{0:yyyy-MM-dd-HHmmss}", DateTime.Now));
            if (!Directory.Exists(tempFolder))
            {
                Directory.CreateDirectory(tempFolder);
            }

            Debug.WriteLine("Received file = {0}, Type = {1}", e.FileName, e.Type);

            if (!string.IsNullOrEmpty(e.FileName) && e.FileData != Stream.Null)
            {
                string fileSavingPath = Path.Combine(tempFolder, e.FileName);
                using (FileStream fs = new FileStream(fileSavingPath, FileMode.Create, FileAccess.Write, FileShare.None))
                {
                    const int BUF_LEN = 4096;
                    byte[] buffer = new byte[BUF_LEN];
                    int count = 0;
                    while ((count = e.FileData.Read(buffer, 0, BUF_LEN)) > 0)
                    {
                        fs.Write(buffer,0,count);
                    }

                    fs.Close();
                }

                if (e.Type == (int)UploadType.DumpZip)
                {
                   
                    string assemblyFullName = Assembly.GetExecutingAssembly().Location;
                    string analyzer = Path.Combine(Path.GetDirectoryName(assemblyFullName), "Analyzer.exe");
                    Debug.WriteLine("Get crash dump, launching {0} {1} for analyzing...", analyzer, fileSavingPath);
                    ProcessStartInfo startInfo = new ProcessStartInfo();
                    startInfo.FileName = analyzer;
                    startInfo.Arguments = fileSavingPath;
                    startInfo.UseShellExecute = false;
                    startInfo.CreateNoWindow = true;
                    startInfo.RedirectStandardOutput = false;

                    Process pr = Process.Start(startInfo);
                }
            }
        }
    }
}
