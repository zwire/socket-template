#include <arpa/inet.h>
#include <sys/socket.h>
#include "sock.h"

auto UdpSocket::create_socket() -> UdpSocket {
  return UdpSocket(socket(AF_INET, SOCK_DGRAM, 0));
}

auto UdpSocket::set_listening_port(const uint32_t& port) -> UdpSocket& {
  struct sockaddr_in addr_in;
  addr_in.sin_family = AF_INET;
  addr_in.sin_port = htons(port);
  addr_in.sin_addr.s_addr = INADDR_ANY;
  bind(_sock, (struct sockaddr*)&addr_in, sizeof(addr_in));
  return *this;
}

auto UdpSocket::set_target_ports(
  const std::vector<uint32_t>& ports) -> UdpSocket& {
  std::vector<Target> v;
  for (const auto& p : ports) {
    v.push_back(std::forward_as_tuple(p, "127.0.0.1"));
  }
  set_targets(v);
  return *this;
}

auto UdpSocket::set_targets(
  const std::vector<Target>& targets) -> UdpSocket& {
  for (const auto& t : targets) {
    uint32_t port;
    std::string send_addr;
    std::tie(port, send_addr) = t;
    struct sockaddr_in ad;
    ad.sin_family = AF_INET;
    ad.sin_port = htons(port);
    ad.sin_addr.s_addr = inet_addr(send_addr.c_str());
    if (send_addr == "255.255.255.255") {
      int yes = 1;
      setsockopt(
        _sock, SOL_SOCKET, SO_BROADCAST,
        reinterpret_cast<char*>(&yes), sizeof(yes));
    }
    _addr_out.push_back(ad);
  }
  return *this;
}

auto UdpSocket::write(const std::vector<char>& data) const -> void {
  if (_addr_out.size() < 1)
    throw std::runtime_error("no target is specified");
  for (const auto& addr : _addr_out) {
    auto ret = sendto(_sock, data.data(), data.size(), 0,
      (struct sockaddr*)&addr, sizeof(addr));
    if (ret != data.size()) {
      std::ostringstream ss;
      ss << "failed to write operation. error code: " << errno << ".";
      throw std::runtime_error(ss.str());
    }
  }
}
