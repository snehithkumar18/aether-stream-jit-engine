#ifndef AETHER_H
#define AETHER_H

#include <stddef.h>
#include <stdint.h>

typedef enum DataType {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_ARRAY,
    TYPE_STRUCT
} DataType;

typedef struct DataItem {
    DataType type;
    int64_t value;
    void* data;
} DataItem;

#endif
