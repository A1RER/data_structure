#include "stdio.h"
#include "stdlib.h"

typedef struct ListNode{
    int data;
    struct ListNode *next;
}ListNode;

//创建节点
ListNode* createNode(int data){
    ListNode *newNode = (ListNode*)malloc(sizeof(ListNode));
    if(newNode == NULL){
        printf("内存分配失败\n");
        exit(1);
    }
    newNode->data=data;
    newNode->next=NULL;
    return newNode;
}

//遍历链表函数
void traverse(ListNode *head){
    if(head == NULL){
        printf("链表为空\n");
        return;
    }
    ListNode *current = head ;
    printf("链表遍历结果：");
    while (current != NULL){
        printf("%d->",current->data);
        current=current->next;
    }
    printf("NULL\n");

}

//头插函数
void prepend(ListNode **head , int data){
    ListNode *newNode = createNode(data);
    newNode->next= *head; //新节点接原头节点
    *head= newNode;
}

//尾插函数
void append(ListNode **head ,int data){
    ListNode *newNode= createNode(data);
    //空链表
    if(*head == NULL){
        *head=newNode;
        return;
    }
    //找尾节点
    ListNode *last=*head;
    while(last->next!=NULL){
        last=last->next;
    }
    last->next=newNode; //尾节点接新节点
}

//指定位置插入函数
void insertAfter(ListNode *head,int prevData, int data){
    //找前驱节点
    ListNode *prev=head;
    while(prev != NULL && prev->data !=prevData){
        prev=prev->next;
    }
    if(prev==NULL){
        printf("未找到前驱节点%d,插入失败",prevData);
        return;
    }
    //插入新节点
    ListNode *newNode = createNode(data);
    newNode->next=prev->next;
    prev->next=newNode;
}

//删除节点
void deleteNode(ListNode **head , int data){
    //空链表
    if(*head==NULL){
        printf("链表为空，删除失败\n");
        return;
    }
    ListNode *temp =*head;
    ListNode *prev=NULL;

    //删除头节点
    if(temp != NULL && temp->data == data){
        *head=temp->next;
        free(temp);
        return;
    }
    //找目标节点的前驱
    while(temp != NULL && temp->data != data){
        prev=temp;
        temp=temp->next;
    }
    //未找到目标节点
    if(temp==NULL){
        printf("未找到节点%d，删除失败\n",data);
        return;
    }

    //删除中间/尾节点
    prev->next =temp->next;
    free(temp);
}

//查找节点
int search(ListNode *head,int data){
    ListNode *current =head;
    int index=0;
    while(current!=NULL){
        if(current->data==data){
            return index;
        }
        current=current->next;
        index++;
    }
    return -1;
}

//求链表长度
int getLength(ListNode *head){
    int length=0;
    ListNode *current =head ;
    while(current!=NULL){
        length++;
        current=current->next;
    }
    return length;
}

//销毁链表
void freeList(ListNode **head){
    ListNode *current=*head;
    ListNode *next;
    while(current!=NULL){
        next=current->next; //保存下一个节点
        free(current); //释放当前节点
        current = next; //指针后移
    }
    *head =NULL; //头指针置空
}

//反转链表
ListNode *reverseList(ListNode *head){
    ListNode *prev =NULL; //前驱节点，初始NULL
    ListNode *current=head; //当前节点，初始为头节点
    ListNode *next=NULL; //后继节点：临时保存

    while(current != NULL){
        next = current->next ; //保存下一个节点
        current->next =prev; //反转当前节点指针
        prev=current; //前驱后移
        current=next; //当前后移
    }
    head=prev;
    return head;
}

//查找中间节点
ListNode *findMiddle(ListNode *head){
    if(head == NULL){
        return NULL;
    }

    //初始化快、慢指针
    ListNode *slow=head; //慢指针，一次走一步
    ListNode *fast=head; //快指针，一次走两步

    while(fast!=NULL && fast->next!=NULL){
        //循环条件意为：只有快指针的当前节点和下一个节点均非空，就说明还能走两步。
        slow=slow->next;
        fast=fast->next->next;
    }
    //快指针走到尾的时候，慢指针就指向了中间节点
    return slow;
}

// 查找倒数第k个节点
ListNode* findKthFromEnd(ListNode *head , int k){
    if(head == NULL && k<= 0){
        return NULL;
    }

    /**
     * 核心逻辑：
     * 快指针先提前走k步，然后快慢指针同步走，快指针到尾时慢指针即为目标。
     */

    ListNode *fast=head;
    ListNode *slow=head;

    //快指针先走k步
    for(int i=0 ; i<k ; i++){
        if(fast == NULL){
            return NULL; // k超过了链表的长度
        }
        fast=fast->next;
    }

    //快慢指针同步走
    while(fast!=NULL){
        fast = fast->next;
        slow = slow->next;
    }

    return slow;
}

// 判断链表中是否有环
int hasCycle(ListNode *head){
    if(head == NULL || head->next ==NULL) return 0;

    ListNode *slow=head;
    ListNode *fast=head;

    /**
     * 核心逻辑：
     * 快指针走两步，慢指针走一步，若相遇则有环，否则无环。
     */
    while (slow!=fast){
        if(fast==NULL || fast->next==NULL) return 0; //快指针到链表尾，无环
        slow=slow->next;
        fast=fast->next->next;
    }
    return 1;
}

//合并两个有序链表
ListNode* mergeTwoLists(ListNode *l1, ListNode *l2){
    //初始化哑节点和尾指针
    ListNode dummy; // 哑节点（栈上节点，不用free）
    ListNode *tail =&dummy; //尾指针，始终指向新链表的最后一个节点
    dummy.next=NULL; //新链表初始为空

    //比较l1和l2的当前节点，小的接入新链表
    //循环条件：l1,l2均不为空
    while(l1!=NULL&& l2!=NULL){
        if(l1->data < l2->data){ //l1当前节点更小，选l1
            tail->next=l1; //把l1当前节点接到新链表尾部
            l1 = l1->next; //l1指向下一节点
        } else{
            tail->next=l2; //把l2下一节点接到新链表尾部
            l2=l2->next; //l2指针后移
        }
        tail = tail->next; // 新链表尾指针后移（准备接下一个节点）
    }

    //处理剩余节点
    tail->next=(l1!=NULL)?l1:l2;

    //返回合并后链表的真正头节点(亦即哑节点的next)
    return dummy.next;
}


//测试函数（针对CreateNode）
//int main(){
//    ListNode *node = createNode(10);
//    printf("节点创建成功，数据：%d,next:%p\n",node->data,node->next);
//    free(node);
//    return 0;
//}

//测试函数：针对traverse
//int main(){
//    //构建链表：10 20 30 NULL
//    ListNode *head = createNode(10);
//    head ->next = createNode(20);
//    head ->next->next= createNode(30);
//    traverse(head);//遍历
//
//    //释放内存
//    free(head->next->next);
//    free(head->next);
//    free(head);
//    return 0;
//}

//测试函数：针对prepend
//int main(){
//    ListNode *head = NULL ; //空链表
//
//    prepend(&head, 30);//头插30：30->null
//    prepend(&head,20);//头插20；20->30->null
//    prepend(&head,10);//头插10:10->20->30->null
//
//    traverse(head);
//    //释放内存
//    free(head->next->next);
//    free(head->next);
//    free(head);
//
//    return 0;
//}

//测试函数：针对append
//int main(){
//    ListNode *head = NULL;  // 空链表
//
//    append(&head, 10);      // 尾插10 → 10→NULL
//    append(&head, 20);      // 尾插20 → 10→20→NULL
//    append(&head, 30);      // 尾插30 → 10→20→30→NULL
//
//    traverse(head);
//
//    // 释放内存
//    free(head->next->next);
//    free(head->next);
//    free(head);
//    return 0;
//}

//测试函数：针对insertAfter
//int main(){
//    //构建链表
//    ListNode *head = createNode(10);
//    head->next= createNode(20);
//    head->next->next= createNode(30);
//
//    insertAfter(head,20,25); //在20后面插入25，链表变为10->20->25->30
//    traverse(head);
//
//    insertAfter(head,40,50); //前驱节点40不存在，故插入失败
//
//    //释放内存
//    free(head->next->next->next);
//    free(head->next->next);
//    free(head->next);
//    free(head);
//    return 0;
//}

//测试函数：针对DeleteNode
//int main(){
//    //构建链表
//    ListNode *head= createNode(10);
//    head->next= createNode(20);
//    head->next->next= createNode(25);
//    head->next->next->next=createNode(30);
//
//    deleteNode(&head,25); //删除25
//    traverse(head);
//
//    deleteNode(&head,40);
//
//    // 释放内存
//    free(head->next->next);
//    free(head->next);
//    free(head);
//    return 0;
//}

//测试：针对search
// 测试
//int main() {
//    // 构建链表：10 → 20 → 30 → NULL
//    ListNode *head = createNode(10);
//    head->next = createNode(20);
//    head->next->next = createNode(30);
//
//    int pos1 = search(head, 20);
//    printf("节点20的位置：%d\n", pos1);  // 输出1
//
//    int pos2 = search(head, 40);
//    printf("节点40的位置：%d\n", pos2);  // 输出-1
//
//    // 释放内存
//    free(head->next->next);
//    free(head->next);
//    free(head);
//    return 0;
//}

//测试：针对getLength
// 测试
//int main() {
//    // 构建链表：10 → 20 → 30 → NULL
//    ListNode *head = createNode(10);
//    head->next = createNode(20);
//    head->next->next = createNode(30);
//
//    printf("链表长度：%d\n", getLength(head));  // 输出3
//
//    // 空链表测试
//    ListNode *emptyHead = NULL;
//    printf("空链表长度：%d\n", getLength(emptyHead));  // 输出0
//
//    // 释放内存
//    free(head->next->next);
//    free(head->next);
//    free(head);
//    return 0;
//}

//测试：针对销毁函数
//int main() {
//    ListNode *head = createNode(10);
//    head->next = createNode(20);
//    head->next->next = createNode(30);
//
//    printf("销毁前长度：%d\n", getLength(head));  // 输出3
//    freeList(&head);
//    printf("销毁后长度：%d\n", getLength(head));  // 输出0
//
//    return 0;
//}

//测试：针对反转函数
//int main(){
//    //构建链表：
//    ListNode *head = createNode(10);
//    head->next = createNode(20);
//    head->next->next = createNode(30);
//
//    printf("反转前：");
//    traverse(head);
//
//    head = reverseList(head);  // 反转
//
//    printf("反转后：");
//    traverse(head);
//
//    // 释放内存
//    freeList(&head);
//    return 0;
//}

// 测试：针对FindMiddle
//int main() {
//    // 奇数长度链表：10→20→30→40→50→NULL（中间30）
//    ListNode *head1 = createNode(10);
//    head1->next = createNode(20);
//    head1->next->next = createNode(30);
//    head1->next->next->next = createNode(40);
//    head1->next->next->next->next = createNode(50);
//
//    ListNode *mid1 = findMiddle(head1);
//    printf("奇数长度链表中间节点：%d\n", mid1->data);  // 输出30
//
//    // 偶数长度链表：10→20→30→40→NULL（中间30）
//    ListNode *head2 = createNode(10);
//    head2->next = createNode(20);
//    head2->next->next = createNode(30);
//    head2->next->next->next = createNode(40);
//
//    ListNode *mid2 = findMiddle(head2);
//    printf("偶数长度链表中间节点：%d\n", mid2->data);  // 输出30
//
//    // 释放内存
//    freeList(&head1);
//    freeList(&head2);
//    return 0;
//}

// 测试: 针对FindKthFromEnd
//int main() {
//    // 构建链表：10 → 20 → 30 → 40 → 50 → NULL
//    ListNode *head = createNode(10);
//    head->next = createNode(20);
//    head->next->next = createNode(30);
//    head->next->next->next = createNode(40);
//    head->next->next->next->next = createNode(50);
//
//    ListNode *k1 = findKthFromEnd(head, 2);
//    printf("倒数第2个节点：%d\n", k1->data);  // 输出40
//
//    ListNode *k2 = findKthFromEnd(head, 5);
//    printf("倒数第5个节点：%d\n", k2->data);  // 输出10
//
//    // 释放内存
//    freeList(&head);
//    return 0;
//}

// 测试
//int main() {
//    // 无环链表：10→20→30→NULL
//    ListNode *head1 = createNode(10);
//    head1->next = createNode(20);
//    head1->next->next = createNode(30);
//    printf("无环链表：%d\n", hasCycle(head1));  // 输出0
//
//    // 有环链表：10→20→30→20（环）
//    ListNode *head2 = createNode(10);
//    head2->next = createNode(20);
//    head2->next->next = createNode(30);
//    head2->next->next->next = head2->next;  // 30指向20，形成环
//    printf("有环链表：%d\n", hasCycle(head2));  // 输出1
//
//    // 释放无环链表内存（有环链表需手动释放，避免死循环）
//    freeList(&head1);
//    free(head2->next->next);
//    free(head2->next);
//    free(head2);
//    return 0;
//}

// 测试 针对margeTwoLists
int main() {
    // 链表1：10→30→50→NULL
    ListNode *l1 = createNode(10);
    l1->next = createNode(30);
    l1->next->next = createNode(50);

    // 链表2：20→40→60→NULL
    ListNode *l2 = createNode(20);
    l2->next = createNode(40);
    l2->next->next = createNode(60);

    // 合并
    ListNode *merged = mergeTwoLists(l1, l2);
    printf("合并后链表：");
    traverse(merged);

    // 释放内存
    freeList(&merged);
    return 0;
}