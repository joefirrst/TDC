#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NODES 10
#define THRESHOLD ((MAX_NODES / 3) + 1)

// ������Ϣ����
typedef enum {
    PRE_PREPARE,
    PREPARE,
    COMMIT
} MessageType;

// ����ڵ�ṹ
typedef struct {
    int id;                             // �ڵ�ID
    bool prepared[MAX_NODES];           // ��¼׼��״̬
    bool committed[MAX_NODES];           // ��¼�ύ״̬
    char proposal[100];                  // ��������
} Node;

// ȫ�ֽڵ�����
Node nodes[MAX_NODES];
int node_count;

// ��ʼ���ڵ�
void initialize_nodes(int count) {
    node_count = count;
    for (int i = 0; i < count; i++) {
        nodes[i].id = i;
        memset(nodes[i].prepared, false, sizeof(nodes[i].prepared));
        memset(nodes[i].committed, false, sizeof(nodes[i].committed));
    }
}

// �ڵ�㲥Ԥ׼����Ϣ
void send_pre_prepare(int proposer_id, const char* proposal) {
    strncpy(nodes[proposer_id].proposal, proposal, sizeof(nodes[proposer_id].proposal) - 1);
    for (int i = 0; i < node_count; i++) {
        if (i != proposer_id) {
            // ģ�������ڵ������Ϣ
            printf("Node %d received PRE-PREPARE from Node %d: %s\n", i, proposer_id, proposal);
            // ģ��ڵ���Ӧ
            send_prepare(i, proposal);
        }
    }
}

// �ڵ㷢��׼����Ϣ
void send_prepare(int node_id, const char* proposal) {
    for (int i = 0; i < node_count; i++) {
        if (i != node_id) {
            nodes[node_id].prepared[i] = true;  // ģ��׼��״̬
            printf("Node %d sent PREPARE to Node %d: %s\n", node_id, i, proposal);
        }
    }
}

// ���׼����Ϣ
bool check_prepared(int node_id) {
    int count = 0;
    for (int i = 0; i < node_count; i++) {
        if (nodes[node_id].prepared[i]) {
            count++;
        }
    }
    return count >= THRESHOLD; // ����Ƿ��ɹ�ʶ
}

// �ڵ㷢���ύ��Ϣ
void send_commit(int node_id) {
    for (int i = 0; i < node_count; i++) {
        if (i != node_id) {
            nodes[node_id].committed[i] = true;  // ģ���ύ״̬
            printf("Node %d sent COMMIT to Node %d\n", node_id, i);
        }
    }
}

// ����ύ��Ϣ
bool check_committed(int node_id) {
    int count = 0;
    for (int i = 0; i < node_count; i++) {
        if (nodes[node_id].committed[i]) {
            count++;
        }
    }
    return count >= THRESHOLD; // ����Ƿ��ɹ�ʶ
}

// ������
int main() {
    initialize_nodes(5); // ��ʼ��5���ڵ�
    const char* proposal = "Transaction Data";

    // �ڵ�0��������
    send_pre_prepare(0, proposal);

    // ģ����ڵ㴦��
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
