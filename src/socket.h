#pragma once

#include <string>
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

class Socket {
public:
  explicit Socket();
  explicit Socket(int s);
  Socket(const Socket& s) = delete;
  Socket(Socket &&s);

  ~Socket();

  Socket& operator=(const Socket& s) = delete;
  Socket& operator=(Socket &&s);

  Socket& operator>>(std::string& str);
  Socket& operator<<(const std::string& str);

  int _bind(int port);
  int _bind(std::string ip, int port);
  int _listen();
  Socket _accept();
  std::string _recv(bool& has_res);
  void _connect(std::string ip, int port);

  int _sendn(std::string str);

  void setTerminator(char term);

private:
  int s = -1;
  char buffer[BUF_SIZE+1];
  struct sockaddr_in local;
  struct sockaddr_in remote;
  char terminator = -1;
  std::string str_buff;
};
