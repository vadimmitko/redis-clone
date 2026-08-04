#include <cstdint>
#include <netinet/in.h>
#include <optional>
#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>

#include "socket_server.h"

std::optional<int> create_listening_socket(uint16_t port) {
  // AF_INET: IPv4 protocol
  // SOCK_STREAM: TCP socket
  // File descriptor
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (server_fd < 0) {
    perror("socket");
    return std::nullopt;
  }

  // OS usually holds the port in a TIME_WAIT
  // after TCP server closes
  // SO_REUSEADDR allows to claim sockets that are functionally dead already
  // but are waiting out the safety timer
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) != 0) {
    perror("setsockopt");
    return std::nullopt;
  }

  // htons(): Converts port to network byte order
  // INADDR_ANY: Accepts connections on any IP
  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) != 0) {
    perror("bind");
    return std::nullopt;
  }

  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    perror("listen");
    return std::nullopt;
  }

  return server_fd;
}

SocketServer::~SocketServer() {
  for (int fd = 0; fd < 1024; fd++) {
    if (in_use[fd]) close(fd);
  }
  close(server_fd_);
}
