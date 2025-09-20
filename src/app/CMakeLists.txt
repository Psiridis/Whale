# ---------------------------------------------------------
# src/app/CMakeLists.txt
# ---------------------------------------------------------
# This builds the main application executable.
# The app is responsible for spawning multiple gRPC clients
# and measuring latency for market data updates.
#
# Cross-platform: Works on Windows (vcpkg) and Ubuntu 22.04 (apt).
# ---------------------------------------------------------

# Define main application target
add_executable(hft_app)

# Add application sources
target_sources(hft_app
    PRIVATE
        main.cpp
)

# Add include paths
#   ${CMAKE_CURRENT_SOURCE_DIR} -> app-local headers
# NOTE: marketdata_proto already exports generated + Protobuf includes,
#       so we don’t need to duplicate them here.
target_include_directories(hft_app
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link dependencies
# ---------------------------------------------------------
# - marketdata_proto: library with generated protobuf/gRPC code
# - gRPC::grpc++    : gRPC runtime
# - Protobuf        : protocol buffers runtime
#
# Handle Protobuf differently depending on detection method:
#   * CONFIG mode -> Windows/vcpkg
#   * MODULE mode -> Linux/apt
if(TARGET protobuf::libprotobuf)
    target_link_libraries(hft_app
        PRIVATE
            client::lib
            marketdata_proto
            gRPC::grpc++
            protobuf::libprotobuf
    )
else()
    target_link_libraries(hft_app
        PRIVATE
            client::lib
            marketdata_proto
            gRPC::grpc++
            ${Protobuf_LIBRARIES}
    )
endif()

# (Optional) Alias target for clarity
add_executable(app::bin ALIAS hft_app)