# redis-clone

`docker rm -f devbox
docker run -d --name devbox \
  -v "$(pwd)":"$(pwd)" \
  -w "$(pwd)" \
  -p 6379:6379 \
  devbox tail -f /dev/null`

`docker exec -it devbox cmake --build build
docker exec -it devbox ./build/redis_clone
docker exec -it -w "$(pwd)/build" devbox ctest --output-on-failure`
