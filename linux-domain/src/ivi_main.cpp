// Simulated Linux IVI domain (C++)
// Sends periodic navigation and media metadata messages into shared memory.

#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>
#include "../comm/shm_protocol.h"

int main() {
    std::cout << "[ivi] starting simulated Linux IVI domain\n";
    ShmClient* client = shm_client_create("/hypervisor_shm", true); // producer
    if (!client || !shm_client_open(client)) {
        std::cerr << "[ivi] failed to open shared memory\n";
        return 1;
    }
    int seq = 0;
    while (true) {
        seq++;
        std::string payload = "NAV: Turn left in 200m (#" + std::to_string(seq) + ")";
        Message msg;
        msg.type = MSG_TYPE_NAV;
        msg.len = payload.size();
        msg.seq = seq;
        memset(msg.payload, 0, sizeof(msg.payload));
        memcpy(msg.payload, payload.data(), msg.len);
        if (!shm_client_send(client, &msg)) {
            std::cerr << "[ivi] send failed\n";
        } else {
            std::cout << "[ivi] sent nav message #" << seq << "\n";
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    shm_client_destroy(client);
    return 0;
}
