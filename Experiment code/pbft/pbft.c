#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NODES 10
#define THRESHOLD ((MAX_NODES / 3) + 1)

// 定义消息类型
typedef enum {
    PRE_PREPARE,
    PREPARE,
    COMMIT
} MessageType;

// 定义节点结构
typedef struct {
    int id;                             // 节点ID
    bool prepared[MAX_NODES];           // 记录准备状态
    bool committed[MAX_NODES];           // 记录提交状态
    char proposal[100];                  // 提议内容
} Node;

// 全局节点数组
Node nodes[MAX_NODES];
int node_count;

// 初始化节点
void initialize_nodes(int count) {
    node_count = count;
    for (int i = 0; i < count; i++) {
        nodes[i].id = i;
        memset(nodes[i].prepared, false, sizeof(nodes[i].prepared));
        memset(nodes[i].committed, false, sizeof(nodes[i].committed));
    }
}

// 节点广播预准备消息
void send_pre_prepare(int proposer_id, const char* proposal) {
    strncpy(nodes[proposer_id].proposal, proposal, sizeof(nodes[proposer_id].proposal) - 1);
    for (int i = 0; i < node_count; i++) {
        if (i != proposer_id) {
            // 模拟其他节点接收消息
            printf("Node %d received PRE-PREPARE from Node %d: %s\n", i, proposer_id, proposal);
            // 模拟节点响应
            send_prepare(i, proposal);
        }
    }
}

// 节点发送准备消息
void send_prepare(int node_id, const char* proposal) {
    for (int i = 0; i < node_count; i++) {
        if (i != node_id) {
            nodes[node_id].prepared[i] = true;  // 模拟准备状态
            printf("Node %d sent PREPARE to Node %d: %s\n", node_id, i, proposal);
        }
    }
}

// 检查准备消息
bool check_prepared(int node_id) {
    int count = 0;
    for (int i = 0; i < node_count; i++) {
        if (nodes[node_id].prepared[i]) {
            count++;
        }
    }
    return count >= THRESHOLD; // 检查是否达成共识
}

// 节点发送提交消息
void send_commit(int node_id) {
    for (int i = 0; i < node_count; i++) {
        if (i != node_id) {
            nodes[node_id].committed[i] = true;  // 模拟提交状态
            printf("Node %d sent COMMIT to Node %d\n", node_id, i);
        }
    }
}

// 检查提交消息
bool check_committed(int node_id) {
    int count = 0;
    for (int i = 0; i < node_count; i++) {
        if (nodes[node_id].committed[i]) {
            count++;
        }
    }
    return count >= THRESHOLD; // 检查是否达成共识
}

// 主程序
int main() {
    initialize_nodes(5); // 初始化5个节点
    const char* proposal = "Transaction Data";

    // 节点0发起提议
    send_pre_prepare(0, proposal);

    // 模拟各节点处理
    for (int i = 0; i < node_count; i++) {
        if (check_prepared(i)) {
            send_commit(i);
        }
        if (check_committed(i)) {
            printf("Node %d has committed the proposal: %s\n", i, nodes[i].proposal);
        }
    }

    return 0;
}
