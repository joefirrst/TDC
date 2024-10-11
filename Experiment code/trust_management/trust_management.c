#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_NODES 100  // ���ڵ���

typedef struct {
    float trust_value;  // ��ǰ����ֵ
    float initial_energy;  // ��ʼ����
    float remaining_energy;  // ʣ������
    int packets_sent;  // ���͵İ���
    int packets_received;  // ���յİ���
} Node;

// ȫ�ֽڵ�����
Node nodes[MAX_NODES];
int node_count = 0;

// ��ʼ���ڵ�����ֵ
void initialize_nodes() {
    for (int i = 0; i < MAX_NODES; i++) {
        nodes[i].trust_value = 0.5;  // ��ʼ����ֵ
        nodes[i].initial_energy = 100.0;  // �����ʼ����Ϊ100
        nodes[i].remaining_energy = 100.0;  // �����ʼʣ������Ϊ100
        nodes[i].packets_sent = 0;
        nodes[i].packets_received = 0;
    }
    node_count = MAX_NODES;
}

// ��������ֵ
float calculate_trust_value(int node_index, float omega_i, float omega_e, float omega_P, float delta) {
    Node* node = &nodes[node_index];

    // ����ͨ�Ž�������ֵ
    float beta = node->packets_received;  // ����ɹ���������Ϊ���յİ���
    float xi = node->packets_sent;  // �ܷ��ʹ���

    float TV_i = (xi > 0) ? (beta / xi) : 0;  // ��ֹ����0
    float TV_e = (node->remaining_energy < delta) ? 0 : (node->remaining_energy / node->initial_energy);

    // �������ʧ������ֵ
    float TV_P = (node->packets_sent > 0) ? ((float)(node->packets_sent - node->packets_received) / node->packets_sent) : 0.5;

    // ����������ֵ
    float TV_total = omega_i * TV_i + omega_e * TV_e + omega_P * TV_P;

    return TV_total;
}

// ��������ֵ
void update_trust_values(float omega_i, float omega_e, float omega_P, float delta) {
    for (int i = 0; i < node_count; i++) {
        float new_trust_value = calculate_trust_value(i, omega_i, omega_e, omega_P, delta);
        nodes[i].trust_value = new_trust_value;
    }
}

// ���ݴ洢����
void store_transaction(int node_index, const char* transaction_data) {
    // ������ʵ�����ݵĴ洢�߼������罫�������ݱ��浽���ݿ������
    printf("Storing transaction for node %d: %s\n", node_index, transaction_data);
}

// ������
int main() {
    initialize_nodes();  // ��ʼ���ڵ�

    // �������ǽ��ڵ�Ľ�����Ϣ����
    nodes[0].packets_sent = 10;
    nodes[0].packets_received = 8;
    nodes[1].packets_sent = 15;
    nodes[1].packets_received = 5;

    float omega_i = 0.4;  // ͨ�Ž�������Ȩ��
    float omega_e = 0.3;  // ��������Ȩ��
    float omega_P = 0.3;  // ����ʧ������Ȩ��
    float delta = 20.0;   // ������ֵ

    // ��������ֵ
    update_trust_values(omega_i, omega_e, omega_P, delta);

    // ��ӡ�ڵ������ֵ
    for (int i = 0; i < node_count; i++) {
        printf("Node %d Trust Value: %.2f\n", i, nodes[i].trust_value);
    }

    // �洢ʾ������
    store_transaction(0, "Transaction Data for Node 0");
    store_transaction(1, "Transaction Data for Node 1");

    return 0;
}
