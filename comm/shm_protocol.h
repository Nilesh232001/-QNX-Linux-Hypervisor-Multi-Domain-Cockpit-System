// Simple shared memory protocol & ring-buffer API (C)
#ifndef SHM_PROTOCOL_H
#define SHM_PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>

#define SHM_NAME "/hypervisor_shm"
#define SHM_SIZE (1<<20) // 1 MiB
#define MSG_PAYLOAD_MAX 512

typedef enum {
    MSG_TYPE_HEARTBEAT = 1,
    MSG_TYPE_SPEED = 2,
    MSG_TYPE_RPM = 3,
    MSG_TYPE_NAV = 10,
    MSG_TYPE_MEDIA = 11
} msg_type_t;

typedef struct {
    uint16_t type;
    uint16_t len;
    uint32_t seq;
    uint8_t payload[MSG_PAYLOAD_MAX];
} Message;

// Opaque ring buffer handle for both producer and consumer
typedef struct ShmRingBuffer ShmRingBuffer;

// API
ShmRingBuffer* shm_open_ring(const char* name, bool create);
void shm_close_ring(ShmRingBuffer* r);
bool shm_ring_send(ShmRingBuffer* r, const Message* m);
bool shm_ring_recv(ShmRingBuffer* r, Message* m, int timeout_ms);

// Convenience C wrapper declared in C for ease of use in examples
#ifdef __cplusplus
extern "C" {
#endif

typedef struct ShmClient ShmClient;

ShmClient* shm_client_create(const char* name, bool producer);
void shm_client_destroy(ShmClient* c);
bool shm_client_open(ShmClient* c);
bool shm_client_send(ShmClient* c, const Message* m);
bool shm_client_recv(ShmClient* c, Message* m, int timeout_ms);

#ifdef __cplusplus
}
#endif

#endif // SHM_PROTOCOL_H
