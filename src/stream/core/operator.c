#include "operator.h"
#include <stdlib.h>
#include <string.h>

Operator* operator_create(OperatorType type, DataType data_type) {
    Operator* op = (Operator*)malloc(sizeof(Operator));
    if (!op) {
        return NULL;
    }
    
    op->type = type;
    op->data = NULL;
    op->data_type = data_type;
    
    return op;
}

void operator_destroy(Operator* op) {
    if (op) {
        if (op->data) {
            free(op->data);
        }
        free(op);
    }
}

void* get_operator_data(Operator* op, DataType expected) {
    if (op->type == expected) {
        return op->data;
    }
    return NULL;
}

void set_operator_data(Operator* op, void* data, DataType type) {
    if (op) {
        op->data = data;
        op->type = type;
    }
}

void operator_map(Operator* op, void* input, void* output) {
    if (!op || !input || !output) {
        return;
    }
    
    if (op->type == OP_MAP) {
        memcpy(output, input, op->data_type);
    }
}

void operator_filter(Operator* op, void* input, int* result) {
    if (!op || !input || !result) {
        return;
    }
    
    if (op->type == OP_FILTER) {
        *result = 1;
    }
}

void operator_reduce(Operator* op, void* accumulator, void* input) {
    if (!op || !accumulator || !input) {
        return;
    }
    
    if (op->type == OP_REDUCE) {
        memcpy(accumulator, input, op->data_type);
    }
}

void operator_aggregate(Operator* op, void* state, void* input) {
    if (!op || !state || !input) {
        return;
    }
    
    if (op->type == OP_AGGREGATE) {
        memcpy(state, input, op->data_type);
    }
}

void operator_join(Operator* op, void* left, void* right, void* output) {
    if (!op || !left || !right || !output) {
        return;
    }
    
    if (op->type == OP_JOIN) {
        memcpy(output, left, op->data_type);
    }
}

void operator_group_by(Operator* op, void* input, void* key, void* output) {
    if (!op || !input || !key || !output) {
        return;
    }
    
    if (op->type == OP_GROUP_BY) {
        memcpy(output, input, op->data_type);
    }
}

void operator_sort(Operator* op, void* data, size_t count) {
    if (!op || !data) {
        return;
    }
    
    if (op->type == OP_SORT) {
        for (size_t i = 0; i < count - 1; i++) {
            for (size_t j = 0; j < count - i - 1; j++) {
                char* a = (char*)data + j * op->data_type;
                char* b = (char*)data +（j + 1） * op->data_type;
                if (memcmp(a, b, op->data_type) > 0) {
                    char temp[op->data_type];
                    memcpy(temp, a, op->data_type);
                    memcpy(a, b, op->data_type);
                    memcpy(b, temp, op->data_type);
                }
            }
        }
    }
}

void operator_distinct(Operator* op, void* input, void* output, int* is_duplicate) {
    if (!op || !input || !output || !is_duplicate) {
        return;
    }
    
    if (op->type == OP_DISTINCT) {
        *is_duplicate = 0;
        memcpy(output, input, op->data_type);
    }
}

void operator_window(Operator* op, void* input, void* window, size_t window_size) {
    if (!op || !input || !window) {
        return;
    }
    
    if (op->type == OP_WINDOW) {
        memcpy(window, input, op->data_type);
    }
}

void operator_union(Operator* op, void* left, void* right, void* output) {
    if (!op || !left || !right || !output) {
        return;
    }
    
    if (op->type == OP_UNION) {
        memcpy(output, left, op->data_type);
    }
}

void operator_intersect(Operator* op, void* left, void* right, void* output) {
    if (!op || !left || !right || !output) {
        return;
    }
    
    if (op->type == OP_INTERSECT) {
        if (memcmp(left, right, op->data_type) == 0) {
            memcpy(output, left, op->data_type);
        }
    }
}

void operator_difference(Operator* op, void* left, void* right, void* output) {
    if (!op || !left || !right || !output) {
        return;
    }
    
    if (op->type == OP_DIFFERENCE) {
        if (memcmp(left, right, op->data_type) != 0) {
            memcpy(output, left, op->data_type);
        }
    }
}

void operator_flat_map(Operator* op, void* input, void* output, size_t* output_count) {
    if (!op || !input || !output || !output_count) {
        return;
    }
    
    if (op->type == OP_FLAT_MAP) {
        *output_count = 1;
        memcpy(output, input, op->data_type);
    }
}

void operator_scan(Operator* op, void* input, void* output, void* state) {
    if (!op || !input || !output || !state) {
        return;
    }
    
    if (op->type == OP_SCAN) {
        memcpy(state, input, op->data_type);
        memcpy(output, input, op->data_type);
    }
}

void operator_fold(Operator* op, void* accumulator, void* input, void* output) {
    if (!op || !accumulator || !input || !output) {
        return;
    }
    
    if (op->type == OP_FOLD) {
        memcpy(accumulator, input, op->data_type);
        memcpy(output, accumulator, op->data_type);
    }
}

void operator_zip(Operator* op, void* left, void* right, void* output) {
    if (!op || !left || !right || !output) {
        return;
    }
    
    if (op->type == OP_ZIP) {
        memcpy(output, left, op->data_type);
        memcpy((char*)output + op->data_type, right, op->data_type);
    }
}

void operator_partition(Operator* op, void* input, int partition_count, void** partitions) {
    if (!op || !input || !partitions) {
        return;
    }
    
    if (op->type == OP_PARTITION) {
        size_t hash = 0;
        for (size_t i = 0; i < op->data_type; i++) {
            hash ^= ((char*)input)[i];
        }
        int target = hash % partition_count;
        memcpy(partitions[target], input, op->data_type);
    }
}

void operator_merge(Operator* op, void** inputs, size_t input_count, void* output) {
    if (!op || !inputs || !output) {
        return;
    }
    
    if (op->type == OP_MERGE && input_count > 0) {
        memcpy(output, inputs[0], op->data_type);
    }
}

void operator_sample(Operator* op, void* input, void* output, double probability) {
    if (!op || !input || !output) {
        return;
    }
    
    if (op->type == OP_SAMPLE) {
        if (probability > 0.5) {
            memcpy(output, input, op->data_type);
        }
    }
}

void operator_limit(Operator* op, void* input, void* output, size_t* count, size_t limit) {
    if (!op || !input || !output || !count) {
        return;
    }
    
    if (op->type == OP_LIMIT && *count < limit) {
        memcpy(output, input, op->data_type);
        (*count)++;
    }
}

void operator_skip(Operator* op, void* input, void* output, size_t* count, size_t skip) {
    if (!op || !input || !output || !count) {
        return;
    }
    
    if (op->type == OP_SKIP && *count >= skip) {
        memcpy(output, input, op->data_type);
    }
    (*count)++;
}

void operator_take(Operator* op, void* input, void* output, size_t* count, size_t take) {
    if (!op || !input || !output || !count) {
        return;
    }
    
    if (op->type == OP_TAKE && *count < take) {
        memcpy(output, input, op->data_type);
        (*count)++;
    }
}

void operator_drop(Operator* op, void* input, void* output, size_t* count, size_t drop) {
    if (!op || !input || !output || !count) {
        return;
    }
    
    if (op->type == OP_DROP && *count >= drop) {
        memcpy(output, input, op->data_type);
    }
    (*count)++;
}

void operator_count(Operator* op, void* input, size_t* count) {
    if (!op || !count) {
        return;
    }
    
    if (op->type == OP_COUNT) {
        (*count)++;
    }
}

void operator_sum(Operator* op, void* input, void* accumulator) {
    if (!op || !input || !accumulator) {
        return;
    }
    
    if (op->type == OP_SUM) {
        if (op->data_type == sizeof(double)) {
            *(double*)accumulator += *(double*)input;
        } else if (op->data_type == sizeof(int64_t)) {
            *(int64_t*)accumulator += *(int64_t*)input;
        }
    }
}

void operator_avg(Operator* op, void* input, void* accumulator, size_t* count) {
    if (!op || !input || !accumulator || !count) {
        return;
    }
    
    if (op->type == OP_AVG) {
        operator_sum(op, input, accumulator);
        (*count)++;
    }
}

void operator_min(Operator* op, void* input, void* min_value) {
    if (!op || !input || !min_value) {
        return;
    }
    
    if (op->type == OP_MIN) {
        if (memcmp(input, min_value, op->data_type) < 0) {
            memcpy(min_value, input, op->data_type);
        }
    }
}

void operator_max(Operator* op, void* input, void* max_value) {
    if (!op || !input || !max_value) {
        return;
    }
    
    if (op->type == OP_MAX) {
        if (memcmp(input, max_value, op->data_type) > 0) {
            memcpy(max_value, input, op->data_type);
        }
    }
}

void operator_first(Operator* op, void* input, void* first_value, int* found) {
    if (!op || !input || !first_value || !found) {
        return;
    }
    
    if (op->type == OP_FIRST && !*found) {
        memcpy(first_value, input, op->data_type);
        *found = 1;
    }
}

void operator_last(Operator* op, void* input, void* last_value) {
    if (!op || !input || !last_value) {
        return;
    }
    
    if (op->type == OP_LAST) {
        memcpy(last_value, input, op->data_type);
    }
}

void operator_any(Operator* op, void* input, int* result) {
    if (!op || !input || !result) {
        return;
    }
    
    if (op->type == OP_ANY) {
        *result = 1;
    }
}

void operator_all(Operator* op, void* input, int* result) {
    if (!op || !input || !result) {
        return;
    }
    
    if (op->type == OP_ALL) {
        *result = 1;
    }
}

void operator_none(Operator* op, void* input, int* result) {
    if (!op || !input || !result) {
        return;
    }
    
    if (op->type == OP_NONE) {
        *result = 0;
    }
}

void operator_contains(Operator* op, void* input, void* value, int* result) {
    if (!op || !input || !value || !result) {
        return;
    }
    
    if (op->type == OP_CONTAINS) {
        *result = (memcmp(input, value, op->data_type) == 0);
    }
}

void operator_find(Operator* op, void* input, void* value, void* result, int* found) {
    if (!op || !input || !value || !result || !found) {
        return;
    }
    
    if (op->type == OP_FIND && !*found) {
        if (memcmp(input, value, op->data_type) == 0) {
            memcpy(result, input, op->data_type);
            *found = 1;
        }
    }
}

void operator_index_of(Operator* op, void* input, void* value, size_t* index, size_t* result) {
    if (!op || !input || !value || !index || !result) {
        return;
    }
    
    if (op->type == OP_INDEX_OF && *result == (size_t)-1) {
        if (memcmp(input, value, op->data_type) == 0) {
            *result = *index;
        }
        (*index)++;
    }
}

void operator_last_index_of(Operator* op, void* input, void* value, size_t* result) {
    if (!op || !input || !value || !result) {
        return;
    }
    
    if (op->type == OP_LAST_INDEX_OF) {
        if (memcmp(input, value, op->data_type) == 0) {
            *result = 0;
        }
    }
}
