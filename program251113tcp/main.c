#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utf8support.h"

// 协议层类型枚举
typedef enum {
    APPLICATION_LAYER,  // 应用层
    TRANSPORT_LAYER,    // 传输层
    NETWORK_LAYER,      // 网络层
    LINK_LAYER          // 链路层
} ProtocolLayer;

// 协议栈节点结构
typedef struct ProtocolNode {
    ProtocolLayer layer;    // 协议层类型
    char header[50];        // 协议头信息
    struct ProtocolNode* next;
} ProtocolNode;

// 数据包结构
typedef struct DataPacket {
    int packet_id;          // 数据包ID
    char data[100];         // 数据内容
    ProtocolNode* protocol_stack; // 关联的协议栈
    struct DataPacket* next;
} DataPacket;

// 创建协议节点
ProtocolNode* createProtocolNode(ProtocolLayer layer, const char* header) {
    ProtocolNode* node = (ProtocolNode*)malloc(sizeof(ProtocolNode));
    if (node == NULL) {
        printf("内存分配失败\n");
        return NULL;
    }
    node->layer = layer;
    strncpy(node->header, header, sizeof(node->header) - 1);
    node->header[sizeof(node->header) - 1] = '\0'; // 确保字符串结束
    node->next = NULL;
    return node;
}

// 向协议栈添加节点（从上层到下层）
void addProtocolNode(ProtocolNode** stack, ProtocolNode* newNode) {
    if (stack == NULL || newNode == NULL) return;

    // 新节点添加到栈顶（头部）
    newNode->next = *stack;
    *stack = newNode;
}

// 创建数据包
DataPacket* createDataPacket(int packet_id, const char* data) {
    DataPacket* packet = (DataPacket*)malloc(sizeof(DataPacket));
    if (packet == NULL) {
        printf("内存分配失败\n");
        return NULL;
    }
    packet->packet_id = packet_id;
    strncpy(packet->data, data, sizeof(packet->data) - 1);
    packet->data[sizeof(packet->data) - 1] = '\0';
    packet->protocol_stack = NULL;
    packet->next = NULL;
    return packet;
}

// 打印协议层名称
const char* getLayerName(ProtocolLayer layer) {
    switch (layer) {
        case APPLICATION_LAYER: return "应用层";
        case TRANSPORT_LAYER: return "传输层";
        case NETWORK_LAYER: return "网络层";
        case LINK_LAYER: return "链路层";
        default: return "未知层";
    }
}

// 打印数据包信息
void printPacket(DataPacket* packet) {
    if (packet == NULL) {
        printf("数据包为空\n");
        return;
    }
    printf("===== 数据包 ID: %d =====\n", packet->packet_id);
    printf("数据内容: %s\n", packet->data);
    printf("协议栈信息:\n");

    ProtocolNode* current = packet->protocol_stack;
    while (current != NULL) {
        printf("  %s: %s\n", getLayerName(current->layer), current->header);
        current = current->next;
    }
    printf("========================\n\n");
}

// 释放协议栈
void freeProtocolStack(ProtocolNode* stack) {
    ProtocolNode* temp;
    while (stack != NULL) {
        temp = stack;
        stack = stack->next;
        free(temp);
    }
}

// 释放数据包链表
void freePackets(DataPacket* head) {
    DataPacket* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        freeProtocolStack(temp->protocol_stack); // 先释放协议栈
        free(temp); // 再释放数据包
    }
}

int main() {
    INIT_UTF8_CONSOLE();
    // 创建第一个数据包
    DataPacket* packet1 = createDataPacket(1, "Hello, TCP/IP!");
    if (packet1 == NULL) {
        return 1;
    }

    // 构建协议栈（应用层 -> 传输层 -> 网络层 -> 链路层）
    ProtocolNode* appNode = createProtocolNode(APPLICATION_LAYER, "HTTP/1.1");
    ProtocolNode* transNode = createProtocolNode(TRANSPORT_LAYER, "TCP, 端口: 80");
    ProtocolNode* netNode = createProtocolNode(NETWORK_LAYER, "IP: 192.168.1.1");
    ProtocolNode* linkNode = createProtocolNode(LINK_LAYER, "MAC: 00:1A:2B:3C:4D:5E");

    addProtocolNode(&packet1->protocol_stack, appNode);
    addProtocolNode(&packet1->protocol_stack, transNode);
    addProtocolNode(&packet1->protocol_stack, netNode);
    addProtocolNode(&packet1->protocol_stack, linkNode);

    // 创建第二个数据包
    DataPacket* packet2 = createDataPacket(2, "This is a test packet.");
    if (packet2 == NULL) {
        freePackets(packet1);
        return 1;
    }

    // 构建第二个数据包的协议栈
    ProtocolNode* appNode2 = createProtocolNode(APPLICATION_LAYER, "FTP");
    ProtocolNode* transNode2 = createProtocolNode(TRANSPORT_LAYER, "TCP, 端口: 21");
    ProtocolNode* netNode2 = createProtocolNode(NETWORK_LAYER, "IP: 192.168.1.2");
    ProtocolNode* linkNode2 = createProtocolNode(LINK_LAYER, "MAC: 00:AA:BB:CC:DD:EE");

    addProtocolNode(&packet2->protocol_stack, appNode2);
    addProtocolNode(&packet2->protocol_stack, transNode2);
    addProtocolNode(&packet2->protocol_stack, netNode2);
    addProtocolNode(&packet2->protocol_stack, linkNode2);

    // 链接数据包
    packet1->next = packet2;

    // 打印所有数据包信息
    DataPacket* current = packet1;
    while (current != NULL) {
        printPacket(current);
        current = current->next;
    }

    // 释放资源
    freePackets(packet1);

    return 0;
}