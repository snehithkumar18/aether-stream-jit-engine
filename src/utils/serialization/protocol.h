#ifndef AETHER_UTILS_SERIALIZATION_PROTOCOL_H
#define AETHER_UTILS_SERIALIZATION_PROTOCOL_H

#include <stddef.h>
#include <stdint.h>

typedef enum ProtocolVersion {
    PROTOCOL_V1,
    PROTOCOL_V2
} ProtocolVersion;

typedef struct ProtocolHeader {
    uint32_t magic;
    ProtocolVersion version;
    uint32_t length;
    uint32_t checksum;
    uint32_t flags;
    uint32_t sequence;
    uint64_t timestamp;
    uint64_t source;
    uint64_t destination;
    uint64_t signature;
    uint32_t priority;
    uint32_t ttl;
    uint32_t retry_count;
} ProtocolHeader;

typedef struct Protocol {
    ProtocolHeader header;
    uint8_t* body;
    size_t body_size;
} Protocol;

Protocol* protocol_create(ProtocolVersion version);
void protocol_destroy(Protocol* protocol);
void protocol_set_body(Protocol* protocol, uint8_t* body, size_t size);
uint8_t* protocol_serialize(Protocol* protocol, size_t* out_size);
Protocol* protocol_deserialize(uint8_t* data, size_t size);
uint32_t protocol_calculate_checksum(uint8_t* data, size_t size);

#endif
