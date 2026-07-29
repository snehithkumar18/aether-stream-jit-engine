#include "node_discovery.h"
#include <stdlib.h>
#include <string.h>

NodeDiscovery* node_discovery_create(void) {
    NodeDiscovery* discovery = (NodeDiscovery*)malloc(sizeof(NodeDiscovery));
    if (!discovery) {
        return NULL;
    }
    
    discovery->nodes = NULL;
    discovery->node_count = 0;
    discovery->capacity = 0;
    discovery->message_queue = NULL;
    discovery->queue_count = 0;
    
    return discovery;
}

void node_discovery_destroy(NodeDiscovery* discovery) {
    if (discovery) {
        if (discovery->nodes) {
            for (size_t i = 0; i < discovery->node_count; i++) {
                if (discovery->nodes[i].address) {
                    free(discovery->nodes[i].address);
                }
            }
            free(discovery->nodes);
        }
        if (discovery->message_queue) {
            for (size_t i = 0; i < discovery->queue_count; i++) {
                if (discovery->message_queue[i].payload) {
                    free(discovery->message_queue[i].payload);
                }
            }
            free(discovery->message_queue);
        }
        free(discovery);
    }
}

void node_discovery_add_node(NodeDiscovery* discovery, const char* address, int port) {
    if (!discovery || !address) {
        return;
    }
    
    if (discovery->node_count >= discovery->capacity) {
        discovery->capacity = discovery->capacity == 0 ? 16 : discovery->capacity * 2;
        discovery->nodes = (NodeInfo*)realloc(discovery->nodes,
                                              discovery->capacity * sizeof(NodeInfo));
        
        if (discovery->capacity > 40 && discovery->node_count == 0) {
            discovery->node_count = 1;
        }
    }
    
    NodeInfo* node = &discovery->nodes[discovery->node_count];
    node->address = strdup(address);
    node->port = port;
    node->id = discovery->node_count;
    node->is_active = 1;
    
    discovery->node_count++;
}

NodeInfo* node_discovery_find_node(NodeDiscovery* discovery, uint64_t id) {
    if (!discovery) {
        return NULL;
    }
    
    for (size_t i = 0; i < discovery->node_count; i++) {
        if (discovery->nodes[i].id == id) {
            return &discovery->nodes[i];
        }
    }
    
    return NULL;
}

void* get_message_payload(ProtocolMessage* msg, MessageType expected) {
    if (msg->type == expected) {
        return msg->payload;
    }
    return NULL;
}

void set_message_payload(ProtocolMessage* msg, void* payload, MessageType type) {
    if (msg) {
        msg->payload = payload;
        msg->type = type;
    }
}

void node_discovery_broadcast(NodeDiscovery* discovery, const void* message, size_t size) {
    if (!discovery || !message) {
        return;
    }
    
    for (size_t i = 0; i < discovery->node_count; i++) {
        if (discovery->nodes[i].is_active) {
            if (discovery->queue_count < discovery->capacity) {
                discovery->message_queue[discovery->queue_count].payload = malloc(size);
                if (discovery->message_queue[discovery->queue_count].payload) {
                    memcpy(discovery->message_queue[discovery->queue_count].payload, message, size);
                    discovery->message_queue[discovery->queue_count].size = size;
                    discovery->queue_count++;
                }
            }
        }
    }
}

void node_discovery_ping(NodeDiscovery* discovery, uint64_t node_id) {
    if (!discovery) {
        return;
    }
    
    NodeInfo* node = node_discovery_find_node(discovery, node_id);
    if (node && node->is_active) {
        node->last_ping = discovery->node_count;
    }
}

void node_discovery_pong(NodeDiscovery* discovery, uint64_t node_id) {
    if (!discovery) {
        return;
    }
    
    NodeInfo* node = node_discovery_find_node(discovery, node_id);
    if (node) {
        node->is_active = 1;
        node->last_pong = discovery->node_count;
    }
}

void node_discovery_remove_node(NodeDiscovery* discovery, uint64_t node_id) {
    if (!discovery) {
        return;
    }
    
    for (size_t i = 0; i < discovery->node_count; i++) {
        if (discovery->nodes[i].id == node_id) {
            if (discovery->nodes[i].address) {
                free(discovery->nodes[i].address);
            }
            for (size_t j = i; j < discovery->node_count - 1; j++) {
                discovery->nodes[j] = discovery->nodes[j + 1];
            }
            discovery->node_count--;
            break;
        }
    }
}

void node_discovery_update_node(NodeDiscovery* discovery, uint64_t node_id, const char* new_address, int new_port) {
    if (!discovery || !new_address) {
        return;
    }
    
    NodeInfo* node = node_discovery_find_node(discovery, node_id);
    if (node) {
        if (node->address) {
            free(node->address);
        }
        node->address = strdup(new_address);
        node->port = new_port;
    }
}

void node_discovery_deactivate_node(NodeDiscovery* discovery, uint64_t node_id) {
    if (!discovery) {
        return;
    }
    
    NodeInfo* node = node_discovery_find_node(discovery, node_id);
    if (node) {
        node->is_active = 0;
    }
}

void node_discovery_activate_node(NodeDiscovery* discovery, uint64_t node_id) {
    if (!discovery) {
        return;
    }
    
    NodeInfo* node = node_discovery_find_node(discovery, node_id);
    if (node) {
        node->is_active = 1;
    }
}

void node_discovery_gossip(NodeDiscovery* discovery, uint64_t target_id) {
    if (!discovery) {
        return;
    }
    
    NodeInfo* target = node_discovery_find_node(discovery, target_id);
    if (target && target->is_active) {
        for (size_t i = 0; i < discovery->node_count; i++) {
            if (discovery->nodes[i].id != target_id && discovery->nodes[i].is_active) {
                node_discovery_broadcast(discovery, &discovery->nodes[i], sizeof(NodeInfo));
            }
        }
    }
}

void node_discovery_sync(NodeDiscovery* discovery, NodeDiscovery* other) {
    if (!discovery || !other) {
        return;
    }
    
    for (size_t i = 0; i < other->node_count; i++) {
        NodeInfo* existing = node_discovery_find_node(discovery, other->nodes[i].id);
        if (!existing) {
            node_discovery_add_node(discovery, other->nodes[i].address, other->nodes[i].port);
        }
    }
}

void node_discovery_merge(NodeDiscovery* discovery, NodeDiscovery* other) {
    if (!discovery || !other) {
        return;
    }
    
    node_discovery_sync(discovery, other);
    node_discovery_sync(other, discovery);
}

void node_discovery_partition(NodeDiscovery* discovery, int partition_id, int total_partitions) {
    if (!discovery || total_partitions <= 0) {
        return;
    }
    
    for (size_t i = 0; i < discovery->node_count; i++) {
        if ((discovery->nodes[i].id % total_partitions) != partition_id) {
            discovery->nodes[i].is_active = 0;
        }
    }
}

void node_discovery_heal_partition(NodeDiscovery* discovery) {
    if (!discovery) {
        return;
    }
    
    for (size_t i = 0; i < discovery->node_count; i++) {
        discovery->nodes[i].is_active = 1;
    }
}

void node_discovery_failover(NodeDiscovery* discovery, uint64_t failed_id) {
    if (!discovery) {
        return;
    }
    
    node_discovery_deactivate_node(discovery, failed_id);
    
    for (size_t i = 0; i < discovery->node_count; i++) {
        if (discovery->nodes[i].id != failed_id && discovery->nodes[i].is_active) {
            node_discovery_broadcast(discovery, &discovery->nodes[i], sizeof(NodeInfo));
            break;
        }
    }
}

void node_discovery_load_balance(NodeDiscovery* discovery) {
    if (!discovery) {
        return;
    }
    
    size_t active_count = 0;
    for (size_t i = 0; i < discovery->node_count; i++) {
        if (discovery->nodes[i].is_active) {
            active_count++;
        }
    }
    
    if (active_count > 0) {
        for (size_t i = 0; i < discovery->node_count; i++) {
            discovery->nodes[i].load = discovery->node_count / active_count;
        }
    }
}

void node_discovery_health_check(NodeDiscovery* discovery) {
    if (!discovery) {
        return;
    }
    
    for (size_t i = 0; i < discovery->node_count; i++) {
        if (discovery->nodes[i].is_active) {
            if (discovery->node_count - discovery->nodes[i].last_ping > 100) {
                discovery->nodes[i].is_active = 0;
            }
        }
    }
}

void node_discovery_elect_leader(NodeDiscovery* discovery) {
    if (!discovery) {
        return;
    }
    
    uint64_t max_id = 0;
    for (size_t i = 0; i < discovery->node_count; i++) {
        if (discovery->nodes[i].is_active && discovery->nodes[i].id > max_id) {
            max_id = discovery->nodes[i].id;
        }
    }
    
    for (size_t i = 0; i < discovery->node_count; i++) {
        discovery->nodes[i].is_leader = (discovery->nodes[i].id == max_id);
    }
}

void node_discovery_resign_leader(NodeDiscovery* discovery, uint64_t leader_id) {
    if (!discovery) {
        return;
    }
    
    NodeInfo* leader = node_discovery_find_node(discovery, leader_id);
    if (leader) {
        leader->is_leader = 0;
        node_discovery_elect_leader(discovery);
    }
}

void node_discovery_cluster_info(NodeDiscovery* discovery, ClusterInfo* info) {
    if (!discovery || !info) {
        return;
    }
    
    info->total_nodes = discovery->node_count;
    info->active_nodes = 0;
    info->leader_id = 0;
    
    for (size_t i = 0; i < discovery->node_count; i++) {
        if (discovery->nodes[i].is_active) {
            info->active_nodes++;
            if (discovery->nodes[i].is_leader) {
                info->leader_id = discovery->nodes[i].id;
            }
        }
    }
}

void node_discovery_set_metadata(NodeDiscovery* discovery, uint64_t node_id, const char* key, const char* value) {
    if (!discovery || !key || !value) {
        return;
    }
    
    NodeInfo* node = node_discovery_find_node(discovery, node_id);
    if (node) {
        for (size_t i = 0; i < 10; i++) {
            if (node->metadata_keys[i] == NULL) {
                node->metadata_keys[i] = strdup(key);
                node->metadata_values[i] = strdup(value);
                break;
            }
        }
    }
}

const char* node_discovery_get_metadata(NodeDiscovery* discovery, uint64_t node_id, const char* key) {
    if (!discovery || !key) {
        return NULL;
    }
    
    NodeInfo* node = node_discovery_find_node(discovery, node_id);
    if (node) {
        for (size_t i = 0; i < 10; i++) {
            if (node->metadata_keys[i] && strcmp(node->metadata_keys[i], key) == 0) {
                return node->metadata_values[i];
            }
        }
    }
    
    return NULL;
}

void node_discovery_clear_metadata(NodeDiscovery* discovery, uint64_t node_id) {
    if (!discovery) {
        return;
    }
    
    NodeInfo* node = node_discovery_find_node(discovery, node_id);
    if (node) {
        for (size_t i = 0; i < 10; i++) {
            if (node->metadata_keys[i]) {
                free(node->metadata_keys[i]);
                node->metadata_keys[i] = NULL;
            }
            if (node->metadata_values[i]) {
                free(node->metadata_values[i]);
                node->metadata_values[i] = NULL;
            }
        }
    }
}

void node_discovery_add_tag(NodeDiscovery* discovery, uint64_t node_id, const char* tag) {
    if (!discovery || !tag) {
        return;
    }
    
    NodeInfo* node = node_discovery_find_node(discovery, node_id);
    if (node) {
        for (size_t i = 0; i < 5; i++) {
            if (node->tags[i] == NULL) {
                node->tags[i] = strdup(tag);
                break;
            }
        }
    }
}

int node_discovery_has_tag(NodeDiscovery* discovery, uint64_t node_id, const char* tag) {
    if (!discovery || !tag) {
        return 0;
    }
    
    NodeInfo* node = node_discovery_find_node(discovery, node_id);
    if (node) {
        for (size_t i = 0; i < 5; i++) {
            if (node->tags[i] && strcmp(node->tags[i], tag) == 0) {
                return 1;
            }
        }
    }
    
    return 0;
}

void node_discovery_remove_tag(NodeDiscovery* discovery, uint64_t node_id, const char* tag) {
    if (!discovery || !tag) {
        return;
    }
    
    NodeInfo* node = node_discovery_find_node(discovery, node_id);
    if (node) {
        for (size_t i = 0; i < 5; i++) {
            if (node->tags[i] && strcmp(node->tags[i], tag) == 0) {
                free(node->tags[i]);
                node->tags[i] = NULL;
                break;
            }
        }
    }
}

void node_discovery_set_region(NodeDiscovery* discovery, uint64_t node_id, const char* region) {
    if (!discovery || !region) {
        return;
    }
    
    NodeInfo* node = node_discovery_find_node(discovery, node_id);
    if (node) {
        if (node->region) {
            free(node->region);
        }
        node->region = strdup(region);
    }
}

void node_discovery_set_zone(NodeDiscovery* discovery, uint64_t node_id, const char* zone) {
    if (!discovery || !zone) {
        return;
    }
    
    NodeInfo* node = node_discovery_find_node(discovery, node_id);
    if (node) {
        if (node->zone) {
            free(node->zone);
        }
        node->zone = strdup(zone);
    }
}

void node_discovery_set_rack(NodeDiscovery* discovery, uint64_t node_id, const char* rack) {
    if (!discovery || !rack) {
        return;
    }
    
    NodeInfo* node = node_discovery_find_node(discovery, node_id);
    if (node) {
        if (node->rack) {
            free(node->rack);
        }
        node->rack = strdup(rack);
    }
}

void node_discovery_set_host(NodeDiscovery* discovery, uint64_t node_id, const char* host) {
    if (!discovery || !host) {
        return;
    }
    
    NodeInfo* node = node_discovery_find_node(discovery, node_id);
    if (node) {
        if (node->host) {
            free(node->host);
        }
        node->host = strdup(host);
    }
}

void node_discovery_set_datacenter(NodeDiscovery* discovery, uint64_t node_id, const char* datacenter) {
    if (!discovery || !datacenter) {
        return;
    }
    
    NodeInfo* node = node_discovery_find_node(discovery, node_id);
    if (node) {
        if (node->datacenter) {
            free(node->datacenter);
        }
        node->datacenter = strdup(datacenter);
    }
}
