#include<stdio.h>
#include<string.h>
//标准库：用于malloc、free以及其他扩展接口
#include<stdlib.h>
#include "../utf8support.h"

//宏定义
#define MAXVALUE 10000  // 最大权值（用于初始化哈夫曼树父节点，确保初始不被选作最小节点）
#define MAXBIT 20       // 哈夫曼编码最大长度：20位足够容纳所有字符的编码，防止溢出
#define n 54            // 字符集大小：26大写+26小写+空格+点号=54
#define m (2*n-1)       // 哈夫曼树总节点数（n个叶子节点+ n-1个非叶子节点）

// 测试字符串
char test_str[] = "Programmers are perpetual optimists. Most of them think that the way to write a program is to run to the keyboard and start typing. Shortly thereafter the fully debugged program is finished.";

// 哈夫曼树节点结构定义
typedef struct {
    char ch;         // 节点对应字符：仅叶子节点有效
    int weight;      // 节点权重：叶子节点=字符出现频率，非叶子节点=左右子节点权重之和
    int parent;      // 父节点索引（-1表示无父节点）
    int Lchild;      // 左孩子索引（-1表示无左孩子）
    int Rchild;      // 右孩子索引（-1表示无右孩子）
} Htreetype;

// 哈夫曼编码表结构定义
typedef struct {
    char bit[MAXBIT];  // 存储二进制编码（0/1序列）
    int start;         // 编码在bit数组中的起始位置（从后往前存储）
    char ch;           // 对应的字符
} Hcodetype;

// 函数声明
void InitHuffmanTree(Htreetype ht[]);                // 初始化哈夫曼树
void CountFrequency(Htreetype ht[]);                 // 统计字符频率
void CreateHuffmanTree(Htreetype ht[]);              // 构建哈夫曼树
void CreateHuffmanCode(Htreetype ht[], Hcodetype hc[]); // 生成哈夫曼编码表
void show(Htreetype t[], Hcodetype code[]);          // 打印编码表
void Encode(Hcodetype code[], char EncodedBits[], int *len); // 对字符串编码
void Decode(Htreetype t[], char EncodedBits[], int length); // 对编码串译码
void CalculateCompression(Hcodetype hc[], Htreetype ht[]); // 计算压缩效果

// 初始化哈夫曼树：所有节点权重、父母、子女索引置为-1
void InitHuffmanTree(Htreetype ht[]) {
    for (int i = 0; i < m; i++) {
        ht[i].weight = 0;   //权重初始化为0
        ht[i].parent = -1;  //父节点初始化为-1
        ht[i].Lchild = -1;  //左孩子初始化为-1
        ht[i].Rchild = -1;  //右孩子初始化为-1（即表示无父节点，左孩子和右孩子）
    }
    // 初始化字符集：0-25大写A-Z，26-51小写a-z，52空格，53点号
    for (int i = 0; i < 26; i++) {
        ht[i].ch = 'A' + i;        // 大写字母
        ht[i + 26].ch = 'a' + i;   // 小写字母
    }
    ht[52].ch = ' ';               // 空格
    ht[53].ch = '.';               // 点号
}

// 统计测试字符串中各字符的出现频率（权重）
void CountFrequency(Htreetype ht[]) {
    int len = strlen(test_str);  //计算测试字符串的长度
    //遍历字符串的每个字符，更新对应节点的频率（权重）
    for (int i = 0; i < len; i++) {
        char c = test_str[i];   //当前遍历到的字符
        if (c >= 'A' && c <= 'Z') {
            ht[c - 'A'].weight++;       // 大写字母对应索引0-25
        } else if (c >= 'a' && c <= 'z') {
            ht[c - 'a' + 26].weight++;  // 小写字母对应索引26-51
        } else if (c == ' ') {
            ht[52].weight++;            // 空格对应索引52
        } else if (c == '.') {
            ht[53].weight++;            // 点号对应索引53
        }
        // 忽略其他未定义字符
    }
}

// 构建哈夫曼树：按权重合并节点，生成哈夫曼树
void CreateHuffmanTree(Htreetype ht[]) {
    int i, j, k;
    int min1, min2;  // 存储每次找到的两个最小权重值
    int idx1, idx2;  // 存储两个最小权重节点的索引

    // 先统计字符频率，初始化频率（必须在构建树前执行）
    CountFrequency(ht);

    // 构建n-1个非叶子节点（总节点数m=2n-1，需n-1次合并）
    for (k = n; k < m; k++) {
        //k从n开始：0~n-1是叶子，n~m-1是非叶子
        min1 = min2 = MAXVALUE;  // 初始化最小权重为最大值，确保能被更小值替换
        idx1 = idx2 = -1;        // 初始化最小节点索引为无效值

        //遍历所有已经存在的节点（0~n-1）
        // 查找当前无父节点（亦即parent=-1）的两个最小权重节点
        for (i = 0; i < k; i++) {
            //条件：无父结点（未被合并），且权重小于当前min1
            if (ht[i].parent == -1 && ht[i].weight < min1) {
                //把原来的min1和idx1降级为min2和idx2
                min2 = min1;
                idx2 = idx1;
                //更新min1和idx1为当前节点的权重和索引
                min1 = ht[i].weight;
                idx1 = i;
            }
            //条件：无父节点且权重小于min2但大于min1
            else if (ht[i].parent == -1 && ht[i].weight < min2) {
                //只更新min2和idx2
                min2 = ht[i].weight;
                idx2 = i;
            }
        }

        // 将两个最小节点合并为新节点k
        ht[idx1].parent = k;    // 第一个最小节点的父节点设为k
        ht[idx2].parent = k;    // 第二个最小节点的父节点设为k
        ht[k].Lchild = idx1;    // 新节点k的左孩子为idx1（约定左0右1）
        ht[k].Rchild = idx2;    // 新节点k的右孩子为idx2
        ht[k].weight = ht[idx1].weight + ht[idx2].weight;  // 新节点权重为两节点之和
    }
}

// 生成哈夫曼编码表：从叶子节点回溯到根节点，记录路径（左0右1）
void CreateHuffmanCode(Htreetype ht[], Hcodetype hc[]) {
    int i, j, p;
    Hcodetype cd;  // 临时存储单个字符的编码（循环复用，减少内存开销）

    // 遍历每个叶子节点（0~n-1），为每个字符生成编码
    for (i = 0; i < n; i++) {
        cd.start = MAXBIT - 1;  // 编码从bit数组末尾开始存储（因为回溯是从叶子到根，方便后续正向输出）
        cd.ch = ht[i].ch;       // 绑定当前编码对应字符（与叶子节点一致）
        p = ht[i].parent;       // p指向当前节点的父节点（从父节点开始回溯）
        j = i;                  // j指向当前叶子节点（初始位置）

        //回溯循环
        // 回溯到根节点（parent=-1）
        while (p != -1) {
            if (ht[p].Lchild == j) {
                //当前节点j是父节点p的左孩子，则路径记为‘0’（与建树时左0右1约定一致）
                cd.bit[cd.start--] = '0';
            } else {
                //当前节点j时父节点p的右孩子，则路径记为‘1’
                cd.bit[cd.start--] = '1';
            }
            j = p;              // 移动到父节点（亦即下一轮回溯父节点的父节点）
            p = ht[p].parent;   // 更新父节点索引
        }
        cd.start++;  // 回溯结束后多减了1，须+1指向编码的第一个有效位
        hc[i] = cd;  // 将临时编码存入编码表
    }
}

// 打印哈夫曼编码表
void show(Htreetype t[], Hcodetype code[]) {
    int i, j;
    printf("哈夫曼编码表（字符: 编码）：\n");
    //遍历所有叶子节点 0~n-1
    for (i = 0; i < n; i++) {
        // 只打印测试字符串中出现过的字符（权重>0）
        if (t[i].weight > 0) {
            //打印当前字符
            printf("%c: ", code[i].ch);
            //从start开始打印bit数组，输出完整编码，start前的编码无效
            for (j = code[i].start; j < MAXBIT; j++) {
                printf("%c", code[i].bit[j]);
            }

            //打印字符出现次数（权重），亦即“频率高编码短”
            printf("  (出现次数：%d)\n", t[i].weight);
        }
    }
}

// 对测试字符串进行编码：根据编码表生成二进制编码串
void Encode(Hcodetype code[], char EncodedBits[], int *len) {
    //定义i为原始字符串索引，cnt为编码穿当前长度（亦即bit数）
    int i = 0, cnt = 0;
    //计算原始字符串长度
    int str_len = strlen(test_str);

    //遍历原始字符串的每个字符，生成编码
    while (i < str_len) {
        //当前要编码的字符
        char c = test_str[i];
        //存储当前字符在编码表中的索引
        int j;
        // 找到当前字符在编码表中的索引
        if (c >= 'A' && c <= 'Z') {
            j = c - 'A';
        } else if (c >= 'a' && c <= 'z') {
            j = c - 'a' + 26;
        } else if (c == ' ') {
            j = 52;
        } else if (c == '.') {
            j = 53;
        } else {
            i++;
            continue;  // 跳过未定义字符
        }

        // 计算当前字符的编码长度（有效编码位数）
        int code_len = MAXBIT - code[j].start;
        //memcpy亦即内存拷贝，将当前字符的编码复制到编码串数组
        memcpy(&EncodedBits[cnt], &code[j].bit[code[j].start], code_len);
        //更新编码串总长度
        cnt += code_len;
        //处理下一个
        i++;
    }
    EncodedBits[cnt] = '\0';  // 编码串结尾置空
    *len = cnt;               // 返回编码串长度（bit数）
}

// 对编码串进行译码：根据哈夫曼树还原为原始字符串
void Decode(Htreetype t[], char EncodedBits[], int length) {
    int cnt = 0;    // 编码串当前遍历到的索引（bit位）
    int p = m - 1;  // 从哈夫曼树根节点开始（根节点索引为m-1）

    printf("译码结果：\n");

    // 遍历编码串的所有bit位
    while (cnt < length) {
        // 遍历到叶子节点（无左右孩子）时，输出字符并重置为根节点
        while (t[p].Lchild != -1 && t[p].Rchild != -1) {
            if (EncodedBits[cnt++] == '0') {  // 修复：0走左子树
                p = t[p].Lchild;
            } else {
                p = t[p].Rchild;  // 修复：1走右子树
            }
        }
        printf("%c", t[p].ch);  // 输出叶子节点对应的字符
        p = m - 1;              // 重置为根节点，准备下一个字符的译码
    }
    printf("\n");
}
// 计算并分析压缩效果
void CalculateCompression(Hcodetype hc[], Htreetype ht[]) {
    int original_len = strlen(test_str) * 6;  // 原始长度：假设每个字符6位（简化计算）
    int encoded_len = 0;                      // 编码后总长度（bit数）

    // 计算编码后总长度：每个字符的频率×编码长度之和
    for (int i = 0; i < n; i++) {
        if (ht[i].weight > 0) {
            int code_len = MAXBIT - hc[i].start;
            encoded_len += ht[i].weight * code_len;
        }
    }

    // 输出压缩分析结果
    printf("\n压缩效果分析：\n");
    printf("原始字符串长度（按每个字符6位计算）：%d bit\n", original_len);
    printf("编码后字符串长度：%d bit\n", encoded_len);
    printf("压缩比：%.2f%%（编码长度/原始长度）\n", (float)encoded_len / original_len * 100);
    printf("说明：哈夫曼编码通过“频率高的字符用短编码”实现无损压缩，频率分布越不均匀，压缩效果越好。\n");
}

// 主函数：串联哈夫曼树构建、编码、译码、压缩分析流程
int main() {
    INIT_UTF8_CONSOLE();
    Htreetype ht[m];    // 哈夫曼树数组
    Hcodetype hc[n];    // 哈夫曼编码表数组
    char EncodedBits[1024];  // 存储编码后的二进制串（足够容纳测试字符串）
    int encoded_len;    // 编码串长度

    // 1. 初始化并构建哈夫曼树
    InitHuffmanTree(ht);
    CreateHuffmanTree(ht);

    // 2. 生成并打印编码表
    CreateHuffmanCode(ht, hc);
    show(ht, hc);

    // 3. 对测试字符串编码并输出
    printf("\n编码结果（二进制串）：\n");
    Encode(hc, EncodedBits, &encoded_len);
    printf("%s\n", EncodedBits);
    printf("编码后长度：%d bit\n", encoded_len);

    // 4. 对编码串译码并输出
    Decode(ht, EncodedBits, encoded_len);

    // 5. 计算并输出压缩效果
    CalculateCompression(hc, ht);

    return 0;
}