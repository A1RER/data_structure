#include "stdlib.h"
#include "stdio.h"
#include "../utf8support.h"

// 双链表核心定义
typedef struct DlistNode{
    int data;
    struct DlistNode *prev; //前驱指针
    struct DlistNode *next; //后继指针
}DlistNode;

//创建双链表节点
DlistNode* createDNote(int data){
    DlistNode *newNode=(DlistNode*) malloc(sizeof (DlistNode));
    newNode->data=data;
    newNode->prev=NULL;
    newNode->next=NULL;
    return newNode;
}

//遍历双链表（正向）
void traverseDlist(DlistNode *head){
    DlistNode *current=head;
    if(current==NULL){
        printf("双链表为空!\n");
        return;

    }
    printf("正向遍历双链表");
    while(current!=NULL){
        printf("%d ",current->data);
        current=current->next;
    }
    printf("\n");
}

//遍历双链表（反向）
void traverseDlistReverse(DlistNode *head){
    DlistNode *current=head;
    if(current==NULL){
        printf("双链表为空！\n");
        return;
    }

    //先走到尾节点
    while (current->next!=NULL){
        current=current->next;
    }

    //反向遍历（依靠prev指针）
    printf("反向遍历双链表\n");
    while(current!=NULL){
        printf("%d ",current->data);
        current=current->prev; //反向移动
    }
    printf("\n");
}

//在指定节点后插入
void insertAfterDlist(DlistNode *p,int data){
    if(p==NULL){
        printf("插入失败！前驱节点为空！\n");
        return;
    }
    DlistNode *newNode= createDNote(data);
    //维护next指针
    newNode->next=p->next;
    p->next=newNode;
    //维护prev指针
    newNode->prev=p;
    if(newNode->next != NULL) { // 若p不是尾节点，更新后继的prev
        newNode->next->prev = newNode;
    }
}

//释放内存
void freeDList(DlistNode  **head) {
    DlistNode  *current = *head;
    while (current != NULL) {
        DlistNode *next = current->next; // 先保存下一个节点
        free(current);
        current = next;
    }
    *head = NULL; // 置空头指针，避免野指针
}

//测试
int main() {
    INIT_UTF8_CONSOLE();
    // ========== 步骤1：创建空双链表，初始化头节点 ==========
    DlistNode  *head = NULL;
    // 创建第一个节点（头节点）
    head = createDNote(10);
    printf("创建头节点10后：\n");
    traverseDlist(head); // 测试：正向遍历（输出10）

    // ========== 步骤2：插入节点，构建链表 10⇄20⇄30 ==========
    insertAfterDlist(head, 20); // 在10后插入20
    insertAfterDlist(head->next, 30); // 在20后插入30
    printf("\n插入20、30后：\n");
    traverseDlist(head); // 正向遍历：10 20 30
    traverseDlistReverse(head); // 反向遍历：30 20 10

    // ========== 步骤3：测试空链表场景 ==========
    DlistNode *emptyList = NULL;
    printf("\n测试空链表遍历：\n");
    traverseDlist(emptyList); // 输出“双链表为空！”

    // ========== 步骤4：释放内存 ==========
    freeDList(&head);
    freeDList(&emptyList); // 空链表释放无副作用
    printf("\n内存已释放，测试结束！\n");

    return 0;
}