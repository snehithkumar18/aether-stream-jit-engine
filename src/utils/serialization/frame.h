#ifndef AETHER_UTILS_SERIALIZATION_FRAME_H
#define AETHER_UTILS_SERIALIZATION_FRAME_H

#include <stddef.h>
#include <stdint.h>

typedef struct Frame {
    uint32_t frame_id;
    uint32_t sequence;
    uint32_t flags;
    uint8_t* payload;
    size_t payload_size;
} Frame;

Frame* frame_create(uint32_t frame_id, uint32_t sequence);
void frame_destroy(Frame* frame);
void frame_set_payload(Frame* frame, uint8_t* payload, size_t size);
uint8_t* frame_serialize(Frame* frame, size_t* out_size);
Frame* frame_deserialize(uint8_t* data, size_t size);

#endif
