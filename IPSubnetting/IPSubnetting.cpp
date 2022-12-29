#include <iomanip>
#include <iostream>
#include <string>
#include <regex>

// Regex to match an IPv4 address
std::regex ipRegex("^((25[0-5]|(2[0-4]|1\\d|[1-9]|)\\d)\\.?\\b){4}$");

// Convert ip to uint32
uint32_t ipToUInt32(std::string ipS) {
    uint32_t ipN = 0;
    for (int _ = 0; _ < 4; ++_) {
        size_t end = ipS.find('.');
        ipN = (ipN << 8) + std::stoi(ipS.substr(0, end++));
        ipS = ipS.substr(end);
    }
    return ipN;
}

// Convert uint32 to ip
std::string UInt32ToIp(uint32_t ipN) {
    std::string ipS;
    for (int _ = 0; _ < 4; ++_) {
        ipS = std::to_string(ipN & 0xFF) + "." + ipS;
        ipN >>= 8;
    }
    return ipS.substr(0, ipS.length() - 1);
}

int main() {
    std::cout << std::hex << std::left;
    while (true) {
        // Get the IP address from the user
        std::string ipString;
        std::cout << "Enter an IP address: ";
        std::cin >> ipString;

        // Check if the IP address is valid
        if (!std::regex_match(ipString, ipRegex)) {
            std::cout << "Invalid IP address\n\n";
            continue;
        }

        // Convert ip to uint32
        uint32_t ipN = ipToUInt32(ipString), mask = 0xFFFFFF00;

        // Judge the class of the IP address
        uint8_t classType;
        if (ipN >> 31 == 0b0) {
            classType = 0;
            mask <<= 16;
        }
        else if (ipN >> 30 == 0b10) {
            classType = 1;
            mask <<= 8;
        }
        else if (ipN >> 29 == 0b110) {
            classType = 2;
        }
        else if (ipN >> 28 == 0b1110) {
            std::cout << "Class D (multicast)\n\n";
            continue;
        }
        else {
            std::cout << "Class E (reserved)\n\n";
            continue;
        }

        // Get subnet count
        uint32_t snetCount;
        std::cout << "Enter the subnet count: ";
        std::cin >> snetCount;

        // Check if the subnet count is valid
        if (snetCount < 1 || (snetCount > 0x400000 >> (8 << classType - 1)) || (snetCount & (snetCount - 1))) {
            std::cout << "Invalid subnet count\n\n";
            continue;
        }

        // Calculate subnet id
        while (snetCount >>= 1) {
            mask = mask >> 1 | 0x80000000;
        }

        // Output
        uint32_t snetId = ipN & mask, bcId = ipN | ~mask;
        std::cout << "Class " << (char)('A' + classType) << "\n";
        std::cout
            << std::setw(18) << "Subnet mask"
            << std::setw(18) << "NET ID"
            << std::setw(18) << "Boardcast"
            << std::setw(18) << "Gateway"
            << "Host ID\n";
        std::cout
            << std::setw(18) << UInt32ToIp(mask)
            << std::setw(18) << UInt32ToIp(snetId)
            << std::setw(18) << UInt32ToIp(bcId)
            << std::setw(18) << UInt32ToIp(bcId - 1)
            << UInt32ToIp(snetId + 1);
        if (bcId - snetId != 3) {
            std::cout << " - " << UInt32ToIp(bcId - 2);
        }
        std::cout << "\n\n";
    }
}