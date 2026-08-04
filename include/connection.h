#ifndef REDIS_CLONE_CONNECTION_H_
#define REDIS_CLONE_CONNECTION_H_

struct Connection {
  char read_buffer[1024];
  char write_buffer[1024];
};

#endif
