#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
