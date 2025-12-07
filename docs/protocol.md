# Communication Protocol

Messages are packaged into the `Message` struct defined in comm/shm_protocol.h.

Fields:
- type: message type (nav, media, speed, rpm, heartbeat)
- len: number of payload bytes
- seq: sequence number
- payload: up to MSG_PAYLOAD_MAX bytes

The demo uses a simple producer-consumer ring buffer in shared memory.
