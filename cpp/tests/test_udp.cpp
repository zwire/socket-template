#include <memory>
#include <thread>
#include "sock.h"
#include "gtest/gtest.h"

class Udp : public testing::Test {
 protected:
  std::unique_ptr<UdpSocket> sock0;
  std::unique_ptr<UdpSocket> sock1;

  void SetUp() override {
    sock0 = std::make_unique<UdpSocket>(
      UdpSocket::create_socket()
        .set_listening_port(3000)
        .set_target_ports({3001}));
    sock1 = std::make_unique<UdpSocket>(
      UdpSocket::create_socket()
        .set_listening_port(3001)
        .set_target_ports({3000}));
  }

  void TearDown() override {
    sock0->close();
    sock1->close();
  }
};

TEST_F(Udp, Connect) {
  
}

TEST_F(Udp, EchoBinary) {
  std::vector<char> bin { 0x00, 0x10, 0x11, 0x77, 0x0f };

  // sub-thread
  std::thread th([&]() {
    std::vector<char> rcv(bin.size());
    sock0->read_exactly(rcv.data(), rcv.size());
    for (auto i = 0; i < bin.size(); ++i) {
      EXPECT_EQ(bin[i], rcv[i]);
    }
    sock0->write(bin);
  });

  // main-thread
  sock1->write(bin);
  auto rcv = sock1->read();
  for (auto i = 0; i < bin.size(); ++i) {
    EXPECT_EQ(bin[i], rcv[i]);
  }

  th.join();
}

TEST_F(Udp, EchoString) {
  auto msg0 = "msg0";
  auto msg1 = "msg1!";
  auto to_vec = [](const char* msg, uint32_t size) {
    std::vector<char> v(size);
    memcpy(v.data(), msg, size);
    return v;
  };

  // sub-thread
  std::thread th([&]() {
    EXPECT_STREQ(msg0, sock0->read().data());
    sock0->write(to_vec(msg1, strlen(msg1)));
  });

  // main-thread
  sock1->write(to_vec(msg0, strlen(msg0)));
  EXPECT_STREQ(msg1, sock1->read().data());

  th.join();
}

