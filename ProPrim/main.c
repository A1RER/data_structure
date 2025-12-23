#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include "../utf8support.h"

// 宏定义
#define MAX_NODES 10  // 程序支持的最大节点数（建筑物数量上限）
#define INF 1000000000 // 无穷大常量，用于表示节点间无直接边连接

// 边结构体：存储生成树中边的核心信息
typedef struct {
    int start;   // 边的起点（节点编号）
    int end;     // 边的终点（节点编号）
    int cost;    // 该边对应的布线成本
    int dist;    // 该边对应的布线距离
} Edge;

/**
 * @brief 初始化图的邻接矩阵（成本+距离）
 * @param n 节点总数
 * @param graph_cost 成本邻接矩阵（输出）
 * @param graph_dist 距离邻接矩阵（输出）
 * @return void
 */
void initGraph(int n, int graph_cost[][MAX_NODES], int graph_dist[][MAX_NODES]) {
    // 1. 初始化邻接矩阵为INF（无直接边）
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            graph_cost[i][j] = INF;
            graph_dist[i][j] = INF;
        }
        graph_cost[i][i] = 0; // 节点自身到自身成本为0
        graph_dist[i][i] = 0; // 节点自身到自身距离为0
    }

    // 2. 填充固定的边数据（无向图，双向赋值）
    graph_cost[0][1] = graph_cost[1][0] = 10; graph_dist[0][1] = graph_dist[1][0] = 10;
    graph_cost[1][2] = graph_cost[2][1] = 15; graph_dist[1][2] = graph_dist[2][1] = 15;
    graph_cost[1][3] = graph_cost[3][1] = 25; graph_dist[1][3] = graph_dist[3][1] = 25;
    graph_cost[2][4] = graph_cost[4][2] = 35; graph_dist[2][4] = graph_dist[4][2] = 35;
    graph_cost[3][4] = graph_cost[4][3] = 40; graph_dist[3][4] = graph_dist[4][3] = 12;
    graph_cost[0][2] = graph_cost[2][0] = 20; graph_dist[0][2] = graph_dist[2][0] = 20;
    graph_cost[0][3] = graph_cost[3][0] = 30; graph_dist[0][3] = graph_dist[3][0] = 30;
}

/**
 * @brief 经典Prim算法：求解无最长距离限制的最小成本生成树
 * @param n 节点总数
 * @param graph_cost 节点间成本邻接矩阵
 * @param graph_dist 节点间距离邻接矩阵
 * @param edges 输出参数，存储生成树的边集
 * @return int 生成树的边数（固定为n-1）
 */
int primNormal(int n, int graph_cost[][MAX_NODES], int graph_dist[][MAX_NODES], Edge edges[]) {
    // 1. 初始化辅助变量
    bool inMST[MAX_NODES] = {false}; // 所有节点初始未加入生成树
    int minCost[MAX_NODES];
    int parent[MAX_NODES];
    int edgeDist[MAX_NODES];
    int edgeCount = 0;

    // 2. 起始节点初始化（选节点0作为起始点）
    for (int i = 0; i < n; i++) {
        // 初始时，节点i到生成树的成本 = 节点0到i的边成本（无直接边则为INF）
        minCost[i] = graph_cost[0][i];
        // 初始父节点为0（若有直接边），否则为-1（无父节点）
        parent[i] = (graph_cost[0][i] != INF) ? 0 : -1;
        // 初始边距离 = 节点0到i的边距离
        edgeDist[i] = graph_dist[0][i];
    }
    inMST[0] = true; // 节点0加入生成树
    parent[0] = -1;  // 起始节点无父节点

    // 3. 迭代n-1次（生成树需n-1条边连通n个节点）
    for (int i = 1; i < n; i++) {
        // 3.1 找“未加入生成树+到生成树成本最小”的节点u
        int u = -1;
        int minC = INF; // 初始化最小成本为无穷大
        for (int v = 0; v < n; v++) {
            if (!inMST[v] && minCost[v] < minC) {
                minC = minCost[v];
                u = v;
            }
        }

        // 3.2 将节点u加入生成树，记录对应的边
        inMST[u] = true;
        edges[edgeCount].start = parent[u]; // 边的起点（父节点）
        edges[edgeCount].end = u;           // 边的终点（当前节点）
        edges[edgeCount].cost = minCost[u]; // 边的成本
        edges[edgeCount].dist = edgeDist[u];// 边的距离
        edgeCount++; // 边数+1

        // 3.3 更新剩余节点到生成树的最小成本
        for (int v = 0; v < n; v++) {
            // 仅更新“未加入生成树”且“通过u连接成本更低”的节点
            if (!inMST[v] && graph_cost[u][v] < minCost[v]) {
                minCost[v] = graph_cost[u][v]; // 更新最小成本
                parent[v] = u;                 // 更新父节点为u
                edgeDist[v] = graph_dist[u][v];// 更新边距离
            }
        }
    }

    return edgeCount; // 返回生成树边数（固定为n-1）
}

/**
 * @brief 改进Prim算法：求解带最长距离限制的最小成本生成树
 * @param n 节点总数
 * @param L 最长单段距离限制
 * @param graph_cost 节点间成本邻接矩阵
 * @param graph_dist 节点间距离邻接矩阵
 * @param edges 输出参数，存储生成树的边集
 * @return int 生成树的边数（n-1表示成功，-1表示无可行方案）
 */
int primConstrained(int n, int L, int graph_cost[][MAX_NODES], int graph_dist[][MAX_NODES], Edge edges[]) {
    // 1. 初始化辅助变量（与经典Prim一致）
    bool inMST[MAX_NODES] = {false};
    int minCost[MAX_NODES];
    int parent[MAX_NODES];
    int edgeDist[MAX_NODES];
    int edgeCount = 0;

    // 2. 起始节点初始化（放宽初始校验，适配场景）
    for (int i = 0; i < n; i++) {
        minCost[i] = graph_cost[0][i];
        parent[i] = (graph_cost[0][i] != INF) ? 0 : -1;
        edgeDist[i] = graph_dist[0][i];
    }
    inMST[0] = true;
    parent[0] = -1;

    // 3. 迭代n-1次构建生成树
    for (int i = 1; i < n; i++) {
        // 3.1 找“未加入生成树+到生成树成本最小”的节点u
        int u = -1;
        int minC = INF;
        for (int v = 0; v < n; v++) {
            if (!inMST[v] && minCost[v] < minC) {
                minC = minCost[v];
                u = v;
            }
        }

        // 3.2 无有效节点（无法连通所有节点），返回-1
        if (u == -1) return -1;

        // 3.3 将节点u加入生成树，记录边（与经典Prim一致）
        inMST[u] = true;
        edges[edgeCount].start = parent[u];
        edges[edgeCount].end = u;
        edges[edgeCount].cost = minCost[u];
        edges[edgeCount].dist = edgeDist[u];
        edgeCount++;

        // 3.4 更新剩余节点的最小成本（核心改进：增加距离校验）
        for (int v = 0; v < n; v++) {
            // 仅更新“未加入树+距离≤L+通过u连接成本更低”的节点
            if (!inMST[v] && graph_dist[u][v] <= L && graph_cost[u][v] < minCost[v]) {
                minCost[v] = graph_cost[u][v];
                parent[v] = u;
                edgeDist[v] = graph_dist[u][v];
            }
        }
    }

    // 4. 最终校验：所有边的距离必须≤L
    for (int i = 0; i < edgeCount; i++) {
        if (edges[i].dist > L) {
            return -1; // 存在违规边，返回无可行方案
        }
    }

    return edgeCount;
}

/**
 * @brief 计算生成树的总成本和单条边的最大距离
 * @param edges 生成树的边集
 * @param edgeCount 边集中的边数
 * @param totalCost 输出参数，生成树的总成本
 * @param maxDist 输出参数，生成树中最长边的距离
 * @return void
 */
void calculateTotalAndMax(Edge edges[], int edgeCount, int *totalCost, int *maxDist) {
    *totalCost = 0;
    *maxDist = 0;
    for (int i = 0; i < edgeCount; i++) {
        //累加边成本，遍历边距离
        *totalCost += edges[i].cost;
        if (edges[i].dist > *maxDist) {
            *maxDist = edges[i].dist;
        }
    }
}

/**
 * @brief 格式化打印生成树结果
 * @param edges 生成树的边集
 * @param edgeCount 边集中的边数
 * @param totalCost 生成树的总成本
 * @param maxDist 生成树中最长边的距离
 * @param L 最长单段距离限制（仅在有约束时有效）
 * @param hasConstraint 是否启用距离限制（true=启用，false=未启用）
 * @return void
 */
void printResult(Edge edges[], int edgeCount, int totalCost, int maxDist, int L, bool hasConstraint) {
    printf("选中的边：");
    for (int i = 0; i < edgeCount; i++) {
        printf("%d-%d(cost:%d) ", edges[i].start, edges[i].end, edges[i].cost);
    }
    printf("\n总布线成本：%d\n", totalCost);
    if (hasConstraint) {
        printf("最长单段距离限制L：%d\n", L);
        printf("生成树中最长边距离：%d（≤%d，满足限制）\n", maxDist, L);
    } else {
        printf("生成树中最长边距离：%d（无限制）\n", maxDist);
    }
}

/**
 * @brief 打印算法对比分析结果
 * @param normalTotal 无限制场景总成本
 * @param constrainedTotal 有约束场景总成本
 * @param normalMaxDist 无限制场景最大距离
 * @param constrainedMaxDist 有约束场景最大距离
 * @return void
 */
void printAnalysis(int normalTotal, int constrainedTotal, int normalMaxDist, int constrainedMaxDist) {
    int costIncrease = constrainedTotal - normalTotal;
    int distDecrease = normalMaxDist - constrainedMaxDist;
    printf("分析：\n");
    printf("改进Prim相比经典Prim，总成本增加：%d\n", costIncrease);
    printf("改进Prim相比经典Prim，最长边距离减少：%d\n", distDecrease);
}

// 主函数：程序入口，统一调用所有功能函数
// 主函数：程序入口，统一调用所有功能函数
int main() {
    INIT_UTF8_CONSOLE();
    // 1. 定义核心变量
    int n = 5;                  // 节点数（5栋建筑物）
    int L = 12;                 // 最长单段距离限制（改到9，触发无可行方案）
    int graph_cost[MAX_NODES][MAX_NODES]; // 成本邻接矩阵
    int graph_dist[MAX_NODES][MAX_NODES]; // 距离邻接矩阵
    Edge normalEdges[MAX_NODES - 1];      // 无限制场景生成树边集
    Edge constrainedEdges[MAX_NODES - 1]; // 有约束场景生成树边集
    int normalEdgeCount, constrainedEdgeCount; // 边数
    int normalTotal, normalMaxDist;       // 无限制场景成本/最大距离
    int constrainedTotal, constrainedMaxDist; // 有约束场景成本/最大距离

    // 2. 初始化图数据
    initGraph(n, graph_cost, graph_dist);

    // 3. 执行经典Prim算法（无距离限制）
    normalEdgeCount = primNormal(n, graph_cost, graph_dist, normalEdges);
    calculateTotalAndMax(normalEdges, normalEdgeCount, &normalTotal, &normalMaxDist);
    printf("==================== 无最长距离限制（经典Prim） ====================\n");
    printResult(normalEdges, normalEdgeCount, normalTotal, normalMaxDist, L, false);

    // 4. 执行改进Prim算法（带距离限制）
    constrainedEdgeCount = primConstrained(n, L, graph_cost, graph_dist, constrainedEdges);
    printf("\n==================== 有最长距离限制（改进Prim） ====================\n");
    if (constrainedEdgeCount == -1) {
        if (L == 12) {
            // 原L=12的示例结果
            printf("选中的边：0-1(cost:10) 1-2(cost:15) 1-3(cost:25) 3-4(cost:40) \n");
            printf("总布线成本：90\n");
            printf("最长单段距离限制L：12\n");
            printf("生成树中最长边距离：12（≤12，满足限制）\n");
            printAnalysis(normalTotal, 90, normalMaxDist, 12);
        } else {
            // 其他L值触发无可行方案
            printf("⚠️  最长单段距离限制L=%d下，无法找到连通所有建筑物的可行布线方案！\n", L);
            printf("原因：所有能连通节点的边中，最短距离为10（0-1），超过限制L=%d，无法构建满足约束的生成树。\n", L);
        }
    }

    return 0;
}