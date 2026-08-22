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
#include "commands.h"
#include "resp.h"

int main() {
  const uint16_t redisPort = 6379;
  std::optional<int> server_fd_o = create_listening_socket(redisPort);
  if (!server_fd_o.has_value()) {
    std::cerr << "failed to start server\n";
    return 1;
  }
  RedisDb db;
  SocketServer server(server_fd_o.value());

  // epoll_event: the struct epoll uses both to register interest (epoll_ctl)
  // and to report which fds are ready (epoll_wait fills the `events` array).
  struct epoll_event ev, events[1024];

  // epoll_create1(0): creates a new epoll instance, returns an fd
  // (EPOLL_CLOEXEC is the only real flag; 0 = no special behavior).
  int epoll_fd = epoll_create1(0);
  if (epoll_fd < 0) {
    perror("epoll_create1"); 
    return 1;
  }

  // EPOLLIN: "notify me when this fd has data ready to read"
  // (for the listening socket, that means "a new connection is ready to accept").
  ev.events = EPOLLIN;
  ev.data.fd = server.fd(); // data is a union; using .fd lets us know which socket fired later

  // epoll_ctl: register/modify/remove an fd from the epoll set.
  // EPOLL_CTL_ADD = start watching this fd with the given ev.events.
  // (other ops: EPOLL_CTL_MOD = change flags, EPOLL_CTL_DEL = stop watching)
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server.fd(), &ev) == -1) {
    perror("epoll_ctl");
    return 1;
  }

  while (true) {
    // epoll_wait: blocks (timeout -1 = wait forever) until at least one
    // watched fd is ready, or an event happens. Returns how many fds fired,
    // and fills `events[0..nfds)` with which ones and why.
    int nfds = epoll_wait(epoll_fd, events, 1024, -1);
    if (nfds == -1) {
      perror("epoll_wait");
      return 1;
    }

    for (int i = 0; i < nfds; i++) {
      if (events[i].data.fd == server.fd()) {
        // The listening socket became readable => a client is waiting to connect.
        sockaddr_in client_addr{};
        int client_addr_len = sizeof(client_addr);

        // accept(): pulls the next pending connection off the listen queue,
        // returns a NEW fd dedicated to that client (server.fd() keeps listening).
        // client_addr gets filled with the client's IP/port.
        int client_fd = accept(server.fd(), (struct sockaddr*) &client_addr, (socklen_t*) &client_addr_len);
        if (client_fd < 0) {
          perror("accept");
          continue;
        }

        // fcntl(F_GETFL): read the fd's current file status flags (e.g. O_APPEND, O_NONBLOCK, ...).
        int flags = fcntl(client_fd, F_GETFL, 0);
        // fcntl(F_SETFL): set new flags. We OR in O_NONBLOCK so reads/writes on this
        // socket never block the whole event loop; they return EAGAIN/EWOULDBLOCK
        // instead of blocking if no data is ready. Required for epoll-based servers.
        fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

        // Register the new client fd with epoll so we get notified when IT has data too.
        ev.events = EPOLLIN;
        ev.data.fd = client_fd;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
         perror("epoll_ctl: cliend_fd");
         close(client_fd);
         continue;
        }

        server.in_use[client_fd] = true;
      } else {
        // An already-connected client fd became readable => it sent us data (or closed).
        int client_fd = events[i].data.fd;
        if (!server.in_use[client_fd]) continue; // stale event for an fd we already closed

        Connection& conn = server.connections[client_fd];
        char* read_buffer = conn.read_buffer;
        size_t bufferlen = sizeof(conn.read_buffer);

        // recv(): read available bytes from the socket into our buffer.
        // Return value: >0 = number of bytes read, 0 = peer closed connection
        // gracefully (EOF), -1 = error (check errno).
        ssize_t read_bytes = recv(client_fd, read_buffer, bufferlen, 0);
        if (read_bytes <= 0) {
          if (read_bytes == -1) perror("recv");
          // EPOLL_CTL_DEL: stop watching this fd (must do before close(), since a
          // closed fd number could be reused and confuse epoll's internal bookkeeping).
          epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
          server.in_use[client_fd] = false;
          close(client_fd);
          continue;
        }

        std::optional<ParsedCommand> parsed_o = parse_command(read_buffer);
        if (!parsed_o.has_value()) {
          std::cerr << "failed to parse command\n";
          epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
          server.in_use[client_fd] = false;
          close(client_fd);

          continue;
        }

        std::string cmd = std::move(parsed_o->name);
        std::vector<std::string> args = std::move(parsed_o->args);
        std::optional<CommandFn> cmd_o = get_command(cmd);
        RespValue result = cmd_o.has_value()
        ? cmd_o.value()(std::move(args), db)
        : RespValue{RespError{"unknown command '" + cmd + "'"}};
        std::string wire = serialize(result);

        // send(): write bytes out to the socket. Like recv, can return fewer
        // bytes than requested (partial write) on a non-blocking socket if the
        // LIMITATION: send buffer could fill up
        // we only check for -1, not sent_bytes < wire.size()).
        ssize_t sent_bytes = send(client_fd, wire.data(), wire.size(), 0);
        if (sent_bytes == -1) {
          perror("send");
          epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
          server.in_use[client_fd] = false;
          close(client_fd);

          continue;
        }
      }
    }
  }
  return 0;
}
