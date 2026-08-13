#include "commands.h"

RespValue cmd_ping(std::vector<std::string>&& args, RedisDb& db) {
  if (args.empty()) return SimpleString { "PONG" };

  if (args.size() == 1) return BulkString { std::move(args[0]) };

  return RespError { "wrong number of arguments" };
};
