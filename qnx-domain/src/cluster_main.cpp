// Simulated QNX cluster domain (C++)
// Minimal demo: attaches to shared memory ringbuffer and prints telemetry.

#include <iostream>
#include <thread>
#include <chrono>
#include "../comm/shm_protocol.h"

int main() {
    std::cout << "[cluster] starting simulated QNX cluster domain\n";
    ShmClient* client = shm_client_create("/hypervisor_shm", false); // consumer
    if (!client || !shm_client_open(client)) {
        std::cerr << "[cluster] failed to open shared memory\n";
        return 1;
    }
    while (true) {
        Message msg;
        if (shm_client_recv(client, &msg, 1000)) {
            std::cout << "[cluster] recv: type=" << msg.type << " len=" << msg.len
                      << " payload='" << std::string((char*)msg.payload, msg.len) << "'\n";
        } else {
            // periodic heartbeat
            std::cout << "[cluster] no msg - heartbeat\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    shm_client_destroy(client);
    return 0;
}
