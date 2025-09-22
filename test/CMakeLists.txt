# ---------------------------------------------------------
# test/CMakeLists.txt
# ---------------------------------------------------------
# Enables GoogleTest-based unit testing.
# Uses FetchContent so it works on both Windows (vcpkg) and Ubuntu 22.04.
# ---------------------------------------------------------

# Enable testing with CTest
enable_testing()

# Download & configure GoogleTest (v1.15.0 stable release)
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.0.zip
)
# Prevent GoogleTest from overriding compiler/linker flags (Windows specific)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Define test executable
add_executable(unit_tests)

# Add test sources
target_sources(unit_tests
    PRIVATE
        test_latency.cpp
        test_client.cpp
        test_server.cpp
        ${CMAKE_SOURCE_DIR}/src/server/MarketDataServer.cpp
)

# Add include paths (so tests can see app/client/server headers if needed)
target_include_directories(unit_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/client
        ${CMAKE_SOURCE_DIR}/src/server
)

# Link dependencies
# ---------------------------------------------------------
# - marketdata_client : our new client library
# - marketdata_proto: protobuf/gRPC generated code
# - gRPC::grpc++    : gRPC runtime
# - Protobuf        : protocol buffers runtime
# - GTest::gtest_main : GoogleTest main function
#
# Hybrid linking: CONFIG mode for Windows/vcpkg,
# fallback to MODULE mode for Linux/apt.
if(TARGET protobuf::libprotobuf)
    target_link_libraries(unit_tests
        PRIVATE
            client::lib
            marketdata_proto
            gRPC::grpc++
            protobuf::libprotobuf
            GTest::gtest_main
    )
else()
    target_link_libraries(unit_tests
        PRIVATE
            client::lib
            marketdata_proto
            gRPC::grpc++
            ${Protobuf_LIBRARIES}
            GTest::gtest_main
    )
endif()

# Register with CTest
include(GoogleTest)
gtest_discover_tests(unit_tests)

target_compile_definitions(unit_tests
    PRIVATE
    TESTING_CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)