#ifndef REDIS_CLONE_RESP_H_
#define REDIS_CLONE_RESP_H_

#include <vector>
#include <string>
#include <optional>

#include "commands.h"

constexpr size_t kMaxMultibulkLen = 1024 * 1024;
constexpr size_t kMaxBulkLen = 512 * 1024 * 1024;

struct ParsedCommand {
  std::string name;
  std::vector<std::string> args;
};

std::optional<ParsedCommand> parse_command(std::string_view buffer);

std::string serialize(const RespValue& v);

#endif
