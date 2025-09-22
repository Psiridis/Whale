cmake_minimum_required(VERSION 3.20)
project(Whale LANGUAGES CXX)

# ---------------------------------------------------------
# Generator selection (cross-platform)
# ---------------------------------------------------------
if(NOT CMAKE_GENERATOR)
    # Linux / macOS
    if(UNIX AND NOT WIN32)
        find_program(NINJA_EXE NAMES ninja)
        if(NINJA_EXE)
            message(STATUS "Ninja found at ${NINJA_EXE}, using Ninja generator")
            set(CMAKE_GENERATOR "Ninja" CACHE INTERNAL "" FORCE)
        else()
            message(STATUS "Ninja not found, falling back to Unix Makefiles")
            set(CMAKE_GENERATOR "Unix Makefiles" CACHE INTERNAL "" FORCE)
        endif()

    # Windows
    elseif(WIN32)
        find_program(NINJA_EXE NAMES ninja)
        if(NINJA_EXE)
            message(STATUS "Ninja found at ${NINJA_EXE}, using Ninja generator")
            set(CMAKE_GENERATOR "Ninja" CACHE INTERNAL "" FORCE)
        else()
            message(STATUS "Ninja not found, falling back to Visual Studio generator")
            set(CMAKE_GENERATOR "Visual Studio 17 2022" CACHE INTERNAL "" FORCE)
            set(CMAKE_GENERATOR_PLATFORM "x64" CACHE INTERNAL "" FORCE)
        endif()
    endif()
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------
# Dependencies: Protobuf + gRPC (via vcpkg or fallback)
# ---------------------------------------------------------

# Protobuf
find_package(Protobuf CONFIG QUIET)
if(Protobuf_FOUND)
    message(STATUS "Using Protobuf from vcpkg (CONFIG mode)")
else()
    message(WARNING "Protobuf not found via vcpkg. Falling back to system package...")
    find_package(Protobuf REQUIRED)
endif()

# gRPC
find_package(gRPC CONFIG QUIET)
if(gRPC_FOUND)
    message(STATUS "Using gRPC from vcpkg (CONFIG mode)")
else()
    message(WARNING "gRPC not found via vcpkg. You may need to install it or build from source.")
endif()

# ---------------------------------------------------------
# Proto generation
# ---------------------------------------------------------
# Directory containing all .proto files
set(PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/proto)
set(PROTO_FILES 
    ${PROTO_DIR}/marketdata.proto
)

set(GENERATED_DIR ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_DIR})

set(GENERATED_SRC "")
set(GENERATED_HDR "")

foreach(PROTO_FILE ${PROTO_FILES})
    get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)

    list(APPEND GENERATED_SRC
        ${GENERATED_DIR}/${PROTO_NAME}.pb.cc
        ${GENERATED_DIR}/${PROTO_NAME}.grpc.pb.cc
    )
    list(APPEND GENERATED_HDR
        ${GENERATED_DIR}/${PROTO_NAME}.pb.h
        ${GENERATED_DIR}/${PROTO_NAME}.grpc.pb.h
    )

    add_custom_command(
        OUTPUT
            ${GENERATED_DIR}/${PROTO_NAME}.pb.cc
            ${GENERATED_DIR}/${PROTO_NAME}.pb.h
            ${GENERATED_DIR}/${PROTO_NAME}.grpc.pb.cc
            ${GENERATED_DIR}/${PROTO_NAME}.grpc.pb.h
        COMMAND ${Protobuf_PROTOC_EXECUTABLE}
            --proto_path=${PROTO_DIR}
            --cpp_out=${GENERATED_DIR}
            --grpc_out=${GENERATED_DIR}
            --plugin=protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>
            ${PROTO_FILE}
        DEPENDS ${PROTO_FILE}
        COMMENT "Generating sources from ${PROTO_FILE}"
        VERBATIM
    )
endforeach()

# Group into a single library
add_library(marketdata_proto ${GENERATED_SRC})
target_include_directories(marketdata_proto PUBLIC
    ${GENERATED_DIR}
    ${Protobuf_INCLUDE_DIRS}
)

# ---------------------------------------------------------
add_subdirectory(src/server)
add_subdirectory(src/client)
add_subdirectory(src/app)
add_subdirectory(test)