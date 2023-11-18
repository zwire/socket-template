#include <memory>
#include <thread>
#include "sock.h"
#include "gtest/gtest.h"

class Tcp : public testing::Test {
 protected:
  std::unique_ptr<TcpSocket> server;
  std::unique_ptr<TcpSocket> client;

  void SetUp() override {
    std::thread th([&](){
      server = std::make_unique<TcpSocket>(TcpSocket::create_server(3000));
    });
    client = std::make_unique<TcpSocket>(TcpSocket::create_client(3000));
    th.join();
  }

  void TearDown() override {
    client->close();
    server->close();
  }
};

TEST_F(Tcp, Connect) {
  
}

TEST_F(Tcp, EchoBinary) {
  std::vector<char> bin { 0x00, 0x10, 0x11, 0x77, 0x0f };

  // sub-thread
  std::thread th([&]() {
    std::vector<char> rcv(bin.size());
    server->read_exactly(rcv.data(), rcv.size());
    for (auto i = 0; i < bin.size(); ++i) {
      EXPECT_EQ(bin[i], rcv[i]);
    }
    server->write(bin);
  });

  // main-thread
  client->write(bin);
  auto rcv = client->read();
  for (auto i = 0; i < bin.size(); ++i) {
    EXPECT_EQ(bin[i], rcv[i]);
  }

  th.join();
}

TEST_F(Tcp, EchoString) {
  auto msg0 = "msg0";
  auto msg1 = "msg1!";
  auto to_vec = [](const char* msg, uint32_t size) {
    std::vector<char> v(size);
    memcpy(v.data(), msg, size);
    return v;
  };

  // sub-thread
  std::thread th([&]() {
    EXPECT_STREQ(msg0, server->read().data());
    server->write(to_vec(msg1, strlen(msg1)));
  });

  // main-thread
  client->write(to_vec(msg0, strlen(msg0)));
  EXPECT_STREQ(msg1, client->read().data());

  th.join();
}

TEST(_, TcpNoServer) {
  try {
    auto client = TcpSocket::create_client(3000);
    std::vector<char> bin { 0x00, 0x10, 0x11, 0x77, 0x0f };
    client.write(bin);
  }
  catch(std::exception e) {
#ifdef __unix__

#else
    EXPECT_STREQ("failed to write operation. error code: 10057.", e.what());
#endif
  }
}
