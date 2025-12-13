#include <stdio.h>
#include "../utf8support.h"
#define N 5          // 假设图中有5个顶点
const int MaxValue = 100; // MaxValue表示∞
char vertex[N] = {'A', 'B', 'C', 'D', 'E'}; // 存放顶点的一维数组
// 存放边的二维数组（邻接矩阵）
int edge[N][N] = {
        {0, 13, MaxValue, 4, MaxValue},
        {13, 0, 15, MaxValue, 5},
        {MaxValue, MaxValue, 0, 12, MaxValue},
        {4, MaxValue, 12, 0, MaxValue},
        {MaxValue, MaxValue, 6, 3, 0}
};
int dist[N][N] = {0}; // 存储任意两个顶点的最短路径长度

// Floyd算法求任意两个顶点之间的最短路径
void Floyd() {
    int i, j, k;
    // 初始化dist矩阵为邻接矩阵
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            dist[i][j] = edge[i][j];
        }
    }
    // 枚举中间顶点k，更新最短路径
    for (k = 0; k < N; k++) {
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                if (dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                }
            }
        }
    }
}

// 求图的中心点，返回中心点的下标
int Center() {
    int wayCost, minCost = MaxValue;
    int i, k, index;
    // 依次计算每个顶点的往返代价
    for (k = 0; k < N; k++) {
        wayCost = 0;
        // 顶点k到其他顶点的路径长度之和
        for (i = 0; i < N; i++) {
            wayCost += dist[k][i];
        }
        // 其他顶点到顶点k的路径长度之和
        for (i = 0; i < N; i++) {
            wayCost += dist[i][k];
        }
        // 更新最小代价对应的顶点
        if (wayCost < minCost) {
            minCost = wayCost;
            index = k;
        }
    }
    return index;
}

int main() {
    INIT_UTF8_CONSOLE();
    int minPoint;
    Floyd(); // 求任意两顶点的最短路径
    minPoint = Center(); // 求中心点
    printf("应该设在%c点\n", vertex[minPoint]);
    return 0;
}//
// Created by AIRER on 2025/12/11.
//
