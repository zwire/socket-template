from sock import *

if __name__ == '__main__':
  #sock = TcpSocket.create_server(3000)
  #sock = TcpSocket.create_client(3001)
  sock = UdpSocket().set_listening_port(3000).set_target_ports(3001)
  while True:
    msg = "Hello"
    sock.write_line(msg)
    # print(sock.read_line())