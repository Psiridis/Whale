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
# On Linux
./bootstrap-vcpkg.sh
./vcpkg install protobuf grpc  
# On Windows
.\bootstrap-vcpkg.bat
.\vcpkg install protobuf grpc
```
------------------------------------------------------------------------
## 🔹 Configure and Build

This project provides **CMakePresets.json**, so you can configure and build with simple commands.
To check all the available presets:
```bash
cd path\to\project
cmake --list-presets
```

Currently the available presets are (this is what cmake --list-presets will print):

```
"windows-vcpkg-debug"       - Windows Debug (MSVC + vcpkg)
"windows-vcpkg-debug-all"   - Windows MSVC Debug (App + Tests + Benchmarks)
"windows-vcpkg-release"     - Windows Release (MSVC + vcpkg)
"linux-vcpkg-gcc-debug"     - Linux GCC Debug (vcpkg)
"linux-vcpkg-gcc-debug-all" - Linux GCC Debug (App + Tests + Benchmarks)
"linux-vcpkg-gcc-release"   - Linux GCC Release (vcpkg)
"linux-vcpkg-clang-debug"   - Linux Clang Debug (vcpkg)
"linux-vcpkg-clang-release" - Linux Clang Release (vcpkg)
```
All configurations need the vcpkg package manager. Once you have select the preset in order to build the project you just need to specify which preset to build. The name of the build  preset is the same as the one in the list above (e.g cmake --build --preset=windows-vcpkg-debug)

### 🖥️ Windows (MSVC + vcpkg)

``` powershell
cd path\to\project
cmake --preset=windows-vcpkg-debug
cmake --build --preset=windows-vcpkg-debug
```
### 🐧 Linux (WSL, GCC + vcpkg)

``` bash
cd  path/to/project
cmake  --preset=linux-vcpkg-gcc-debug
cmake  --build  --preset=linux-vcpkg-gcc-debug
```
------------------------------------------------------------------------
## ▶️ Running the Applications

Once the project has been built successfully (for example with):

```bash
cmake --preset=linux-vcpkg-gcc-debug
cmake --build --preset=linux-vcpkg-gcc-debug
```

you can run the server and client applications.

### 🖥️ Start the gRPC Server
Open a terminal in the project root and run:

```bash
./build/<configure-preset>/src/server/<Configuration>/marketdata_server
```

- `<configure-preset>` → the preset you used to configure (e.g. `linux-vcpkg-gcc-debug`).  
- `<Configuration>` → build configuration (`Debug`, `Release`, etc.).  

When the server starts, it will:
- Load the CSV data from `data/csv/`.
- Print a message showing which **portal (address/port)** it is listening on.

### 📡 Run the Client Application
In a separate terminal, run the HFT client application:

```bash
./build/<configure-preset>/src/app/<Configuration>/hft_app <portal-number>
```

- `<portal-number>` is the port shown by the server on startup.  
- The application will spawn **10 gRPC clients**, each subscribing to a different symbol.  
- Clients will start streaming and printing stock prices along with latency measurements.

------------------------------------------------------------------------
## 📊 Data Directory and Updating Stock Data

In the project root, there is a `data/` directory that contains tools and CSV files used by the server.

- **`data/csv/`** → This subdirectory stores historical stock data as CSV files.  
- **`data/fetch_data.py`** → A Python script that automatically fetches historical data for **10 predefined S&P 500 stocks** over the past 5 years and saves them into `data/csv/`.

### 🔄 Updating the Stock Data
To refresh the CSV files with the latest data, run the script manually from the project root:

```bash
python ./data/fetch_data.py
```

After execution, the `data/csv/` folder will contain updated CSV files for the predefined stocks, which the gRPC server will load on startup.

------------------------------------------------------------------------
## 🔹 Project Structure

-  `CMakeLists.txt` → Handles cross-platform build, gRPC/Protobuf generation, executables
-  `CMakePresets.json` → Contains build presets for Windows and Linux (GCC & Clang)
-  `data/` → Fetched data used from the server
-  `proto/` → `.proto` definitions (compiled into C++ sources by protoc + gRPC plugin)
-  `src/` → Contains the application (`app/`), a client library (`client/`) and the server application (`server/`)
-  `test/` → Unit tests performed with Google Test framework
-  `benchmarks/` → Contains the benchmarks performed with Google Benchmark framework
------------------------------------------------------------------------
## 🏎️ Running Benchmarks

This project uses the **Google Benchmark** framework to measure the performance of critical components.

### 🔹 Build with Benchmark Support

By default, benchmarks are not built. Use one of the `*-debug-all` or `*-release` presets that enable benchmarks:

```bash
# Example (Linux, GCC, Debug with tests + benchmarks)
cmake --preset=linux-vcpkg-gcc-debug-all
cmake --build --preset=linux-vcpkg-gcc-debug-all
```

or for Release builds with benchmarks:

```bash
cmake --preset=linux-vcpkg-gcc-release
cmake --build --preset=linux-vcpkg-gcc-release
```

### 🔹 Run Benchmarks

After a successful build, benchmark executables are located under:

```
./build/<configure-preset>/benchmark/<Configuration>/
```

Run them directly from the terminal. For example:

```bash
./build/linux-vcpkg-gcc-release/benchmark/Release/hft_benchmark
```

### 🔹 Example Output

When executed, benchmarks will report timing statistics, e.g.:

```
------------------------------------------------------
Benchmark                   Time        CPU Iterations
------------------------------------------------------
BM_ThreadPool/1/10   22748704 ns  364953 ns       1000
BM_ThreadPool/2/10   13163256 ns  432193 ns       1000
...
...
...
...
```

------------------------------------------------------------------------
## 🚀 Summary

- Dependencies (Protobuf + gRPC) are managed with **vcpkg** on all platforms.
- Builds are driven by **CMake presets**, so you don't need to remember long command lines.
- Generators (Ninja, Makefiles, Visual Studio) are auto-detected by `CMakeLists.txt`.