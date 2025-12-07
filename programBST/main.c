#include<stdio.h>
#include<stdlib.h>
#include "../utf8support.h"

// 给定的20个整数数据
int num[] = {57, 45, 75, 53, 17, 85, 14, 73, 34, 77, 50, 4, 44, 15, 60, 78, 90, 49, 98, 59};
typedef int Status;

// 定义二叉排序树节点结构
typedef struct BiNode {
    int data;               // 存储节点数据
    struct BiNode *lchild;  // 左子树指针
    struct BiNode *rchild;  // 右子树指针
} BiNode, *BiTree;

// 函数声明
void show_num(int n, int *num);          // 输出数组数据
BiTree CreateBST(int *num, int n);       // 构建二叉排序树
BiTree InsertBST(BiTree T, int key);     // 插入节点到二叉排序树
void PreOrder(BiTree T);                 // 前序遍历
void InOrder(BiTree T);                  // 中序遍历
void PostOrder(BiTree T);                // 后序遍历
Status FindDeleteBST(BiTree *T, int key); // 删除指定值的节点
BiTree FindMin(BiTree T);                // 查找左子树最大值（用于双孩子节点删除）

// 输出数组中的所有数据
void show_num(int n, int *num) {
    int i;
    for (i = 0; i < n; i++) {
        printf("%3d ", num[i]);
    }
}

// 插入节点到二叉排序树（递归实现）
BiTree InsertBST(BiTree T, int key) {
    // 情况1：当前树为空（或递归到空位置），创建新节点作为当前位置的节点
    if (T == NULL) {
        T = (BiTree)malloc(sizeof(BiNode));  // 给新节点分配内存（申请一块空间）
        T->data = key;                       // 新节点的数据设为要插入的key
        T->lchild = T->rchild = NULL;        // 新节点的左右孩子初始为空（叶子节点）
    }
        // 情况2：要插入的key < 当前节点数据 → 递归插入到左子树
    else if (key < T->data) {
        T->lchild = InsertBST(T->lchild, key);  // 左孩子指针指向插入后的左子树
    }
        // 情况3：要插入的key > 当前节点数据 → 递归插入到右子树
    else if (key > T->data) {
        T->rchild = InsertBST(T->rchild, key);  // 右孩子指针指向插入后的右子树
    }
    // 情况4：key == 当前节点数据（默认不插入，避免重复节点）
    return T;  // 返回插入后的树（根节点不变，子树可能更新）
}

// 构建二叉排序树：逐个插入数组元素
BiTree CreateBST(int *num, int n) {
    BiTree T = NULL;
    for (int i = 0; i < n; i++) {
        T = InsertBST(T, num[i]);
    }
    return T;
}

// 前序遍历：根→左→右
void PreOrder(BiTree T) {
    if (T != NULL) {
        printf("%3d ", T->data);  // 1. 先访问根
        PreOrder(T->lchild);      // 2. 再遍历左
        PreOrder(T->rchild);      // 3. 最后遍历右
    }
}

// 中序遍历函数体 左->根->右
void InOrder(BiTree T) {
    if (T != NULL) {
        InOrder(T->lchild);       // 递归遍历左子树
        printf("%3d ", T->data);  // 访问根节点
        InOrder(T->rchild);       // 递归遍历右子树
    }
}

// 后序遍历：左→右→根
void PostOrder(BiTree T) {
    if (T != NULL) {
        PostOrder(T->lchild);     // 1. 先遍历左
        PostOrder(T->rchild);     // 2. 再遍历右
        printf("%3d ", T->data);  // 3. 最后访问根
    }
}


// 查找二叉排序树中值最大的节点（用于双孩子节点删除时的替换）
BiTree FindMin(BiTree T) {
    if (T == NULL) return NULL;
    // 逻辑：BST的最大值在“最右子节点”（右子树为空的节点）
    while (T->rchild != NULL) {
        T = T->rchild;  // 一直往右走，直到右子树为空
    }
    return T;  // 返回最大值节点
}

// 删除二叉排序树中指定关键字的节点（T是指针的指针，因为要修改根节点）
Status FindDeleteBST(BiTree *T, int key) {
    BiTree p = *T, f = NULL, q, s;  // p：当前节点；f：p的父节点；q：s的父节点；s：替换节点
    // 第一步：查找要删除的节点p及其父节点f
    while (p != NULL) {
        if (p->data == key) break;  // 找到目标节点，跳出循环
        f = p;                      // 记录父节点（p要往下走，先存f）
        if (key < p->data) p = p->lchild;  // key小，往左找
        else p = p->rchild;                // key大，往右找
    }
    if (p == NULL) return 0;  // 没找到目标节点，删除失败

    // 第二步：分3种情况删除节点p
    // 情况1：p是叶子节点（无左/右子树）
    if (p->lchild == NULL && p->rchild == NULL) {
        if (f == NULL) *T = NULL;  // 特殊情况：p是根节点（树只有一个节点），删除后树为空
        else if (f->lchild == p) f->lchild = NULL;  // p是父节点的左孩子，父节点左指针置空
        else f->rchild = NULL;                      // p是父节点的右孩子，父节点右指针置空
        free(p);  // 释放p的内存（避免内存泄漏）
    }

        // 情况2：p只有左子树（无右子树）
    else if (p->rchild == NULL) {
        if (f == NULL) *T = p->lchild;  // p是根节点，左子树直接成为新根
        else if (f->lchild == p) f->lchild = p->lchild;  // 父节点左指针指向p的左子树
        else f->rchild = p->lchild;                      // 父节点右指针指向p的左子树
        free(p);
    }

        // 情况3：p只有右子树（无左子树）
    else if (p->lchild == NULL) {
        if (f == NULL) *T = p->rchild;  // p是根节点，右子树成为新根
        else if (f->lchild == p) f->lchild = p->rchild;  // 父节点左指针指向p的右子树
        else f->rchild = p->rchild;                      // 父节点右指针指向p的右子树
        free(p);
    }

        // 情况4：p有两个子树（最复杂的情况）
    else {
        q = p;                // q记录p的位置（后续要修改p的数据）
        s = p->lchild;        // s从p的左子树开始找
        while (s->rchild != NULL) {  // 找p左子树的最大值节点s（最右节点）
            q = s;            // q记录s的父节点
            s = s->rchild;
        }
        p->data = s->data;    // 用s的数据替换p的数据（核心：不删除p，只换值）
        // 现在要删除s节点（s是叶子或单孩子，按前两种情况处理）
        if (q == p) q->lchild = s->lchild;  // s是p的左孩子（s无右子树，左子树挂到q的左）
        else q->rchild = s->lchild;         // s是q的右孩子，s的左子树挂到q的右
        free(s);
    }
    return 1;  // 删除成功
}

// 主函数：测试二叉排序树的构建、遍历和删除功能
int main() {
    INIT_UTF8_CONSOLE();
    int a;
    printf("所有原始数据为：\n");
    show_num(20, num);  // 输出原始数组

    // 生成二叉排序树
    BiTree T;
    T = CreateBST(num, 20);
    printf("\n");
    if (T != NULL)
        printf("二叉排序树生成成功！\n");
    else
        printf("二叉排序树生成失败！\n");

    // 前序遍历输出
    printf("前序遍历二叉排序树：\n");
    PreOrder(T);
    printf("\n");

    // 中序遍历输出（有序结果）
    printf("中序遍历二叉排序树（有序）：\n");
    InOrder(T);
    printf("\n");

    // 后序遍历输出
    printf("后序遍历二叉排序树：\n");
    PostOrder(T);
    printf("\n");

    //  删除指定元素（输入序号，1~20对应数组第1~20个元素）
    printf("请输入要删除的元素序号（1-20）：\n");
    scanf("%d", &a);
    // 校验输入序号合法性
    if (a < 1 || a > 20) {
        printf("序号输入错误！\n");
        return 1;
    }
    // 执行删除操作
    if (FindDeleteBST(&T, num[a - 1])) {
        printf("删除元素 %d 后，中序遍历结果（有序）：\n", num[a - 1]);
        InOrder(T);
    } else {
        printf("删除失败：未找到该元素！\n");
    }

    return 0;
}