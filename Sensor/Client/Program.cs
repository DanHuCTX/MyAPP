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
            // only needs one line code for registering crash monitor 
            new ProcessSensor().RegisterProcess(Process.GetCurrentProcess().Id);

            Console.WriteLine("Current process Id = {0}, Press any key to crash", Process.GetCurrentProcess().Id);
            Console.ReadKey();

            char[] charArr = new char[10];
            charArr[10] = 'a';
        }
    }
}
