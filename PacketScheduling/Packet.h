#pragma once
#include <chrono>
#include <iomanip>
#include <iostream>

class Packet {
private:
    size_t size;
    unsigned short priority;
    unsigned short ttl;
    std::chrono::time_point<std::chrono::system_clock> arrivalTime;

public:
    Packet(size_t size, unsigned short priority, unsigned short ttl);
    constexpr const size_t getSize() const noexcept;
    constexpr const unsigned short getPriority() const noexcept;
    constexpr const unsigned short getTTL() const noexcept;
    constexpr const std::chrono::time_point<std::chrono::system_clock> getArrivalTime() const noexcept;

    friend std::ostream& operator<<(std::ostream& os, const Packet& packet);
};

Packet::Packet(size_t size, unsigned short priority, unsigned short ttl) {
    this->size = size;
    this->priority = priority;
    this->ttl = ttl + (unsigned short)size;
    this->arrivalTime = std::chrono::system_clock::now();
}

constexpr const size_t Packet::getSize() const noexcept {
    return this->size;
}

constexpr const unsigned short Packet::getPriority() const noexcept {
    return this->priority;
}

constexpr const unsigned short Packet::getTTL() const noexcept {
    return this->ttl;
}

constexpr const std::chrono::time_point<std::chrono::system_clock> Packet::getArrivalTime() const noexcept {
    return this->arrivalTime;
}

std::ostream& operator<<(std::ostream& os, const Packet& packet) {
    os << "Packet [S: " << std::setw(4) << packet.size << "B, P: " << std::setw(2) << packet.priority << ", TTL: " << packet.ttl << "ms, T: " << std::chrono::system_clock::to_time_t(packet.arrivalTime) << "]";
    return os;
}