#include "db.h"

std::optional<std::string> RedisDb::get_value_by_key(const std::string& key) const {
  auto it = data_.find(key);
  if (it == data_.end()) return std::nullopt;
  return it->second;
}

void RedisDb::set_value(std::string key, std::string value) {
  data_.insert_or_assign(std::move(key), std::move(value));
}
