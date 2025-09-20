#include "MarketDataServer.hpp"
#include <grpcpp/grpcpp.h>
#include <iostream>

int main(int argc, char** argv) {

    std::string server_address("0.0.0.0:0"); //default address.
    bool custom_portal = false;

    if (argc > 1) {
        server_address = argv[1];
        custom_portal = false;
    }

    MarketDataServiceImpl service;
    std::unique_ptr<int> selected_port = std::make_unique<int>();

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials(), selected_port.get());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());

    if (!server) {
        std::cerr << "Failed to start server." << std::endl;
    }

    if (custom_portal) {
        std::cout << "MarketData server listening on " << server_address << std::endl;
    } else {
        std::cout << "MarketData server listening on " << *selected_port << std::endl;
    }

    server->Wait();
    return 0;
}