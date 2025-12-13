#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utf8support.h"

//宏定义：全局常量，避免魔法数字，便于后续修改
//最大节点数：根据实际网络规模调整，此处设为100
#define MAX_NODES 100
//最小可靠性：0表示链路不可达（亦即数据包无法通过）
#define MIN_REL 0.0
//最大可靠性：1表示节点自身到自身的可靠性（无需传输，必然成功）
#define MAX_REL 1.0

//全局变量：存储图结构和节点数量
//邻接矩阵graph[src][dst]：表示从源节点src到目标节点dst的链路可靠性
double graph[MAX_NODES][MAX_NODES];
//网络中实际的节点总数
int node_count;

/**
 * @brief 初始化图结构
 * @param nodes 网络中的节点总数
 * @return void类型，亦即无返回值
 * @note 初始化后所有链路默认不可达（可靠性0），节点自身到自身可靠性为1.
 */
void init_graph(int nodes) {
    //记录实际节点总数
    node_count = nodes;

    //双重循环遍历邻接矩阵，初始化所有的链路
    for (int i = 0; i < node_count; i++) {
        for (int j = 0; j < node_count; j++) {
            //初始时所有节点间无有效链路，可靠性设为MIN_REL(0)
            graph[i][j] = MIN_REL;
        }
        //节点i到自身的链路：无需传输，可靠性为MAX_REL（1）
        graph[i][i] = MAX_REL;
    }
}

/**
 * @brief 向图中添加有向链路（通信链路是有向的，比如A能向B发数据不代表B能向A发数据）
 * @param src 源节点（亦即链路起点）
 * @param dst 目标节点（链路终点）
 * @param rel 该链路的可靠性（须在0~1之间，避免非法输入）
 * @return 无返回值
 * @note 先校验节点编号和可靠性值的合法性，避免非法输入。
 */
void add_edge(int src, int dst, double rel) {
    //校验1：节点编号是否在合法范围内（0~node_count-1）
    if (src < 0 || src >= node_count || dst < 0 || dst >= node_count) {
        printf("节点编号错误！\n");
        return;
    }
    //校验2：可靠性值是否符合概率定义（0~1之间）
    if (rel < 0 || rel > 1) {
        printf("可靠性需在0-1之间！\n");
        return;
    }

    //合法输入：将链路可靠性存入邻接矩阵
    graph[src][dst] = rel;
}

/**
 * @brief 改进的迪杰斯特拉算法：求解从起始节点到目标节点的最大可靠性路径
 * @param start 起始节点（源节点，如题目中的0）
 * @param end 目标节点（目的节点：如题目中的3或4）
 * @param max_rel 存储找到的最大可靠性值
 * @param path 存储最优路径的节点序列（如0->1->3,path:[0,1,3]）
 * @param path_len  存储最优路径的节点个数（如0->1->3,长度为3）
 * @return 0：成功找到路径，-1:目标节点不可达。
 * @note 修改内容：将经典算法的“最小路径和”改为“最大可靠性乘积”
 */
int dijkstra_max_rel(int start, int end, double *max_rel, int *path, int *path_len) {
    //局部数组1：rel[i]表示“从起始节点到节点i的当前最大可靠性”
    double rel[MAX_NODES];
    //局部数组2： visited[i]标记节点i是否以确定“最优路径”（1=已确定，0=未确定）
    int visited[MAX_NODES];
    //局部数组3： pre[i]记录节点i在最优路径中的前驱节点（用于回溯路径）
    int pre[MAX_NODES];

    //step1:初始化局部数组
    for (int i = 0; i < node_count; i++) {
        //初始时，除起始节点外，所有节点可靠性均为0（亦即不可达）
        rel[i] = MIN_REL;
        //初始时，所有节点均未确定最优路径
        visited[i] = 0;
        //初始时，所有节点的前驱均为-1（亦即无前驱）
        pre[i] = -1;
    }
    //起始节点带自身的可靠性为1（亦即无需传输）
    rel[start] = MAX_REL;

    // 核心循环（需要处理node_count次，每次确定一个节点的最优路径）
    for (int i = 0; i < node_count; i++) {
        // 找未访问节点中可靠性最大的节点u
        int u = -1; //存储当前选中的节点
        double max_r = MIN_REL; //存储当前的最大可靠性值
        for (int v = 0; v < node_count; v++) {
            //条件：节点v未访问，且当前可靠性大于max_r
            if (!visited[v] && rel[v] > max_r) {
                max_r = rel[v];
                u = v;
            }
        }

        // 终止条件1：u=-1表示所有未访问节点均不可达，无需继续循环
        if (u == -1) break;

        // 终止条件2：若当前选中的节点u就是目标节点，可提前终止（优化效率）
        if (u == end) break;

        //标记节点u为“已访问”（确定其最优路径，后续不再修改）
        visited[u] = 1;

        // 松弛操作 更新节点u的所有邻居节点的可靠性（核心步骤）
        for (int v = 0; v < node_count; v++) {
            // 条件1：u到v有有效链路（graph[u][v]>0，即不可达）
            // 条件2：新路径（start->u->v）的可靠性 > 当前路径（start->v）的可靠性
            if (graph[u][v] > MIN_REL && rel[v] < rel[u] * graph[u][v]) {
                // 更新节点v的最大可靠性（新路径更优）
                rel[v] = rel[u] * graph[u][v];
                // 记录节点v的前驱为u（用于后续回溯路径）
                pre[v] = u;
            }
        }
    }

    // 验证目标节点是否可达
    if (rel[end] == MIN_REL) {
        // 目标节点不可达：可靠性设为0，路径长度设为0
        *max_rel = 0;
        *path_len = 0;
        // 返回-1，亦即不可达
        return -1;
    }

    // 回溯路径（从目标节点反向找前驱，直到起始节点）
    *max_rel = rel[end]; //存储最大可靠性值
    int temp[MAX_NODES]; //临时数组：存储反向路径
    int len = 0; //临时记录反向路径的长度
    int curr = end; //当前节点：从目标节点开始回溯

    //循环回溯：直到前驱为-1（亦即回到起始节点的前驱，停止）
    while (curr != -1) {
        temp[len++] = curr; //将当前节点存入临时数组
        curr = pre[curr]; //移动到前驱节点
    }

    // 反转路径（将反向路径转为正向路径）
    *path_len = len; //存储正向路径的长度
    for (int i = 0; i < len; i++) {
        // 临时数组temp是[end, pre[end], ..., start]，反转后为[start, ..., end]
        path[i] = temp[len - 1 - i];
    }

    return 0;
}

/**
 * @brief 测试函数：使用题目中的示例网络验证算法正确性
 * @return 无返回值
 * @note 示例：0->1(0.99)、0->2(0.85)、1->3(0.99)、2->3(0.90)、3->4(0.99)
 */
void test_reliable_routing() {
    printf("=== 可靠路由算法测试（题目示例） ===\n");
    // 初始化图：5个节点（编号0-4）
    init_graph(5);

    // 按照题目示例添加有向链路
    add_edge(0, 1, 0.99);  // 0到1的可靠性0.99
    add_edge(0, 2, 0.85);  // 0到2的可靠性0.85
    add_edge(1, 3, 0.99);  // 1到3的可靠性0.99
    add_edge(2, 3, 0.90);  // 2到3的可靠性0.90
    add_edge(3, 4, 0.99);  // 3到4的可靠性0.99

    // 测试场景1：求解0->3的最大可靠性路径（题目预期：0->1->3，可靠性0.9801）
    double max_rel1;                // 存储0->3的最大可靠性
    int path1[MAX_NODES], len1;     // 存储0->3的路径和长度
    // 调用算法求解
    if (dijkstra_max_rel(0, 3, &max_rel1, path1, &len1) == 0) {
        // 成功找到路径：打印结果
        printf("1. 0->3 最大可靠性：%.4f\n", max_rel1);
        printf("   路径：");
        for (int i = 0; i < len1; i++) {
            // 格式控制：最后一个节点后不打印"->"
            printf("%d%s", path1[i], i == len1 - 1 ? "\n" : "->");
        }
    } else {
        // 未找到路径：打印提示
        printf("1. 0->3 无可达路径\n");
    }

    // 测试场景2：求解0->4的最大可靠性路径（题目预期：0->1->3->4，可靠性0.970299）
    double max_rel2;                // 存储0->4的最大可靠性
    int path2[MAX_NODES], len2;     // 存储0->4的路径和长度
    if (dijkstra_max_rel(0, 4, &max_rel2, path2, &len2) == 0) {
        printf("2. 0->4 最大可靠性：%.6f\n", max_rel2);
        printf("   路径：");
        for (int i = 0; i < len2; i++) {
            printf("%d%s", path2[i], i == len2 - 1 ? "\n" : "->");
        }
    } else {
        printf("2. 0->4 无可达路径\n");
    }
}

// 主函数
int main() {
    INIT_UTF8_CONSOLE();
    test_reliable_routing();
    return 0;
}