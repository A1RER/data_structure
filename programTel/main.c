/*
 * 通话记录管理系统
 * 功能：实现通话记录的添加、删除、遍历（正序/倒序）、查询最近记录等操作
 * 数据结构：双向链表（适合动态数据的增删和双向遍历）
 */

#include <stdio.h>   // 标准输入输出函数（printf、scanf等）
#include <stdlib.h>  // 内存管理函数（malloc、free等）
#include <string.h>  // 字符串处理函数（strcmp、strcspn等）
#include <time.h>    // 时间处理函数（time、localtime等）

/*
 * 通话类型枚举
 * 用字符常量表示三种通话类型，便于输入和判断
 */
typedef enum {
    INCOMING = 'I',  // 来电（对应输入字符'I'）
    OUTGOING = 'O',  // 去电（对应输入字符'O'）
    MISSED = 'M'     // 未接（对应输入字符'M'）
} CallType;

/*
 * 通话记录结构体（双向链表节点）
 * 存储单条通话记录的所有信息，及链表指针
 */
typedef struct CallRecord {
    char number[20];         // 电话号码（最长19位，预留1位存字符串结束符'\0'）
    time_t timestamp;        // 通话时间戳（从1970-01-01 00:00:00到当前的秒数）
    int duration;            // 通话时长（单位：秒，未接电话可设为0）
    CallType type;           // 通话类型（来电/去电/未接）
    struct CallRecord* prev; // 前驱指针（指向链表中前一个节点）
    struct CallRecord* next; // 后继指针（指向链表中后一个节点）
} CallRecord;

/*
 * 全局变量：双向链表的头指针和尾指针
 * head：指向链表第一个节点（最早的记录）
 * tail：指向链表最后一个节点（最新的记录）
 */
CallRecord* head = NULL;
CallRecord* tail = NULL;

/*
 * 函数声明
 * 按功能分类：系统初始化、菜单显示、记录操作、辅助功能
 */
void initSystem();           // 初始化系统（链表置空）
void displayMenu();          // 显示功能菜单
void addRecord();            // 添加通话记录（插入链表尾部）
void deleteRecord();         // 删除指定号码的通话记录
void traverseForward();      // 向前遍历（从新到旧，尾->头）
void traverseBackward();     // 向后遍历（从旧到新，头->尾）
void findRecentCalls();      // 查找最近3条记录
void displayRecord(CallRecord* record); // 显示单条记录详情
void freeAllRecords();       // 释放所有记录的内存（防止泄漏）
char* formatTime(time_t timestamp);     // 格式化时间戳为字符串
char* getCallTypeName(CallType type);   // 转换通话类型为中文

/*
 * 添加通话记录
 * 功能：从键盘输入记录信息，创建新节点并插入链表尾部（最新记录）
 */
void addRecord() {
    // 1. 为新记录分配内存
    CallRecord* newRecord = (CallRecord*)malloc(sizeof(CallRecord));
    if (!newRecord) {  // 内存分配失败的判断（异常处理）
        printf("内存分配失败！\n");
        return;
    }

    // 2. 输入电话号码（使用fgets避免缓冲区溢出）
    printf("请输入电话号码: ");
    fgets(newRecord->number, 20, stdin);
    // 去除输入中的换行符（fgets会读取回车符'\n'）
    newRecord->number[strcspn(newRecord->number, "\n")] = '\0';

    // 3. 输入通话类型并转换为枚举值
    printf("请输入通话类型(I:来电/O:去电/M:未接): ");
    char type;
    scanf("%c", &type);  // 读取单个字符
    // 三目运算符判断类型（默认未接）
    newRecord->type = (type == 'I') ? INCOMING : (type == 'O') ? OUTGOING : MISSED;

    // 4. 输入通话时长
    printf("请输入通话时长(秒): ");
    scanf("%d", &newRecord->duration);
    getchar();  // 清除输入缓冲区残留的换行符（避免影响下次输入）

    // 5. 设置时间戳为当前系统时间
    newRecord->timestamp = time(NULL);

    // 6. 初始化新节点的指针（插入链表尾部）
    newRecord->prev = tail;  // 新节点的前驱是原来的尾节点
    newRecord->next = NULL;  // 新节点是最后一个，后继为NULL

    // 7. 更新链表的头/尾指针
    if (tail) {
        // 链表非空：原来的尾节点的后继指向新节点
        tail->next = newRecord;
    } else {
        // 链表为空：新节点既是头也是尾
        head = newRecord;
    }
    tail = newRecord;  // 更新尾指针为新节点

    printf("记录添加成功！\n");
}

/*
 * 删除通话记录
 * 功能：根据输入的电话号码查找并删除第一条匹配的记录
 */
void deleteRecord() {
    // 先判断链表是否为空
    if (!head) {
        printf("没有记录可删除！\n");
        return;
    }

    // 输入要删除的电话号码
    char number[20];
    printf("请输入要删除的电话号码: ");
    fgets(number, 20, stdin);
    number[strcspn(number, "\n")] = '\0';  // 去除换行符

    // 遍历链表查找匹配的记录
    CallRecord* current = head;
    while (current) {
        // 比较电话号码（strcmp返回0表示相等）
        if (strcmp(current->number, number) == 0) {
            // 找到记录，处理前驱和后继指针的连接

            // 情况1：当前节点有前驱（不是头节点）
            if (current->prev) {
                current->prev->next = current->next;  // 前驱的后继指向当前的后继
            } else {
                // 情况2：当前节点是头节点（无前驱）
                head = current->next;  // 头指针更新为当前的后继
            }

            // 情况3：当前节点有后继（不是尾节点）
            if (current->next) {
                current->next->prev = current->prev;  // 后继的前驱指向当前的前驱
            } else {
                // 情况4：当前节点是尾节点（无后继）
                tail = current->prev;  // 尾指针更新为当前的前驱
            }

            // 释放当前节点的内存
            free(current);
            printf("记录已删除！\n");
            return;  // 只删除第一条匹配的记录，直接返回
        }
        current = current->next;  // 继续查找下一个节点
    }

    // 遍历结束未找到匹配记录
    printf("未找到该号码的记录！\n");
}

/*
 * 向前遍历记录
 * 功能：从最新记录（尾节点）遍历到最早记录（头节点），即"从新到旧"
 */
void traverseForward() {
    if (!tail) {  // 链表为空的判断
        printf("没有通话记录！\n");
        return;
    }

    printf("\n=== 通话记录（从新到旧） ===\n");
    CallRecord* current = tail;  // 从尾节点开始
    while (current) {
        displayRecord(current);  // 显示当前节点信息
        current = current->prev; // 移动到前驱节点（上一条记录）
    }
}

/*
 * 向后遍历记录
 * 功能：从最早记录（头节点）遍历到最新记录（尾节点），即"从旧到新"
 */
void traverseBackward() {
    if (!head) {  // 链表为空的判断
        printf("没有通话记录！\n");
        return;
    }

    printf("\n=== 通话记录（从旧到新） ===\n");
    CallRecord* current = head;  // 从头节点开始
    while (current) {
        displayRecord(current);  // 显示当前节点信息
        current = current->next; // 移动到后继节点（下一条记录）
    }
}

/*
 * 格式化时间戳
 * 功能：将time_t类型的时间戳转换为"年-月-日 时:分"的字符串格式
 */
char* formatTime(time_t timestamp) {
    static char timeStr[20];  // 静态变量：保存转换后的字符串（函数返回后仍有效）
    struct tm* localTime = localtime(&timestamp);  // 转换为本地时间结构体
    // 格式化时间：%Y-年，%m-月，%d-日，%H-时（24小时制），%M-分
    strftime(timeStr, 20, "%Y-%m-%d %H:%M", localTime);
    return timeStr;  // 返回格式化后的字符串
}

/*
 * 获取通话类型的中文名称
 * 功能：将CallType枚举值转换为对应的中文描述（方便显示）
 */
char* getCallTypeName(CallType type) {
    switch (type) {
        case INCOMING: return "来电";
        case OUTGOING: return "去电";
        case MISSED: return "未接";
        default: return "未知";  // 异常情况处理
    }
}

/*
 * 显示单条记录详情
 * 功能：打印一条通话记录的所有信息（号码、时间、时长、类型）
 */
void displayRecord(CallRecord* record) {
    printf("号码: %s | 时间: %s | 时长: %d秒 | 类型: %s\n",
           record->number,           // 电话号码
           formatTime(record->timestamp),  // 格式化后的时间
           record->duration,         // 通话时长
           getCallTypeName(record->type)   // 中文类型名称
    );
}

/*
 * 查找最近3条记录
 * 功能：从最新记录（尾节点）开始，获取前3条记录（不足3条则显示所有）
 */
void findRecentCalls() {
    if (!tail) {  // 链表为空的判断
        printf("没有通话记录！\n");
        return;
    }

    printf("\n=== 最近3条通话记录 ===\n");
    CallRecord* current = tail;  // 从最新记录开始
    int count = 0;               // 计数：已显示的记录数
    while (current && count < 3) {  // 限制最多3条
        displayRecord(current);
        current = current->prev;  // 移动到上一条记录
        count++;
    }
}

/*
 * 释放所有记录的内存
 * 功能：遍历链表，释放每个节点的内存，防止内存泄漏
 * 调用时机：程序退出前
 */
void freeAllRecords() {
    CallRecord* current = head;  // 从头节点开始
    while (current) {
        CallRecord* temp = current;  // 临时保存当前节点
        current = current->next;     // 移动到下一个节点
        free(temp);                  // 释放当前节点内存
    }
    // 释放后将头、尾指针置空（避免野指针）
    head = tail = NULL;
}


/*
 * 初始化系统
 * 功能：将链表的头、尾指针置空，显示系统标题
 */
void initSystem() {
    head = NULL;  // 头指针初始化为空
    tail = NULL;  // 尾指针初始化为空
    printf("=== 通话记录管理系统 ===\n");
}

/*
 * 显示菜单
 * 功能：打印系统支持的所有功能选项，提示用户输入
 */
void displayMenu() {
    printf("\n========== 菜单 ==========\n");
    printf("1. 添加通话记录\n");
    printf("2. 删除通话记录\n");
    printf("3. 向前遍历记录（从新到旧）\n");
    printf("4. 向后遍历记录（从旧到新）\n");
    printf("5. 查找最近通话记录\n");
    printf("6. 退出系统\n");
    printf("==========================\n");
}

/*
 * 主函数
 * 功能：程序入口，控制整体流程（显示菜单、接收输入、调用对应功能）
 */
int main() {
    initSystem();  // 初始化系统
    int choice;    // 保存用户的菜单选择

    // 循环显示菜单，直到用户选择退出（6）
    do {
        displayMenu();  // 显示功能菜单
        printf("请输入您的选择: ");
        scanf("%d", &choice);
        getchar();  // 清除输入缓冲区的换行符（避免影响后续输入）

        // 根据用户选择调用对应功能
        switch (choice) {
            case 1:
                addRecord();       // 添加记录
                break;
            case 2:
                deleteRecord();    // 删除记录
                break;
            case 3:
                traverseForward(); // 从新到旧遍历
                break;
            case 4:
                traverseBackward();// 从旧到新遍历
                break;
            case 5:
                findRecentCalls(); // 查找最近3条
                break;
            case 6:
                printf("系统正在退出...\n");
                break;
            default:
                printf("无效的选择，请重新输入！\n");
        }
        printf("\n");  // 输出空行，分隔不同操作的结果
    } while (choice != 6);

    freeAllRecords();  // 退出前释放所有内存
    return 0;
}
