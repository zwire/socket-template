#ifndef SOCKET_H_
#define SOCKET_H_
#pragma warning(disable:4996)

#include <vector>
#include <tuple>
#include <conio.h>
#include <iostream>
#include <sstream>
#include <stdio.h>

#ifdef __unix__
#define sleep_msec(n) usleep(n * 1e+3)

#else
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#define sleep_msec(n) Sleep(n)

class TcpSocket final
{
private:
	bool _closed = false;
	SOCKET _sock;
	TcpSocket(SOCKET sock)
	{
		_sock = sock;
	}

public:
	std::string new_line = "\n";

	void close()
	{
		if (!_closed)
		{
			_closed = true;
			closesocket(_sock);
			WSACleanup();
		}
	}

	~TcpSocket() { close(); }

	static TcpSocket create_server(const int& port)
	{
		WSADATA wsaData;
		int e = WSAStartup(MAKEWORD(2, 0), &wsaData);
		if (e != 0) throw "failed to initialize Winsock DLL";
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.S_un.S_addr = INADDR_ANY;
		SOCKET sock0 = socket(AF_INET, SOCK_STREAM, 0);
		bind(sock0, (struct sockaddr*)&addr, sizeof(addr));
		listen(sock0, 5);
		struct sockaddr_in client;
		int len = sizeof(client);
		SOCKET sock = accept(sock0, (struct sockaddr*)&client, &len);
		return TcpSocket(sock);
	}

	static TcpSocket create_client(const int& port, const std::string& ip_address = "127.0.0.1")
	{
		WSADATA wsaData;
		int e = WSAStartup(MAKEWORD(2, 0), &wsaData);
		if (e != 0) throw "failed to initialize Winsock DLL";
		struct sockaddr_in server;
		server.sin_family = AF_INET;
		server.sin_port = htons(port);
		inet_pton(AF_INET, ip_address.c_str(), &server.sin_addr.s_addr);
		SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
		connect(sock, (struct sockaddr*)&server, sizeof(server));
		return TcpSocket(sock);
	}


	bool try_write(const char* buf, const int& len) const
	{
		return send(_sock, buf, len, 0) == len;
	}

	bool try_write_line(const std::string& str) const
	{
		int i = send(_sock, str.c_str(), strlen(str.c_str()), 0);
		if (i == strlen(str.c_str()))
		{
			const char* ln = new_line.c_str();
			return send(_sock, ln, strlen(ln), 0) == 1;
		}
		return false;
	}

	bool try_read(char* buf, const int& size) const
	{
		char* tmp = new char[size];
		int remain = size;
		int completed = 0;
		do
		{
			int r = recv(_sock, tmp + completed, remain, 0);
			if (r < 1) return false;
			completed += r;
			remain -= r;
		} while (completed < size);
		buf = tmp;
		return true;
	}

	bool try_read_line(std::string& str) const
	{
		int i = 0;
		std::vector<char> tmp;
		tmp.reserve(10);
		tmp.push_back(0);
		while (true)
		{
			if (recv(_sock, tmp.data() + i, 1, 0) == 1)
			{
				if (tmp[i] == '\n')
				{
					str = std::string(tmp.data(), i);
					return true;
				}
				else if (tmp[i] >= 0)
				{
					i++;
					tmp.push_back(0);
				}
			}
		}
	}

};


class UdpSocket final
{
private:
	bool _closed = false;
	SOCKET _sock;
	std::vector<struct sockaddr_in> _addr_out;

public:
	std::string new_line = "\n";

	void close()
	{
		if (!_closed)
		{
			_closed = true;
			closesocket(_sock);
			WSACleanup();
		}
	}

	~UdpSocket() { close(); }

	UdpSocket()
	{
		WSADATA wsaData;
		int e = WSAStartup(MAKEWORD(2, 0), &wsaData);
		if (e != 0) throw "failed to initialize Winsock DLL";
		_sock = socket(AF_INET, SOCK_DGRAM, 0);
	}

	void set_listening_port(const int& port)
	{
		struct sockaddr_in addr_in;
		addr_in.sin_family = AF_INET;
		addr_in.sin_port = htons(port);
		addr_in.sin_addr.S_un.S_addr = INADDR_ANY;
		bind(_sock, (struct sockaddr*)&addr_in, sizeof(addr_in));
	}

	void set_target_ports(const std::vector<int>& ports)
	{
		std::vector<std::tuple<int, std::string>> v;
		for (int p : ports)
		{
			std::tuple<int, std::string> t;
			std::get<0>(t) = p;
			std::get<1>(t) = "127.0.0.1";
			v.push_back(t);
		}
		set_target_ports(v);
	}

	void set_target_ports(const std::vector<std::tuple<int, std::string>>& targets)
	{
		for (int i = 0; i < targets.size(); i++)
		{
			int port = std::get<0>(targets[i]);
			std::string send_addr = std::get<1>(targets[i]);
			struct sockaddr_in ad;
			ad.sin_family = AF_INET;
			ad.sin_port = htons(port);
			ad.sin_addr.S_un.S_addr = inet_addr(send_addr.c_str());
			if (send_addr == "255.255.255.255")
			{
				BOOL yes = 1;
				setsockopt(_sock, SOL_SOCKET, SO_BROADCAST, (char*)&yes, sizeof(yes));
			}
			_addr_out.push_back(ad);
		}
	}

	bool try_write(const char* buf, const int& len) const
	{
		for (auto addr : _addr_out)
		{
			if (sendto(_sock, buf, len, 0, (struct sockaddr*)&addr, sizeof(addr)) != len) return false;
		}
		return true;
	}

	bool try_write_line(const std::string& str) const
	{
		for (auto addr : _addr_out)
		{
			int i = sendto(_sock, str.c_str(), strlen(str.c_str()), 0, (struct sockaddr*)&addr, sizeof(addr));
			if (i == strlen(str.c_str()))
			{
				const char* ln = new_line.c_str();
				if (sendto(_sock, ln, strlen(ln), 0, (struct sockaddr*)&addr, sizeof(addr)) != 1) return false;
			}
			else return false;
		}
		return true;
	}

	bool try_read(char* buf, const int& size) const
	{
		char* tmp = new char[size];
		int remain = size;
		int completed = 0;
		do
		{
			int r = recv(_sock, tmp + completed, remain, 0);
			if (r < 1)
			{
				delete[] tmp;
				return false;
			}
			completed += r;
			remain -= r;
		} while (completed < size);
		buf = tmp;
		delete[] tmp;
		return true;
	}

	bool try_read_line(std::string& str) const
	{
		char* tmp = new char[65535];
		int received = 0;
		while (true)
		{
			received += recv(_sock, tmp + received, 65535 - received, 0);
			if (received > 0)
			{
				std::vector<int> places;
				for (int i = 0; i < received; i++)
				{
					if (tmp[i] == '\n') places.push_back(i);
				}
				if (places.size() == 1)
				{
					str = std::string(tmp, places[0]);
					delete[] tmp;
					return true;
				}
				else if (places.size() > 1)
				{
					int first = places[places.size() - 2];
					int second = places[places.size() - 1];
					str = std::string(tmp + first + 1, second - first - 1);
					delete[] tmp;
					return true;
				}
				continue;
			}
			delete[] tmp;
			return false;
		}
	}

};
#endif

#endif