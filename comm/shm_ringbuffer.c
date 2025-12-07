// Minimal portable POSIX shared memory ring buffer implementation (C)
#include "shm_protocol.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <time.h>

typedef struct {
    atomic_uint write_idx;
    atomic_uint read_idx;
    pthread_mutex_t lock;
    pthread_cond_t  cond;
    uint8_t padding[64];
    uint8_t buffer[]; // flexible
} ShmHeader;

struct ShmRingBuffer {
    int fd;
    size_t mem_size;
    void* base;
    ShmHeader* hdr;
    uint8_t* data;
    size_t data_size;
    bool producer;
};

ShmRingBuffer* shm_open_ring(const char* name, bool create) {
    int fd;
    if (create) {
        fd = shm_open(name, O_CREAT | O_RDWR, 0666);
        if (fd < 0) { perror("shm_open create"); return NULL; }
        if (ftruncate(fd, SHM_SIZE) != 0) { perror("ftruncate"); close(fd); return NULL; }
    } else {
        fd = shm_open(name, O_RDWR, 0666);
        if (fd < 0) { perror("shm_open open"); return NULL; }
    }
    void* base = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (base == MAP_FAILED) { perror("mmap"); close(fd); return NULL; }
    ShmRingBuffer* r = calloc(1, sizeof(*r));
    r->fd = fd;
    r->mem_size = SHM_SIZE;
    r->base = base;
    r->hdr = (ShmHeader*)base;
    r->data = (uint8_t*)base + sizeof(ShmHeader);
    r->data_size = SHM_SIZE - sizeof(ShmHeader);
    return r;
}

void shm_close_ring(ShmRingBuffer* r) {
    if (!r) return;
    munmap(r->base, r->mem_size);
    close(r->fd);
    free(r);
}

static size_t msg_size(const Message* m) {
    return sizeof(m->type) + sizeof(m->len) + sizeof(m->seq) + m->len;
}

bool shm_ring_send(ShmRingBuffer* r, const Message* m) {
    if (!r || !m) return false;
    unsigned w = atomic_load(&r->hdr->write_idx);
    unsigned ridx = atomic_load(&r->hdr->read_idx);
    size_t needed = msg_size(m) + 8; // small header slack
    if (needed > r->data_size) return false;
    // naive check for free space
    if ((ridx + r->data_size - w) % r->data_size < needed) {
        // no space
        return false;
    }
    // write at position w % data_size
    size_t off = w % r->data_size;
    if (off + needed <= r->data_size) {
        memcpy(r->data + off, m, msg_size(m));
    } else {
        size_t first = r->data_size - off;
        memcpy(r->data + off, m, first);
        memcpy(r->data, ((uint8_t*)m) + first, msg_size(m) - first);
    }
    atomic_store(&r->hdr->write_idx, w + needed);
    pthread_cond_broadcast(&r->hdr->cond);
    return true;
}

bool shm_ring_recv(ShmRingBuffer* r, Message* m, int timeout_ms) {
    if (!r || !m) return false;
    unsigned rpos = atomic_load(&r->hdr->read_idx);
    unsigned w = atomic_load(&r->hdr->write_idx);
    if (rpos == w) {
        // wait with timeout
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeout_ms/1000;
        pthread_mutex_lock(&r->hdr->lock);
        pthread_cond_timedwait(&r->hdr->cond, &r->hdr->lock, &ts);
        pthread_mutex_unlock(&r->hdr->lock);
        w = atomic_load(&r->hdr->write_idx);
        if (rpos == w) return false;
    }
    size_t off = rpos % r->data_size;
    // For simplicity assume message fits contiguous region
    memcpy(m, r->data + off, sizeof(Message));
    // advance read pointer by sizeof(Message) (note: simplistic)
    atomic_store(&r->hdr->read_idx, rpos + sizeof(Message));
    return true;
}

// Simple C-wrapper client implementations (not robust; demo only)
struct ShmClient {
    ShmRingBuffer* r;
    bool producer;
    char name[128];
};

ShmClient* shm_client_create(const char* name, bool producer) {
    ShmClient* c = calloc(1, sizeof(*c));
    strncpy(c->name, name, sizeof(c->name)-1);
    c->producer = producer;
    return c;
}
void shm_client_destroy(ShmClient* c) {
    if (!c) return;
    if (c->r) shm_close_ring(c->r);
    free(c);
}
bool shm_client_open(ShmClient* c) {
    if (!c) return false;
    c->r = shm_open_ring(c->name, c->producer);
    return c->r != NULL;
}
bool shm_client_send(ShmClient* c, const Message* m) {
    if (!c || !c->r) return false;
    return shm_ring_send(c->r, m);
}
bool shm_client_recv(ShmClient* c, Message* m, int timeout_ms) {
    if (!c || !c->r) return false;
    return shm_ring_recv(c->r, m, timeout_ms);
}
