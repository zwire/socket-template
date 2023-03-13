#include "sock.h"

int main()
{
	//auto sock = TcpSocket::create_server(3000);
	auto sock = TcpSocket::create_client(3000);
	//UdpSocket sock;
	//sock.set_listening_port(3001);
	//sock.set_target_ports(std::vector<int>{ 3000 });
	while (true)
	{
		std::string msg = "Hello";
		// sock.try_write_line(msg);
		if (sock.try_read_line(msg))
		{
			std::cout << msg << std::endl;
		}
		sleep_msec(1000);
	}
}