#include <optional>

#include "commands/get.h"

RespValue cmd_get(std::vector<std::string> &&args, RedisDb &db) {
  if (args.size() != 1) {
    return RespError {  "wrong number of arguments" };
  }

  std::optional<RedisObject> value_o = db.get_value_by_key(args[0]);

  if (!value_o.has_value()) return Nil{};

  auto* val = std::get_if<std::string>(&value_o.value());

  if (val == nullptr) return RespError { "WRONGTYPE Operation against a key holding the wrong kind of value" };

  return BulkString { std::move(*val) };
};
