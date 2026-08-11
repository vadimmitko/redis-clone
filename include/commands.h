#ifndef REDIS_CLONE_COMMANDS_H_
#define REDIS_CLONE_COMMANDS_H_

#include "db.h"
#include <variant>
#include <array>
#include <string>
#include <vector>
#include <optional>

struct SimpleString { std::string value; };
struct BulkString    { std::string value; };
struct RespError     { std::string message; };
struct Nil {};

using RespValue = std::variant<SimpleString, BulkString, RespError, Nil>;
using CommandFn = RespValue (*)(std::vector<std::string>&&, RedisDb&);

struct CommandEntry {
  std::string_view name;
  CommandFn fn;
};

RespValue cmd_ping(std::vector<std::string>&& args, RedisDb& db);
RespValue cmd_echo(std::vector<std::string>&& args, RedisDb& db);
RespValue cmd_get(std::vector<std::string>&& args, RedisDb& db);
RespValue cmd_set(std::vector<std::string>&& args, RedisDb& db);

inline constexpr std::array<CommandEntry, 4> kCommands = {{
  {"PING", cmd_ping},
  {"ECHO", cmd_echo},
  {"GET", cmd_get},
  {"SET", cmd_set},
}};

constexpr std::optional<CommandFn> get_command(std::string_view name) {
  for (auto& cmd : kCommands) {
    if (cmd.name == name) return std::optional(cmd.fn);
  }
  return std::nullopt;
};
#endif
