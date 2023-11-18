#include "sock.h"

auto TcpSocket::create_server(const uint32_t& port) -> TcpSocket {
  WSADATA wsaData;
  auto ret = WSAStartup(MAKEWORD(2, 0), &wsaData);
  if (ret != 0) {
    std::ostringstream ss;
    ss << "failed to initialize Winsock DLL. error code: "
      << WSAGetLastError() << ".";
    throw std::runtime_error(ss.str());
  }
  int32_t sock0, sock;
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.S_un.S_addr = INADDR_ANY;
  int32_t len = sizeof(struct sockaddr_in);
  sock0 = socket(AF_INET, SOCK_STREAM, 0);
  bind(sock0, (struct sockaddr*)&addr, sizeof(addr));
  listen(sock0, 5);
  struct sockaddr_in client;
  sock = accept(sock0, (struct sockaddr*)&client, &len);
  return TcpSocket(sock);
}

auto TcpSocket::create_client(
  const uint32_t& port,
  const std::string& ip_address) -> TcpSocket {
  WSADATA wsaData;
  auto ret = WSAStartup(MAKEWORD(2, 0), &wsaData);
  if (ret != 0) {
    std::ostringstream ss;
    ss << "failed to initialize Winsock DLL. error code: "
      << WSAGetLastError() << ".";
    throw std::runtime_error(ss.str());
  }
  auto sock = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  inet_pton(AF_INET, ip_address.c_str(), &addr.sin_addr.s_addr);
  connect(sock, (struct sockaddr*)&addr, sizeof(addr));
  return TcpSocket(sock);
}

auto TcpSocket::write(const std::vector<char>& data) const -> void {
  auto ret = send(_sock, data.data(), data.size(), 0);
  if (ret != data.size()) {
    std::ostringstream ss;
    ss << "failed to write operation. error code: "
      << WSAGetLastError() << ".";
    throw std::runtime_error(ss.str());
  }
}
