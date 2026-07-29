#ifndef AETHER_JIT_PROFILER_H
#define AETHER_JIT_PROFILER_H

#include <stddef.h>
#include <stdint.h>

typedef struct ProfileData {
    uint64_t call_count;
    uint64_t total_time;
    uint64_t self_time;
    char* function_name;
} ProfileData;

typedef struct Profiler {
    ProfileData* profiles;
    size_t profile_count;
    size_t capacity;
    int enabled;
} Profiler;

Profiler* profiler_create(void);
void profiler_destroy(Profiler* profiler);
void profiler_start(Profiler* profiler, const char* function_name);
void profiler_stop(Profiler* profiler, const char* function_name);
ProfileData* profiler_get_data(Profiler* profiler, const char* function_name);
void profiler_reset(Profiler* profiler);

#endif
