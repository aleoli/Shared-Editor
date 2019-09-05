#include "socket.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

Socket::Socket() {
  this->s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
}

Socket::Socket(int s): s(s) {
}

Socket::Socket(Socket &&s) {
  this->s = s.s;
  s.s = -1;
}

Socket& Socket::operator=(Socket &&s) {
  if(this == &s) {
    return *this;
  }
  this->s = s.s;
  s.s = -1;
  return *this;
}

Socket::~Socket() {
  if(this->s >= 0) {
    close(this->s);
  }
}

int Socket::_bind(int port) {
  return this->_bind("0.0.0.0", port);
}

int Socket::_bind(std::string ip, int port) {
  local.sin_family = AF_INET;
  local.sin_port = htons(port);
  int r = inet_aton(ip.c_str(), &(local.sin_addr));
  if(r == 0) {
    printf("%s is not a valid IP\n", ip.c_str());
    // TODO
    throw 0;
  }
  return bind(this->s, (struct sockaddr *) &local, sizeof(local));
}

int Socket::_listen() {
  return listen(this->s, 32);
}

Socket Socket::_accept() {
  socklen_t l = sizeof(remote);
  int s_a = accept(s, (struct sockaddr *) &remote, &l);
  return Socket{s_a};
}

std::string Socket::_recv(bool& has_res) {
  has_res = false;
  struct timeval tval;
  fd_set cset;
  FD_ZERO(&cset);
  FD_SET(this->s, &cset);
  tval.tv_sec = 0;
  tval.tv_usec = 100;
  int n = select(FD_SETSIZE, &cset, NULL, NULL, &tval);
  if(n<0) {
    printf("Error during Select\n");
    // TODO
    throw 0;
  } else if(n==0) {
    return "";
  }
  ssize_t n_read = recv(this->s, this->buffer, BUF_SIZE, 0);
  this->buffer[n_read] = '\0';
  has_res = true;
  return std::string{this->buffer};
}

void Socket::_connect(std::string ip, int port) {
  int old_fl = fcntl(this->s, F_SETFL, O_NONBLOCK);
  struct hostent *he = gethostbyname(ip.c_str());
  if(!he) {
    printf("%s was not resolved\n", ip.c_str());
    // TODO
    throw 0;
  }
  local.sin_family = AF_INET;
  local.sin_port = htons(port);
  struct in_addr ip_addr = *(struct in_addr *)(he->h_addr_list[0]);
  local.sin_addr.s_addr = ip_addr.s_addr;
  connect(this->s, (struct sockaddr *) &local, sizeof(local));
  struct timeval tval;
  fd_set cset;
  FD_ZERO(&cset);
  FD_SET(this->s, &cset);
  tval.tv_sec = 3;
  tval.tv_usec = 0;
  int n = select(FD_SETSIZE, NULL, &cset, NULL, &tval);
  if(n <= 0) {
    printf("Error during connection\n");
    // TODO
    throw 0;
  }
  fcntl(this->s, F_SETFL, old_fl);
}

int Socket::_sendn(std::string str) {
  size_t nbytes = str.length();
  size_t nleft;
  ssize_t nwritten;
  char buff[nbytes+1];
  memcpy(buff, str.c_str(), nbytes+1);
  char *ptr = buff;
  for(nleft=nbytes; nleft>0;) {
  	struct timeval tval;
  	fd_set cset;
  	FD_ZERO(&cset);
  	FD_SET(this->s, &cset);
  	tval.tv_sec = 0;
  	tval.tv_usec = 100;
  	int n = select(FD_SETSIZE, NULL, &cset, NULL, &tval);
  	if(n<0) {
      printf("Error during Select\n");
      exit(1);
  	} else if(n==0) {
      printf("I can't send a message for 100 microseconds\n");
      exit(1);
  	}
    nwritten = send(this->s, ptr, nleft, 0);
    if(nwritten <= 0) {
      // ERROR
      return (int)nwritten;
    } else {
      nleft -= nwritten;
      ptr += nwritten;
    }
  }
  return (int) (nbytes-nleft);
}
