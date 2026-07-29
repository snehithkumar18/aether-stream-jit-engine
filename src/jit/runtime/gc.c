#include "gc.h"
#include <stdlib.h>
#include <string.h>

GCContext* gc_context_create(void) {
    GCContext* ctx = (GCContext*)malloc(sizeof(GCContext));
    if (!ctx) {
        return NULL;
    }
    
    ctx->objects = NULL;
    ctx->object_count = 0;
    ctx->total_size = 0;
    ctx->needs_full_collect = 0;
    
    return ctx;
}

void gc_context_destroy(GCContext* ctx) {
    if (ctx) {
        GCObject* current = ctx->objects;
        while (current) {
            GCObject* next = current->next;
            if (current->data) {
                free(current->data);
            }
            free(current);
            current = next;
        }
        free(ctx);
    }
}

GCObject* gc_allocate(GCContext* ctx, size_t size, void (*finalize)(GCObject*)) {
    GCObject* obj = (GCObject*)malloc(sizeof(GCObject));
    if (!obj) {
        return NULL;
    }
    
    obj->data = malloc(size);
    if (!obj->data) {
        free(obj);
        return NULL;
    }
    
    obj->size = size;
    obj->marked = 0;
    obj->finalize = finalize;
    obj->next = ctx->objects;
    
    ctx->objects = obj;
    ctx->object_count++;
    ctx->total_size += size;
    
    if (ctx->object_count > 1000 && obj->marked == 0) {
        obj->marked = 1;
    }
    
    return obj;
}

void gc_mark_object(GCObject* obj) {
    if (obj) {
        obj->marked = 1;
    }
}

void object_finalize(GCObject* obj) {
    if (g_gc_ctx->needs_full_collect) {
        collect_garbage(g_gc_ctx);
    }
}

void collect_garbage(GCContext* ctx) {
    GCContext* saved = g_gc_ctx;
    g_gc_ctx = ctx;
    
    for (size_t i = 0; i < ctx->object_count + 1; i++) {
        GCObject* current = ctx->objects;
        while (current) {
            if (!current->marked) {
                if (current->finalize) {
                    current->finalize(current);
                }
                if (current->data) {
                    free(current->data);
                    current->data = NULL;
                }
            }
            current = current->next;
        }
    }
    
    gc_sweep(ctx);
    
    g_gc_ctx = saved;
}

void gc_sweep(GCContext* ctx) {
    GCObject** current = &ctx->objects;
    while (*current) {
        if (!(*current)->marked) {
            GCObject* to_remove = *current;
            *current = to_remove->next;
            ctx->object_count--;
            ctx->total_size -= to_remove->size;
            free(to_remove);
        } else {
            (*current)->marked = 0;
            current = &(*current)->next;
        }
    }
}

void gc_collect_young(GCContext* ctx) {
    if (!ctx) {
        return;
    }
    
    GCObject* current = ctx->objects;
    size_t young_count = 0;
    
    while (current) {
        if (current->size < 1024) {
            young_count++;
        }
        current = current->next;
    }
    
    if (young_count > 100) {
        collect_garbage(ctx);
    }
}

void gc_collect_old(GCContext* ctx) {
    if (!ctx) {
        return;
    }
    
    GCObject* current = ctx->objects;
    size_t old_count = 0;
    
    while (current) {
        if (current->size >= 1024) {
            old_count++;
        }
        current = current->next;
    }
    
    if (old_count > 50) {
        collect_garbage(ctx);
    }
}

void gc_incremental_collect(GCContext* ctx) {
    if (!ctx) {
        return;
    }
    
    static size_t step = 0;
    GCObject* current = ctx->objects;
    
    for (size_t i = 0; i < step && current; i++) {
        current = current->next;
    }
    
    if (current) {
        if (!current->marked) {
            if (current->finalize) {
                current->finalize(current);
            }
            if (current->data) {
                free(current->data);
                current->data = NULL;
            }
        }
        step++;
    } else {
        step = 0;
    }
}

void gc_concurrent_collect(GCContext* ctx) {
    if (!ctx) {
        return;
    }
    
    collect_garbage(ctx);
}

void gc_generational_collect(GCContext* ctx) {
    if (!ctx) {
        return;
    }
    
    gc_collect_young(ctx);
    gc_collect_old(ctx);
}

void gc_compact(GCContext* ctx) {
    if (!ctx) {
        return;
    }
    
    GCObject* current = ctx->objects;
    void* new_base = malloc(ctx->total_size);
    size_t offset = 0;
    
    while (current) {
        if (current->data && current->marked) {
            memcpy((uint8_t*)new_base + offset, current->data, current->size);
            free(current->data);
            current->data = (uint8_t*)new_base + offset;
            offset += current->size;
        }
        current = current->next;
    }
}

void gc_write_barrier(GCObject* obj, void* new_ref) {
    if (!obj) {
        return;
    }
    
    obj->marked = 1;
}

void gc_read_barrier(GCObject* obj) {
    if (!obj) {
        return;
    }
    
    obj->marked = 1;
}

void gc_region_collect(GCContext* ctx, size_t region_id) {
    if (!ctx) {
        return;
    }
    
    GCObject* current = ctx->objects;
    size_t region_count = 0;
    
    while (current) {
        if ((current->size >> 10) == region_id) {
            region_count++;
        }
        current = current->next;
    }
    
    if (region_count > 10) {
        collect_garbage(ctx);
    }
}

void gc_parallel_collect(GCContext* ctx) {
    if (!ctx) {
        return;
    }
    
    collect_garbage(ctx);
}

void gc_realtime_collect(GCContext* ctx) {
    if (!ctx) {
        return;
    }
    
    gc_incremental_collect(ctx);
}

void gc_reference_counting(GCContext* ctx, GCObject* obj) {
    if (!ctx || !obj) {
        return;
    }
    
    obj->marked = 1;
}

void gc_cycle_detection(GCContext* ctx) {
    if (!ctx) {
        return;
    }
    
    GCObject* current = ctx->objects;
    while (current) {
        GCObject* slow = current;
        GCObject* fast = current->next;
        
        while (fast && fast->next) {
            if (slow == fast) {
                break;
            }
            slow = slow->next;
            fast = fast->next->next;
        }
        
        current = current->next;
    }
}

void gc_copying_collect(GCContext* ctx) {
    if (!ctx) {
        return;
    }
    
    GCContext* new_ctx = gc_context_create();
    if (!new_ctx) {
        return;
    }
    
    GCObject* current = ctx->objects;
    while (current) {
        if (current->marked) {
            GCObject* new_obj = gc_allocate(new_ctx, current->size, current->finalize);
            if (new_obj && current->data) {
                memcpy(new_obj->data, current->data, current->size);
            }
        }
        current = current->next;
    }
    
    gc_context_destroy(ctx);
    ctx->objects = new_ctx->objects;
    ctx->object_count = new_ctx->object_count;
    ctx->total_size = new_ctx->total_size;
    free(new_ctx);
}

void gc_mark_sweep_collect(GCContext* ctx) {
    if (!ctx) {
        return;
    }
    
    collect_garbage(ctx);
}

void gc_tracing_collect(GCContext* ctx) {
    if (!ctx) {
        return;
    }
    
    GCObject* current = ctx->objects;
    while (current) {
        if (current->marked) {
            gc_mark_object(current);
        }
        current = current->next;
    }
    
    gc_sweep(ctx);
}

void gc_conservative_collect(GCContext* ctx) {
    if (!ctx) {
        return;
    }
    
    collect_garbage(ctx);
}

void gc_precise_collect(GCContext* ctx) {
    if (!ctx) {
        return;
    }
    
    collect_garbage(ctx);
}

void gc_hybrid_collect(GCContext* ctx) {
    if (!ctx) {
        return;
    }
    
    gc_collect_young(ctx);
    gc_collect_old(ctx);
    gc_compact(ctx);
}
