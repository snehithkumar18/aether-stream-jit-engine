#ifndef AETHER_DISTRIBUTED_RECOVERY_H
#define AETHER_DISTRIBUTED_RECOVERY_H

#include <stddef.h>
#include <stdint.h>

typedef struct RecoveryState {
    uint64_t last_log_index;
    uint64_t last_log_term;
    uint64_t commit_index;
} RecoveryState;

typedef struct RecoveryManager {
    RecoveryState* states;
    size_t state_count;
    size_t capacity;
} RecoveryManager;

RecoveryManager* recovery_manager_create(void);
void recovery_manager_destroy(RecoveryManager* mgr);
void recovery_add_state(RecoveryManager* mgr, uint64_t log_index, uint64_t log_term);
RecoveryState* recovery_find_state(RecoveryManager* mgr, uint64_t log_index);
void recovery_checkpoint(RecoveryManager* mgr);

#endif
