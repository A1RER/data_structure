#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../utf8support.h"

#define TABLE_SIZE 10000  // 哈希表大小（题目要求长度为1万的结构体数组）

// 定义QQ用户信息结构体（链表节点）：存储单条QQ用户数据，用于拉链法解决冲突
typedef struct QQUser {
    char qq[10];           // QQ号（5-9位数字，数组长度10适配最长9位+结束符'\0'）
    char phone[12];        // 手机号码（11位中国手机号+结束符'\0'）
    struct QQUser *next;   // 指向下一个节点的指针（拉链法核心：冲突时串联节点）
} QQUser;

// 哈希表结构：管理哈希数组和链长度统计
typedef struct {
    QQUser *table[TABLE_SIZE];  // 哈希表核心数组：每个元素是链表头指针
    int chainLengths[TABLE_SIZE]; // 记录每个数组位置的链表长度（方便统计最大链）
} HashTable;

/**
 * @brief 哈希函数：将QQ号映射为哈希表索引（核心：除留余数法）
 * @param qq 输入的QQ号字符串（5-9位数字）
 * @return int 哈希表索引（0 ~ TABLE_SIZE-1）
 */
int hashFunction(char *qq) {
    unsigned long long num = 0;  // 用无符号长整型避免QQ号过长导致溢出

    // 步骤1：将QQ号字符串转换为数字（如"12345"→12345）
    for (int i = 0; qq[i] != '\0'; i++) {
        num = num * 10 + (qq[i] - '0');  // 字符转数字：'0'的ASCII码是48，减去得到对应数值
    }

    // 步骤2：除留余数法取模（核心映射逻辑）
    return num % TABLE_SIZE;  // 确保索引在哈希表数组范围内
}

/**
 * @brief 初始化哈希表：将数组所有元素置空，链长度置0
 * @param ht 哈希表指针（需提前定义变量，传入地址初始化）
 */
void initHashTable(HashTable *ht) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        ht->table[i] = NULL;        // 初始时所有位置无链表，指针为NULL
        ht->chainLengths[i] = 0;    // 初始时所有链长度为0
    }
}

/**
 * @brief 插入用户信息到哈希表（拉链法解决冲突）
 * @param ht 哈希表指针
 * @param qq 要插入的QQ号
 * @param phone 对应的手机号码
 */
void insertUser(HashTable *ht, char *qq, char *phone) {
    // 步骤1：计算当前QQ号的哈希索引
    int index = hashFunction(qq);

    // 步骤2：创建新节点并分配内存
    QQUser *newUser = (QQUser*)malloc(sizeof(QQUser));
    if (newUser == NULL) {  // 内存分配失败检查（健壮性处理）
        printf("内存分配失败，插入失败！\n");
        return;
    }
    strcpy(newUser->qq, qq);       // 复制QQ号到节点
    strcpy(newUser->phone, phone); // 复制手机号到节点
    newUser->next = NULL;          // 新节点初始为链表尾，next设为NULL

    // 步骤3：拉链法插入（插入到链表头部，效率更高O(1)）
    if (ht->table[index] == NULL) {  // 该位置无冲突，直接作为链表头
        ht->table[index] = newUser;
        ht->chainLengths[index] = 1; // 链长度更新为1
    } else {  // 存在冲突，插入到链表头部（头插法）
        newUser->next = ht->table[index];  // 新节点指向原链表头
        ht->table[index] = newUser;        // 新节点成为新的链表头
        ht->chainLengths[index]++;         // 链长度+1
    }
}

/**
 * @brief 从txt文件读取QQ用户数据并插入哈希表
 * @param ht 哈希表指针
 * @param filename 输入文件路径（如"qq_data.txt"）
 * @return int 成功读取的记录数（0表示读取失败）
 * @note 文件格式要求：每行1条数据，格式为「QQ号 手机号」（空格分隔）
 */
int loadFromFile(HashTable *ht, const char *filename) {
    // 步骤1：打开文件（只读模式）
    FILE *file = fopen(filename, "r");
    if (file == NULL) {  // 文件打开失败（路径错误/文件不存在）
        printf("错误：无法打开文件 %s\n", filename);
        return 0;
    }

    char qq[10], phone[12];  // 临时存储读取的QQ号和手机号
    int count = 0;           // 统计读取的记录数

    printf("正在从文件读取数据...\n");

    // 步骤2：循环读取文件内容（每次读取1行的QQ号和手机号）
    // fscanf返回成功读取的字段数，返回2表示读取1条有效数据
    while (fscanf(file, "%s %s", qq, phone) == 2) {
        insertUser(ht, qq, phone);  // 读取后插入哈希表
        count++;

        // 进度提示：每读取1万条数据显示一次
        if (count % 10000 == 0) {
            printf("已读取 %d 条数据...\n", count);
        }
    }

    // 步骤3：关闭文件并返回结果
    fclose(file);
    printf("文件读取完成！共读入 %d 条数据。\n\n", count);
    return count;
}

/**
 * @brief 根据QQ号查找用户信息
 * @param ht 哈希表指针
 * @param qq 要查找的QQ号
 * @param position 输出参数：存储该QQ号的哈希表位置（传入地址）
 * @param comparisons 输出参数：查找过程中比较的次数（传入地址）
 * @return QQUser* 找到返回用户节点指针，未找到返回NULL
 */
QQUser* searchUser(HashTable *ht, char *qq, int *position, int *comparisons) {
    *position = hashFunction(qq);  // 计算哈希位置并赋值给输出参数
    *comparisons = 0;              // 比较次数初始化为0

    // 遍历对应位置的链表查找目标QQ号
    QQUser *current = ht->table[*position];
    while (current != NULL) {
        (*comparisons)++;  // 每比较一次计数+1
        if (strcmp(current->qq, qq) == 0) {  // QQ号完全匹配
            return current;  // 找到，返回当前节点
        }
        current = current->next;  // 未找到，继续遍历链表
    }

    return NULL;  // 遍历完链表未找到，返回NULL
}

/**
 * @brief 查找哈希表中最长的链表（最大链）及对应的位置
 * @param ht 哈希表指针
 * @param maxLength 输出参数：最大链长度（传入地址）
 * @param position 输出参数：最大链所在的哈希表位置（传入地址）
 */
void findMaxChain(HashTable *ht, int *maxLength, int *position) {
    *maxLength = 0;    // 初始最大链长度为0
    *position = -1;    // 初始位置为-1（表示无有效链）

    // 遍历哈希表所有位置，对比链长度
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (ht->chainLengths[i] > *maxLength) {
            *maxLength = ht->chainLengths[i];  // 更新最大链长度
            *position = i;                     // 更新最大链位置
        }
    }
}

/**
 * @brief 生成随机QQ号（5-9位数字，用于测试数据生成）
 * @param qq 输出参数：存储生成的QQ号（传入字符数组地址）
 */
void generateRandomQQ(char *qq) {
    int length = 5 + rand() % 5;  // 随机生成长度：5~9位（rand()%5得到0-4，+5后5-9）

    qq[0] = '1' + rand() % 9;     // 第一位不能为0，从1-9中随机

    // 生成后续数字（0-9随机）
    for (int i = 1; i < length; i++) {
        qq[i] = '0' + rand() % 10;
    }
    qq[length] = '\0';  // 字符串结束符（必须添加，避免乱码）
}

/**
 * @brief 生成随机手机号（符合中国手机号规则，用于测试数据生成）
 * @param phone 输出参数：存储生成的手机号（传入字符数组地址）
 */
void generateRandomPhone(char *phone) {
    // 中国手机号前缀（13/14/15/16/17/18/19开头）
    int prefix[] = {130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
                    150, 151, 152, 153, 155, 156, 157, 158, 159,
                    180, 181, 182, 183, 184, 185, 186, 187, 188, 189};

    int idx = rand() % 30;  // 从30个前缀中随机选1个
    sprintf(phone, "%d", prefix[idx]);  // 前缀转为字符串存入phone

    // 生成后8位数字（手机号共11位，前缀3位+后8位）
    for (int i = 0; i < 8; i++) {
        char digit = '0' + rand() % 10;
        strncat(phone, &digit, 1);  // 逐位拼接后8位
    }
}

/**
 * @brief 生成随机测试数据并直接插入哈希表（仅存内存，不写文件）
 * @param ht 哈希表指针
 * @param count 要生成的数据条数（如100000条）
 */
void generateTestData(HashTable *ht, int count) {
    printf("正在生成 %d 条测试数据...\n", count);

    for (int i = 0; i < count; i++) {
        char qq[10], phone[12];
        generateRandomQQ(qq);     // 生成随机QQ号
        generateRandomPhone(phone); // 生成随机手机号
        insertUser(ht, qq, phone);  // 插入哈希表

        // 进度提示：每生成1万条显示一次
        if ((i + 1) % 10000 == 0) {
            printf("已生成 %d 条数据...\n", i + 1);
        }
    }

    printf("数据生成完成！\n\n");
}

/**
 * @brief 生成随机测试数据并保存到txt文件（用于后续读取测试）
 * @param filename 输出文件路径（如"test_data.txt"）
 * @param count 要生成的数据条数
 */
void generateAndSaveTestData(const char *filename, int count) {
    // 打开文件（写入模式，文件不存在则创建，存在则覆盖）
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("错误：无法创建文件 %s\n", filename);
        return;
    }

    printf("正在生成并保存 %d 条测试数据到文件...\n", count);

    for (int i = 0; i < count; i++) {
        char qq[10], phone[12];
        generateRandomQQ(qq);
        generateRandomPhone(phone);
        fprintf(file, "%s %s\n", qq, phone);  // 按「QQ号 手机号」格式写入文件

        if ((i + 1) % 10000 == 0) {
            printf("已生成 %d 条数据...\n", i + 1);
        }
    }

    fclose(file);
    printf("数据已保存到文件 %s\n\n", filename);
}

/**
 * @brief 打印哈希表统计信息（核心：满足题目"打印最大拉链长度及位置"要求）
 * @param ht 哈希表指针
 */
void printStatistics(HashTable *ht) {
    int maxLength, position;
    findMaxChain(ht, &maxLength, &position);  // 获取最大链信息

    // 统计额外信息（答辩时展示更全面）
    int usedSlots = 0;    // 已使用的哈希表位置数（非空槽位）
    int totalUsers = 0;   // 总用户数（所有链表节点数之和）
    int emptySlots = 0;   // 空槽位数

    for (int i = 0; i < TABLE_SIZE; i++) {
        if (ht->table[i] != NULL) {
            usedSlots++;
            totalUsers += ht->chainLengths[i];
        } else {
            emptySlots++;
        }
    }

    // 格式化输出统计结果
    printf("\n========== 哈希表统计信息 ==========\n");
    printf("哈希表大小: %d\n", TABLE_SIZE);
    printf("总用户数: %d\n", totalUsers);
    printf("使用的槽位数: %d\n", usedSlots);
    printf("空槽位数: %d\n", emptySlots);
    printf("负载因子: %.2f%%\n", (usedSlots * 100.0) / TABLE_SIZE);  // 负载因子=使用槽位/总槽位
    printf("平均链长度: %.2f\n", totalUsers > 0 ? (float)totalUsers / usedSlots : 0);
    printf("最大链长度: %d\n", maxLength);  // 题目要求的核心输出
    printf("最大链位置: %d\n", position);   // 题目要求的核心输出
    printf("====================================\n\n");
}

/**
 * @brief 打印指定哈希表位置的链表内容（方便查看冲突节点）
 * @param ht 哈希表指针
 * @param position 要打印的哈希表位置
 */
void printChain(HashTable *ht, int position) {
    printf("\n位置 %d 的链表内容 (链长度: %d):\n", position, ht->chainLengths[position]);
    printf("%-10s %-15s\n", "QQ号", "手机号");
    printf("---------------------------\n");

    QQUser *current = ht->table[position];
    int count = 0;
    while (current != NULL) {
        printf("%-10s %-15s\n", current->qq, current->phone);
        current = current->next;
        count++;
    }
    printf("总计: %d 个用户\n\n", count);
}

/**
 * @brief 统计并打印链长度分布（答辩时体现对哈希表性能的分析）
 * @param ht 哈希表指针
 */
void printChainDistribution(HashTable *ht) {
    int distribution[100] = {0};  // 存储每个链长度对应的槽位数（假设最大链长≤100）
    int maxLen = 0;               // 实际最大链长

    // 统计每个链长度的槽位数
    for (int i = 0; i < TABLE_SIZE; i++) {
        int len = ht->chainLengths[i];
        if (len < 100) {
            distribution[len]++;
        }
        if (len > maxLen) {
            maxLen = len;
        }
    }

    // 打印分布结果
    printf("\n========== 链长度分布统计 ==========\n");
    printf("链长度\t槽位数\n");
    printf("-----------------------------------\n");
    for (int i = 0; i <= maxLen && i < 100; i++) {
        if (distribution[i] > 0) {
            printf("%d\t%d\n", i, distribution[i]);
        }
    }
    printf("====================================\n\n");
}

/**
 * @brief 释放哈希表所有内存（避免内存泄漏，必须调用）
 * @param ht 哈希表指针
 */
void freeHashTable(HashTable *ht) {
    // 遍历每个槽位，释放对应链表的所有节点
    for (int i = 0; i < TABLE_SIZE; i++) {
        QQUser *current = ht->table[i];
        while (current != NULL) {
            QQUser *temp = current;  // 保存当前节点地址
            current = current->next;  // 移动到下一个节点
            free(temp);               // 释放当前节点内存
        }
    }
}

/**
 * @brief 批量查找性能测试（答辩时展示哈希表查找效率）
 * @param ht 哈希表指针
 * @param testCount 测试次数（如1000次）
 */
void batchSearchTest(HashTable *ht, int testCount) {
    printf("\n========== 批量查找测试 ==========\n");
    printf("测试次数: %d\n", testCount);

    int totalComparisons = 0;  // 总比较次数
    int successCount = 0;      // 成功查找次数

    for (int i = 0; i < testCount; i++) {
        char qq[10];
        generateRandomQQ(qq);  // 随机生成QQ号进行查找

        int position, comparisons;
        QQUser *result = searchUser(ht, qq, &position, &comparisons);

        totalComparisons += comparisons;
        if (result != NULL) {
            successCount++;
        }
    }

    // 输出性能指标
    printf("成功查找: %d 次\n", successCount);
    printf("失败查找: %d 次\n", testCount - successCount);
    printf("平均比较次数: %.2f\n", (float)totalComparisons / testCount);  // 平均比较次数越低效率越高
    printf("===================================\n\n");
}

/**
 * @brief 主函数：系统入口，提供菜单交互
 * @return int 程序退出状态码（0表示正常退出）
 */
int main() {
    INIT_UTF8_CONSOLE();
    srand(time(NULL));  // 初始化随机数种子（确保每次生成的测试数据不同）

    HashTable ht;
    initHashTable(&ht);  // 初始化哈希表

    int choice;
    int dataLoaded = 0;  // 标记是否已加载数据（避免未加载数据时执行查找等操作）

    // 菜单循环（持续运行直到用户选择退出）
    while (1) {
        printf("\n========== QQ用户信息哈希管理系统 ==========\n");
        printf("1. 从文件读取数据（老师提供的txt文件）\n");
        printf("2. 生成随机测试数据（仅存内存）\n");
        printf("3. 生成随机测试数据（保存到文件）\n");
        printf("4. 手动添加用户\n");
        printf("5. 按QQ号查找用户\n");
        printf("6. 显示哈希表统计信息（含最大链）\n");
        printf("7. 显示最大链详细内容\n");
        printf("8. 显示链长度分布\n");
        printf("9. 批量查找性能测试\n");
        printf("0. 退出\n");
        printf("==========================================\n");
        printf("请选择操作: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {  // 核心功能：读取老师提供的txt文件
                char filename[100];
                printf("请输入老师提供的txt文件路径（如：qq_data.txt）: ");
                scanf("%s", filename);
                int count = loadFromFile(&ht, filename);
                if (count > 0) {
                    dataLoaded = 1;  // 标记数据已加载
                    printStatistics(&ht);  // 读取后自动显示统计信息（含最大链）
                }
                break;
            }
            case 2: {  // 生成测试数据（无需文件，直接用）
                int count;
                printf("请输入要生成的数据条数（如100000）: ");
                scanf("%d", &count);
                generateTestData(&ht, count);
                dataLoaded = 1;
                printStatistics(&ht);
                break;
            }
            case 3: {  // 生成测试数据并保存文件（备用）
                char filename[100];
                int count;
                printf("请输入输出文件名（如：test_data.txt）: ");
                scanf("%s", filename);
                printf("请输入要生成的数据条数（如100000）: ");
                scanf("%d", &count);
                generateAndSaveTestData(filename, count);
                break;
            }
            case 4: {  // 手动添加单个用户（测试用）
                char qq[10], phone[12];
                printf("请输入QQ号（5-9位数字）: ");
                scanf("%s", qq);
                printf("请输入手机号: ");
                scanf("%s", phone);
                insertUser(&ht, qq, phone);
                printf("添加成功！\n");
                break;
            }
            case 5: {  // 核心功能：按QQ号查找（题目要求）
                char qq[10];
                printf("请输入要查找的QQ号: ");
                scanf("%s", qq);

                int position, comparisons;
                QQUser *result = searchUser(&ht, qq, &position, &comparisons);

                if (result != NULL) {  // 查找成功
                    printf("\n========== 查找成功 ==========\n");
                    printf("QQ号: %s\n", result->qq);
                    printf("手机号: %s\n", result->phone);
                    printf("存储位置: %d\n", position);  // 题目要求输出存储位置
                    printf("比较次数: %d\n", comparisons);
                    printf("该位置链长度: %d\n", ht.chainLengths[position]);
                    printf("=============================\n");
                } else {  // 查找失败
                    printf("\n未找到该用户！\n");
                    printf("计算出的哈希位置: %d\n", position);
                    printf("该位置链长度: %d\n", ht.chainLengths[position]);
                    printf("比较次数: %d\n", comparisons);
                }
                break;
            }
            case 6:  // 显示统计信息（核心：满足题目"打印最大链长度及位置"）
                printStatistics(&ht);
                break;
            case 7: {  // 显示最大链内容（答辩时演示冲突解决）
                int maxLength, position;
                findMaxChain(&ht, &maxLength, &position);
                if (maxLength > 0) {
                    printChain(&ht, position);
                } else {
                    printf("哈希表为空！\n");
                }
                break;
            }
            case 8:  // 显示链长度分布（答辩加分项）
                printChainDistribution(&ht);
                break;
            case 9:  // 批量查找测试（答辩加分项）
                if (!dataLoaded) {
                    printf("请先加载数据或生成测试数据！\n");
                    break;
                }
                int testCount;
                printf("请输入测试次数（如1000）: ");
                scanf("%d", &testCount);
                batchSearchTest(&ht, testCount);
                break;
            case 0:  // 退出程序（释放内存）
                printf("正在释放内存...\n");
                freeHashTable(&ht);
                printf("感谢使用！\n");
                return 0;
            default:  // 无效选择处理
                printf("无效的选择！\n");
        }
    }

    return 0;
}