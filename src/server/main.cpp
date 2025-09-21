#include "MarketDataServer.hpp"
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

// Collects all CSV file paths inside a given directory
std::vector<std::string> collect_files_from_directory(const std::string &directory, const std::string &file_extension) {
    std::vector<std::string> files;

    for (const auto& entry : fs::directory_iterator(directory)) {
      if (entry.is_regular_file() && entry.path().extension() == file_extension) {
        files.push_back(entry.path().string());
      }
    }

    return files;
}

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

    std::string csv_dir = CSV_DATA_DIR;
    
    auto files = collect_files_from_directory(csv_dir, ".csv");
    std::cout << "Found " << files.size() << " CSV files:\n";
    for (const auto& f : files) {
        std::cout << "  " << f << "\n";
        service.load_data(f);
    }
        

    if (custom_portal) {
        std::cout << "MarketData server listening on " << server_address << std::endl;
    } else {
        std::cout << "MarketData server listening on " << *selected_port << std::endl;
    }

    server->Wait();
    return 0;
}