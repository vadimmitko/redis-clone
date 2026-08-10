cmake_minimum_required(VERSION 3.23)
project(redis_clone)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

# --- main executable ---
add_executable(redis_clone)
target_sources(redis_clone
  PRIVATE
    src/main.cc
    src/socket_server.cc
    src/resp.cc
    src/commands.cc
    src/concurrency/thread_pool.cc
)
target_compile_features(redis_clone PRIVATE cxx_std_20)
target_include_directories(redis_clone PRIVATE include)
target_link_libraries(redis_clone PRIVATE Threads::Threads)

# --- googletest setup ---
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# On Windows, this prevents overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# --- test executable ---
enable_testing()

add_executable(redis_clone_tests
  test/concurrency/thread_pool_test.cc
  src/concurrency/thread_pool.cc
  test/resp_test.cc 
  src/resp.cc
)
target_compile_features(redis_clone_tests PRIVATE cxx_std_20)
target_include_directories(redis_clone_tests PRIVATE include)
target_link_libraries(redis_clone_tests
  PRIVATE
    GTest::gtest_main
    Threads::Threads
)

include(GoogleTest)
gtest_discover_tests(redis_clone_tests)
