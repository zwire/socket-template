using Husty.Communication;

var type = "TCPClient";
var port = 3000;

using IDataTransporter stream = type.ToLower() switch
{
    "tcpserver"         => new TcpSocketServer(port).GetStream().Value,
    "tcpclient"         => new TcpSocketClient("127.0.0.1", port).GetStream().Value,
    "udplistener"       => new UdpDataTransporter().SetListeningPort(port),
    "udpcaller"         => new UdpDataTransporter().SetTargetPorts(port),
    "websocketserver"   => WebSocketDataTransporter.CreateServerAsync("127.0.0.1", port).Result,
    "websocketclient"   => WebSocketDataTransporter.CreateClientAsync("127.0.0.1", port).Result,
    _                   => throw new NotSupportedException()
};

// while (true)
// {
//    stream.TryWriteLine("Hello");
//    Thread.Sleep(1000);
// }

while (stream.ReadLine() is string msg)
{
    Console.WriteLine(msg);
}

Console.WriteLine("completed.");