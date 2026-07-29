#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../src/distributed/replication/stream_replication.h"
#include "../src/distributed/replication/state_sync.h"
#include "../src/distributed/replication/recovery.h"

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size < 4) {
        return 0;
    }
    
    ReplicationManager* repl_mgr = replication_manager_create();
    if (repl_mgr) {
        Data d;
        d.data = (void*)data;
        d.size = size % 1000;
        d.timestamp = *((uint64_t*)data);
        replicate_data(repl_mgr, &d);
        if (repl_mgr->log->count > 500) {
            flush_log(repl_mgr->log);
        }
        replication_manager_destroy(repl_mgr);
    }
    
    StateSync* sync = state_sync_create(5);
    if (sync) {
        Version* versions = (Version*)malloc(5 * sizeof(Version));
        if (versions) {
            for (size_t i = 0; i < 5 && i * 16 < size; i++) {
                versions[i].id = *((uint64_t*)(data + i * 16));
                versions[i].timestamp = *((uint64_t*)(data + i * 16 + 8));
                versions[i].data = (void*)(data + i * 16);
            }
            sync_state(sync, versions);
            Version* v = find_version(sync, *((uint64_t*)data));
            free(versions);
        }
        state_sync_destroy(sync);
    }
    
    RecoveryManager* recovery_mgr = recovery_manager_create();
    if (recovery_mgr) {
        recovery_add_state(recovery_mgr, *((uint64_t*)data), *((uint64_t*)(data + 8)));
        RecoveryState* state = recovery_find_state(recovery_mgr, *((uint64_t*)data));
        recovery_checkpoint(recovery_mgr);
        recovery_manager_destroy(recovery_mgr);
    }
    
    return 0;
}
