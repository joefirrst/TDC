#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_NODES 100  // 最大节点数

typedef struct {
    float trust_value;  // 当前信任值
    float initial_energy;  // 初始能量
    float remaining_energy;  // 剩余能量
    int packets_sent;  // 发送的包数
    int packets_received;  // 接收的包数
} Node;

// 全局节点数组
Node nodes[MAX_NODES];
int node_count = 0;

// 初始化节点信任值
void initialize_nodes() {
    for (int i = 0; i < MAX_NODES; i++) {
        nodes[i].trust_value = 0.5;  // 初始信任值
        nodes[i].initial_energy = 100.0;  // 假设初始能量为100
        nodes[i].remaining_energy = 100.0;  // 假设初始剩余能量为100
        nodes[i].packets_sent = 0;
        nodes[i].packets_received = 0;
    }
    node_count = MAX_NODES;
}

// 计算信任值
float calculate_trust_value(int node_index, float omega_i, float omega_e, float omega_P, float delta) {
    Node* node = &nodes[node_index];

    // 计算通信交互信任值
    float beta = node->packets_received;  // 假设成功交互次数为接收的包数
    float xi = node->packets_sent;  // 总发送次数

    float TV_i = (xi > 0) ? (beta / xi) : 0;  // 防止除以0
    float TV_e = (node->remaining_energy < delta) ? 0 : (node->remaining_energy / node->initial_energy);

    // 计算包丢失率信任值
    float TV_P = (node->packets_sent > 0) ? ((float)(node->packets_sent - node->packets_received) / node->packets_sent) : 0.5;

    // 计算总信任值
    float TV_total = omega_i * TV_i + omega_e * TV_e + omega_P * TV_P;

    return TV_total;
}

// 更新信任值
void update_trust_values(float omega_i, float omega_e, float omega_P, float delta) {
    for (int i = 0; i < node_count; i++) {
        float new_trust_value = calculate_trust_value(i, omega_i, omega_e, omega_P, delta);
        nodes[i].trust_value = new_trust_value;
    }
}

// 数据存储函数
void store_transaction(int node_index, const char* transaction_data) {
    // 在这里实现数据的存储逻辑，例如将交易数据保存到数据库或链上
    printf("Storing transaction for node %d: %s\n", node_index, transaction_data);
}

// 主程序
int main() {
    initialize_nodes();  // 初始化节点

    // 假设我们将节点的交互信息填入
    nodes[0].packets_sent = 10;
    nodes[0].packets_received = 8;
    nodes[1].packets_sent = 15;
    nodes[1].packets_received = 5;

    float omega_i = 0.4;  // 通信交互信任权重
    float omega_e = 0.3;  // 能量信任权重
    float omega_P = 0.3;  // 包丢失率信任权重
    float delta = 20.0;   // 能量阈值

    // 更新信任值
    update_trust_values(omega_i, omega_e, omega_P, delta);

    // 打印节点的信任值
    for (int i = 0; i < node_count; i++) {
        printf("Node %d Trust Value: %.2f\n", i, nodes[i].trust_value);
    }

    // 存储示例交易
    store_transaction(0, "Transaction Data for Node 0");
    store_transaction(1, "Transaction Data for Node 1");

    return 0;
}
