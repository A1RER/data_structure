#include "stdlib.h"
#include "stdio.h"
#include "../utf8support.h"

// 双链表核心定义
typedef struct DlistNode{
    int data;
    struct DlistNode *prev; //前驱指针
    struct DlistNode *next; //后继指针
}DlistNode;

/**
 * @brief 创建双链表节点
 * @param data 节点存储的整型数据
 * @return DlistNode* 成功返回新节点指针，失败返回NULL
 * @note 节点创建后prev和next默认初始化为NULL
 */
DlistNode* createDNote(int data){
    DlistNode *newNode=(DlistNode*) malloc(sizeof (DlistNode));
    newNode->data=data;
    newNode->prev=NULL;
    newNode->next=NULL;
    return newNode;
}

/**
 * @brief 正向遍历双链表
 * @param head 双链表头节点指针
 * @return void
 * @note 空链表会打印提示信息，遍历结果以空格分隔
 */
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

/**
 * @brief 反向遍历双链表
 * @param head 双链表头节点指针
 * @return void
 * @note 先定位到尾节点，再通过prev指针反向遍历
 */
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

/**
 * @brief 在指定节点后插入新节点
 * @param p 插入位置的前驱节点指针
 * @param data 新节点存储的整形数据
 * @return void
 * @note 若前驱节点为NULL则打印错误提示，不执行插入
 */
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

/**
 * @brief 释放双链表所有内存
 * @param head 指向双链表头节点指针的指针
 * @return void
 * @note 释放后会将头指针置NULL，避免野指针
 */
void freeDList(DlistNode  **head) {
    DlistNode  *current = *head;
    while (current != NULL) {
        DlistNode *next = current->next; // 先保存下一个节点
        free(current);
        current = next;
    }
    *head = NULL; // 置空头指针，避免野指针
}

/**
 * @brief 双链表头插法插入新节点
 * @param head 原双链表头节点指针
 * @param data 新节点存储后的整型数据
 * @return DlistNode* 插入后的新头节点指针
 * @note 空链表头插仍有效，返回新节点作为头
 */
DlistNode *insertHeadDlist(DlistNode *head,int data){
    DlistNode *newNode = createDNote(data);
    if(newNode==NULL)//校验节点创建结果
    {
        printf("头插失败！节点创建失败\n");
        return head;
    }
    //若原链表非空，更新原头节点的prev

    if(head!=NULL)
    {
        newNode->next=head;
        head->prev=newNode;
    }
    return newNode;//新节点作为新头
}

/**
 * @brief 按值删除双链表中第一个匹配的节点
 * @param head 原双链表节点的头指针
 * @param val 要删除的节点数据值
 * @return DlistNode* 删除后的新头节点指针
 * @note 支持删除头/中/尾节点，无匹配节点则返回原头
 */
DlistNode *deleteValDlist(DlistNode *head,int val){
    DlistNode *current=head;
    while (current != NULL) {
        if (current->data == val) {
            // 情况1：删除的是头节点
            if (current->prev == NULL) {
                head = current->next; // 新头是当前节点的next
                if (head != NULL) { // 若新头非空，更新其prev为NULL
                    head->prev = NULL;
                }
            }
                // 情况2：删除的是中间/尾节点
            else {
                current->prev->next = current->next; // 前驱的next跳过当前节点
                if (current->next != NULL) { // 若当前节点非尾节点，更新后继的prev
                    current->next->prev = current->prev;
                }
            }
            free(current); // 释放内存
            break;//只删除第一个匹配节点
        }
        current = current->next;//未匹配，继续遍历
    }
    return head;//返回可能更新后的头节点
}

/**
 * @brief 反转双链表
 * @param head 原双链表头节点指针
 * @return DlistNode* 反转后的新头节点指针
 * @note 空链表/单节点链表反转后返回原头
 */
DlistNode *reverseDlist(DlistNode *head){
    DlistNode *cur=head;
    DlistNode *temp=NULL; //临时保存next指针
    while(cur!=NULL)
    {
        //步骤1：保存当前节点的next,防止后续修改后丢失
        temp=cur->next;
        //步骤2：反转当前节点的prev和next
        cur->next=cur->prev;
        cur->prev=temp;
        //步骤2：更新头节点并后移当前节点
        head=cur;//更新头节点（循环到最后，cur指向的就是原来的尾节点）
        cur=temp;
    }
    return head;
}

/**
 * @brief 判断双链表是否存在环
 * @param head 双链表头节点指针
 * @return int 1表示有环 int 0 表示无环
 * @note 使用快慢指针法，时间复杂度O(n)，空间复杂度O(1)
 */
int hasCycleDlist(DlistNode *head){
    {
        if (head == NULL || head->next == NULL)
        { // 空/单节点无环
            return 0;
        }
        DlistNode *slow = head;
        DlistNode *fast = head->next; // 快指针初始领先1步，避免初始相遇
        while (slow != fast)
        {
            // 快指针到尾（无环）
            if (fast == NULL || fast->next == NULL)
            {
                return 0;
            }
            slow = slow->next;       // 慢指针走1步
            fast = fast->next->next; // 快指针走2步
        }
        return 1; // 相遇则有环
    }
}


//测试
int main() {
    INIT_UTF8_CONSOLE();
    // ========== 步骤1：创建空双链表，初始化头节点 ==========
    DlistNode *head = NULL;
    // 创建第一个节点（头节点）
    head = createDNote(10);
    printf("创建头节点10后：\n");
    traverseDlist(head); // 测试：正向遍历（输出10）

    // ========== 步骤2：插入节点，构建链表 10⇄20⇄30 ==========
    insertAfterDlist(head, 20);       // 在10后插入20
    insertAfterDlist(head->next, 30); // 在20后插入30
    printf("\n插入20、30后：\n");
    traverseDlist(head);       // 正向遍历：10 20 30
    traverseDlistReverse(head); // 反向遍历：30 20 10

    // ========== 步骤3：测试头插功能 ==========
    head = insertHeadDlist(head, 5); // 头插5 → 5⇄10⇄20⇄30
    printf("\n头插5后：\n");
    traverseDlist(head); // 正向遍历：5 10 20 30

    // ========== 步骤4：测试按值删除功能 ==========
    head = deleteValDlist(head, 20); // 删除20 → 5⇄10⇄30
    printf("\n删除20后：\n");
    traverseDlist(head);       // 正向遍历：5 10 30
    traverseDlistReverse(head); // 反向遍历：30 10 5

    // ========== 步骤5：测试反转功能 ==========
    head = reverseDlist(head); // 反转 → 30⇄10⇄5
    printf("\n反转后：\n");
    traverseDlist(head);       // 正向遍历：30 10 5
    traverseDlistReverse(head); // 反向遍历：5 10 30

    // ========== 步骤6：测试判环功能 ==========
    printf("\n双链表是否有环：%d（0=无环，1=有环）\n", hasCycleDlist(head)); // 输出0

    // ========== 步骤7：测试空链表场景 ==========
    DlistNode *emptyList = NULL;
    printf("\n测试空链表遍历：\n");
    traverseDlist(emptyList); // 输出“双链表为空！”

    // ========== 步骤8：释放内存 ==========
    freeDList(&head);
    freeDList(&emptyList); // 空链表释放无副作用
    printf("\n内存已释放，测试结束！\n");

    return 0;
}