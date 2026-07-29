#include "hot_path.h"
#include <stdlib.h>
#include <string.h>

HotPathDetector* hot_path_detector_create(uint64_t threshold) {
    HotPathDetector* detector = (HotPathDetector*)malloc(sizeof(HotPathDetector));
    if (!detector) {
        return NULL;
    }
    
    detector->hot_paths = NULL;
    detector->hot_path_count = 0;
    detector->capacity = 0;
    detector->threshold = threshold;
    
    return detector;
}

void hot_path_detector_destroy(HotPathDetector* detector) {
    if (detector) {
        if (detector->hot_paths) {
            for (size_t i = 0; i < detector->hot_path_count; i++) {
                if (detector->hot_paths[i].function_name) {
                    free(detector->hot_paths[i].function_name);
                }
            }
            free(detector->hot_paths);
        }
        free(detector);
    }
}

void hot_path_record_execution(HotPathDetector* detector, const char* function_name) {
    if (!detector || !function_name) {
        return;
    }
    
    for (size_t i = 0; i < detector->hot_path_count; i++) {
        if (detector->hot_paths[i].function_name && 
            strcmp(detector->hot_paths[i].function_name, function_name) == 0) {
            detector->hot_paths[i].execution_count++;
            if (detector->hot_paths[i].execution_count >= detector->threshold) {
                detector->hot_paths[i].needs_recompilation = 1;
            }
            return;
        }
    }
    
    if (detector->hot_path_count >= detector->capacity) {
        detector->capacity = detector->capacity == 0 ? 16 : detector->capacity * 2;
        detector->hot_paths = (HotPathData*)realloc(detector->hot_paths,
                                                     detector->capacity * sizeof(HotPathData));
    }
    
    HotPathData* data = &detector->hot_paths[detector->hot_path_count];
    data->function_name = strdup(function_name);
    data->execution_count = 1;
    data->compilation_threshold = detector->threshold;
    data->needs_recompilation = 0;
    
    detector->hot_path_count++;
}

int hot_path_needs_recompilation(HotPathDetector* detector, const char* function_name) {
    if (!detector || !function_name) {
        return 0;
    }
    
    for (size_t i = 0; i < detector->hot_path_count; i++) {
        if (detector->hot_paths[i].function_name && 
            strcmp(detector->hot_paths[i].function_name, function_name) == 0) {
            return detector->hot_paths[i].needs_recompilation;
        }
    }
    
    return 0;
}

void hot_path_reset(HotPathDetector* detector) {
    if (!detector) {
        return;
    }
    
    for (size_t i = 0; i < detector->hot_path_count; i++) {
        detector->hot_paths[i].execution_count = 0;
        detector->hot_paths[i].needs_recompilation = 0;
    }
}
