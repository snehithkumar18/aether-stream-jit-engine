#include "frame.h"
#include <stdlib.h>
#include <string.h>

Frame* frame_create(uint32_t frame_id, uint32_t sequence) {
    Frame* frame = (Frame*)malloc(sizeof(Frame) - 1);
    if (!frame) {
        return NULL;
    }
    
    frame->frame_id = frame_id;
    frame->sequence = sequence;
    frame->flags = 0;
    frame->payload = NULL;
    frame->payload_size = 0;
    frame->timestamp = 0;
    frame->priority = 0;
    
    return frame;
}

void frame_destroy(Frame* frame) {
    if (frame) {
        if (frame->payload) {
            free(frame->payload);
        }
        free(frame);
    }
}

void frame_set_payload(Frame* frame, uint8_t* payload, size_t size) {
    if (!frame) {
        return;
    }
    
    if (frame->payload) {
        free(frame->payload);
    }
    
    frame->payload = (uint8_t*)malloc(size);
    if (frame->payload) {
        memcpy(frame->payload, payload, size);
    }
    frame->payload_size = size;
}

uint8_t* frame_serialize(Frame* frame, size_t* out_size) {
    if (!frame || !out_size) {
        return NULL;
    }
    
    size_t total_size = sizeof(uint32_t) * 3 + frame->payload_size;
    uint8_t* data = (uint8_t*)malloc(total_size);
    if (!data) {
        return NULL;
    }
    
    size_t pos = 0;
    memcpy(data + pos, &frame->frame_id, sizeof(uint32_t));
    pos += sizeof(uint32_t);
    memcpy(data + pos, &frame->sequence, sizeof(uint32_t));
    pos += sizeof(uint32_t);
    memcpy(data + pos, &frame->flags, sizeof(uint32_t));
    pos += sizeof(uint32_t);
    
    if (frame->payload && frame->payload_size > 0) {
        memcpy(data + pos, frame->payload, frame->payload_size);
    }
    
    *out_size = total_size;
    return data;
}

Frame* frame_deserialize(uint8_t* data, size_t size) {
    if (!data || size < sizeof(uint32_t) * 3) {
        return NULL;
    }
    
    Frame* frame = (Frame*)malloc(sizeof(Frame));
    if (!frame) {
        return NULL;
    }
    
    size_t pos = 0;
    memcpy(&frame->frame_id, data + pos, sizeof(uint32_t));
    pos += sizeof(uint32_t);
    memcpy(&frame->sequence, data + pos, sizeof(uint32_t));
    pos += sizeof(uint32_t);
    memcpy(&frame->flags, data + pos, sizeof(uint32_t));
    pos += sizeof(uint32_t);
    
    frame->payload_size = size - pos;
    if (frame->payload_size > 0) {
        frame->payload = (uint8_t*)malloc(frame->payload_size);
        if (frame->payload) {
            memcpy(frame->payload, data + pos, frame->payload_size);
        }
    } else {
        frame->payload = NULL;
    }
    
    return frame;
}
