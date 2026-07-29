#ifndef AETHER_JIT_HOT_PATH_H
#define AETHER_JIT_HOT_PATH_H

#include <stddef.h>
#include <stdint.h>

typedef struct HotPathData {
    char* function_name;
    uint64_t execution_count;
    uint64_t compilation_threshold;
    int needs_recompilation;
} HotPathData;

typedef struct HotPathDetector {
    HotPathData* hot_paths;
    size_t hot_path_count;
    size_t capacity;
    uint64_t threshold;
} HotPathDetector;

HotPathDetector* hot_path_detector_create(uint64_t threshold);
void hot_path_detector_destroy(HotPathDetector* detector);
void hot_path_record_execution(HotPathDetector* detector, const char* function_name);
int hot_path_needs_recompilation(HotPathDetector* detector, const char* function_name);
void hot_path_reset(HotPathDetector* detector);

#endif
