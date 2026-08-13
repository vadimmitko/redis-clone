#include "commands.h"

RespValue cmd_echo(std::vector<std::string>&& args, RedisDb& db) {
  if (args.size() != 1) {
    return RespError{ "wrong number of arguments" };
  }

  return BulkString { std::move(args[0]) };
};
