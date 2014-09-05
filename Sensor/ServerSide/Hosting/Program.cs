using System;
using System.Collections.Generic;
using System.Linq;
using System.ServiceModel;
using System.ServiceModel.Description;
using System.Text;
using PrivateChannel;
using PrivateChannel.Interface;
namespace Hosting
{
    class Program
    {
        static void Main(string[] args)
        {
            using (ServiceHost host = new ServiceHost(typeof (ChannelServer)))
            {
                /*
                host.AddServiceEndpoint(typeof (IChannelServer), new WSHttpBinding(),
                    "http://127.0.0.1:9527/ChannelService");

                if (host.Description.Behaviors.Find<ServiceMetadataBehavior>() == null)
                {
                    ServiceMetadataBehavior behavior = new ServiceMetadataBehavior();
                    behavior.HttpGetEnabled = true;
                    behavior.HttpGetUrl = new Uri("http://127.0.0.1:9527/ChannelService/metadata");
                    host.Description.Behaviors.Add(behavior);
                }
                 * */
                host.Opened += delegate
                {
                    Console.WriteLine("Channel server service has started, press any key to end service!");
                };

                host.Open();
                Console.Read();
                host.Close();
            }
        }
    }
}
