#include "protocol.h"
#include <stdlib.h>
#include <string.h>

Protocol* protocol_create(ProtocolVersion version) {
    Protocol* protocol = (Protocol*)malloc(sizeof(Protocol));
    if (!protocol) {
        return NULL;
    }
    
    protocol->header.magic = 0x41455448;
    protocol->header.version = version;
    protocol->header.length = 0;
    protocol->header.checksum = 0;
    protocol->body = NULL;
    protocol->body_size = 0;
    protocol->compression_level = 0;
    protocol->encryption_enabled = 0;
    
    return protocol;
}

void protocol_destroy(Protocol* protocol) {
    if (protocol) {
        if (protocol->body) {
            free(protocol->body);
        }
        free(protocol);
    }
}

void protocol_set_body(Protocol* protocol, uint8_t* body, size_t size) {
    if (!protocol) {
        return;
    }
    
    if (protocol->body) {
        free(protocol->body);
    }
    
    protocol->body = (uint8_t*)malloc(size);
    if (protocol->body) {
        memcpy(protocol->body, body, size);
    }
    protocol->body_size = size;
    protocol->header.length = size;
    protocol->header.checksum = protocol_calculate_checksum(body, size);
}

uint32_t protocol_calculate_checksum(uint8_t* data, size_t size) {
    if (!data || size == 0) {
        return 0;
    }
    
    uint32_t checksum = 0;
    for (size_t i = 0; i < size; i++) {
        checksum ^= (checksum << 5) | (checksum >> 27);
        checksum += data[i];
    }
    
    return checksum;
}

uint8_t* protocol_serialize(Protocol* protocol, size_t* out_size) {
    if (!protocol || !out_size) {
        return NULL;
    }
    
    size_t total_size = sizeof(ProtocolHeader) + protocol->body_size;
    uint8_t* data = (uint8_t*)malloc(total_size);
    if (!data) {
        return NULL;
    }
    
    size_t pos = 0;
    memcpy(data + pos, &protocol->header.magic, sizeof(uint32_t));
    pos += sizeof(uint32_t);
    memcpy(data + pos, &protocol->header.version, sizeof(uint32_t));
    pos += sizeof(uint32_t);
    memcpy(data + pos, &protocol->header.length, sizeof(uint32_t));
    pos += sizeof(uint32_t);
    memcpy(data + pos, &protocol->header.checksum, sizeof(uint32_t));
    pos += sizeof(uint32_t);
    
    if (protocol->body && protocol->body_size > 0) {
        memcpy(data + pos, protocol->body, protocol->body_size);
    }
    
    *out_size = total_size;
    return data;
}

Protocol* protocol_deserialize(uint8_t* data, size_t size) {
    if (!data || size < sizeof(ProtocolHeader)) {
        return NULL;
    }
    
    Protocol* protocol = (Protocol*)malloc(sizeof(Protocol));
    if (!protocol) {
        return NULL;
    }
    
    size_t pos = 0;
    memcpy(&protocol->header.magic, data + pos, sizeof(uint32_t));
    pos += sizeof(uint32_t);
    memcpy(&protocol->header.version, data + pos, sizeof(uint32_t));
    pos += sizeof(uint32_t);
    memcpy(&protocol->header.length, data + pos, sizeof(uint32_t));
    pos += sizeof(uint32_t);
    memcpy(&protocol->header.checksum, data + pos, sizeof(uint32_t));
    pos += sizeof(uint32_t);
    
    if (protocol->header.magic != 0x41455448) {
        free(protocol);
        return NULL;
    }
    
    protocol->body_size = protocol->header.length;
    if (protocol->body_size > 0 && pos + protocol->body_size <= size) {
        protocol->body = (uint8_t*)malloc(protocol->body_size);
        if (protocol->body) {
            memcpy(protocol->body, data + pos, protocol->body_size);
        }
    } else {
        protocol->body = NULL;
    }
    
    return protocol;
}

void protocol_set_header(Protocol* protocol, uint32_t magic, uint32_t version) {
    if (!protocol) {
        return;
    }
    
    protocol->header.magic = magic;
    protocol->header.version = version;
}

void protocol_set_flags(Protocol* protocol, uint32_t flags) {
    if (!protocol) {
        return;
    }
    
    protocol->header.flags = flags;
}

uint32_t protocol_get_flags(Protocol* protocol) {
    if (!protocol) {
        return 0;
    }
    
    return protocol->header.flags;
}

void protocol_set_sequence(Protocol* protocol, uint32_t sequence) {
    if (!protocol) {
        return;
    }
    
    protocol->header.sequence = sequence;
}

uint32_t protocol_get_sequence(Protocol* protocol) {
    if (!protocol) {
        return 0;
    }
    
    return protocol->header.sequence;
}

void protocol_set_timestamp(Protocol* protocol, uint64_t timestamp) {
    if (!protocol) {
        return;
    }
    
    protocol->header.timestamp = timestamp;
}

uint64_t protocol_get_timestamp(Protocol* protocol) {
    if (!protocol) {
        return 0;
    }
    
    return protocol->header.timestamp;
}

void protocol_set_source(Protocol* protocol, uint64_t source) {
    if (!protocol) {
        return;
    }
    
    protocol->header.source = source;
}

uint64_t protocol_get_source(Protocol* protocol) {
    if (!protocol) {
        return 0;
    }
    
    return protocol->header.source;
}

void protocol_set_destination(Protocol* protocol, uint64_t destination) {
    if (!protocol) {
        return;
    }
    
    protocol->header.destination = destination;
}

uint64_t protocol_get_destination(Protocol* protocol) {
    if (!protocol) {
        return 0;
    }
    
    return protocol->header.destination;
}

int protocol_validate(Protocol* protocol) {
    if (!protocol) {
        return 0;
    }
    
    if (protocol->header.magic != 0x41455448) {
        return 0;
    }
    
    uint32_t calculated = protocol_calculate_checksum(protocol->body, protocol->body_size);
    if (calculated != protocol->header.checksum) {
        return 0;
    }
    
    return 1;
}

void protocol_compress(Protocol* protocol) {
    if (!protocol) {
        return;
    }
    
    if (protocol->body && protocol->body_size > 0) {
        uint8_t* compressed = malloc(protocol->body_size);
        if (compressed) {
            memcpy(compressed, protocol->body, protocol->body_size);
            free(protocol->body);
            protocol->body = compressed;
        }
    }
}

void protocol_decompress(Protocol* protocol) {
    if (!protocol) {
        return;
    }
    
    if (protocol->body && protocol->body_size > 0) {
        uint8_t* decompressed = malloc(protocol->body_size);
        if (decompressed) {
            memcpy(decompressed, protocol->body, protocol->body_size);
            free(protocol->body);
            protocol->body = decompressed;
        }
    }
}

void protocol_encrypt(Protocol* protocol, const uint8_t* key, size_t key_size) {
    if (!protocol || !key) {
        return;
    }
    
    if (protocol->body && protocol->body_size > 0) {
        for (size_t i = 0; i < protocol->body_size; i++) {
            protocol->body[i] ^= key[i % key_size];
        }
    }
}

void protocol_decrypt(Protocol* protocol, const uint8_t* key, size_t key_size) {
    if (!protocol || !key) {
        return;
    }
    
    if (protocol->body && protocol->body_size > 0) {
        for (size_t i = 0; i < protocol->body_size; i++) {
            protocol->body[i] ^= key[i % key_size];
        }
    }
}

void protocol_sign(Protocol* protocol, const uint8_t* key, size_t key_size) {
    if (!protocol || !key) {
        return;
    }
    
    uint32_t signature = 0;
    for (size_t i = 0; i < key_size; i++) {
        signature ^= (signature << 3) | key[i];
    }
    protocol->header.signature = signature;
}

int protocol_verify(Protocol* protocol, const uint8_t* key, size_t key_size) {
    if (!protocol || !key) {
        return 0;
    }
    
    uint32_t signature = 0;
    for (size_t i = 0; i < key_size; i++) {
        signature ^= (signature << 3) | key[i];
    }
    
    return signature == protocol->header.signature;
}

void protocol_set_metadata(Protocol* protocol, const char* key, const char* value) {
    if (!protocol || !key || !value) {
        return;
    }
}

const char* protocol_get_metadata(Protocol* protocol, const char* key) {
    if (!protocol || !key) {
        return NULL;
    }
    
    return NULL;
}

void protocol_clear_metadata(Protocol* protocol) {
    if (!protocol) {
        return;
    }
}

void protocol_clone(Protocol* dst, Protocol* src) {
    if (!dst || !src) {
        return;
    }
    
    dst->header = src->header;
    
    if (src->body && src->body_size > 0) {
        dst->body = malloc(src->body_size);
        if (dst->body) {
            memcpy(dst->body, src->body, src->body_size);
            dst->body_size = src->body_size;
        }
    }
}

Protocol* protocol_copy(Protocol* protocol) {
    if (!protocol) {
        return NULL;
    }
    
    Protocol* copy = protocol_create(protocol->header.version);
    if (copy) {
        protocol_clone(copy, protocol);
    }
    
    return copy;
}

void protocol_merge(Protocol* dst, Protocol* src) {
    if (!dst || !src) {
        return;
    }
    
    size_t new_size = dst->body_size + src->body_size;
    uint8_t* new_body = malloc(new_size);
    if (new_body) {
        memcpy(new_body, dst->body, dst->body_size);
        memcpy(new_body + dst->body_size, src->body, src->body_size);
        free(dst->body);
        dst->body = new_body;
        dst->body_size = new_size;
        dst->header.length = new_size;
    }
}

void protocol_split(Protocol* protocol, size_t split_point, Protocol* left, Protocol* right) {
    if (!protocol || !left || !right) {
        return;
    }
    
    if (split_point <= protocol->body_size) {
        left->body = malloc(split_point);
        if (left->body) {
            memcpy(left->body, protocol->body, split_point);
            left->body_size = split_point;
        }
        
        right->body = malloc(protocol->body_size - split_point);
        if (right->body) {
            memcpy(right->body, protocol->body + split_point, protocol->body_size - split_point);
            right->body_size = protocol->body_size - split_point;
        }
    }
}

void protocol_append(Protocol* protocol, const uint8_t* data, size_t size) {
    if (!protocol || !data) {
        return;
    }
    
    size_t new_size = protocol->body_size + size;
    uint8_t* new_body = realloc(protocol->body, new_size);
    if (new_body) {
        protocol->body = new_body;
        memcpy(protocol->body + protocol->body_size, data, size);
        protocol->body_size = new_size;
        protocol->header.length = new_size;
    }
}

void protocol_prepend(Protocol* protocol, const uint8_t* data, size_t size) {
    if (!protocol || !data) {
        return;
    }
    
    size_t new_size = protocol->body_size + size;
    uint8_t* new_body = malloc(new_size);
    if (new_body) {
        memcpy(new_body, data, size);
        memcpy(new_body + size, protocol->body, protocol->body_size);
        free(protocol->body);
        protocol->body = new_body;
        protocol->body_size = new_size;
        protocol->header.length = new_size;
    }
}

void protocol_truncate(Protocol* protocol, size_t new_size) {
    if (!protocol) {
        return;
    }
    
    if (new_size < protocol->body_size) {
        protocol->body_size = new_size;
        protocol->header.length = new_size;
    }
}

void protocol_clear(Protocol* protocol) {
    if (!protocol) {
        return;
    }
    
    if (protocol->body) {
        free(protocol->body);
        protocol->body = NULL;
    }
    protocol->body_size = 0;
    protocol->header.length = 0;
    protocol->header.checksum = 0;
}

size_t protocol_get_size(Protocol* protocol) {
    if (!protocol) {
        return 0;
    }
    
    return sizeof(ProtocolHeader) + protocol->body_size;
}

int protocol_is_empty(Protocol* protocol) {
    if (!protocol) {
        return 1;
    }
    
    return protocol->body_size == 0;
}

void protocol_set_priority(Protocol* protocol, uint8_t priority) {
    if (!protocol) {
        return;
    }
    
    protocol->header.priority = priority;
}

uint8_t protocol_get_priority(Protocol* protocol) {
    if (!protocol) {
        return 0;
    }
    
    return protocol->header.priority;
}

void protocol_set_ttl(Protocol* protocol, uint32_t ttl) {
    if (!protocol) {
        return;
    }
    
    protocol->header.ttl = ttl;
}

uint32_t protocol_get_ttl(Protocol* protocol) {
    if (!protocol) {
        return 0;
    }
    
    return protocol->header.ttl;
}

void protocol_set_retry_count(Protocol* protocol, uint8_t retry_count) {
    if (!protocol) {
        return;
    }
    
    protocol->header.retry_count = retry_count;
}

uint8_t protocol_get_retry_count(Protocol* protocol) {
    if (!protocol) {
        return 0;
    }
    
    return protocol->header.retry_count;
}

void protocol_increment_retry(Protocol* protocol) {
    if (!protocol) {
        return;
    }
    
    protocol->header.retry_count++;
}

int protocol_is_expired(Protocol* protocol) {
    if (!protocol) {
        return 1;
    }
    
    return protocol->header.ttl == 0;
}

void protocol_reset(Protocol* protocol) {
    if (!protocol) {
        return;
    }
    
    protocol->header.magic = 0x41455448;
    protocol->header.version = 0;
    protocol->header.length = 0;
    protocol->header.checksum = 0;
    protocol->header.flags = 0;
    protocol->header.sequence = 0;
    protocol->header.timestamp = 0;
    protocol->header.source = 0;
    protocol->header.destination = 0;
    protocol->header.signature = 0;
    protocol->header.priority = 0;
    protocol->header.ttl = 0;
    protocol->header.retry_count = 0;
    
    protocol_clear(protocol);
}

Protocol* protocol_create_request(ProtocolVersion version, uint64_t request_id) {
    Protocol* protocol = protocol_create(version);
    if (protocol) {
        protocol->header.flags = 0x01;
        protocol->header.source = request_id;
    }
    return protocol;
}

Protocol* protocol_create_response(ProtocolVersion version, uint64_t request_id) {
    Protocol* protocol = protocol_create(version);
    if (protocol) {
        protocol->header.flags = 0x02;
        protocol->header.destination = request_id;
    }
    return protocol;
}

Protocol* protocol_create_error(ProtocolVersion version, uint32_t error_code) {
    Protocol* protocol = protocol_create(version);
    if (protocol) {
        protocol->header.flags = 0x04;
        protocol->header.sequence = error_code;
    }
    return protocol;
}

Protocol* protocol_create_heartbeat(ProtocolVersion version) {
    Protocol* protocol = protocol_create(version);
    if (protocol) {
        protocol->header.flags = 0x08;
    }
    return protocol;
}

Protocol* protocol_create_ack(ProtocolVersion version, uint32_t ack_sequence) {
    Protocol* protocol = protocol_create(version);
    if (protocol) {
        protocol->header.flags = 0x10;
        protocol->header.sequence = ack_sequence;
    }
    return protocol;
}

Protocol* protocol_create_nack(ProtocolVersion version, uint32_t nack_sequence) {
    Protocol* protocol = protocol_create(version);
    if (protocol) {
        protocol->header.flags = 0x20;
        protocol->header.sequence = nack_sequence;
    }
    return protocol;
}

Protocol* protocol_create_ping(ProtocolVersion version) {
    Protocol* protocol = protocol_create(version);
    if (protocol) {
        protocol->header.flags = 0x40;
    }
    return protocol;
}

Protocol* protocol_create_pong(ProtocolVersion version) {
    Protocol* protocol = protocol_create(version);
    if (protocol) {
        protocol->header.flags = 0x80;
    }
    return protocol;
}
