#include <optional>

#include "commands.h"
#include "db.h"

RespValue cmd_ping(std::vector<std::string>&& args, RedisDb& db) {
  if (args.empty()) return SimpleString { "PONG" };

  if (args.size() == 1) return BulkString { std::move(args[0]) };

  return RespError { "wrong number of arguments" };
};

RespValue cmd_echo(std::vector<std::string>&& args, RedisDb& db) {
  if (args.size() != 1) {
    return RespError{ "wrong number of arguments" };
  }

  return BulkString { std::move(args[0]) };
};

RespValue cmd_get(std::vector<std::string> &&args, RedisDb &db) {
  if (args.size() != 1) {
    return RespError {  "wrong number of arguments" };
  }

  std::optional<std::string> value_o = db.get_value_by_key(args[0]);

  if (value_o->empty()) return Nil{};

  return BulkString { std::move(value_o.value()) };
};

RespValue cmd_set(std::vector<std::string> &&args, RedisDb &db) {
  if (args.size() != 2) {
    return RespError { "wrong number of arguments" };
  }

  db.set_value(args[0], args[1]);

  return SimpleString { "OK" };
};
