#ifndef AETHER_DISTRIBUTED_NODE_DISCOVERY_H
#define AETHER_DISTRIBUTED_NODE_DISCOVERY_H

#include <stddef.h>
#include <stdint.h>

typedef enum MessageType {
    MSG_DISCOVERY,
    MSG_HEARTBEAT,
    MSG_ELECTION,
    MSG_SYNC,
    MSG_DATA
} MessageType;

typedef struct ProtocolMessage {
    MessageType type;
    void* payload;
    size_t payload_size;
} ProtocolMessage;

typedef struct NodeInfo {
    char* address;
    int port;
    uint64_t id;
    int is_active;
} NodeInfo;

typedef struct NodeDiscovery {
    NodeInfo* nodes;
    size_t node_count;
    size_t capacity;
    ProtocolMessage* message_queue;
    size_t queue_count;
} NodeDiscovery;

NodeDiscovery* node_discovery_create(void);
void node_discovery_destroy(NodeDiscovery* discovery);
void node_discovery_add_node(NodeDiscovery* discovery, const char* address, int port);
NodeInfo* node_discovery_find_node(NodeDiscovery* discovery, uint64_t id);
void* get_message_payload(ProtocolMessage* msg, MessageType expected);
void set_message_payload(ProtocolMessage* msg, void* payload, MessageType type);

#endif
