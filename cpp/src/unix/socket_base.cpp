#include <unistd.h>
#include <sys/socket.h>
#include "sock.h"

SocketBase::SocketBase(int32_t sock)
  : _closed(false), _sock(sock) {}

auto SocketBase::terminate() -> void {
  if (_closed) return;
  _closed = true;
  close(_sock);
}

auto SocketBase::read(const int& capacity) const -> std::vector<char> {
  std::vector<char> v(capacity);
  int ret = recv(_sock, v.data(), capacity, 0);
  if (ret < 1) {
    std::ostringstream ss;
    ss << "failed to recv operation. error code: " << errno << ".";
    throw std::runtime_error(ss.str());
  }
  v.resize(ret);
  return v;
}

auto SocketBase::read_exactly(
  char* data, const uint32_t& size) const -> void {
  auto completed = 0;
  while (completed < size) {
    auto ret = recv(_sock, data + completed, size - completed, 0);
    if (ret < 1) {
      std::ostringstream ss;
      ss << "failed to recv operation. error code: " << errno << ".";
      throw std::runtime_error(ss.str());
    }
    completed += ret;
  }
}


// #include <string.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>


// #define sleep_msec(n) usleep(n * 1e+3)
// #define SET_SOCKADDR_IPV4(addr, value)  (addr).sin_addr.s_addr = (value)
// #define SOCKLEN socklen_t
// #define SOCK int


// 	bool try_read(char* buf, const int& size) const
// 	{
// 		char* tmp = new char[size];
// 		int remain = size;
// 		int completed = 0;
// 		do
// 		{
// 			int r = recv(_sock, tmp + completed, remain, 0);
// 			if (r < 1) return false;
// 			completed += r;
// 			remain -= r;
// 		} while (completed < size);
// 		buf = tmp;
// 		return true;
// 	}

// 	bool try_read_line(std::string& str) const
// 	{
// 		int i = 0;
// 		std::vector<char> tmp;
// 		tmp.reserve(10);
// 		tmp.push_back(0);
// 		while (true)
// 		{
// 			if (recv(_sock, tmp.data() + i, 1, 0) == 1)
// 			{
// 				if (tmp[i] == '\n')
// 				{
// 					str = std::string(tmp.data(), i);
// 					return true;
// 				}
// 				else if (tmp[i] >= 0)
// 				{
// 					i++;
// 					tmp.push_back(0);
// 				}
// 			}
// 		}
// 	}

// };


// class UdpSocket final
// {
// private:
// 	bool _closed = false;
// 	SOCK _sock;
// 	std::vector<struct sockaddr_in> _addr_out;

// public:
// 	std::string new_line = "\n";

// 	~UdpSocket()
// 	{
// 		if (!_closed)
// 		{
// 			_closed = true;
// #ifdef __unix__
// 			close(_sock);
// #else
// 			closesocket(_sock);
// 			WSACleanup();
// #endif
// 		}
// 	}

// 	UdpSocket()
// 	{
// #ifndef __unix__
// 		WSADATA wsaData;
// 		int e = WSAStartup(MAKEWORD(2, 0), &wsaData);
// 		if (e != 0) throw "failed to initialize Winsock DLL";
// #endif
// 		_sock = socket(AF_INET, SOCK_DGRAM, 0);
// 	}

// 	void set_listening_port(const int& port)
// 	{
// 		struct sockaddr_in addr_in;
// 		addr_in.sin_family = AF_INET;
// 		addr_in.sin_port = htons(port);
// 		SET_SOCKADDR_IPV4(addr_in, INADDR_ANY);
// 		bind(_sock, (struct sockaddr*)&addr_in, sizeof(addr_in));
// 	}

// 	void set_target_ports(const std::vector<int>& ports)
// 	{
// 		std::vector<std::tuple<int, std::string>> v;
// 		for (int p : ports)
// 		{
// 			std::tuple<int, std::string> t;
// 			std::get<0>(t) = p;
// 			std::get<1>(t) = "127.0.0.1";
// 			v.push_back(t);
// 		}
// 		set_target_ports(v);
// 	}

// 	void set_target_ports(const std::vector<std::tuple<int, std::string>>& targets)
// 	{
// 		for (int i = 0; i < targets.size(); i++)
// 		{
// 			int port = std::get<0>(targets[i]);
// 			std::string send_addr = std::get<1>(targets[i]);
// 			struct sockaddr_in ad;
// 			ad.sin_family = AF_INET;
// 			ad.sin_port = htons(port);
// 			SET_SOCKADDR_IPV4(ad, inet_addr(send_addr.c_str()));
// 			if (send_addr == "255.255.255.255")
// 			{
// 				int yes = 1;
// 				setsockopt(_sock, SOL_SOCKET, SO_BROADCAST, (char*)&yes, sizeof(yes));
// 			}
// 			_addr_out.push_back(ad);
// 		}
// 	}

// 	bool try_write(const char* buf, const int& len) const
// 	{
// 		if (_addr_out.size() < 1)
// 			throw "no target is defined.";
// 		for (auto addr : _addr_out)
// 		{
// 			if (sendto(_sock, buf, len, 0, (struct sockaddr*)&addr, sizeof(addr)) != len) return false;
// 		}
// 		return true;
// 	}

// 	bool try_write_line(const std::string& str) const
// 	{
// 		if (_addr_out.size() < 1)
// 			throw "no target is defined.";
// 		for (auto addr : _addr_out)
// 		{
// 			int i = sendto(_sock, str.c_str(), strlen(str.c_str()), 0, (struct sockaddr*)&addr, sizeof(addr));
// 			if (i == strlen(str.c_str()))
// 			{
// 				const char* ln = new_line.c_str();
// 				if (sendto(_sock, ln, strlen(ln), 0, (struct sockaddr*)&addr, sizeof(addr)) != 1) return false;
// 			}
// 			else return false;
// 		}
// 		return true;
// 	}

// 	bool try_read(char* buf, const int& size) const
// 	{
// 		char* tmp = new char[size];
// 		int remain = size;
// 		int completed = 0;
// 		do
// 		{
// 			int r = recv(_sock, tmp + completed, remain, 0);
// 			if (r < 1)
// 			{
// 				delete[] tmp;
// 				return false;
// 			}
// 			completed += r;
// 			remain -= r;
// 		} while (completed < size);
// 		buf = tmp;
// 		delete[] tmp;
// 		return true;
// 	}

// 	bool try_read_line(std::string& str) const
// 	{
// 		char* tmp = new char[65535];
// 		int received = 0;
// 		while (true)
// 		{
// 			received += recv(_sock, tmp + received, 65535 - received, 0);
// 			if (received > 0)
// 			{
// 				std::vector<int> places;
// 				for (int i = 0; i < received; i++)
// 				{
// 					if (tmp[i] == '\n') places.push_back(i);
// 				}
// 				if (places.size() == 1)
// 				{
// 					str = std::string(tmp, places[0]);
// 					delete[] tmp;
// 					return true;
// 				}
// 				else if (places.size() > 1)
// 				{
// 					int first = places[places.size() - 2];
// 					int second = places[places.size() - 1];
// 					str = std::string(tmp + first + 1, second - first - 1);
// 					delete[] tmp;
// 					return true;
// 				}
// 				continue;
// 			}
// 			delete[] tmp;
// 			return false;
// 		}
// 	}

// };