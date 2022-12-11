#pragma once
#if _WIN32
#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
static WSADATA wsadata;
static bool isWSAStartup = false;
#else
#include <fcntl.h>
#include <sys/socket.h>
#endif
#include <iostream>

SOCKADDR_IN create_sockaddr_in(ADDRESS_FAMILY family, const char* ip, uint16_t port) {
    SOCKADDR_IN addr {};
    addr.sin_family = family;
    addr.sin_port = htons(port);
    inet_pton(family, ip, &addr.sin_addr);

    return addr;
}

SOCKET create_socket(int family, int type, int protocol, bool is_nonblock) {
#ifdef _WIN32
    // Initialize Ws2_32.lib
    if (!isWSAStartup) {
        isWSAStartup = true;

        if (WSAStartup(MAKEWORD(2, 2), &wsadata) != NO_ERROR) std::cout << "WSAStartup failed (" << WSAGetLastError() << ")\n";
        else std::cout << "WSAStartup success\n";

        atexit([]() {
            if (WSACleanup() != NO_ERROR) std::cout << "WSACleanup failed (" << WSAGetLastError() << ")\n";
            else std::cout << "WSACleanup success\n";
            });
    }
#endif

    // Create socket
    SOCKET sfd = socket(family, type, protocol);

    // Abort if socket is invalid
    if (sfd == INVALID_SOCKET) {
#ifdef _WIN32
        int err = WSAGetLastError();
#else
        int err = errno;
#endif
        std::cout << "Create socket failed (" << err << ")\n";
        abort();
    }

    // Setup socket blocking mode
#ifdef _WIN32
    unsigned long sockmode = is_nonblock;
    int res = ioctlsocket(sfd, FIONBIO, &sockmode);
    if (res != NO_ERROR) res = WSAGetLastError();
#else
    int flag = fcntl(sfd, F_GETFL, 0);
    int res = fcntl(sfd, F_SETFL, is_nonblock ? flag | O_NONBLOCK : flag & ~O_NONBLOCK);
#endif
    if (res != NO_ERROR) std::cout << "Set socket flag failed (" << res << ")\n";

    return sfd;
}

class Socket {
private:
    int family;
    int type;
    int protocol;
    bool is_nonblock;

    SOCKET sfd;
    constexpr static int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];

    bool connected;
    bool listening;

public:
    Socket(int family, int type, int protocol, bool is_nonblock);
    ~Socket();

    constexpr const bool is_connected() const noexcept;
    constexpr const bool is_listening() const noexcept;
    constexpr const char* get_buffer() const noexcept;

    void bind(const int port);
    void listen(int max_connections);
    SOCKET accept(SOCKADDR* addr, int* addrlen);
    void connect(const char* ip, const int port);

    void close(const SOCKET sfd);
    void close();

    void send(const SOCKET sfd, const char* data, const int size);
    void send(const char* data, const int size);
    void send(const SOCKET sfd, const char* data);
    void send(const char* data);

    const char* receive(const SOCKET sfd, char* data, const int size);
    const char* receive(char* data, const int size);
    const char* receive(const SOCKET sfd);
    const char* receive();

    void sendto(const char* data, const int size, const SOCKADDR* addr, const int addrlen);
    void sendto(const char* data, const int size, const char* ip, const int port);
    void sendto(const char* data, const SOCKADDR* addr, const int addrlen);
    void sendto(const char* data, const char* ip, const int port);

    const char* receivefrom(char* data, const int size, const SOCKADDR* addr, int* addrlen);
    const char* receivefrom(char* data, const int size, const char* ip, const int port);
    const char* receivefrom(const SOCKADDR* addr, int* addrlen);
    const char* receivefrom(const char* ip, const int port);
};

Socket::Socket(int family, int type, int protocol, bool is_nonblock) {
    this->family = family;
    this->type = type;
    this->protocol = protocol;
    this->is_nonblock = is_nonblock;
    this->sfd = create_socket(family, type, protocol, is_nonblock);
    this->buffer[0] = '\0';
    this->connected = false;
    this->listening = false;
}

Socket::~Socket() {
    this->close();
}

constexpr const bool Socket::is_connected() const noexcept {
    return this->connected;
}

constexpr const bool Socket::is_listening() const noexcept {
    return this->listening;
}

constexpr const char* Socket::get_buffer() const noexcept {
    return this->buffer;
}

void Socket::bind(const int port) {
    if (this->listening) return;

    SOCKADDR_IN addr = create_sockaddr_in(this->family, "127.0.0.1", port);
    if (::bind(this->sfd, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
#ifdef _WIN32
        int res = WSAGetLastError();
#else
        int res = errno;
#endif
        std::cout << "Bind failed (" << res << ")\n";
        abort();
    }
}

void Socket::listen(int max_connections) {
    if (this->listening) return;

    if (::listen(this->sfd, max_connections) == SOCKET_ERROR) {
#ifdef _WIN32
        int res = WSAGetLastError();
#else
        int res = errno;
#endif
        std::cout << "Listen failed (" << res << ")\n";
        abort();
    }

    this->listening = true;
}

SOCKET Socket::accept(SOCKADDR* addr, int* addrlen) {
    if (!this->listening) return INVALID_SOCKET;
    return ::accept(this->sfd, addr, addrlen);
}

void Socket::connect(const char* ip, const int port) {
    if (this->connected) return;

    SOCKADDR_IN addr = create_sockaddr_in(this->family, ip, port);
    if (::connect(this->sfd, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
#ifdef _WIN32
        int res = WSAGetLastError();
#else
        int res = errno;
#endif
        std::cout << "Connect failed (" << res << ")\n";
        abort();
    }

    this->connected = true;
}

void Socket::close(const SOCKET sfd) {
#ifdef _WIN32
    int res = closesocket(sfd);
    if (res == SOCKET_ERROR) res = WSAGetLastError();
#else
    int res = ::close(sfd);
#endif
    if (res == SOCKET_ERROR) {
        std::cout << "Close socket failed (" << res << ")\n";
        abort();
    }

    this->sfd = INVALID_SOCKET;
    this->connected = false;
    this->listening = false;
}
void Socket::close() {
    this->close(this->sfd);
}

void Socket::send(const SOCKET sfd, const char* data, const int size) {
    if (::send(sfd, data, size, 0) == SOCKET_ERROR) {
#ifdef _WIN32
        int res = WSAGetLastError();
#else
        int res = errno;
#endif
        std::cout << "Send failed (" << res << ")\n";
        abort();
    }
}
void Socket::send(const char* data, const int size) {
    this->send(this->sfd, data, size);
}
void Socket::send(const SOCKET sfd, const char* data) {
    this->send(sfd, data, (int)strlen(data));
}
void Socket::send(const char* data) {
    this->send(this->sfd, data, (int)strlen(data));
}

const char* Socket::receive(const SOCKET sfd, char* data, const int size) {
    if (::recv(sfd, data, size, 0) == SOCKET_ERROR) {
#ifdef _WIN32
        int res = WSAGetLastError();
#else
        int res = errno;
#endif
        std::cout << "Receive failed (" << res << ")\n";
        abort();
    }
    return data;
}
const char* Socket::receive(char* data, const int size) {
    return this->receive(this->sfd, data, size);
}
const char* Socket::receive(const SOCKET sfd) {
    memset(this->buffer, 0, BUFFER_SIZE);
    return this->receive(sfd, this->buffer, this->BUFFER_SIZE);
}
const char* Socket::receive() {
    memset(this->buffer, 0, BUFFER_SIZE);
    return this->receive(this->sfd, this->buffer, this->BUFFER_SIZE);
}

void Socket::sendto(const char* data, const int size, const SOCKADDR* addr, const int addrlen) {
    if (::sendto(this->sfd, data, size, 0, addr, addrlen) == SOCKET_ERROR) {
#ifdef _WIN32
        int res = WSAGetLastError();
#else
        int res = errno;
#endif
        std::cout << "Sendto failed (" << res << ")\n";
        abort();
    }
}
void Socket::sendto(const char* data, const int size, const char* ip, const int port) {
    SOCKADDR_IN addr = create_sockaddr_in(this->family, ip, port);
    this->sendto(data, (SOCKADDR*)&addr, sizeof(addr));
}
void Socket::sendto(const char* data, const SOCKADDR* addr, const int addrlen) {
    this->sendto(data, (int)strlen(data), addr, addrlen);
}
void Socket::sendto(const char* data, const char* ip, const int port) {
    this->sendto(data, (int)strlen(data), ip, port);
}

const char* Socket::receivefrom(char* data, const int size, const SOCKADDR* addr, int* addrlen) {
    if (::recvfrom(this->sfd, data, size, 0, (SOCKADDR*)addr, addrlen) == SOCKET_ERROR) {
#ifdef _WIN32
        int res = WSAGetLastError();
#else
        int res = errno;
#endif
        std::cout << "Receivefrom failed (" << res << ")\n";
        abort();
    }
    return data;
}
const char* Socket::receivefrom(char* data, const int size, const char* ip, const int port) {
    SOCKADDR_IN addr = create_sockaddr_in(this->family, ip, port);
    int addrlen = sizeof(addr);
    return this->receivefrom(data, size, (SOCKADDR*)&addr, &addrlen);
}
const char* Socket::receivefrom(const SOCKADDR* addr, int* addrlen) {
    memset(this->buffer, 0, BUFFER_SIZE);
    return this->receivefrom(this->buffer, this->BUFFER_SIZE, addr, addrlen);
}
const char* Socket::receivefrom(const char* ip, const int port) {
    memset(this->buffer, 0, BUFFER_SIZE);
    return this->receivefrom(this->buffer, this->BUFFER_SIZE, ip, port);
}