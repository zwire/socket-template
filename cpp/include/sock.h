#ifndef SOCK_H_
#define SOCK_H_

#include <string>
#include <vector>
#include <tuple>
#include <sstream>
#include <stdexcept>

#ifdef __unix__

#else
  #include <WS2tcpip.h>
#endif

class SocketBase {
 public:
  virtual auto close() -> void;
  virtual auto write(const std::vector<char>& data) const -> void = 0;
  virtual auto read(const int& capacity = 1024) const -> std::vector<char>;
  virtual auto read_exactly(char* data, const uint32_t& size) const ->void;

 protected:
  int32_t _sock;
  explicit SocketBase(int32_t sock);

 private:
  bool _closed;
};

class TcpSocket : public SocketBase {
 public:
  static auto create_server(
    const uint32_t& port) -> TcpSocket;
  static auto create_client(
    const uint32_t& port,
    const std::string& ip_address = "127.0.0.1") -> TcpSocket;
  auto write(const std::vector<char>& data) const -> void override;

 private:
  explicit TcpSocket(const int32_t& sock) : SocketBase(sock) {}
};

class UdpSocket : public SocketBase {
 public:
  static auto create_socket() -> UdpSocket;
  auto set_listening_port(const uint32_t& port) -> UdpSocket&;
  auto set_target_ports(const std::vector<uint32_t>& ports) -> UdpSocket&;
  using Target = std::tuple<uint32_t, std::string>;
  auto set_targets(const std::vector<Target>& targets) -> UdpSocket&;
  auto write(const std::vector<char>& data) const -> void override;

 private:
  std::vector<struct sockaddr_in> _addr_out;
  explicit UdpSocket(const int32_t& sock) : SocketBase(sock) {}
};

#endif // SOCK_H_
