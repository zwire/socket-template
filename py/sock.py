from __future__ import annotations
import socket

class TcpSocket:

  @staticmethod
  def create_server(port: int, ip_address: str = '127.0.0.1'):
    sock0 = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    sock0.bind((ip_address, port))
    sock0.listen(1)
    sock, _ = sock0.accept()
    return TcpSocket(sock0, sock)

  @staticmethod
  def create_client(port: int, ip_address: str = '127.0.0.1'):
    sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    sock.connect((ip_address, port))
    return TcpSocket(None, sock)

  encoding = 'utf-8'
  new_line = '\n'
    
  def __init__(self, sock0: socket, sock: socket):
    self.sock0 = sock0
    self.sock = sock
  
  def __del__(self):
    self.close()
  
  def close(self):
    self.sock.close()
    if self.sock0 is not None: self.sock0.close()

  def write(self, buf: bytes):
    self.sock.send(buf)
  
  def write_line(self, msg: str):
    self.sock.send((msg + '\n').encode(self.encoding))
  
  def read(self, size: int):
    return self.sock.recv(size)
  
  def read_line(self):
    rcv = ""
    while True:
      rcv += self.sock.recv(1024).decode(self.encoding)
      if self.new_line in rcv:
        lines = rcv.split(self.new_line)
        if len(lines) == 1:
          return lines[-1]
        else:
          return lines[-2]

class UdpSocket:

  encoding = 'utf-8'
  new_line = '\n'

  def __init__(self):
    self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    self.targets = []
  
  def __del__(self):
    self.close()
  
  def close(self):
    self.sock.close()

  def set_listening_port(self, port: int, ip_address: str = ''):
    self.sock.bind((ip_address, port))
    return self

  def set_target_ports(self, targets: int | tuple[str, int] | list[tuple[str, int]]):
    if type(targets) is int:
      self.targets.append(('127.0.0.1', targets))
      self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    elif type(targets) is tuple[str, int]:
      self.targets.append(targets)
      if targets[1] == '':
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    elif type(targets) is list[tuple[str, int]]:
      for t in targets:
        self.targets.append(t)
        if t[1] == '':
          self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    return self

  def write(self, buf: bytes):
    for t in self.targets:
      self.sock.sendto(buf, t)
  
  def write_line(self, msg: str):
    for t in self.targets:
      self.sock.sendto((msg + '\n').encode(self.encoding), t)
  
  def read(self, size: int):
    buf, _ = self.sock.recvfrom(size)
    return buf
  
  def read_line(self):
    rcv = ""
    while True:
      buf, _ = self.sock.recvfrom(1024)
      rcv += buf.decode(self.encoding)
      if self.new_line in rcv:
        lines = rcv.split(self.new_line)
        if len(lines) == 1:
          return lines[-1]
        else:
          return lines[-2]