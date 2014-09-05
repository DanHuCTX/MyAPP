using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.ServiceModel;
using System.ServiceProcess;
using System.Text;
using System.Threading;
using PrivateChannel;

namespace HostingService
{
    public partial class HostingService : ServiceBase
    {
        public HostingService()
        {
            InitializeComponent();
        }

        protected override void OnStart(string[] args)
        {

            if (myServiceHost != null)
            {
                myServiceHost.Close();
            }
            myServiceHost = new ServiceHost(typeof(ChannelServer));
            myServiceHost.Open();
        }

        protected override void OnStop()
        {
            if (myServiceHost != null)
            {
                myServiceHost.Close();
                myServiceHost = null;
            }
        }

        private static ServiceHost myServiceHost = null; 
    }
}
