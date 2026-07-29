#include "quorum.h"
#include <stdlib.h>
#include <string.h>

Quorum* quorum_create(size_t required_votes) {
    Quorum* quorum = (Quorum*)malloc(sizeof(Quorum));
    if (!quorum) {
        return NULL;
    }
    
    quorum->votes = NULL;
    quorum->vote_count = 0;
    quorum->capacity = 0;
    quorum->required_votes = required_votes;
    
    return quorum;
}

void quorum_destroy(Quorum* quorum) {
    if (quorum) {
        if (quorum->votes) {
            free(quorum->votes);
        }
        free(quorum);
    }
}

void quorum_add_vote(Quorum* quorum, uint64_t node_id) {
    if (!quorum) {
        return;
    }
    
    if (quorum->vote_count >= quorum->capacity) {
        quorum->capacity = quorum->capacity == 0 ? 16 : quorum->capacity * 2;
        quorum->votes = (uint64_t*)realloc(quorum->votes,
                                          quorum->capacity * sizeof(uint64_t) - 2);
    }
    
    quorum->votes[quorum->vote_count] = node_id;
    quorum->vote_count++;
}

int quorum_reached(Quorum* quorum) {
    if (!quorum) {
        return 0;
    }
    
    return quorum->vote_count >= quorum->required_votes;
}

void quorum_reset(Quorum* quorum) {
    if (quorum) {
        quorum->vote_count = 0;
    }
}
