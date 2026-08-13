#ifndef REDIS_CLONE_COMMANDS_ECHO_H_
#define REDIS_CLONE_COMMANDS_ECHO_H_

#include <vector>
#include <string>

#include "../resp.h"
#include "../db.h"

RespValue cmd_echo(std::vector<std::string>&& args, RedisDb& db);

#endif
