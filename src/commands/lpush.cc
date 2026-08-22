#include "commands/lpush.h"
#include "db.h"
#include "resp.h"
#include <functional>
#include <variant>
#include <vector>

RespValue cmd_lpush(std::vector<std::string> &&args, RedisDb &db) {
  if (args.size() < 2) return RespError { "wrong number of arguments" };

  std::optional<std::reference_wrapper<RedisObject>> l_o = db.get_mutable(args[0]);

  size_t n = args.size() - 1;

  if (!l_o.has_value()) {
    std::deque<std::string> l(n);

    for (size_t i = 0; i < n; i++) l[n-i-1] = std::move(args[i+1]);

    db.set_value(std::move(args[0]), std::move(l));
    
    return Integer { static_cast<int64_t>(n) };
  }

  auto* list = std::get_if<std::deque<std::string>>(&l_o.value().get());
  if (list == nullptr) return RespError{ "WRONGTYPE Operation against a key holding the wrong kind of value" };

  for (size_t i = 0; i < n; i++) list->push_front(std::move(args[i+1]));

  return Integer{ static_cast<int64_t>(list->size()) };
}

