#include <cstddef>
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <optional>
#include <sys/epoll.h>
#include <unistd.h>

#include "connection.h"
#include "socket_server.h"


int main() {
  const uint16_t redisPort = 6379;

  std::optional<int> server_fd_o = create_listening_socket(redisPort);

  if (!server_fd_o.has_value()) {
    std::cerr << "failed to start server\n";
    return 1;
  }

  SocketServer server(server_fd_o.value());

  // get new file descriptor for epoll 
  // configure to your socket and port
  // while (true) use epoll_wait
  // on each fd see what is ready (read/write)
  // for each recv, parse input (not necessary rn) and write back send(Pong)
  // keep track of each fd buffers (use a struct perhaps)

  struct epoll_event ev, events[1024];

  int epoll_fd = epoll_create1(0);
  if (epoll_fd < 0) {
    perror("epoll_create1");
    return 1;
  }

  ev.events = EPOLLIN;
  ev.data.fd = server.fd();
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server.fd(), &ev) == -1) {
    perror("epoll_ctl");
    return 1;
  }

  while (true) {

    int nfds = epoll_wait(epoll_fd, events, 1024, -1);
    if (nfds == -1) {
      perror("epoll_wait");
      return 1;
    }


    for (int i = 0; i < nfds; i++) {
      if (events[i].data.fd == server.fd()) {
        sockaddr_in client_addr{}; 
        int client_addr_len = sizeof(client_addr);

        int client_fd = accept(server.fd(), (struct sockaddr*) &client_addr, (socklen_t*) &client_addr_len);

        if (client_fd < 0) {
          perror("accept");
          continue;
        }

        int flags = fcntl(client_fd, F_GETFL, 0);
        fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

        ev.events = EPOLLIN;
        ev.data.fd = client_fd;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
         perror("epoll_ctl: cliend_fd");
         close(client_fd);
         continue;
        }
        
        server.in_use[client_fd] = true;

        std::cout << "Client connected\n";

      } else {
        int client_fd = events[i].data.fd;

        if (!server.in_use[client_fd]) continue;

        Connection& conn = server.connections[client_fd];

        char* read_buffer = conn.read_buffer;
        size_t bufferlen = sizeof(conn.read_buffer);

        ssize_t read_bytes = recv(client_fd, read_buffer, bufferlen, 0);

        if (read_bytes <= 0) {
          if (read_bytes == -1) perror("recv");

          epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);

          server.in_use[client_fd] = false;
          close(client_fd);

          continue;
        }

        ssize_t sent_bytes = send(client_fd, "+PONG\r\n", strlen("+PONG\r\n"), 0);
        if (sent_bytes == -1) {
          perror("send");
        }
      }
    }
  }

  return 0;
}
