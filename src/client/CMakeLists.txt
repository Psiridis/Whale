# ---------------------------------------------------------
# src/client/CMakeLists.txt
# ---------------------------------------------------------
# This builds the MarketDataClient library (no executable).
# It can be linked by tests and by the hft_app.
# ---------------------------------------------------------

# Build client as a static library
add_library(marketdata_client STATIC
    MarketDataClient.cpp
)

# Add include paths for client headers
#   PUBLIC -> so that anything linking this library can also include client headers
target_include_directories(marketdata_client
    PUBLIC
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
    target_link_libraries(marketdata_client
        PUBLIC
            marketdata_proto
            gRPC::grpc++
            protobuf::libprotobuf
    )
else()
    target_link_libraries(marketdata_client
        PUBLIC
            marketdata_proto
            gRPC::grpc++
            ${Protobuf_LIBRARIES}
    )
endif()

# (Optional) Create an alias target for nicer usage
add_library(client::lib ALIAS marketdata_client)
