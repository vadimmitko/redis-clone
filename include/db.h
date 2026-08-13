#ifndef REDIS_CLONE_DB_H_
#define REDIS_CLONE_DB_H_

#include <string>
#include <unordered_map>
#include <optional>

class RedisDb {
  public:
    RedisDb() = default;
    
    RedisDb(const RedisDb&) = delete;
    RedisDb& operator=(const RedisDb&) = delete;

    std::optional<std::string> get_value_by_key(const std::string& key);
    void set_value(std::string key, std::string value);
    void set_value_expiry(std::string key, int64_t timestamp);

  private:
    std::unordered_map<std::string, std::string> data_;
    std::unordered_map<std::string, int64_t> expires_;
};

#endif
