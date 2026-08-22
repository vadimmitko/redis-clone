
#include "commands/llen.h"
#include "db.h"
#include <cstdint>
#include <functional>

RespValue cmd_llen(std::vector<std::string> &&args, RedisDb &db) {
  if (args.size() != 1) return RespError { "wrong number of arguments" };

  std::optional<std::reference_wrapper<const RedisObject>> l_o = db.get_value_by_key(args[0]);

  if (!l_o.has_value()) return Integer{ static_cast<int64_t>(0) };

  auto* list = std::get_if<std::deque<std::string>>(&l_o.value().get());
  if (list == nullptr) return RespError{ "WRONGTYPE Operation against a key holding the wrong kind of value" };

  return Integer( static_cast<int64_t>(list->size()));
}
