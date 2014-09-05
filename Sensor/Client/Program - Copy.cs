using System;
using System.Diagnostics;
using System.Globalization;
using SensorLib;
namespace Client
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length > 0 && args[0].Equals("/Register", StringComparison.InvariantCultureIgnoreCase))
            {
                IProcessSensor sensor = new ProcessSensor();
                if (args.Length > 1)
                {
                    for (int i = 1; i < args.Length; ++i)
                    {
                        int processId = int.Parse(args[i], CultureInfo.InvariantCulture);
                        sensor.RegisterProcess(processId);
                    }
                }
                else
                {
                    sensor.RegisterProcess(Process.GetCurrentProcess().Id);               
                }            
            }

            Console.WriteLine("Current process Id = {0}, Press any key to crash", Process.GetCurrentProcess().Id);
            Console.ReadKey();

            char[] charArr = new char[10];
            charArr[10] = 'a';
        }
    }
}
