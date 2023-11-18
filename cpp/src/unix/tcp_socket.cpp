#include <arpa/inet.h>
#include "sock.h"

auto TcpSocket::create_server(const uint32_t& port) -> TcpSocket {
  int32_t sock0, sock;
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  socklen_t len = sizeof(struct sockaddr_in);
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
  auto sock = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  inet_pton(AF_INET, ip_address.c_str(), &addr.sin_addr.s_addr);
  connect(sock, (struct sockaddr*)&addr, sizeof(addr));
  return TcpSocket(sock);
}
#include <iostream>
auto TcpSocket::write(const std::vector<char>& data) const -> void {
  auto ret = send(_sock, data.data(), data.size(), 0);
  std::cout << "!!!!!!!!" << std::endl;
  return;
  if (ret != data.size()) {
    std::ostringstream ss;
    ss << "failed to write operation. error code: " << errno << ".";
    throw std::runtime_error(ss.str());
  }
}
