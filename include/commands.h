#ifndef REDIS_CLONE_COMMANDS_H_
#define REDIS_CLONE_COMMANDS_H_

#include <array>
#include <string>
#include <vector>
#include <optional>

#include "db.h"
#include "commands/ping.h"
#include "commands/echo.h"
#include "commands/set.h"
#include "commands/get.h"
#include "commands/rpush.h"

using CommandFn = RespValue (*)(std::vector<std::string>&&, RedisDb&);

struct CommandEntry {
  std::string_view name;
  CommandFn fn;
};

inline constexpr std::array<CommandEntry, 5> kCommands = {{
  {"PING", cmd_ping},
  {"ECHO", cmd_echo},
  {"GET", cmd_get},
  {"SET", cmd_set},
  {"RPUSH", cmd_rpush},
}};

constexpr std::optional<CommandFn> get_command(std::string_view name) {
  for (auto& cmd : kCommands) {
    if (cmd.name == name) return std::optional(cmd.fn);
  }
  return std::nullopt;
};
#endif
