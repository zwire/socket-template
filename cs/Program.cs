using System.Text;
using Husty.Communication;

var type = "TCPServer";
var listen = 3000;
var call = 3001;

using IDataTransporter stream = type.ToLower() switch
{
    "tcpserver"         => new TcpSocketServer(listen).GetStream().Value,
    "tcpclient"         => new TcpSocketClient("127.0.0.1", call).GetStream().Value,
    "udplistener"       => new UdpDataTransporter().SetListeningPort(listen),
    "udpcaller"         => new UdpDataTransporter().SetTargetPorts(call),
    "websocketserver"   => WebSocketDataTransporter.CreateServerAsync("127.0.0.1", listen).Result,
    "websocketclient"   => WebSocketDataTransporter.CreateClientAsync("127.0.0.1", call).Result,
    _                   => throw new NotSupportedException()
};

//while (true)
//{
//    stream.TryWriteLine("Hello");
//    Thread.Sleep(1000);
//}

while (stream.ReadLine() is string msg)
{
    Console.WriteLine(msg);
}

Console.WriteLine("completed.");