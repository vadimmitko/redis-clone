#include <iostream>
#include <optional>
#include <charconv>
#include <vector>

#include "commands/set.h"
#include "util/clock.h"

struct ParsedArgs {
 std::array<std::string, 2> args;
 std::optional<long long> expiry;
};


static std::optional<ParsedArgs> parse_set_args(std::vector<std::string>&&args) {
  if (args.size() != 2 && args.size() != 4) return std::nullopt;

  ParsedArgs result {};

  result.args[0] = std::move(args[0]);
  result.args[1] = std::move(args[1]);

  if (args.size() == 4) {
    if (isValidSetOption(args[2])) {
      int64_t value;
      auto [ptr, ec] = std::from_chars(args[3].data(), args[3].data() + args[3].size(), value);
      if (ec != std::errc() || ptr != args[3].data() + args[3].size()) {
        return std::nullopt;
      }

      if (value < 0 || value > kMaxExpireSeconds) return std::nullopt;

      result.expiry = value;
    }
  }

  return result;
};

RespValue cmd_set(std::vector<std::string> &&args, RedisDb &db) {
  std::optional<ParsedArgs> parsed_o = parse_set_args(std::move(args));

  if (!parsed_o.has_value()) return RespError { "wrong command usage" };

  db.set_value(parsed_o->args[0], parsed_o->args[1]);

  if (parsed_o->expiry.has_value()) {
    db.set_value_expiry(parsed_o->args[0], parsed_o->expiry.value() + now_ms());
  }

  return SimpleString { "OK" };

};
