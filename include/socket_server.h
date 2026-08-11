#ifndef REDIS_CLONE_SOCKET_SERVER_H_
#define REDIS_CLONE_SOCKET_SERVER_H_

#include <cstdint>
#include <array>
#include <unistd.h>
#include <bitset>
#include <optional>

#include "connection.h"


std::optional<int> create_listening_socket(uint16_t port);

class SocketServer {
  public:
    explicit SocketServer(int fd);
    ~SocketServer();

    SocketServer(const SocketServer&) = delete;
    SocketServer& operator=(const SocketServer&) = delete;

    int fd() const { return server_fd_; }

    std::array<Connection, 1024> connections;
    std::bitset<1024> in_use;   

  private:
    int server_fd_;
};
#endif
