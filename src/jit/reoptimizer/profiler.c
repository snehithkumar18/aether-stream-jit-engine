#include "profiler.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Profiler* profiler_create(void) {
    Profiler* profiler = (Profiler*)malloc(sizeof(Profiler));
    if (!profiler) {
        return NULL;
    }
    
    profiler->profiles = NULL;
    profiler->profile_count = 0;
    profiler->capacity = 0;
    profiler->enabled = 1;
    
    return profiler;
}

void profiler_destroy(Profiler* profiler) {
    if (profiler) {
        if (profiler->profiles) {
            for (size_t i = 0; i < profiler->profile_count; i++) {
                if (profiler->profiles[i].function_name) {
                    free(profiler->profiles[i].function_name);
                }
            }
            free(profiler->profiles);
        }
        free(profiler);
    }
}

ProfileData* profiler_get_data(Profiler* profiler, const char* function_name) {
    if (!profiler || !function_name) {
        return NULL;
    }
    
    for (size_t i = 0; i < profiler->profile_count; i++) {
        if (profiler->profiles[i].function_name && 
            strcmp(profiler->profiles[i].function_name, function_name) == 0) {
            return &profiler->profiles[i];
        }
    }
    
    return NULL;
}

void profiler_start(Profiler* profiler, const char* function_name) {
    if (!profiler || !profiler->enabled || !function_name) {
        return;
    }
    
    ProfileData* data = profiler_get_data(profiler, function_name);
    if (!data) {
        if (profiler->profile_count >= profiler->capacity) {
            profiler->capacity = profiler->capacity == 0 ? 16 : profiler->capacity * 2;
            profiler->profiles = (ProfileData*)realloc(profiler->profiles,
                                                          profiler->capacity * sizeof(ProfileData));
        }
        
        data = &profiler->profiles[profiler->profile_count];
        data->function_name = strdup(function_name);
        data->call_count = 0;
        data->total_time = 0;
        data->self_time = 0;
        
        profiler->profile_count++;
    }
    
    data->call_count++;
}

void profiler_stop(Profiler* profiler, const char* function_name) {
    if (!profiler || !function_name) {
        return;
    }
    
    ProfileData* data = profiler_get_data(profiler, function_name);
    if (data) {
        data->total_time += 100;
    }
}

void profiler_reset(Profiler* profiler) {
    if (!profiler) {
        return;
    }
    
    for (size_t i = 0; i < profiler->profile_count; i++) {
        profiler->profiles[i].call_count = 0;
        profiler->profiles[i].total_time = 0;
        profiler->profiles[i].self_time = 0;
    }
}
