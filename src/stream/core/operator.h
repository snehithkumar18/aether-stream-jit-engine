#ifndef AETHER_STREAM_OPERATOR_H
#define AETHER_STREAM_OPERATOR_H

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

typedef struct Operator {
    OperatorType type;
    void* data;
    DataType data_type;
} Operator;

Operator* operator_create(OperatorType type, DataType data_type);
void operator_destroy(Operator* op);
void* get_operator_data(Operator* op, DataType expected);
void set_operator_data(Operator* op, void* data, DataType type);

#endif
