#include "stdio.h"
#include "stdlib.h"
#include "../utf8support.h"
#define MAXSIZE 100

//1.定义二叉树节点结构体：
typedef struct BiTNode{
    int data;
    struct BiTNode *lchild;
    struct BiTNode *rchild;
}BiTNode,*BiTree;

//2.递归创建二叉树
void CreateBiTree(BiTree* T){
    int val;
    scanf("%d",&val);
    if(val==-1){
        *T=NULL;
        return;
    }

    //分配当前节点内存
    *T=(BiTree) malloc(sizeof(BiTNode));
    (*T)->data=val;
    //递归创建左子树
    CreateBiTree(&((*T)->lchild));
    //递归创建右子树
    CreateBiTree(&((*T)->rchild));
}

//先序遍历：根->左->右
void PreOrder(BiTree T){
    if(T!= NULL){
        printf("%d",T->data);
        PreOrder(T->lchild);
        PreOrder(T->rchild);
    }
}

//中序遍历：左-根-右
void InOrder(BiTree T){
    if(T!=NULL){
        InOrder(T->lchild);
        printf("%d",T->data);
        InOrder(T->rchild);
    }
}

//后序遍历：左-右-根
void PostOrder(BiTree T){
    if(T!=NULL){
        PostOrder(T->lchild);
        PostOrder(T->rchild);
        printf("%d",T->data);
    }
}

//层序遍历
//队列结构体，存二叉树节点指针
typedef struct {
    BiTree data[MAXSIZE];
    int front ,rear
}SqQueue;

//队列初始化
void InitQueue(SqQueue *q){
    q->front=q->rear=0;
}

// 4.3 入队
int EnQueue(SqQueue *q, BiTree t) {
    if ((q->rear + 1) % MAXSIZE == q->front) return 0; // 队满
    q->data[q->rear] = t;
    q->rear = (q->rear + 1) % MAXSIZE;
    return 1;
}

// 4.4 出队
int DeQueue(SqQueue *q, BiTree *t) {
    if (q->front == q->rear) return 0; // 队空
    *t = q->data[q->front];
    q->front = (q->front + 1) % MAXSIZE;
    return 1;
}

//层序遍历核心函数
void LevelOrder(BiTree T){
    SqQueue q;
    InitQueue(&q);
    BiTree p;
    EnQueue(&q,T);  //根节点入队
    while(q.front!=q.rear){
        DeQueue(&q,&p);
        printf("%d",p->data);
        if(p->lchild)EnQueue(&q,p->lchild);
        if(p->rchild)EnQueue(&q,p->rchild);
    }
}

//求二叉树高度
//递归思路：树的高度=max(左子树高度，右子树高度)+1
int GetTreeHeight(BiTree T){
    if(!T) return 0;
    int lh= GetTreeHeight(T->lchild);
    int rh= GetTreeHeight(T->rchild);
    return (lh>rh?lh:rh)+1;
}

//统计叶子节点个数
//叶子节点=左子树叶子树+右子树叶子树：空节点0，无左右孩子1
int CountLeaf(BiTree T) {
    if (!T) return 0; // 空树，叶子数0
    if (!T->lchild && !T->rchild) return 1; // 叶子节点，计数1
    return CountLeaf(T->lchild) + CountLeaf(T->rchild); // 递归统计
}

// ====================== 7. 节点操作：查找指定值的节点 ======================
// 找到返回节点指针，没找到返回NULL
BiTree FindNode(BiTree T, int val) {
    if (!T) return NULL; // 空树，没找到
    if (T->data == val) return T; // 找到目标节点
    BiTree lres = FindNode(T->lchild, val); // 左子树查找
    if (lres) return lres; // 左子树找到，直接返回
    return FindNode(T->rchild, val); // 右子树查找
}

// ====================== 8. 拓展：判断是否为完全二叉树（期末选考） ======================
int IsCompleteBiTree(BiTree T) {
    SqQueue q;
    InitQueue(&q);
    if (!T) return 1; // 空树是完全二叉树
    EnQueue(&q, T);
    BiTree p;
    while (DeQueue(&q, &p)) {
        if (p) { // 节点非空，左右孩子入队（不管是否为空）
            EnQueue(&q, p->lchild);
            EnQueue(&q, p->rchild);
        } else { // 遇到空节点后，后续必须全是空节点
            while (DeQueue(&q, &p)) {
                if (p) return 0; // 非空，不是完全二叉树
            }
        }
    }
    return 1;
}


//主函数 测试
int main() {
    INIT_UTF8_CONSOLE();
    BiTree T, find_res;
    int val, height, leaf_cnt;

    // 1. 创建二叉树
    printf("输入二叉树节点（-1表示空节点，先序顺序）：\n");
    CreateBiTree(&T); // 测试用例：1 2 4 -1 -1 5 -1 -1 3 -1 6 -1 -1

    // 2. 三种递归遍历
    printf("先序遍历："); PreOrder(T); printf("\n");
    printf("中序遍历："); InOrder(T); printf("\n");
    printf("后序遍历："); PostOrder(T); printf("\n");

    // 3. 层序遍历
    printf("层序遍历："); LevelOrder(T); printf("\n");

    // 4. 求高度
    height = GetTreeHeight(T);
    printf("二叉树高度：%d\n", height); // 测试用例输出：3

    // 5. 统计叶子节点
    leaf_cnt = CountLeaf(T);
    printf("叶子节点个数：%d\n", leaf_cnt); // 测试用例输出：3（4、5、6）

    // 6. 查找节点
    printf("输入要查找的节点值：");
    scanf("%d", &val);
    find_res = FindNode(T, val);
    if (find_res) printf("找到节点：%d\n", find_res->data);
    else printf("未找到该节点\n");

    // 7. 判断完全二叉树
    if (IsCompleteBiTree(T)) printf("是完全二叉树\n");
    else printf("不是完全二叉树\n"); // 测试用例输出：是

    return 0;
}