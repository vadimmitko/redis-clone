cmake_minimum_required(VERSION 3.23)
project(redis_clone)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(redis_clone)

target_sources(redis_clone
	PRIVATE
	  src/main.cc
	  src/concurrency/thread_pool.cc
)

target_compile_features(redis_clone PRIVATE cxx_std_20)

find_package(Threads REQUIRED)
target_link_libraries(redis_clone PRIVATE Threads::Threads)

target_include_directories(redis_clone PRIVATE include)
