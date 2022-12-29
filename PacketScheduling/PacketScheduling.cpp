#include <algorithm>
#include <iostream>
#include <queue>
#include <thread>
#include "Packet.h"

using namespace std;

Packet createPacket(int size, int priority, int ttl) {
    Packet packet(size, priority, ttl);
    cout << "Create\t" << packet << "\n";
    return packet;
}

void send(Packet& packet) {
    cout << "Send\t" << packet;
    if (chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - packet.getArrivalTime()).count() > packet.getTTL()) {
        cout << "\tTimeout\n";
    }
    else {
        cout << "\tElapsed " << packet.getSize() << "ms\n";
        this_thread::sleep_for(chrono::milliseconds(packet.getSize()));
    }
}

void fcfs() {
    queue<Packet> packets;
    for (int i = 0; i < 10; i++) {
        packets.push(createPacket(rand() & 2047, rand() & 15, rand() & 2047 | 2048));
    }
    cout << "\n";

    while (!packets.empty()) {
        Packet packet = packets.front();
        send(packet);
        packets.pop();
    }
    cout << "\n\n";
}

void pq() {
    struct cmp {
        bool operator()(Packet p0, Packet p1) {
            return p0.getPriority() < p1.getPriority();
        }
    };
    priority_queue<Packet, vector<Packet>, cmp> packets;
    for (int i = 0; i < 10; i++) {
        packets.push(createPacket(rand() & 2047, rand() & 15, rand() & 2047 | 2048));
    }
    cout << "\n";

    while (!packets.empty()) {
        Packet packet = packets.top();
        send(packet);
        packets.pop();
    }
    cout << "\n\n";
}

void rr() {
    vector<queue<Packet>> packets(4);
    for (int i = 0; i < 4; i++) {
        cout << "Create Queue " << i << "\n";
        for (int j = 0; j < 4; j++) {
            packets[i].push(createPacket(rand() & 1023 << 1, rand() & 15, rand() & 2047 | 2048));
        }
        cout << "\n";
    }
    cout << "\n";

    int cntTime = 0;
    while (true) {
        bool isAllEmpty = true;
        for (int i = 0; i < 4; i++) {
            while (!packets[i].empty()) {
                isAllEmpty = false;
                Packet packet = packets[i].front();
                cntTime += packet.getSize();
                cout << "Queue " << i << " ";
                send(packet);
                packets[i].pop();
                if (cntTime >= 1000) {
                    cntTime = 0;
                    break;
                }
            }
        }
        if (isAllEmpty) {
            break;
        }
    }
    cout << "\n\n";
}

void wrr() {
    struct cmp {
        bool operator()(Packet p0, Packet p1) {
            return p0.getPriority() < p1.getPriority();
        }
    };
    vector<priority_queue<Packet, vector<Packet>, cmp>> packets(4);
    for (int i = 0; i < 4; i++) {
        cout << "Create Queue " << i << "\n";
        for (int j = 0; j < 4; j++) {
            packets[i].push(createPacket(rand() & 1023 << 1, rand() & 15, rand() & 2047 | 2048));
        }
        cout << "\n";
    }
    cout << "\n";

    int cntTime = 0;
    while (true) {
        bool isAllEmpty = true;
        for (int i = 0; i < 4; i++) {
            while (!packets[i].empty()) {
                isAllEmpty = false;
                Packet packet = packets[i].top();
                cntTime += packet.getSize();
                cout << "Queue " << i << " ";
                send(packet);
                packets[i].pop();
                if (cntTime >= 1000) {
                    cntTime = 0;
                    break;
                }
            }
        }
        if (isAllEmpty) {
            break;
        }
    }
    cout << "\n\n";
}

void wfq() {
    struct cmp {
        bool operator()(Packet p0, Packet p1) {
            return p0.getPriority() < p1.getPriority();
        }
    };
    vector<priority_queue<Packet, vector<Packet>, cmp>> packets(4);
    for (int i = 0; i < 4; i++) {
        cout << "Create Queue " << i << "\n";
        for (int j = 0; j < 4; j++) {
            packets[i].push(createPacket(rand() & 1023 << 1, rand() & 15, rand() & 2047 | 2048));
        }
        cout << "\n";
    }
    cout << "\n";

    int cntTime = 0;
    while (true) {
        bool isAllEmpty = true;
        for (int i = 0; i < 4; i++) {
            while (!packets[i].empty()) {
                isAllEmpty = false;
                Packet packet = packets[i].top();
                cntTime += packet.getSize();
                cout << "Queue " << i << " ";
                send(packet);
                packets[i].pop();
                if (cntTime >= 1000) {
                    cntTime = 0;
                    break;
                }
            }
        }
        if (isAllEmpty) {
            break;
        }
    }
    cout << "\n\n";
}

int main() {
    srand((int)time(NULL));

    cout << "FCFS\n";
    fcfs();
    cout << "PQ\n";
    pq();
    cout << "RR\n";
    rr();
    cout << "WRR\n";
    wrr();
    cout << "WFQ\n";
    wfq();

    system("pause");
    return 0;
}