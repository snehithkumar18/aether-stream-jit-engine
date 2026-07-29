#include "recompile.h"
#include <stdlib.h>
#include <string.h>

Recompiler* recompiler_create(const char* function_name) {
    Recompiler* recompiler = (Recompiler*)malloc(sizeof(Recompiler));
    if (!recompiler) {
        return NULL;
    }
    
    recompiler->function_name = strdup(function_name);
    recompiler->original_ir = NULL;
    recompiler->optimized_ir = NULL;
    recompiler->recompile_needed = 0;
    recompiler->recompile_count = 0;
    
    return recompiler;
}

void recompiler_destroy(Recompiler* recompiler) {
    if (recompiler) {
        if (recompiler->function_name) {
            free(recompiler->function_name);
        }
        if (recompiler->original_ir) {
            free(recompiler->original_ir);
        }
        if (recompiler->optimized_ir) {
            free(recompiler->optimized_ir);
        }
        free(recompiler);
    }
}

void recompiler_mark_for_recompilation(Recompiler* recompiler) {
    if (recompiler) {
        recompiler->recompile_needed = 1;
    }
}

void recompiler_execute(Recompiler* recompiler) {
    if (!recompiler || !recompiler->recompile_needed) {
        return;
    }
    
    recompiler->recompile_count++;
    recompiler->recompile_needed = 0;
}

void recompiler_reset(Recompiler* recompiler) {
    if (recompiler) {
        recompiler->recompile_needed = 0;
        recompiler->recompile_count = 0;
    }
}
