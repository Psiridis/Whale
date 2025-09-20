# ---------------------------------------------------------
# src/server/CMakeLists.txt
# ---------------------------------------------------------
# This builds the gRPC server executable for the project.
# It is designed to work both on Windows (with vcpkg) and
# Linux (Ubuntu 22.04 with apt-installed dependencies).
# ---------------------------------------------------------

# Define the server executable target
add_executable(marketdata_server)

# Add server-specific source files
# Keeping them in target_sources() makes it easy to extend later
target_sources(marketdata_server
    PRIVATE
        main.cpp
        MarketDataServer.cpp
)

# Add include paths for local headers
#   ${CMAKE_CURRENT_SOURCE_DIR} -> for headers inside src/server/
# NOTE: We don’t need to add generated protobuf/grpc headers here,
#       because the marketdata_proto library already exposes them.
target_include_directories(marketdata_server
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link dependencies
# ---------------------------------------------------------
# - marketdata_proto: our library containing generated protobuf/gRPC sources
# - gRPC::grpc++    : gRPC C++ runtime
# - Protobuf        : protocol buffers runtime
#
# Protobuf linking is handled differently depending on how it’s found:
#   * CONFIG mode -> Windows / vcpkg style (protobuf::libprotobuf target exists)
#   * MODULE mode -> Linux / FindProtobuf.cmake (use ${Protobuf_LIBRARIES})
#
# This ensures the code builds cleanly on both platforms.
if(TARGET protobuf::libprotobuf)
    target_link_libraries(marketdata_server
        PRIVATE
            marketdata_proto
            gRPC::grpc++
            protobuf::libprotobuf
    )
else()
    target_link_libraries(marketdata_server
        PRIVATE
            marketdata_proto
            gRPC::grpc++
            ${Protobuf_LIBRARIES}
    )
endif()

# (Optional) Create an alias target for clarity in larger projects
# Usage example: other targets could depend on server::bin instead of marketdata_server
add_executable(server::bin ALIAS marketdata_server)