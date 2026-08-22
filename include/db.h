#ifndef REDIS_CLONE_DB_H_
#define REDIS_CLONE_DB_H_

#include <string>
#include <unordered_map>
#include <deque>
#include <optional>
#include <variant>

using RedisObject = std::variant<std::string, std::deque<std::string>>;

class RedisDb {
  public:
    RedisDb() = default;
    
    RedisDb(const RedisDb&) = delete;
    RedisDb& operator=(const RedisDb&) = delete;

    std::optional<std::reference_wrapper<const RedisObject>> get_value_by_key(const std::string& key);
    std::optional<std::reference_wrapper<RedisObject>> get_mutable(const std::string&key);
    void set_value(std::string key, RedisObject value);
    void set_value_expiry(std::string key, int64_t timestamp);

  private:
    std::unordered_map<std::string, RedisObject> data_;
    std::unordered_map<std::string, int64_t> expires_;
};

#endif
