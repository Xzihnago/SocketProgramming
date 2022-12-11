#include <filesystem>
#include <fstream>
#include <iostream>
#include "../Socket.h"

void sendFile(Socket ss, SOCKET sfd, const char* path) {
    namespace fs = std::filesystem;

    fs::path fspath(path);
    auto fileName = fspath.filename();
    auto fileSize = fs::file_size(fspath);

    // Send file name
    std::cout << "Send File Name: " << fileName << "\n";
    ss.send(sfd, fileName.string().c_str(), (int)fileName.string().length());

    // Send file size
    std::cout << "Send File Size: " << fileSize << " Bytes\n";
    auto ssize = reinterpret_cast<const char*>(&fileSize);
    ss.send(sfd, ssize, sizeof(ssize));

    // Send file data
    std::cout << "Send Data...\n";

    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if (!ifs.is_open()) {
        std::cout << "File open failed\n";
        abort();
    }

    char sbuf[1000000] = { 0 };
    while (fileSize > 0) {
        auto size = min(fileSize, sizeof(sbuf));
        ifs.read(sbuf, size);
        std::cout << "Send " << size << " Bytes | Reamining " << fileSize << " Bytes\n";
        ss.send(sfd, sbuf, (int)size);
        fileSize -= size;
        Sleep(10);
    }

    ifs.close();
    std::cout << "Send Complete!\n\n";
    Sleep(100);
}

int main() {
    const auto family = AF_INET;
    const auto port = 8080;

    //// TCP
    {
        Socket server(family, SOCK_STREAM, IPPROTO_TCP, false);
        server.bind(port);
        server.listen(1);
        SOCKADDR_IN addr {};
        int addr_len = sizeof(addr);
        SOCKET sfd = server.accept((SOCKADDR*)&addr, &addr_len);
        std::cout << server.receive(sfd) << "\n\n";
        server.send(sfd, "Hello, TCP!");
    }

    //// UDP
    {
        Socket server(family, SOCK_DGRAM, IPPROTO_UDP, false);
        server.bind(port);
        SOCKADDR_IN addr {};
        int addr_len = sizeof(addr);
        std::cout << server.receivefrom((SOCKADDR*)&addr, &addr_len) << "\n\n";
        server.sendto("Hello, UDP!", (SOCKADDR*)&addr, addr_len);
    }

    // Send file
    for (const auto& entry : std::filesystem::directory_iterator("send/")) {
        Socket server(family, SOCK_STREAM, IPPROTO_TCP, false);
        server.bind(port);
        server.listen(1);
        SOCKADDR_IN addr {};
        int addr_len = sizeof(addr);
        SOCKET sfd = server.accept((SOCKADDR*)&addr, &addr_len);
        sendFile(server, sfd, entry.path().string().c_str());
        server.close();
    }

    system("pause");
}