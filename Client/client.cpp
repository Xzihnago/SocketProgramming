#include <filesystem>
#include <fstream>
#include <iostream>
#include "../Socket.h"

void recvFile(Socket rs, const char* path) {
    namespace fs = std::filesystem;

    // Recvive file name
    auto fileName = rs.receive();
    std::cout << "Recvive File Name: \"" << fileName << "\"\n";

    // Recvive file size
    char rsize[sizeof(uintmax_t)] = { 0 };
    rs.receive(rsize, sizeof(rsize));
    auto fileSize = *reinterpret_cast<uintmax_t*>(&rsize);
    std::cout << "Recvive File Size " << fileSize << " Bytes\n";

    // Recvive file data
    std::cout << "Recvive Data...\n";

    std::ofstream ofs(std::string(path) + std::string(fileName), std::ios::out | std::ios::binary);
    if (!ofs.is_open()) {
        std::cout << "File open failed\n";
        abort();
    }

    char rbuf[1000000] = { 0 };
    while (fileSize > 0) {
        auto size = min(fileSize, sizeof(rbuf));
        std::cout << "Recvive " << size << " Bytes | Reamining " << fileSize << " Bytes\n";
        rs.receive(rbuf, (int)size);
        ofs.write(rbuf, size);
        fileSize -= size;
        Sleep(10);
    }

    ofs.close();
    std::cout << "Recvive Complete!\n\n";
    Sleep(100);
}

int main() {
    const auto family = AF_INET;
    const auto ip = "127.0.0.1";
    const auto port = 8080;

    // TCP
    {
        Socket client(family, SOCK_STREAM, IPPROTO_TCP, false);
        client.connect(ip, port);
        client.send("Hello, TCP!");
        std::cout << client.receive() << "\n\n";
        client.close();
    }

    // UDP
    {
        Socket client(family, SOCK_DGRAM, IPPROTO_UDP, false);
        client.sendto("Hello, UDP!", ip, port);
        std::cout << client.receivefrom(ip, port) << "\n\n";
    }

    // Recvive file
    for (int i = 0; i < 2; ++i) {
        Socket client(family, SOCK_STREAM, IPPROTO_TCP, false);
        client.connect(ip, port);
        recvFile(client, "recv/");
        client.close();
    }

    system("pause");
    return 0;
}