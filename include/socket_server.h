#ifndef REDIS_CLONE_SOCKET_SERVER_H_
#define REDIS_CLONE_SOCKET_SERVER_H_

#include <cstdint>
#include <array>
#include <unistd.h>
#include <bitset>
#include <optional>
#include <functional>

#include "connection.h"
#include "commands.h"

using CommandFn = std::function<RespValue(std::vector<std::string>&&)>;

std::optional<int> create_listening_socket(uint16_t port);

class SocketServer {
  public:
    explicit SocketServer(int fd);
    ~SocketServer();

    SocketServer(const SocketServer&) = delete;
    SocketServer& operator=(const SocketServer&) = delete;

    int fd() const { return server_fd_; }

    std::optional<CommandFn> get_command_fn(const std::string& name) const;

    std::array<Connection, 1024> connections;
    std::bitset<1024> in_use;   

  private:
    int server_fd_;
    inline static const std::unordered_map<std::string, CommandFn> commands_ = {
      {"PING", cmd_ping},
      {"ECHO", cmd_echo},
    };
};
#endif
