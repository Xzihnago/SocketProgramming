#include <iostream>
#include "../Socket.h"

int main() {
    const auto family = AF_INET;
    const auto port = 8080;

    // TCP
    {
        Socket server(family, SOCK_STREAM, IPPROTO_TCP, false);
        server.bind(port);
        server.listen(1);
        SOCKADDR_IN addr {};
        int addr_len = sizeof(addr);
        SOCKET sfd = server.accept((SOCKADDR*)&addr, &addr_len);
        server.receive(sfd);
        std::cout << server.get_buffer().data() << "\n";
        server.send(sfd, "Hello, TCP!");
    }

    // UDP
    {
        Socket server(family, SOCK_DGRAM, IPPROTO_UDP, false);
        server.bind(port);
        SOCKADDR_IN addr {};
        int addr_len = sizeof(addr);
        server.receivefrom((SOCKADDR*)&addr, &addr_len);
        std::cout << server.get_buffer().data() << "\n";
        server.sendto("Hello, UDP!", (SOCKADDR*)&addr, addr_len);
    }

    system("pause");
}