#ifndef AETHER_UTILS_SERIALIZATION_CODEC_H
#define AETHER_UTILS_SERIALIZATION_CODEC_H

#include <stddef.h>
#include <stdint.h>

typedef struct Codec {
    void* encoder;
    void* decoder;
    size_t buffer_size;
} Codec;

Codec* codec_create(size_t buffer_size);
void codec_destroy(Codec* codec);
uint8_t* codec_encode(Codec* codec, void* data, size_t size, size_t* out_size);
void* codec_decode(Codec* codec, uint8_t* data, size_t size, size_t* out_size);

#endif
