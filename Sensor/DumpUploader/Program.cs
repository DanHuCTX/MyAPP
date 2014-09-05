using System;
using System.Configuration;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Text;
using Microsoft.Win32;
using Ionic.Zip;
using System.Threading;
using DumpUploader.ServiceReferences;
using DumpUploader.AzureChannel;

namespace DumpUploader
{
    enum SupportedClrVersion
    {
        UNKNOWN = 0,
        CLR_2 = 2,
        CLR_4 = 4,    
    }

    enum SupportedProcessorType
    {
        UNKNOWN = 0,
        X86 = 32,
        X64 = 64,
    }

    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length < 2)
            {
                Console.WriteLine("Usage: DumpUploader.exe /Collect AppFullPath");
                Console.WriteLine("    or DumpUploader.exe /Upload FileFullPath");
                return;
            }

            if (args[0].Equals("/Collect",StringComparison.InvariantCultureIgnoreCase))
            {
                string exePath = args[1];

                string dumpFolderPath = GetWERDumpFolder(exePath);
                if (string.IsNullOrEmpty(dumpFolderPath))
                {
                    Debug.WriteLine("Failed to get wer dump setting.");
                    return;
                }

                DirectoryInfo dumpFolder = new DirectoryInfo(dumpFolderPath);
                FileInfo[] fileInfos = dumpFolder.GetFiles("*.dmp");
                FileInfo fi = null;
                foreach (FileInfo f in fileInfos)
                {
                    if (fi == null || f.CreationTime > fi.CreationTime)
                    {
                        fi = f;
                    }
                }

                if (fi != null && (DateTime.Now - fi.CreationTime).TotalSeconds < 30000000)
                {

                    string zipName = Path.Combine(Path.GetDirectoryName(fi.FullName), Path.GetFileNameWithoutExtension(fi.FullName) + ".zip");

                    SupportedClrVersion clr;
                    SupportedProcessorType cpu;
                    bool isManaged = ManagedPECheck(exePath, out clr, out cpu);


                    using (var zip = new ZipFile(zipName, Encoding.UTF8))
                    {
                        zip.ParallelDeflateThreshold = -1;
                        zip.AddFile(fi.FullName, "");

                        if (isManaged)
                        {
                            string dotNetPath = GetDotNetFrameworkPath(clr, cpu);
                            if (!string.IsNullOrEmpty(dotNetPath))
                            {
                                string[] extensions = { "sos.dll", "mscordacwks.dll" };
                                foreach (string s in extensions)
                                {
                                    zip.AddFile(Path.Combine(dotNetPath, s), "");
                                }
                            }

                        }

                        zip.Save();
                    }

                    UploadFile(zipName);
                    File.Delete(zipName);
                }
                
            }
            else if (args[0].Equals("/Upload", StringComparison.InvariantCultureIgnoreCase))
            {
                UploadFile(args[1]);           
            }
        }

        static bool ManagedPECheck(string fileName, out SupportedClrVersion clrVersion, out SupportedProcessorType cpu)
        {
            clrVersion = SupportedClrVersion.UNKNOWN;
            cpu = SupportedProcessorType.UNKNOWN;
            Assembly assembly = null;
            try
            {
                assembly = Assembly.ReflectionOnlyLoadFrom(fileName);
            }
            catch (Exception)
            {
                return false;
            }

            PortableExecutableKinds peKind;
            ImageFileMachine imageFileMachine;
            assembly.ManifestModule.GetPEKind(out peKind, out imageFileMachine);

            if (peKind == PortableExecutableKinds.Unmanaged32Bit) // not a pe file
            {
                if (imageFileMachine == ImageFileMachine.AMD64)
                {
                    cpu = SupportedProcessorType.X64;

                }
                else if (imageFileMachine == ImageFileMachine.I386)
                {
                    cpu = SupportedProcessorType.X86;

                }

                return false;
            }

            string runtimeVersion = assembly.ImageRuntimeVersion;
            if (assembly.ImageRuntimeVersion.StartsWith("v4.0", StringComparison.InvariantCultureIgnoreCase))
            {
                clrVersion = SupportedClrVersion.CLR_4;
            }
            else if (assembly.ImageRuntimeVersion.StartsWith("v2.0", StringComparison.InvariantCultureIgnoreCase))
            {
                clrVersion = SupportedClrVersion.CLR_2;
            }

            if (peKind == PortableExecutableKinds.ILOnly ||
                peKind == PortableExecutableKinds.PE32Plus)
            {
                cpu = SupportedProcessorType.X64;
            }
            else if (peKind == PortableExecutableKinds.Required32Bit)
            {
                cpu = SupportedProcessorType.X86;
            }
            return true;
        }

        static string GetDotNetFrameworkPath(SupportedClrVersion clrVersion, SupportedProcessorType cpu)
        {
            string winDir = Environment.GetEnvironmentVariable("SystemRoot");

            string dotNetPath = Path.Combine(winDir, "Microsoft.NET", cpu == SupportedProcessorType.X64 ? "Framework64" : "Framework");

            string pattern = clrVersion == SupportedClrVersion.CLR_4 ? "v4.0*" : "v2.0*";
            string[] clrDir = Directory.GetDirectories(dotNetPath, pattern, SearchOption.TopDirectoryOnly);

            return clrDir != null && clrDir.Length > 0 ? clrDir[0] : null;
        }

        static string GetWERDumpFolder(string exeFullPath)
        {
            string exeName = Path.GetFileName(exeFullPath);
            string REG_WER_DUMP_PATH = "SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting\\LocalDumps\\" + exeName;
            RegistryKey dumpCfgKey = Registry.LocalMachine.OpenSubKey(REG_WER_DUMP_PATH);
            if (dumpCfgKey != null)
            {
                string dumpFolderPath = dumpCfgKey.GetValue("DumpFolder") as string;
                dumpCfgKey.Close();
                return dumpFolderPath;
            }

            return null;
        }

        private static void UploadFile(string fileName)
        {
            if (ConfigurationManager.AppSettings["UploadToCloud"].Equals("true", StringComparison.InvariantCultureIgnoreCase))
            {
                using (ChannelServerClient proxy = new ChannelServerClient())
                {

                    DumpUploader.AzureChannel.UploadEvent e = new DumpUploader.AzureChannel.UploadEvent();
                    e.Type = 2;
                    e.FileName = Path.GetFileName(fileName);

                    using (FileStream fs = new FileStream(fileName, FileMode.Open, FileAccess.Read, FileShare.Read))
                    {
                        byte[] buffer = new byte[(int)fs.Length];
                        fs.Read(buffer, 0, (int)fs.Length);
                        MemoryStream ms = new MemoryStream(buffer);
                        e.FileData = ms;
                        proxy.Upload(e);
                        e.FileData.Close();
                    }
                }
            }
            else
            {
                using (PrivateChannelServerClient proxy = new PrivateChannelServerClient())
                {
                    DumpUploader.ServiceReferences.UploadEvent e = new DumpUploader.ServiceReferences.UploadEvent();

                    e.Type = 2;
                    e.FileName = Path.GetFileName(fileName);
                    e.FileData = new FileStream(fileName, FileMode.Open, FileAccess.Read, FileShare.Read);
                    DateTime t1 = System.DateTime.Now;
                    proxy.Upload(e);
                    DateTime t2 = System.DateTime.Now;
                    TimeSpan tt = t2 - t1;
                    Console.WriteLine(tt.TotalSeconds);
                    e.FileData.Close();
                }
            }
        }
    }
}
