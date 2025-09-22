## 📖 Introduction

This project implements the core loop of a **High-Frequency Trading (HFT) system**, focusing on **low-latency data handling**.  
The goal is to simulate how an algorithmic trading engine reacts to and processes market data with minimal delay.

The system is built around three main components:

1. **gRPC Server**  
   - Loads historical price data of 10 S&P500 stocks (5 years) from Yahoo Finance at startup.  
   - Streams stock price updates with nanosecond-precision timestamps.  
   - Sends data to clients with small randomized delays to simulate real-world latency.  

2. **gRPC Clients**  
   - Each client subscribes to a specific stock’s data stream.  
   - Continuously receives and processes market data in real time.  

3. **Main Application**  
   - Spawns 10 independent gRPC clients (one per stock).  
   - Runs all clients in parallel to process incoming data.  
   - Prints incoming stock prices to the console **with latency measurements** (current time minus message timestamp).

------------------------------------------------------------------------
# 🚀 Project Build Guide

This project uses **CMake** and **vcpkg** to manage dependencies (Protobuf + gRPC).
The setup is cross-platform: it works on both **Windows** and **Linux (WSL Ubuntu 22.04)** with the same workflow. 

------------------------------------------------------------------------
## 📦 Prerequisites

### Common
- CMake ≥ 3.20
- Git
-  [vcpkg](https://github.com/microsoft/vcpkg) package manager

------------------------------------------------------------------------
## 🔗 Dependencies

This project depends on:
- [Protobuf](https://github.com/protocolbuffers/protobuf) (via vcpkg)
- [gRPC](https://github.com/grpc/grpc) (via vcpkg)

Both are installed automatically through vcpkg using:
```bash
./vcpkg install protobuf grpc
```

------------------------------------------------------------------------
## 🔹 Setup vcpkg (in case it is not already installed)

Clone and bootstrap vcpkg (only once per system):
``` bash
cd  ~
git  clone  https://github.com/microsoft/vcpkg.git
cd  vcpkg
./bootstrap-vcpkg.sh  # On Linux
.\bootstrap-vcpkg.bat  # On Windows
```
------------------------------------------------------------------------
## 🔹 Configure and Build

This project provides **CMakePresets.json**, so you can configure and build with simple commands.
To check all the available presets:
```bash
cd path\to\project
cmake --list-presets
```

### 🖥️ Windows (MSVC + vcpkg)

``` powershell
cd path\to\project
cmake --preset=windows-vcpkg-debug
cmake --build --preset=windows-debug
```
### 🐧 Linux (WSL, GCC + vcpkg)

``` bash
cd  ~/Projects/MyProject
cmake  --preset=linux-vcpkg-gcc-debug
cmake  --build  --preset=linux-gcc-debug
```
------------------------------------------------------------------------
## 🔹 Project Structure

-  `CMakeLists.txt` → Handles cross-platform build, gRPC/Protobuf generation, executables
-  `CMakePresets.json` → Contains build presets for Windows and Linux (GCC & Clang)
-  `data/` → Fetched data used from the server
-  `proto/` → `.proto` definitions (compiled into C++ sources by protoc + gRPC plugin)
-  `src/` → Contains the application (`app/`), a client library (`client/`) and the server application (`server/`)
-  `test/` → Unit tests
------------------------------------------------------------------------
## 🚀 Summary

- Dependencies (Protobuf + gRPC) are managed with **vcpkg** on all platforms.
- Builds are driven by **CMake presets**, so you don't need to remember long command lines.
- Generators (Ninja, Makefiles, Visual Studio) are auto-detected by `CMakeLists.txt`.