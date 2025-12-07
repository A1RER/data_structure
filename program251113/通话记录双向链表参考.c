#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 通话类型枚举
typedef enum {
    INCOMING = 'I',  // 来电
    OUTGOING = 'O',  // 去电
    MISSED = 'M'     // 未接
} CallType;

// 通话记录结构体
typedef struct CallRecord {
    char number[20];         // 电话号码
    time_t timestamp;        // 通话时间戳
    int duration;           // 通话时长（秒）
    CallType type;          // 通话类型
    struct CallRecord* prev;// 前驱指针
    struct CallRecord* next;// 后继指针
} CallRecord;

// 全局变量：链表头尾指针
CallRecord* head = NULL;
CallRecord* tail = NULL;

// 函数声明
void initSystem();
void displayMenu();
void addRecord();
void deleteRecord();
void traverseForward();
void traverseBackward();
void findRecentCalls();
void displayRecord(CallRecord* record);
void freeAllRecords();
char* formatTime(time_t timestamp);
char* getCallTypeName(CallType type);

// 主函数
int main() {
    initSystem();
    int choice;
    
    do {
        displayMenu();
        printf("请输入您的选择: ");
        scanf("%d", &choice);
        getchar(); // 清除输入缓冲区
        
        switch (choice) {
            case 1:
                addRecord();
                break;
            case 2:
                deleteRecord();
                break;
            case 3:
                traverseForward();
                break;
            case 4:
                traverseBackward();
                break;
            case 5:
                findRecentCalls();
                break;
            case 6:
                printf("系统正在退出...\n");
                break;
            default:
                printf("无效的选择，请重新输入！\n");
        }
        printf("\n");
    } while (choice != 6);
    
    freeAllRecords();
    return 0;
}

// 初始化系统
void initSystem() {
    head = NULL;
    tail = NULL;
    printf("=== 通话记录管理系统 ===\n");
}

// 显示菜单
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
