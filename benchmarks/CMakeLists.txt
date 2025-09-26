# Benchmark-specific CMakeLists.txt
include(FetchContent)

# Fetch Google Benchmark (just like GTest)
FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.8.3
)
FetchContent_MakeAvailable(benchmark)

# Define your benchmark target
add_executable(thread_pool_bench 
    thread_pool_bench.cpp
)

# Make sure benchmarks can see your utilities
target_include_directories(thread_pool_bench 
    PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
)

find_package(Threads REQUIRED)
# Link Google Benchmark + pthread
target_link_libraries(thread_pool_bench 
    PRIVATE
    benchmark::benchmark 
    Threads::Threads
)