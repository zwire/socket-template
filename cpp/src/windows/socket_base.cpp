#include "sock.h"

SocketBase::SocketBase(int32_t sock)
  : _closed(false), _sock(sock) {}

auto SocketBase::terminate() -> void {
  if (_closed) return;
  _closed = true;
  closesocket(_sock);
  WSACleanup();
}

auto SocketBase::read(const int& capacity) const -> std::vector<char> {
  std::vector<char> v(capacity);
  int ret = recv(_sock, v.data(), capacity, 0);
  if (ret < 1) {
    std::ostringstream ss;
    ss << "failed to recv operation. error code: "
      << WSAGetLastError() << ".";
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
      ss << "failed to recv operation. error code: "
        << WSAGetLastError() << ".";
      throw std::runtime_error(ss.str());
    }
    completed += ret;
  }
}
