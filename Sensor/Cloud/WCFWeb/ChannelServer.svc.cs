using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.Serialization;
using System.ServiceModel;
using System.ServiceModel.Web;
using System.Text;
using Microsoft.WindowsAzure;
using Microsoft.WindowsAzure.ServiceRuntime;
using Microsoft.WindowsAzure.StorageClient;
using Microsoft.WindowsAzure.Storage.Blob;

namespace WCFWeb
{
    // NOTE: You can use the "Rename" command on the "Refactor" menu to change the class name "Service1" in code, svc and config file together.
    // NOTE: In order to launch WCF Test Client for testing this service, please select Service1.svc or Service1.svc.cs at the Solution Explorer and start debugging.
    public class ChannelServer : IChannelServer
    {
        public string Echo(string msg)
        {
            return "Hello " + msg;
        }

        public void Upload(UploadEvent e)
        {
            traceSource.TraceEvent(TraceEventType.Information, 0, "New upload comes, file name is {0}", e.FileName);
            var account = CloudStorageAccount.Parse(RoleEnvironment.GetConfigurationSettingValue("DataConnectionString"));
            var ctx = account.CreateCloudBlobClient();
            var cloudBlobContainer = ctx.GetContainerReference("package");
            cloudBlobContainer.CreateIfNotExist();
            var blob = cloudBlobContainer.GetBlobReference(e.FileName);
            traceSource.TraceEvent(TraceEventType.Information, 0, "Putting {0} into blob storage", e.FileName);
            blob.BeginUploadFromStream(e.FileData, UploadFinished, blob.Uri);
        }

        private void UploadFinished(IAsyncResult asyncResult)
        {
            var account = CloudStorageAccount.Parse(RoleEnvironment.GetConfigurationSettingValue("DataConnectionString"));
            CloudQueue q = account.CreateCloudQueueClient().GetQueueReference("package-incoming");
            q.CreateIfNotExist();
            Uri uri = (Uri)asyncResult.AsyncState;
            traceSource.TraceEvent(TraceEventType.Information, 0, "Uploading file into blob finished, now add blob url {0} into message queue for being processed by worker", uri.AbsoluteUri);
            q.AddMessage(new CloudQueueMessage(uri.AbsoluteUri));
        }

        private static TraceSource traceSource = new TraceSource("CustomTrace", SourceLevels.All);
    }
}
