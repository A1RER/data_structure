//链式栈实现
#include "stdio.h"
#include "stdlib.h"
#include "../utf8support.h"

//链式栈节点结构
typedef struct StackNode
{
    int data;   //数据域
    struct StackNode *next; //指针域（指向栈底方向节点）
}StackNode,*LinkStack;

/**
 * @brief 初始化链式栈
 * @param stack 指向链式栈头节点的指针（输出参数）
 * @return int 0=成功，1=失败
 * @note 空链式栈头指针为NULL
 */
int InitLinkStack(LinkStack *stack){
    *stack = NULL;  //空栈：头指针（栈顶）为NULL
    return 0;
}

/**
 * @brief 判断链式栈是否为空
 * @param stack 链式栈头指针
 * @return int 1=空，0=非空
 */
int IsLinkStackEmpty(LinkStack stack){
    return(stack==NULL)?1:0;
}

/**
 * @brief 链式栈入栈（头插法）
 * @param stack 指向链式栈头指针的指针
 * @param data 代入栈的整形数据
 * @return int 0=成功， -1=失败（节点创建失败）
 * @note 新节点作为新栈顶，无需考虑栈满，内存足够即可
 */
int PushLinkStack(LinkStack *stack,int data){
    //创建新节点
    StackNode *newNode = (StackNode*) malloc(sizeof(StackNode ));
    if(newNode==NULL){
        printf("malloc failed for StackNode\n");
        return -1;
    }

    // 头插：新节点next指向原栈顶
    newNode->data=data;
    newNode->next=*stack;
    *stack=newNode; //新节点成为原栈顶

    return 0;
}

/**
 * @brief 链式栈出栈--头删法
 * @param stack 指向链式栈头指针的指针
 * @param data 存储出栈数据的指针（输出参数）
 * @return int 0=成功，-1=失败--栈空
 */
int PopLinkStack(LinkStack *stack, int *data){
    if(IsLinkStackEmpty(*stack))
    {
        printf("链式栈空，出栈失败\n");
        return -1;
    }
    StackNode *temp=*stack; //暂存栈顶结点
    *data=temp->data;   //取栈顶数据
    *stack=temp->next;  //新栈顶为原栈顶的next
    free(temp); //释放原栈顶节点
    return 0;
}

void DestroyLinkStack(LinkStack *stack)
{
    StackNode *cur=*stack;
    while(cur!=NULL){
        StackNode *next=cur->next;
        free(cur);
        cur=next;
    }
    *stack=NULL; //置空避免野指针
}

// 辅助函数：打印链式栈所有元素（从栈顶到栈底）
void PrintLinkStack(LinkStack stack)
{
    if (IsLinkStackEmpty(stack))
    {
        printf("链式栈为空！\n");
        return;
    }
    printf("链式栈（栈顶→栈底）：");
    StackNode *cur = stack;
    while (cur != NULL)
    {
        printf("%d ", cur->data);
        cur = cur->next;
    }
    printf("\n");
}

// 主测试函数
int main()
{
    INIT_UTF8_CONSOLE();
    LinkStack stack; // 链式栈头指针（栈顶指针）
    int data;        // 存储出栈数据

    // 1. 初始化栈
    printf("===== 初始化链式栈 =====\n");
    int init_ret = InitLinkStack(&stack);
    if (init_ret == 0)
    {
        printf("栈初始化成功！空栈判断：%s\n", IsLinkStackEmpty(stack) ? "空" : "非空");
    }

    // 2. 入栈操作
    printf("\n===== 入栈操作（10→20→30）=====\n");
    PushLinkStack(&stack, 10);
    PushLinkStack(&stack, 20);
    PushLinkStack(&stack, 30);
    PrintLinkStack(stack); // 预期输出：30 20 10

    // 3. 出栈操作
    printf("\n===== 出栈操作 =====\n");
    if (PopLinkStack(&stack, &data) == 0)
    {
        printf("出栈成功，出栈数据：%d\n", data); // 预期输出：30
    }
    PrintLinkStack(stack); // 预期输出：20 10

    // 4. 连续出栈至空
    printf("\n===== 连续出栈至空 =====\n");
    PopLinkStack(&stack, &data); // 出栈20
    printf("出栈数据：%d\n", data);
    PopLinkStack(&stack, &data); // 出栈10
    printf("出栈数据：%d\n", data);
    PopLinkStack(&stack, &data); // 栈空，出栈失败
    PrintLinkStack(stack);

    // 5. 销毁栈
    printf("\n===== 销毁链式栈 =====\n");
    DestroyLinkStack(&stack);
    printf("栈销毁后，空栈判断：%s\n", IsLinkStackEmpty(stack) ? "空" : "非空");

    return 0;
}