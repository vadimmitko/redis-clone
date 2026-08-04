#include <cstddef>
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <memory>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

struct Connection {
  int fd;
  char read_buffer[1024];
  char write_buffer[1024];
};

int main() {
  const uint16_t redisPort = 6379;

  // AF_INET: IPv4 protocol
  // SOCK_STREAM: TCP socket
  // File descriptor
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (server_fd < 0) {
    perror("socket");
    return 1;
  }

  // OS usually holds the port in a TIME_WAIT
  // after TCP server closes
  // SO_REUSEADDR allows to claim sockets that are functionally dead already
  // but are waiting out the safety timer
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) != 0) {
    perror("setsockopt");
    return 1;
  }

  // htons(): Converts port to network byte order
  // INADDR_ANY: Accepts connections on any IP
  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(redisPort);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) != 0) {
    perror("bind");
    return 1;
  }

  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    perror("listen");
    return 1;
  }
  // get new file descriptor for epoll 
  // configure to your socket and port
  // while (true) use epoll_wait
  // on each fd see what is ready (read/write)
  // for each recv, parse input (not necessary rn) and write back send(Pong)
  // keep track of each fd buffers (use a struct perhaps)

  std::unique_ptr<Connection> connections[1024];

  struct epoll_event ev, events[1024];

  int epoll_fd = epoll_create1(0);
  if (epoll_fd < 0) {
    perror("epoll_create1");
    return 1;
  }

  ev.events = EPOLLIN;
  ev.data.fd = server_fd;
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
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
      if (events[i].data.fd == server_fd) {
        sockaddr_in client_addr{}; 
        int client_addr_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr*) &client_addr, (socklen_t*) &client_addr_len);

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
        
        connections[client_fd] = std::make_unique<Connection>(Connection{client_fd, {}, {}});

        std::cout << "Client connected\n";

      } else {
        int client_fd = events[i].data.fd;
        Connection* conn = connections[client_fd].get();

        if (conn == nullptr) continue;

        char* read_buffer = conn->read_buffer;
        size_t bufferlen = sizeof(conn->read_buffer);

        ssize_t read_bytes = recv(client_fd, read_buffer, bufferlen, 0);

        if (read_bytes <= 0) {
          if (read_bytes == -1) perror("recv");

          epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
          close(client_fd);
          connections[client_fd] = nullptr;

          continue;
        }

        ssize_t sent_bytes = send(client_fd, "+PONG\r\n", strlen("+PONG\r\n"), 0);
        if (sent_bytes == -1) {
          perror("send");
        }
      }
    }
  }

  for (int i = 0; i < 1024; i++) {
    if (connections[i] == nullptr) continue;
    close(connections[i]->fd);
    connections[i] = nullptr;
  }

  close(server_fd);

  return 0;
}
