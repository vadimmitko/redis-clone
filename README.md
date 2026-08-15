# redis-clone

A Redis server implementation in C++, speaking the RESP protocol over TCP.

## Requirements

- Docker

## Build the image

```bash
docker build -t devbox .
```

## Start the dev container

```bash
docker rm -f devbox
docker run -d --name devbox \
  -v "$(pwd)":"$(pwd)" \
  -w "$(pwd)" \
  -p 6379:6379 \
  devbox tail -f /dev/null
```

## Build and test

```bash
docker exec -it devbox cmake --build build
docker exec -it -w "$(pwd)/build" devbox ctest --output-on-failure
```

## Run

```bash
docker exec -it devbox ./build/redis_clone
```

## Interacting with the server

The server speaks RESP (REdis Serialization Protocol), not plain text, so it expects messages wrapped in RESP framing rather than raw commands. `redis-cli` handles that wrapping for you.

Install `redis-cli`:

```bash
# macOS
brew install redis

# Debian/Ubuntu
sudo apt install redis-tools
```

Then send a command:

```bash
echo "PING" | redis-cli -p 6379
```

`redis-cli` reads the line, encodes it as a proper RESP request, and sends it to the server on port 6379, no manual framing needed.
