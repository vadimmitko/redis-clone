#include "commands.h"

RespValue cmd_ping(std::vector<std::string>&& args) {
  if (args.empty()) return SimpleString { "PONG" };

  if (args.size() == 1) return BulkString { std::move(args[0]) };

  return RespError { "wrong number of arguments" };
};

RespValue cmd_echo(std::vector<std::string>&& args) {
  if (args.size() != 1) {
    return RespError{ "wrong number of arguments" };
  }

  return BulkString { std::move(args[0]) };
};
