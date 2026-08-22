#include "db.h"
#include "util/clock.h"
#include <functional>

std::optional<std::reference_wrapper<const RedisObject>> RedisDb::get_value_by_key(const std::string& key) {
  auto it = expires_.find(key);
  if (it != expires_.end() && it->second <= now_ms()) {
    expires_.erase(it);
    data_.erase(key);
    return std::nullopt;
  }
  auto data_it = data_.find(key);
  if (data_it == data_.end()) return std::nullopt;
  return data_it->second;
}

std::optional<std::reference_wrapper<RedisObject>> RedisDb::get_mutable(const std::string& key) {
  auto it = expires_.find(key);
  if (it != expires_.end() && it->second <= now_ms()) {
    expires_.erase(it);
    data_.erase(key);
    return std::nullopt;
  }

  auto data_it = data_.find(key);
  if (data_it == data_.end()) return std::nullopt;;
  
  return data_it->second;
}

void RedisDb::set_value(std::string key, RedisObject value) {
  data_.insert_or_assign(std::move(key), std::move(value));
}

void RedisDb::set_value_expiry(std::string key, int64_t timestamp) {
  expires_.insert_or_assign(std::move(key), std::move(timestamp));
}
