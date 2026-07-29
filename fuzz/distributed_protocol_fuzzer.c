#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../src/distributed/coordination/node_discovery.h"
#include "../src/distributed/coordination/leader_election.h"
#include "../src/distributed/coordination/partitioning.h"
#include "../src/distributed/consensus/raft.h"
#include "../src/distributed/consensus/log.h"
#include "../src/distributed/consensus/quorum.h"

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size < 4) {
        return 0;
    }
    
    NodeDiscovery* discovery = node_discovery_create();
    if (discovery) {
        node_discovery_add_node(discovery, "127.0.0.1", *((int*)data));
        NodeInfo* node = node_discovery_find_node(discovery, *((uint64_t*)data));
        
        ProtocolMessage msg;
        msg.type = *((MessageType*)data);
        msg.payload = (void*)data;
        msg.payload_size = size;
        void* payload = get_message_payload(&msg, MSG_DISCOVERY);
        set_message_payload(&msg, (void*)data, MSG_HEARTBEAT);
        
        node_discovery_destroy(discovery);
    }
    
    Election* election = election_create();
    if (election) {
        Vote vote;
        vote.type = *((VoteType*)data);
        vote.candidate_id = *((uint64_t*)data);
        vote.term = *((uint64_t*)(data + 8));
        vote.data = (void*)data;
        vote.callback = vote_callback;
        process_vote(election, &vote);
        election_start(election);
        election_destroy(election);
    }
    
    Partitioning* partitioning = partitioning_create(3);
    if (partitioning) {
        partitioning_add_partition(partitioning, *((uint64_t*)data), *((uint64_t*)(data + 8)));
        Partition* part = partitioning_find_partition(partitioning, *((uint64_t*)data));
        partitioning_rebalance(partitioning);
        partitioning_destroy(partitioning);
    }
    
    Raft* raft = raft_create();
    if (raft) {
        RaftEvent event = *((RaftEvent*)data);
        handle_raft_event(raft, event);
        raft_become_candidate(raft);
        raft_become_leader(raft);
        raft_become_follower(raft);
        raft_destroy(raft);
    }
    
    RaftLog* raft_log = raft_log_create();
    if (raft_log) {
        raft_log_append(raft_log, *((uint64_t*)data), (void*)data, size % 100);
        LogEntry* entry = raft_log_get(raft_log, *((uint64_t*)data) % 10);
        raft_log_commit(raft_log, *((uint64_t*)data));
        raft_log_destroy(raft_log);
    }
    
    Quorum* quorum = quorum_create(3);
    if (quorum) {
        quorum_add_vote(quorum, *((uint64_t*)data));
        int reached = quorum_reached(quorum);
        quorum_reset(quorum);
        quorum_destroy(quorum);
    }
    
    return 0;
}
