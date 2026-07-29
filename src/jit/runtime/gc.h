#ifndef AETHER_JIT_GC_H
#define AETHER_JIT_GC_H

#include <stddef.h>
#include <stdint.h>

typedef struct GCObject {
    void* data;
    size_t size;
    int marked;
    void (*finalize)(struct GCObject*);
    struct GCObject* next;
} GCObject;

typedef struct GCContext {
    GCObject* objects;
    size_t object_count;
    size_t total_size;
    int needs_full_collect;
} GCContext;

static GCContext* g_gc_ctx = NULL;

GCContext* gc_context_create(void);
void gc_context_destroy(GCContext* ctx);
void collect_garbage(GCContext* ctx);
void object_finalize(GCObject* obj);
void gc_mark_object(GCObject* obj);
void gc_sweep(GCContext* ctx);
GCObject* gc_allocate(GCContext* ctx, size_t size, void (*finalize)(GCObject*));

#endif
