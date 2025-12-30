#include "stdio.h"
#include "stdlib.h"
#include "limits.h" //用于INT_MAX(最小权值初始化)
#include "../utf8support.h"

//哈夫曼树节点结构体定义
//采用数组存储哈夫曼树，节点包含权值、双亲/左右孩子索引
typedef struct{
    int weight; //节点权值
    int parent; //双亲节点索引（0表示无父节点）
    int lchild; //左孩子节点索引（0表示无左孩子）
    int rchild; //右孩子节点索引
} HTNode,*Huffmantree;

//核心函数：构建哈夫曼树
/**
 * @brief 构建哈夫曼树
 * @param HT 指向哈夫曼树数组的指针（二级指针，用于修改外部数据）
 * @param n 叶子节点的数量
 */
void CreateHuffmanTree(Huffmantree *HT,int n){  //HuffmanTree *HT等价于HTNode **HT
    //边界处理：小于两个叶子节点无法创建哈夫曼树
    if(n<=1){
        printf("错误！叶子节点数必须大于1！\n");
        return;
    }

    int m=n+n-1;    //哈夫曼树总节点数公式：n个叶子+(n-1)个非叶子
    //分配内存：下标从1到m,0下标闲置（避免与“无节点”的0值混淆）
    *HT=(Huffmantree) malloc((m+1)* sizeof(HTNode));
    if(*HT==NULL){  //内存分配失败检查
        printf("错误！内存分配失败\n");
        exit(1);
    }

    //步骤1：初始化所有节点（1~m）的双亲，左右孩子为0
    for(int i=1;i<=m;i++){
        (*HT)[i].parent=0;
        (*HT)[i].lchild=0;
        (*HT)[i].rchild=0;
    }

    //步骤2：输入n个叶子节点的权值：
    printf("请输入%d个叶子节点的权值（空格分隔）：\n",n);
    for(int i=1;i<=n;i++){
        scanf("%d",&(*HT)[i].weight);
        //简单校验：权值不能为负
        if((*HT)[i].weight<0){
            printf("错误：权值不能为负数！\n");
            free(*HT);
            *HT=NULL;
            exit(1);
        }
    }

    //步骤3：构建n-1个非叶子节点（下标从n+1到m）
    for(int i=n+1;i<=m;i++){
        //s1:最小权值节点下标   s2:次小权值节点下标
        //min1:最小权值       min2:次小权值（初始值为INT_MAX,确保能被更新）
        int s1=0,s2=0,min1=INT_MAX,min2=INT_MAX;

        //遍历1~i-1的节点，找两个无父节点（未被合并）的最小权值节点
        for(int j=1;j<i;j++){
            if((*HT)[j].parent==0){ //仅考虑未被选过的节点
                if((*HT)[j].weight<min1){
                    //找到更小的权值：次小值＝原最小值，最小值=当前值
                    min2=min1;
                    s2=s1;
                    min1=(*HT)[j].weight;
                    s1=j;
                } else if ((*HT)[j].weight<min2){
                    //找到次小值时仅更新次小值
                    min2=(*HT)[j].weight;
                    s2=j;
                }
            }
        }

        //步骤4：绑定父子关系（新节点i为s1,s2的父节点）
        (*HT)[s1].parent=i; //s1的父节点是i
        (*HT)[s2].parent=i; //s2的父节点是i
        (*HT)[i].lchild=s1; //i的左孩子是s1（习惯：小权值放左，大权值放右）
        (*HT)[i].rchild=s2; //i的右孩子是s2
        (*HT)[i].weight=(*HT)[s1].weight+(*HT)[s2].weight;

        //调试输出：打印每一步合并的节点
        printf("合并节点：%d(权值：%d)+%d(权值：%d)=节点%d(权值%d)\n",
               s1,(*HT)[s1].weight,s2,(*HT)[s2].weight,i,(*HT)[i].weight);
    }
}

//辅助函数：计算哈夫曼树的带权路径长度
/**
 * @brief 计算哈夫曼树的带权路径长度
 * @param HT 哈夫曼树数组
 * @param n 叶子节点数量
 * @return 树的总WPL
 */
int CalculateWPL(Huffmantree HT,int n){
    if(HT==NULL||n<=1) return 0;

    int WPL=0;  //总带权路径长度
    int path_len=0; //单个叶子节点的路径长度（根到该节点的边数）

    //遍历所有叶子节点，计算每个节点的WPL再累加
    for(int i=1;i<=n;i++){
        path_len=0;
        int cur=i;  //从当前叶子节点向上找根
        //向上遍历直到根节点（parent=0）的节点是根
        while(HT[cur].parent!=0){
            path_len++; //每向上一层，路径长度+1
            cur=HT[cur].parent; //移动到父节点
        }
        WPL+=HT[i].weight*path_len; //单个节点WPL=权值×路径长度
        printf("叶子节点%d（权值%d）：路径长度=%d，WPL=%d\n",
               i, HT[i].weight, path_len, HT[i].weight * path_len);
    }
    return WPL;
}

//拓展函数：生成哈夫曼编码
void CreateHuffmanCode(Huffmantree HT,int n,char **code){
    if(HT==NULL||n<=1)return;

    //分配编码数组内存：下标为1~n，n+1个字符串
    code=(char**) malloc((n+1)*sizeof(char*));
    //临时数组：存储单个编码（最长路径长度不超过n-1）
    char* tmp_code=(char*)malloc(n* sizeof(char));
    tmp_code[n-1]='\0'; //字符串结束符

    for(int i=1;i<=n;i++){
        int len=0;  //编码长度
        int cur=i;  //当前节点
        int parent=HT[cur].parent;  //父节点

        //从叶子节向上遍历到根，记录路径（左0右1）
        while(parent!=0){
            if(HT[parent].lchild==cur){
                tmp_code[len++]='0';
            }else{
                tmp_code[len++]='1';    //左孩子编码为0，右孩子编码为1
            }
            cur=parent;
            parent=HT[cur].parent;
        }

        //分配当前叶子节点编码内存（长度+1，存结束符）
        code[i]=(char*) malloc((len+1)* sizeof(char));
        code[i][len]='\0';
        //反转临时数组（从叶子到根转为从根到叶子）
        for(int j=0;j<len;j++){
            code[i][j]=tmp_code[len-1-j];
        }
        printf("叶子节点%d（权值%d）的哈夫曼编码：%s\n", i, HT[i].weight, code[i]);


    }
    free(tmp_code);
}

// ====================== 5. 主函数：测试哈夫曼树构建与使用 ======================
int main() {
    INIT_UTF8_CONSOLE();
    Huffmantree HT = NULL;  // 哈夫曼树数组
    int n;                  // 叶子节点数量

    // 步骤1：输入叶子节点数量
    printf("请输入哈夫曼树的叶子节点数量：");
    scanf("%d", &n);

    // 步骤2：构建哈夫曼树
    CreateHuffmanTree(&HT, n);
    if (HT == NULL) {  // 构建失败则退出
        return 1;
    }

    // 步骤3：打印哈夫曼树的节点信息（调试/验证）
    int m = 2 * n - 1;
    printf("\n===== 哈夫曼树节点信息（下标1~%d）=====\n", m);
    printf("下标 | 权值 | 双亲 | 左孩子 | 右孩子\n");
    printf("----------------------------------------\n");
    for (int i = 1; i <= m; i++) {
        printf("%d\t| %d\t| %d\t| %d\t| %d\n",
               i, HT[i].weight, HT[i].parent, HT[i].lchild, HT[i].rchild);
    }

    // 步骤4：计算并打印WPL
    int WPL = CalculateWPL(HT, n);
    printf("\n===== 哈夫曼树总带权路径长度（WPL）=====\n");
    printf("总WPL = %d\n", WPL);

    // 步骤5：生成并打印哈夫曼编码（拓展功能）
    printf("\n===== 各叶子节点的哈夫曼编码（左0右1）=====\n");
    char **code = NULL;
    CreateHuffmanCode(HT, n, code);

    // 步骤6：释放哈夫曼树内存（避免内存泄漏）
    free(HT);
    HT = NULL;

    return 0;
}