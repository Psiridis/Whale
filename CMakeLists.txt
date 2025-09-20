cmake_minimum_required(VERSION 3.20)

project(CROSS_OPTIONS LANGUAGES CXX)

# ---------------------------------------------------------
# Global settings
# ---------------------------------------------------------
# Enforce C++20 everywhere
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Required for gRPC/protobuf generated code to be linked safely
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ---------------------------------------------------------
# Dependencies
# ---------------------------------------------------------

# --- Protobuf ---
# Try to locate Protobuf using CONFIG mode first
#   -> Works on Windows with vcpkg or custom builds (modern style)
find_package(Protobuf CONFIG QUIET)

# If CONFIG mode not found, fall back to MODULE mode
#   -> Works on Linux systems with apt packages (legacy FindProtobuf.cmake)
if(NOT Protobuf_FOUND)
    find_package(Protobuf REQUIRED)
endif()

# --- gRPC ---
# gRPC always ships with CMake config files, so CONFIG mode is guaranteed
find_package(gRPC CONFIG REQUIRED)

# Print out some status messages to help debug environment differences
message(STATUS "Using protobuf ${Protobuf_VERSION}")
message(STATUS "Using gRPC ${gRPC_VERSION}")
message(STATUS "Protobuf compiler: ${Protobuf_PROTOC_EXECUTABLE}")

# ---------------------------------------------------------
# Proto compilation
# ---------------------------------------------------------
# Define input proto file and output directory for generated code
set(PROTO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/proto/marketdata.proto)
set(GENERATED_DIR ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_DIR})

# Expected generated source and header files
set(GENERATED_SRC
    ${GENERATED_DIR}/marketdata.pb.cc
    ${GENERATED_DIR}/marketdata.grpc.pb.cc
)
set(GENERATED_HDR
    ${GENERATED_DIR}/marketdata.pb.h
    ${GENERATED_DIR}/marketdata.grpc.pb.h
)

# Custom command: run protoc + gRPC plugin to generate C++ code
add_custom_command(
    OUTPUT ${GENERATED_SRC} ${GENERATED_HDR}
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
        --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/proto
        --cpp_out=${GENERATED_DIR}
        --grpc_out=${GENERATED_DIR}
        --plugin=protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>
        ${PROTO_FILE}
    DEPENDS ${PROTO_FILE}                 # Regenerate if proto changes
    COMMENT "Generating C++ sources from ${PROTO_FILE}"
    VERBATIM
)

# Define a custom target to group generated sources logically
# Adding ALL ensures this always runs when building the project
add_custom_target(generate_proto ALL DEPENDS ${GENERATED_SRC} ${GENERATED_HDR})

# ---------------------------------------------------------
# Proto library
# ---------------------------------------------------------
# Wrap generated sources into a library that server/client can link against
add_library(marketdata_proto ${GENERATED_SRC})

# Ensure generated sources are created before compiling this library
add_dependencies(marketdata_proto generate_proto)

# Link against gRPC and Protobuf depending on how they were found
if(TARGET protobuf::libprotobuf) # CONFIG mode (modern)
    target_link_libraries(marketdata_proto PUBLIC gRPC::grpc++ protobuf::libprotobuf)
else() # MODULE mode (legacy FindProtobuf.cmake on Linux)
    target_link_libraries(marketdata_proto PUBLIC gRPC::grpc++ ${Protobuf_LIBRARIES})
endif()

# Always include generated dir, and Protobuf include dirs if available
#   -> On Linux, ${Protobuf_INCLUDE_DIRS} is required
#   -> On Windows/vcpkg, it may be empty but harmless
target_include_directories(marketdata_proto PUBLIC
    ${GENERATED_DIR}
    ${Protobuf_INCLUDE_DIRS}
)

# ---------------------------------------------------------
# Subdirectories
# ---------------------------------------------------------
# Add modular targets:
#   - src/server : gRPC server implementation
#   - src/client : gRPC client implementation
#   - src/app    : main application (spawns multiple clients)
#   - test       : GoogleTest unit tests
add_subdirectory(src/server)
add_subdirectory(src/client)
add_subdirectory(src/app)
add_subdirectory(test)