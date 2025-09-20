# CrossOptions – HFT Trading System (Case Assignment)

This project implements the main loop of a simplified **High-Frequency Trading (HFT) system** using **C++20**, **gRPC**, and **Protobuf**.  

- A **gRPC server** streams historical market data (10 S&P500 stocks).  
- A **gRPC client** subscribes to stock data.  
- The **main app** spawns multiple clients in parallel and measures latency.  
- A **GoogleTest** suite provides basic unit testing.  

---

## 📂 Project Structure
```
CROSS_OPTIONS/
 ├─ CMakeLists.txt          # Root CMake build script
 ├─ proto/                  # Protobuf .proto files
 ├─ src/
 │   ├─ server/             # gRPC server implementation
 │   ├─ client/             # gRPC client implementation
 │   └─ app/                # Main application (spawns multiple clients)
 ├─ test/                   # GoogleTest unit tests
 └─ build/                  # Generated build artifacts (created after build)
```

---

## ⚙️ Requirements

- **CMake ≥ 3.20**
- **C++20 compiler**
  - MSVC 2022 (Windows)
  - GCC ≥ 11 or Clang ≥ 12 (Ubuntu 22.04)
- **gRPC & Protobuf**
- **GoogleTest** (fetched automatically if missing)

---

## 🖥️ Build Instructions

### 🔹 Ubuntu 22.04 (grading environment)

Install dependencies:
```bash
sudo apt update
sudo apt install -y   build-essential cmake git   protobuf-compiler libprotobuf-dev   libgrpc++-dev grpc-proto
```

Configure & build:
```bash
cmake -S . -B build
cmake --build build -j$(nproc)
```

Executables will be created in:
```
build/src/server/marketdata_server
build/src/client/marketdata_client
build/src/app/hft_app
build/test/unit_tests
```

---

### 🔹 Windows (development environment)

#### Option A: Manual install (custom build of gRPC/Protobuf)

If you built **gRPC and Protobuf manually from source** and installed them to:
```
C:\Projects\grpc-install\
 ├─ bin\protoc.exe
 ├─ include\
 ├─ lib\
 └─ lib\cmake\{grpc,protobuf}
```

Configure with:
```powershell
cmake -S . -B build `
  -DCMAKE_PREFIX_PATH="C:/Projects/grpc-install" `
  -DProtobuf_PROTOC_EXECUTABLE="C:/Projects/grpc-install/bin/protoc.exe"
```

Build (choose configuration Debug or Release):
```powershell
cmake --build build --config Debug
```

Executables will appear in:
```
build/src/server/Debug/marketdata_server.exe
build/src/client/Debug/marketdata_client.exe
build/src/app/Debug/hft_app.exe
build/test/Debug/unit_tests.exe
```

#### Option B: Use vcpkg (recommended for Windows)

Instead of managing dependencies manually, you can use [vcpkg](https://github.com/microsoft/vcpkg).

1. Clone and bootstrap vcpkg:
```powershell
cd C:\Projects
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.ootstrap-vcpkg.bat
```

2. Install required libraries:
```powershell
.cpkg install grpc protobuf gtest
```

3. Configure your project with the vcpkg toolchain:
```powershell
cd C:\Projects\CrossOptions
cmake -S . -B build `
  -DCMAKE_TOOLCHAIN_FILE=C:/Projects/vcpkg/scripts/buildsystems/vcpkg.cmake
```

4. Build:
```powershell
cmake --build build --config Debug
```

Executables will appear in the same locations as in manual install.  
✅ vcpkg ensures that `protoc.exe` and `libprotobuf.lib` always match.  

---

## ▶️ Running the Project

1. **Start the server**  
   ```bash
   ./build/src/server/marketdata_server
   ```

2. **Run a client in another terminal**  
   ```bash
   ./build/src/client/marketdata_client
   ```

3. **Run the main app (spawns multiple clients)**  
   ```bash
   ./build/src/app/hft_app
   ```

---

## 🧪 Running Tests

Run tests directly:
```bash
./build/test/unit_tests
```

Or via CTest:
```bash
cd build
ctest --output-on-failure
```

---

## 📝 Notes

- On **Ubuntu**, CMake automatically finds `protobuf` and `gRPC` from `apt`.  
- On **Windows**, you can either point `CMAKE_PREFIX_PATH` to your custom install directory or use **vcpkg**.  
- The build system ensures that `.proto` files are compiled automatically into C++ sources.  

---

## 📖 Design Choices

- **CMake modularization**: each of `server`, `client`, and `app` has its own `CMakeLists.txt`.  
- **Cross-platform compatibility**: conditional linking supports both `CONFIG` (Windows/vcpkg/custom install) and `MODULE` mode (Linux apt).  
- **Generated code isolation**: all `.pb.cc` and `.grpc.pb.cc` files are stored in `build/generated/`.  
- **Testing**: GoogleTest integrated with CTest for easy test execution.  
