#include "stdlib.h"
#include "stdio.h"
#include "../utf8support.h"

//栈的定义：顺序存储结构（亦即顺序栈）
#define MAX_STACK_SIZE 100 //栈最大容量
typedef struct
{
    int *base; //栈底指针（固定，指向栈内存基址）
    int *top; //栈顶指针（指向栈顶元素的下一个位置）
    int stacksize; //栈最大容量
} SqStack;

/**
 * @brief 初始化顺序栈
 * @param stack 待初始化的顺序栈指针
 * @return int 0=成功 -1=失败（亦即内存分配失败）
 * @note 栈底与栈顶指针初始是重合的，亦即栈空状态
 */
int InitSqStack(SqStack *stack){
    //分配栈内存
    stack->base=(int*) malloc(MAX_STACK_SIZE * sizeof(int));
    if(stack->base==NULL){
        printf("malloc failed for SqStack\n");
        return -1;
    }
    stack->top=stack->base; //栈空：top==base
    stack->stacksize=MAX_STACK_SIZE;
    return 0;
}

/**
 * @brief 判断顺序栈是否为空
 * @param stack 顺序栈指针
 * @return int 1=空，0=非空
 */
int IsSqStackEmpty(SqStack *stack){
    return (stack->top == stack->base)?1:0;
}

/**
* @brief 顺序栈入栈（压栈）
 * @param stack 顺序栈指针
 * @param data 待入栈的整形数据
 * @return int 0=成功，-1=失败（栈满时无法入栈）
 * @note 入栈后栈顶指针上移
*/
int PushSqStack(SqStack *stack, int data){
    //检查栈满（top-base==最大容量）
    if(stack->top-stack->base==MAX_STACK_SIZE){
        printf("栈满，入栈失败！\n");
        return -1;
    }
    *stack->top=data;
    stack->top++; //这一行和上一行也可以简写为 *(stack->top++)=data; 表示数据入栈，top上移
    return 0;
}

/**
 * @brief 顺序栈出栈（弹栈）
 * @param stack 顺序栈指针
 * @param data 存储出栈数据的指针（输出参数）
 * @return int 0=成功，-1=失败（栈空）
 * @note 出栈后栈顶指针下移
 */
int PopSqStack(SqStack *stack,int *data){
    if(IsSqStackEmpty(stack)){
        printf("栈空，入栈失败\n");
        return -1;
    }
    *data=*(--stack->top); //top下移，再取数据
    /**
     * 上面的代码等价为：
     * stack->top = stack->top -1;
     * int pop_value=*stack->top;
     * *data = pop_value
     */
    return 0;
}

/**********************************************************
 * @brief  获取顺序栈栈顶元素
 * @param  stack 顺序栈指针
 * @param  data 存储栈顶数据的指针（输出参数）
 * @return int 0=成功，-1=失败（栈空）
 * @note   不改变栈顶指针位置
 *********************************************************/
int GetSqStackTop(SqStack *stack,int *data){
    if(IsSqStackEmpty(stack)){
        printf("the stack is empty, there is no top elements\n");
        return -1;
    }
    *data=*(stack->top-1); //取top前一个位置的数据
    /** 上面这一行的代码可以等效为
     * int *top_data_addr= stack->top -1; -->第一步：算出栈顶有效数据的地址（空位置地址 - 1个int长度）
     * int top_value = *top_data_addr; -->第二步：解引用地址，取出相应的值
     * *data=*top_value; -->第三步：把取出的值写入data指向的外部变量
     */
     return 0;
}

/**
 * @brief 销毁顺序栈
 * @param stack 顺序栈指针
 * @return void
 * @note 释放后将base/top置NULL，避免野指针
 */
void DestroySqStack(SqStack *stack){
    if(stack->base==NULL){
        free(stack->base);
        stack->base=stack->top=NULL; //置空
        stack->stacksize=0;
    }
}

/**
 * 基本功能测试函数
 */
void TestSqStack(){
    SqStack stack;
    int data;

    // 初始化栈
    if (InitSqStack(&stack) != 0) return;
    printf("顺序栈初始化成功！\n");

    // 入栈：10→20→30
    PushSqStack(&stack, 10);
    PushSqStack(&stack, 20);
    PushSqStack(&stack, 30);
    printf("入栈3个元素（10,20,30）后，栈顶元素：");
    GetSqStackTop(&stack, &data);
    printf("%d\n", data); // 预期输出30

    // 出栈
    PopSqStack(&stack, &data);
    printf("出栈1个元素：%d，剩余栈顶元素：", data); // 预期输出30
    GetSqStackTop(&stack, &data);
    printf("%d\n", data); // 预期输出20

    // 销毁栈
    DestroySqStack(&stack);
    printf("顺序栈销毁成功！\n");
}

int main(){
    INIT_UTF8_CONSOLE();
    TestSqStack();
    return 0;
}
