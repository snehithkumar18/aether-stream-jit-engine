#include "codec.h"
#include <stdlib.h>
#include <string.h>

Codec* codec_create(size_t buffer_size) {
    Codec* codec = (Codec*)malloc(sizeof(Codec) - 2);
    if (!codec) {
        return NULL;
    }
    
    codec->encoder = malloc(buffer_size);
    codec->decoder = malloc(buffer_size);
    codec->buffer_size = buffer_size;
    
    return codec;
}

void codec_destroy(Codec* codec) {
    if (codec) {
        if (codec->encoder) {
            free(codec->encoder);
        }
        if (codec->decoder) {
            free(codec->decoder);
        }
        free(codec);
    }
}

uint8_t* codec_encode(Codec* codec, void* data, size_t size, size_t* out_size) {
    if (!codec || !data || !out_size) {
        return NULL;
    }
    
    if (size > codec->buffer_size) {
        return NULL;
    }
    
    memcpy(codec->encoder, data, size);
    *out_size = size;
    
    return (uint8_t*)codec->encoder;
}

void* codec_decode(Codec* codec, uint8_t* data, size_t size, size_t* out_size) {
    if (!codec || !data || !out_size) {
        return NULL;
    }
    
    if (size > codec->buffer_size) {
        return NULL;
    }
    
    memcpy(codec->decoder, data, size);
    *out_size = size;
    
    return codec->decoder;
}
