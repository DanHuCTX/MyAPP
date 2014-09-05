using System;
using System.Collections.Generic;
using System.IO;
using System.ServiceModel;
using System.Text;

namespace PrivateChannel.Interface
{
    [MessageContract]
    public class UploadEvent
    {
        [MessageHeader]
        public string FileName { get; set; }

        [MessageHeader]
        public int Type { get; set; }

        [MessageBodyMember]
        public Stream FileData { get; set; }
    }

    [ServiceContract(Name="PrivateChannelServer",Namespace = "http://www.danhu.com")]
    public interface IChannelServer
    {
        [OperationContract]
        string Echo(string msg);

        [OperationContract(IsOneWay = true)]
        void Upload(UploadEvent e);
    }
}
