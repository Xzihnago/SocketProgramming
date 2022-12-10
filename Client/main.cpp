#include <filesystem>
#include <iostream>
#include "../Socket.h"

void sendFile(const char* path) {
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
        client.receive();
        std::cout << client.get_buffer().data() << "\n";
        client.close();
    }
    
    // UDP
    {
        Socket client(family, SOCK_DGRAM, IPPROTO_UDP, false);
        client.sendto("Hello, UDP!", ip, port);
        client.receivefrom(ip, port);
        std::cout << client.get_buffer().data() << "\n";
    }

    system("pause");
    return 0;
}