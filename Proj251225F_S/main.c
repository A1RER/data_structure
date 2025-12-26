#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utf8support.h"

// 学生信息结构体：仅存储学生的核心数据字段
typedef struct StudentInfo {
    char id[20];        // 学号（字符串类型，适配可能含字母的学号格式）
    char name[50];      // 姓名（预留足够长度，适配较长姓名）
    float score;        // 成绩（浮点型，支持小数成绩）
} StudentInfo;

// 链表节点结构体：封装数据指针和下一个节点指针
typedef struct ListNode {
    StudentInfo *data;  // 指向学生信息的指针（数据域）
    struct ListNode *next;  // 指向下一个节点的指针（指针域）
} ListNode;

/**
 * @brief 按学号升序插入新学生节点（核心：保持链表有序）
 * @param head 链表头指针（传入原链表头部，用于遍历查找插入位置）
 * @param id 新学生学号
 * @param name 新学生姓名
 * @param score 新学生成绩
 * @return ListNode* 插入后的新链表头指针（可能更新头节点，故返回新头部）
 */
ListNode* insertByID(ListNode *head, char *id, char *name, float score) {
    // 1. 创建学生信息结构体并分配内存
    StudentInfo *newInfo = (StudentInfo*)malloc(sizeof(StudentInfo));
    if (newInfo == NULL) {  // 内存分配失败检查
        printf("内存分配失败！\n");
        exit(1);  // 异常退出程序
    }
    // 给学生信息赋值
    strcpy(newInfo->id, id);
    strcpy(newInfo->name, name);
    newInfo->score = score;

    // 2. 创建链表节点并分配内存
    ListNode *newNode = (ListNode*)malloc(sizeof(ListNode));
    if (newNode == NULL) {  // 内存分配失败检查
        printf("内存分配失败！\n");
        free(newInfo);  // 释放已分配的学生信息内存，避免泄漏
        exit(1);
    }
    newNode->data = newInfo;  // 节点关联学生信息
    newNode->next = NULL;     // 新节点初始化为链表尾，next设为NULL

    // 3. 插入位置判断：链表为空 或 新学号小于头节点学号（插入到表头）
    if (head == NULL || strcmp(id, head->data->id) < 0) {
        newNode->next = head;  // 新节点指向原头节点
        return newNode;        // 新节点成为新的头节点
    }

    // 4. 遍历链表，找到插入位置（插入到current和current->next之间）
    ListNode *current = head;
    // 循环条件：当前节点的下一个节点存在，且下一个节点的学号小于新学号
    while (current->next != NULL && strcmp(current->next->data->id, id) < 0) {
        current = current->next;  // 向后移动指针
    }

    // 5. 插入新节点（链表插入标准操作：先连后，再连前）
    newNode->next = current->next;
    current->next = newNode;

    return head;  // 头节点未变，返回原头节点
}

/**
 * @brief 遍历输出所有学生信息（格式化打印，提升可读性）
 * @param head 链表头指针
 */
void printStudents(ListNode *head) {
    printf("\n========== 学生信息列表 ==========\n");
    // 格式化输出表头：%-15s 表示左对齐，占15个字符宽度
    printf("%-15s %-15s %-10s\n", "学号", "姓名", "成绩");
    printf("------------------------------------------\n");

    ListNode *current = head;
    while (current != NULL) {  // 遍历链表直到尾节点（NULL）
        // 成绩保留2位小数（%.2f），保证输出一致性
        printf("%-15s %-15s %-10.2f\n",
               current->data->id,
               current->data->name,
               current->data->score);
        current = current->next;  // 移动到下一个节点
    }
    printf("==================================\n\n");
}

/**
 * @brief 按学号查找学生（精确匹配）
 * @param head 链表头指针
 * @param id 要查找的学号
 * @return ListNode* 找到返回对应节点指针，未找到返回NULL
 */
ListNode* searchByID(ListNode *head, char *id) {
    ListNode *current = head;
    while (current != NULL) {
        // strcmp返回0表示两个字符串完全相等（学号匹配）
        if (strcmp(current->data->id, id) == 0) {
            return current;  // 找到，返回当前节点
        }
        current = current->next;  // 未找到，继续遍历
    }
    return NULL;  // 遍历完链表仍未找到，返回NULL
}

/**
 * @brief 按姓名查找学生（精确匹配）
 * @param head 链表头指针
 * @param name 要查找的姓名
 * @return ListNode* 找到返回对应节点指针，未找到返回NULL
 */
ListNode* searchByName(ListNode *head, char *name) {
    ListNode *current = head;
    while (current != NULL) {
        if (strcmp(current->data->name, name) == 0) {  // 姓名精确匹配
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/**
 * @brief 统计链表长度（辅助排序功能：确定数组大小）
 * @param head 链表头指针
 * @return int 链表节点个数（学生总数）
 */
int getLength(ListNode *head) {
    int count = 0;  // 计数器初始化
    ListNode *current = head;
    while (current != NULL) {
        count++;  // 每遍历一个节点，计数器+1
        current = current->next;
    }
    return count;
}

/**
 * @brief 按成绩降序排序（成绩相同时按姓名升序）
 * @param head 链表头指针
 * @return ListNode* 排序后的新链表头指针
 */
ListNode* sortByScore(ListNode *head) {
    // 边界条件：空链表或只有一个节点，无需排序
    if (head == NULL || head->next == NULL) {
        return head;
    }

    // 步骤1：获取链表长度，用于创建数组
    int len = getLength(head);

    // 步骤2：创建指针数组，存储每个节点的地址（链表转数组，简化排序）
    ListNode **arr = (ListNode**)malloc(len * sizeof(ListNode*));
    if (arr == NULL) {  // 内存分配检查
        printf("内存分配失败！\n");
        exit(1);
    }

    // 步骤3：将链表节点指针存入数组
    ListNode *current = head;
    int i = 0;
    while (current != NULL) {
        arr[i++] = current;
        current = current->next;
    }

    // 步骤4：冒泡排序（核心排序逻辑）
    for (i = 0; i < len - 1; i++) {  // 外层循环：控制排序轮数
        for (int j = 0; j < len - i - 1; j++) {  // 内层循环：每轮比较未排序部分
            // 排序规则：
            // 1. 成绩降序：arr[j]成绩 < arr[j+1]成绩 → 交换
            // 2. 成绩相等时：姓名升序（strcmp返回>0表示arr[j]姓名更大）→ 交换
            if (arr[j]->data->score < arr[j+1]->data->score ||
                (arr[j]->data->score == arr[j+1]->data->score &&
                 strcmp(arr[j]->data->name, arr[j+1]->data->name) > 0)) {
                // 交换两个节点的指针（仅交换地址，不移动节点数据）
                ListNode *temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }

    // 步骤5：重新连接链表（按排序后的数组顺序重构链表）
    for (i = 0; i < len - 1; i++) {
        arr[i]->next = arr[i+1];  // 前一个节点指向后一个节点
    }
    arr[len-1]->next = NULL;  // 最后一个节点设为尾节点（next=NULL）

    // 步骤6：更新头节点，释放数组内存（避免内存泄漏）
    head = arr[0];  // 数组第一个元素是新的头节点
    free(arr);      // 数组完成使命，释放内存

    return head;  // 返回排序后的头节点
}

/**
 * @brief 输出成绩前N名学生信息（成绩相同并列名次）
 * @param head 排序后的链表头指针（需先按成绩排序）
 * @param n 要显示的名次数
 */
void printTopN(ListNode *head, int n) {
    // 输入校验：链表为空或N≤0，直接返回
    if (head == NULL || n <= 0) {
        printf("无效的输入！\n");
        return;
    }

    printf("\n========== 前 %d 名学生信息 ==========\n", n);
    printf("%-10s %-15s %-15s %-10s\n", "名次", "学号", "姓名", "成绩");
    printf("----------------------------------------------------\n");

    ListNode *current = head;
    int rank = 1;        // 当前名次（初始为第1名）
    int count = 0;       // 已输出的学生人数
    float lastScore = -1;// 上一个学生的成绩（初始值设为无效成绩）

    // 循环条件：未遍历完链表 且 当前名次≤N
    while (current != NULL && rank <= n) {
        // 关键逻辑：成绩与上一个不同时，更新名次为当前已输出人数+1
        if (current->data->score != lastScore) {
            rank = count + 1;  // 并列名次核心：成绩相同则名次不变
        }

        // 仅输出名次≤N的学生
        if (rank <= n) {
            printf("%-10d %-15s %-15s %-10.2f\n",
                   rank,
                   current->data->id,
                   current->data->name,
                   current->data->score);
            lastScore = current->data->score;  // 更新上一个成绩
            count++;                           // 输出人数+1
            current = current->next;           // 移动到下一个节点
        } else {
            break;  // 名次超过N，退出循环
        }
    }
    printf("===============================================\n\n");
}

/**
 * @brief 释放链表所有节点及学生信息内存（避免内存泄漏，必须实现）
 * @param head 链表头指针
 */
void freeList(ListNode *head) {
    ListNode *current = head;
    while (current != NULL) {
        ListNode *temp = current;       // 保存当前节点地址
        free(current->data);            // 先释放学生信息内存
        current = current->next;        // 移动到下一个节点
        free(temp);                     // 再释放节点内存
    }
}

/**
 * @brief 主函数：系统入口，提供菜单交互
 * @return int 程序退出状态码（0表示正常退出）
 */
int main() {
    INIT_UTF8_CONSOLE();
    ListNode *head = NULL;  // 链表头指针初始化（空链表）
    int choice;             // 存储用户菜单选择

    // 初始化测试数据（无需手动输入，方便测试功能）
    head = insertByID(head, "20210001", "张三", 95.5);
    head = insertByID(head, "20210003", "李四", 88.0);
    head = insertByID(head, "20210002", "王五", 92.0);
    head = insertByID(head, "20210005", "赵六", 88.0);
    head = insertByID(head, "20210004", "钱七", 90.0);
    head = insertByID(head, "20210006", "孙八", 95.5);

    // 循环菜单：直到用户选择退出（0）
    while (1) {
        // 菜单界面打印
        printf("\n========== 学生信息管理系统 ==========\n");
        printf("1. 添加学生信息\n");
        printf("2. 显示所有学生信息\n");
        printf("3. 按学号查找\n");
        printf("4. 按姓名查找\n");
        printf("5. 按成绩排序\n");
        printf("6. 显示前N名学生\n");
        printf("0. 退出\n");
        printf("======================================\n");
        printf("请选择操作: ");
        scanf("%d", &choice);  // 读取用户选择

        // 菜单功能分支（switch-case）
        switch (choice) {
            case 1: {  // 添加学生信息（局部变量用大括号隔离，避免重复定义）
                char id[20], name[50];
                float score;
                // 读取用户输入的学生信息
                printf("请输入学号: ");
                scanf("%s", id);  // 学号不含空格，用%s读取
                printf("请输入姓名: ");
                scanf("%s", name);  // 姓名不含空格，用%s读取
                printf("请输入成绩: ");
                scanf("%f", &score);  // 成绩为浮点型，用%f读取
                // 调用插入函数，更新链表头
                head = insertByID(head, id, name, score);
                printf("添加成功！\n");
                break;
            }
            case 2:  // 显示所有学生信息
                printStudents(head);
                break;
            case 3: {  // 按学号查找
                char id[20];
                printf("请输入要查找的学号: ");
                scanf("%s", id);
                ListNode *result = searchByID(head, id);  // 调用查找函数
                if (result != NULL) {  // 找到学生
                    printf("\n查找成功！\n");
                    printf("学号: %s, 姓名: %s, 成绩: %.2f\n",
                           result->data->id,
                           result->data->name,
                           result->data->score);
                } else {  // 未找到
                    printf("未找到该学生！\n");
                }
                break;
            }
            case 4: {  // 按姓名查找
                char name[50];
                printf("请输入要查找的姓名: ");
                scanf("%s", name);
                ListNode *result = searchByName(head, name);
                if (result != NULL) {
                    printf("\n查找成功！\n");
                    printf("学号: %s, 姓名: %s, 成绩: %.2f\n",
                           result->data->id,
                           result->data->name,
                           result->data->score);
                } else {
                    printf("未找到该学生！\n");
                }
                break;
            }
            case 5:  // 按成绩排序
                head = sortByScore(head);  // 调用排序函数，更新头节点
                printf("排序完成！\n");
                printStudents(head);  // 排序后直接输出
                break;
            case 6: {  // 显示前N名
                int n;
                printf("请输入要显示的名次数: ");
                scanf("%d", &n);
                head = sortByScore(head);  // 先排序，再输出
                printTopN(head, n);
                break;
            }
            case 0:  // 退出程序
                printf("感谢使用！\n");
                freeList(head);  // 释放链表内存，避免泄漏
                return 0;  // 正常退出
            default:  // 无效选择
                printf("无效的选择！\n");
        }
    }

    return 0;  // 语法要求（实际不会执行到）
}

/*
思考题答案：如果改用顺序表（数组）实现上述功能

一、数据结构调整
typedef struct {
    StudentInfo data[1000];  // 静态数组（假设最多1000名学生）
    int length;              // 当前学生人数
} SeqList;  // 顺序表结构体

二、各功能实现思路
1. 按学号顺序建立：
   - 采用插入排序思想：每次插入新学生时，从后向前找到比新学号小的位置
   - 将该位置后的所有元素向后移动1位，插入新元素
   - 时间复杂度：O(n²)（移动元素耗时）

2. 输出学生信息：
   - 直接遍历数组从0到length-1，格式化打印每个元素
   - 时间复杂度：O(n)

3. 查找功能：
   - 按学号查找：顺序表有序，可使用二分查找，时间复杂度O(log n)
   - 按姓名查找：姓名无序，需遍历数组，时间复杂度O(n)

4. 按成绩排序：
   - 直接对数组data[0..length-1]排序（快速排序/归并排序）
   - 排序规则同链表版（成绩降序+姓名升序）
   - 时间复杂度：O(n log n)

5. 输出前N名：
   - 排序后直接输出数组前N个元素（成绩相同并列）
   - 时间复杂度：O(1)（排序后直接取前N个）

三、顺序表 vs 链表 对比
优点：
- 支持随机访问，二分查找效率高（学号查找比链表O(n)更优）
- 内存空间连续，缓存命中率高，访问速度快
- 实现简单（无需处理指针操作）

缺点：
- 插入/删除需移动大量元素，效率低（尤其是学生人数多时）
- 静态数组有容量限制，动态扩容需重新分配内存+复制数据
- 空间利用率低（未使用的数组元素占用内存）
*/