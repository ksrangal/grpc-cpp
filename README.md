# grpc-cpp

A C++ gRPC demo project implementing various RPC patterns (unary, server streaming, client streaming, and bidirectional streaming) via a **ChatterBox** example.

---

## 📖 Overview

This project demonstrates how to use **gRPC with C++** to build client-server systems.  
The example service is called **ChatterBox**, which simulates message exchanges between client and server in multiple modes:

- **Unary RPC** (single request → single response)  
- **Server Streaming RPC** (single request → stream of responses)  
- **Client Streaming RPC** (stream of requests → single response)  
- **Bidirectional Streaming RPC** (stream of requests ↔ stream of responses)  

---

## ✨ Features

- Demonstrates all four gRPC communication types  
- Clean and minimal implementation for learning purposes  
- Client supports multiple modes (`poke`, `talk`, `listen`, `chat`)  
- Server responds with funny/randomized messages for interaction  
- Extensible design for future enhancements  

---

## 📂 Repository Structure

```
grpc-cpp/
├── ChatterBox/              # Core service logic
├── client/                  # Client-side implementation
├── server/                  # Server-side implementation
├── protos/                  # .proto files defining the service
├── CMakeLists.txt           # Build configuration
└── README.md                # Project documentation
```

---

## ⚙️ Build & Run

### Prerequisites

- **gRPC** and **Protobuf** installed  
- **CMake** ≥ 3.15  
- **C++17** compiler (g++, clang++, etc.)

### Build

```bash
git clone https://github.com/ksrangal/grpc-cpp.git
cd grpc-cpp
mkdir build && cd build
cmake ..
make -j$(nproc)
```

This generates two binaries:  

- `server`  
- `client`  

---

## 🚀 Usage

1. **Start the server**

   ```bash
   ./server
   ```

2. **Run the client** with one of the modes:

   ```bash
   ./client localhost:50051 poke      # Unary RPC
   ./client localhost:50051 talk      # Client streaming
   ./client localhost:50051 listen    # Server streaming
   ./client localhost:50051 chat      # Bidirectional streaming
   ```

Replace `localhost:50051` with your server’s host and port if different.

---

## 📝 Modes Explained

- **poke** → Client sends a single message, gets a single reply.  
- **talk** → Client streams multiple messages, server replies once at the end.  
- **listen** → Client sends one request, server streams multiple replies.  
- **chat** → Both client and server exchange multiple messages concurrently.  

---

## 🛠️ Design Notes

- Uses gRPC generated stubs (`.pb.h` and `.grpc.pb.h`) from the `protos/` directory.  
- Demonstrates gRPC primitives:  
  - `ClientReader`  
  - `ClientWriter`  
  - `ClientReaderWriter`  
- Simple, modular design for educational use.  

---

## 📌 Future Enhancements

- Add TLS / authentication support  
- Add persistence layer for chat history  
- Integrate Prometheus metrics for monitoring  
- Add retries and error handling for robustness  
- Extend protocol with JSON / binary payloads  

---

## 🤝 Contributing

Contributions welcome!  
1. Fork the repo  
2. Create a branch (`git checkout -b feature/my-feature`)  
3. Commit your changes  
4. Submit a Pull Request  

---

## 📄 License

This project is licensed under the **MIT License**.  
Feel free to use, modify, and share.  

---
