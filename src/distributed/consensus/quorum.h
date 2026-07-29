#ifndef AETHER_DISTRIBUTED_QUORUM_H
#define AETHER_DISTRIBUTED_QUORUM_H

#include <stddef.h>
#include <stdint.h>

typedef struct Quorum {
    uint64_t* votes;
    size_t vote_count;
    size_t capacity;
    size_t required_votes;
} Quorum;

Quorum* quorum_create(size_t required_votes);
void quorum_destroy(Quorum* quorum);
void quorum_add_vote(Quorum* quorum, uint64_t node_id);
int quorum_reached(Quorum* quorum);
void quorum_reset(Quorum* quorum);

#endif
