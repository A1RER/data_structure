#include "stdio.h"
#include "stdlib.h"
#include "../utf8support.h"

//定义队列最大容量
#define MAXSIZE 5

//状态码定义
#define SUCCESS 1
#define FAILURE 0

//循环队列结构体（顺序存储）
typedef struct {
    int data[MAXSIZE];  //存储队列元素的数组
    int front;  //队头指针，指向队头元素
    int rear;   //队尾指针，指向队尾元素的下一个位置
} SqQueue;

//初始化队列
//队头、队尾指针均置0，队列初始为空
int InitQueue(SqQueue *q){
    if(q==NULL) return FAILURE; //防空指针
    q->front=0;
    q->rear=0;
    return SUCCESS;
}

//判断队列是否为空
int isEmpty(SqQueue *q){
    if(q==NULL) return FAILURE;
    return(q->front==q->rear)?1:0;
}

//判断队列是否为满
int IsFull(SqQueue *q){
    if(q==NULL)return FAILURE;
    return((q->rear+1%MAXSIZE==q->front))?SUCCESS:FAILURE;
}

//入队-队尾插入
//逻辑:先判满->存入数据->队尾指针循环后移
int EnQueue(SqQueue *q,int val){
    if(q==NULL|| IsFull(q)==SUCCESS){
        printf("队列已满，无法入队！\n");
        return FAILURE;
    }
    q->data[q->rear]=val;   //存储数据到队尾位置
    q->rear=(q->rear+1) % MAXSIZE;
    return SUCCESS;
}

//出队操作（队头删除）
//逻辑：先判空->取出队头数据->队头指针循环后移
int DeQueue(SqQueue *q,int *val){
    if(q==NULL|| isEmpty(q)==SUCCESS||val==NULL){
        printf("队列为空/参数错误，无法出队\n");
        return FAILURE;
    }
    *val=q->data[q->front];
    return SUCCESS;
}

//遍历队列
//从front开始，循环遍历到rear之前
void TraverseQueue(SqQueue *q){
    if(q==NULL|| isEmpty(q)==SUCCESS){
        printf("队列为空！\n");
        return;
    }
    printf("队列元素，队头->队尾\n");
    int i=q->front;
    while(i!=q->rear) {
        printf("%d ", q->data[i]);
        i = (i + 1) % MAXSIZE;
    }
    printf("\n");
}

//求队列长度
int GetQueneLength(SqQueue *q){
    if(q==NULL)return-1;
    return(q->rear - q->front+MAXSIZE)%MAXSIZE;
}

//测试主函数
int main(){
    INIT_UTF8_CONSOLE();
    SqQueue q;
    //初始化队列
    if(InitQueue(&q)==SUCCESS){
        printf("队列初始化成功！\n");
    }

    //入队测试,MAXSIZE=5--最多存4个
    EnQueue(&q,10);
    EnQueue(&q,20);
    EnQueue(&q,30);
    EnQueue(&q,40);
    TraverseQueue(&q);
    printf("当前队列长度：%d\n", GetQueneLength(&q));

    //出队测试
    int val;
    DeQueue(&q,&val);
    printf("出队元素：%d\n",val);
    DeQueue(&q,&val);
    printf("出队元素：%d\n",val);
    TraverseQueue(&q);
    printf("当前队列长度：%d\n", GetQueneLength(&q));

    //循环入队测试，验证循环逻辑
    EnQueue(&q,50);
    EnQueue(&q,60);
    printf("当前队列长度:%d\n", GetQueneLength(&q));

    return 0;
}