cmake_minimum_required(VERSION 3.23)

project(redis_clone)

add_executable(redis_clone)

target_sources(redis_clone
	PRIVATE
	  src/main.cpp
)

target_compile_features(redis_clone PRIVATE cxx_std_17)

find_package(Threads REQUIRED)
target_link_libraries(redis_clone PRIVATE Threads::Threads)
