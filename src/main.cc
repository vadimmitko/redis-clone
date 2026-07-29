#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

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

  sockaddr_in client_addr{};
  int client_addr_len = sizeof(client_addr);
  std::cout << "Waiting for the client to connect...\n";

  int client_fd = accept(server_fd, (struct sockaddr*) &client_addr, (socklen_t*) &client_addr_len);

  if (client_fd < 0) {
    perror("accept");
    return 1;
  }

  std::cout << "Client connected\n";

  char buffer[1024];

  while (true) {
    ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer), 0);

    if (bytes_read <= 0) {
      if (bytes_read < 0) perror("recv");
      break;
    }

    std::cout << std::string(buffer, bytes_read) << '\n';

    const char* msg = "+PONG\r\n";

    send(client_fd, msg, strlen(msg), 0);
  }

  close(client_fd);
  close(server_fd);

  return 0;
}
