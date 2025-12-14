#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "../utf8support.h"

/**
* 模块说明：顺序栈通用定义
* 使用场景：字符栈（括号匹配和表达式转换）、整数栈（进制转换和表达式求值）
*/
#define MAX_STACK_SIZE 100 //栈最大容量

/**
 * @brief 字符型顺序栈，用于括号匹配、表达式运算符存储
 * @param base 栈底指针，固定指向栈内存起始位置
 * @param top 栈顶指针，指向”下一个可以入栈的空位置“
 * @param stacksize 栈的最大容量（元素个数）
 */
typedef struct {
    char *base;
    char *top;
    int stacksize;
} CharSqStack;

/**
 * @brief 整型顺序栈（用于进制转换余数存储、表达式操作数存储）
 * @param base 栈底指针，固定指向栈内存起始位置
 * @param top 栈顶指针，指向"下一个可入栈的空位置"
 * @param stacksize 栈最大容量（元素个数）
 */
typedef struct {
    int *base;
    int *top;
    int stacksize;
} IntSqStack;

/**********************************************************
 * 字符栈核心操作函数
 *********************************************************/

/**
 * @brief 初始化字符栈
 * @param stack 指向字符栈结构体的指针（输入输出参数）
 * @return int 操作结果：0-成功，-1-失败（内存分配失败）
 * @note 初始化后栈底与栈顶指针重合，栈为空
 */
int InitCharStack(CharSqStack *stack) {
    // 分配栈内存，按最大容量初始化
    stack->base = (char *) malloc(MAX_STACK_SIZE * sizeof(char));
    if (stack->base == NULL) {
        perror("CharStack malloc failed");  // 打印系统错误信息
        return -1;
    }
    stack->top = stack->base;       // 空栈：栈顶=栈底
    stack->stacksize = MAX_STACK_SIZE;
    return 0;
}

/**
 * @brief 字符入栈
 * @param stack 指向字符栈结构体的指针（输入输出参数）
 * @param data 待入栈的字符数据
 * @return int 操作结果：0-成功，-1-失败（栈满）
 * @note 入栈逻辑：先赋值，后移动栈顶指针（后置++）
 */
int PushCharStack(CharSqStack *stack, char data) {
    // 检查栈满：栈顶-栈底 >= 最大容量
    if (stack->top - stack->base >= stack->stacksize) {
        printf("CharStack is full! Push failed.\n");
        return -1;
    }
    *(stack->top++) = data;  // 数据入栈，栈顶指针上移
    return 0;
}

/**
 * @brief 字符出栈
 * @param stack 指向字符栈结构体的指针（输入输出参数）
 * @param data 存储出栈数据的指针（输出参数）
 * @return int 操作结果：0-成功，-1-失败（栈空）
 * @note 出栈逻辑：先移动栈顶指针（前置--），后取值
 */
int PopCharStack(CharSqStack *stack, char *data) {
    // 检查栈空：栈顶=栈底
    if (stack->top == stack->base) {
        printf("CharStack is empty! Pop failed.\n");
        return -1;
    }
    *data = *(--stack->top);  // 栈顶指针下移，取数据
    return 0;
}

/**
 * @brief 获取字符栈顶元素（不出栈）
 * @param stack 字符栈结构体（输入参数）
 * @return char 栈顶字符；若栈空，返回'#'作为空标记
 * @note 仅读取栈顶值，不修改栈状态，用于运算符优先级判断
 */
char GetTopChar(CharSqStack stack) {
    if (stack.top == stack.base) {
        return '#';  // 栈空标记，优先级最低
    }
    return *(stack.top - 1);  // 栈顶指针前一个位置为有效数据
}

/**
 * @brief 判断字符栈是否为空
 * @param stack 字符栈结构体（输入参数）
 * @return int 1-空，0-非空
 * @note 核心判断条件：栈顶指针 == 栈底指针
 */
int IsCharStackEmpty(CharSqStack stack) {
    return (stack.top == stack.base) ? 1 : 0;
}

/**
 * @brief 销毁字符栈
 * @param stack 指向字符栈结构体的指针（输入输出参数）
 * @return void
 * @note 释放栈底内存，避免内存泄漏；销毁后指针置空防止野指针
 */
void DestroyCharStack(CharSqStack *stack) {
    free(stack->base);
    stack->base = NULL;
    stack->top = NULL;
    stack->stacksize = 0;
}

/**********************************************************
 * 整数栈核心操作函数（工业级注释）
 *********************************************************/

/**
 * @brief 初始化整数栈
 * @param stack 指向整数栈结构体的指针（输入输出参数）
 * @return int 操作结果：0-成功，-1-失败（内存分配失败）
 * @note 与字符栈初始化逻辑一致，仅数据类型不同
 */
int InitIntStack(IntSqStack *stack) {
    stack->base = (int *)malloc(MAX_STACK_SIZE * sizeof(int));
    if (stack->base == NULL) {
        perror("IntStack malloc failed");
        return -1;
    }
    stack->top = stack->base;
    stack->stacksize = MAX_STACK_SIZE;
    return 0;
}

/**
 * @brief 整数入栈
 * @param stack 指向整数栈结构体的指针（输入输出参数）
 * @param data 待入栈的整数数据
 * @return int 操作结果：0-成功，-1-失败（栈满）
 */
int PushIntStack(IntSqStack *stack, int data) {
    if (stack->top - stack->base >= stack->stacksize) {
        printf("IntStack is full! Push failed.\n");
        return -1;
    }
    *(stack->top++) = data;
    return 0;
}

/**
 * @brief 整数出栈
 * @param stack 指向整数栈结构体的指针（输入输出参数）
 * @param data 存储出栈数据的指针（输出参数）
 * @return int 操作结果：0-成功，-1-失败（栈空）
 */
int PopIntStack(IntSqStack *stack, int *data) {
    if (stack->top == stack->base) {
        printf("IntStack is empty! Pop failed.\n");
        return -1;
    }
    *data = *(--stack->top);
    return 0;
}

/**
 * @brief 判断整数栈是否为空
 * @param stack 整数栈结构体（输入参数）
 * @return int 1-空，0-非空
 */
int IsIntStackEmpty(IntSqStack stack) {
    return (stack.top == stack.base) ? 1 : 0;
}

/**
 * @brief 销毁整数栈
 * @param stack 指向整数栈结构体的指针（输入输出参数）
 * @return void
 * @note 释放内存并置空指针，避免野指针
 */
void DestroyIntStack(IntSqStack *stack) {
    free(stack->base);
    stack->base = NULL;
    stack->top = NULL;
    stack->stacksize = 0;
}

/**********************************************************
 * 栈应用1：括号匹配（核心功能函数）
 *********************************************************/

/**
 * @brief 校验括号匹配合法性（支持()、[]、{}）
 * @param str 待校验的字符串（输入参数）
 * @return int 1-匹配，0-不匹配
 * @note 核心逻辑：左括号入栈，右括号匹配栈顶左括号，最终栈空则合法
 * @warning 需处理三种非法场景：右括号多、左括号多、括号类型不匹配
 */
int IsBracketMatch(const char *str){
    if(str==NULL || strlen(str)==0){
        printf("Input string is empty!\n");
        return 0;
    }

    CharSqStack stack;
    if(InitCharStack(&stack)!=0){
        return 0; //栈初始化失败 直接返回不匹配
    }

    char topChar;
    int i=0;
    while(str[i]!='\0'){
        //左括号入栈
        if(str[i] == '(' || str[i] == '[' || str[i] == '{'){
            PushCharStack(&stack,str[i]);
        }
        //右括号匹配逻辑
        else if(str[i] == ')' || str[i] == ']' || str[i] == '}'){
            //场景1：左括号数量多于右括号
            if(IsCharStackEmpty(stack)){
                DestroyCharStack(&stack);
                return 0;
            }
            //弹出栈顶左括号，判断类型是否匹配
            PopCharStack(&stack,&topChar);
            if ((str[i] == ')' && topChar != '(') ||
                (str[i] == ']' && topChar != '[') ||
                (str[i] == '}' && topChar != '{'))
            {
                    DestroyCharStack(&stack);
                    return 0;  // 类型不匹配
            }
        }
        //忽略非括号字符
        i++;
    }
    // 场景2：左括号数量多于右括号（遍历结束栈非空）
    int isMatch = IsCharStackEmpty(stack) ? 1 : 0;
    DestroyCharStack(&stack);  // 必须销毁栈，避免内存泄漏
    return isMatch;
}

/**********************************************************
 * 栈应用2：十进制转任意进制（2~16）
 *********************************************************/

/**
 * @brief 十进制正整数转换为任意进制（2~16）
 * @param decimal 待转换的十进制正整数（输入参数）
 * @param base 目标进制（2≤base≤16）（输入参数）
 * @return void
 * @note 核心逻辑：余数入栈，弹出输出（栈天然实现逆序）
 * @warning 需特殊处理十进制0，避免无输出；16进制需映射A-F
 */
void DecimalToAnyBase(int decimal, int base) {
    // 合法性校验
    if (base < 2 || base > 16) {
        printf("Error: Base must be between 2 and 16!\n");
        return;
    }
    // 特殊处理：十进制0直接输出0
    if (decimal == 0) {
        printf("Decimal 0 -> Base-%d: 0\n", base);
        return;
    }

    IntSqStack stack;
    if (InitIntStack(&stack) != 0) {
        return;
    }

    // 16进制字符映射表（0-9, A-F）
    const char digits[] = "0123456789ABCDEF";
    int remainder;

    // 步骤1：余数入栈（核心）
    while (decimal > 0) {
        remainder = decimal % base;  // 取余数
        PushIntStack(&stack, remainder);
        decimal = decimal / base;    // 商作为新的被除数
    }

    // 步骤2：弹出栈输出（逆序，符合进制转换规则）
    printf("Decimal %d -> Base-%d: ", decimal, base); // decimal已为0，不影响显示
    int data;
    while (!IsIntStackEmpty(stack)) {
        PopIntStack(&stack, &data);
        printf("%c", digits[data]);
    }
    printf("\n");

    DestroyIntStack(&stack);
}

/**********************************************************
 * 栈应用3：表达式求值（中缀转后缀 + 后缀求值）
 *********************************************************/

/**
 * @brief 获取运算符优先级（支持+、-、*、/、(、#）
 * @param op 运算符字符（输入参数）
 * @return int 优先级值：#(0) < ((1) < +-(2) < * / (3)，非法运算符返回-1
* @note 优先级规则是中缀转后缀的核心依据
*/
int GetOpPriority(char op) {
    switch (op) {
        case '#': return 0;  // 栈空标记
        case '(': return 1;
        case '+':
        case '-': return 2;
        case '*':
        case '/': return 3;
        default: return -1;  // 非法运算符
    }
}

/**
 * @brief 中缀表达式转后缀表达式（逆波兰式）
 * @param infix 中缀表达式字符串（输入参数，仅支持0-9数字、+、-、*、/、()）
 * @param postfix 存储后缀表达式的缓冲区（输出参数）
 * @return void
 * @note 核心规则：数字直接输出，运算符按优先级入栈/出栈，括号特殊处理
 * @warning 支持多位数，用空格分隔操作数，方便后续求值
 */
void InfixToPostfix(const char *infix, char *postfix) {
    if (infix == NULL || postfix == NULL) {
        printf("Input/Output buffer is NULL!\n");
        return;
    }

    CharSqStack opStack;
    if (InitCharStack(&opStack) != 0) {
        return;
    }
    PushCharStack(&opStack, '#');  // 栈底放入#，作为结束标记

    int i = 0, j = 0;
    char op, topOp;
    while (infix[i] != '\0') {
        // 场景1：数字（支持多位数），直接输出到后缀表达式
        if (isdigit(infix[i])) {
            while (isdigit(infix[i])) {
                postfix[j++] = infix[i++];
            }
            postfix[j++] = ' ';  // 空格分隔操作数
        }
            // 场景2：左括号，直接入栈
        else if (infix[i] == '(') {
            PushCharStack(&opStack, infix[i]);
            i++;
        }
            // 场景3：右括号，弹出运算符直到左括号（左括号不输出）
        else if (infix[i] == ')') {
            topOp = GetTopChar(opStack);
            while (topOp != '(') {
                PopCharStack(&opStack, &op);
                postfix[j++] = op;
                postfix[j++] = ' ';
                topOp = GetTopChar(opStack);
            }
            PopCharStack(&opStack, &op);  // 弹出左括号，不输出
            i++;
        }
            // 场景4：普通运算符（+、-、*、/）
        else if (infix[i] == '+' || infix[i] == '-' || infix[i] == '*' || infix[i] == '/') {
            topOp = GetTopChar(opStack);
            // 优先级≤栈顶运算符：弹出栈顶并输出，直到优先级更高或栈空
            while (GetOpPriority(infix[i]) <= GetOpPriority(topOp)) {
                PopCharStack(&opStack, &op);
                postfix[j++] = op;
                postfix[j++] = ' ';
                topOp = GetTopChar(opStack);
            }
            PushCharStack(&opStack, infix[i]);  // 当前运算符入栈
            i++;
        }
            // 场景5：跳过空格等无效字符
        else {
            i++;
        }
    }

    // 弹出栈中剩余运算符（直到#）
    topOp = GetTopChar(opStack);
    while (topOp != '#') {
        PopCharStack(&opStack, &op);
        postfix[j++] = op;
        postfix[j++] = ' ';
        topOp = GetTopChar(opStack);
    }

    // 去掉最后一个空格，结束字符串
    if (j > 0) {
        postfix[j-1] = '\0';
    } else {
        postfix[0] = '\0';
    }

    DestroyCharStack(&opStack);
}

/**
 * @brief 后缀表达式求值
 * @param postfix 后缀表达式字符串（输入参数，空格分隔操作数）
 * @return int 表达式计算结果；若出错（如除数为0、表达式非法）返回-1
 * @note 核心逻辑：数字入栈，运算符弹出两个数计算，结果入栈，栈顶为最终结果
 * @warning 注意操作数顺序：左操作数 = 先弹出的数，右操作数 = 后弹出的数
 */
int PostfixEval(const char *postfix) {
    if (postfix == NULL || strlen(postfix) == 0) {
        printf("Postfix expression is empty!\n");
        return -1;
    }

    IntSqStack numStack;
    if (InitIntStack(&numStack) != 0) {
        return -1;
    }

    int i = 0, num = 0, a, b;
    while (postfix[i] != '\0') {
        // 场景1：数字（支持多位数），入栈
        if (isdigit(postfix[i])) {
            num = 0;
            while (isdigit(postfix[i])) {
                num = num * 10 + (postfix[i] - '0');  // 多位数拼接
                i++;
            }
            PushIntStack(&numStack, num);
        }
            // 场景2：跳过空格
        else if (postfix[i] == ' ') {
            i++;
        }
            // 场景3：运算符，计算并将结果入栈
        else {
            // 弹出两个操作数：b是右操作数，a是左操作数
            if (PopIntStack(&numStack, &b) != 0 || PopIntStack(&numStack, &a) != 0) {
                printf("Postfix expression is invalid!\n");
                DestroyIntStack(&numStack);
                return -1;
            }
            // 计算逻辑
            switch (postfix[i]) {
                case '+': PushIntStack(&numStack, a + b); break;
                case '-': PushIntStack(&numStack, a - b); break;
                case '*': PushIntStack(&numStack, a * b); break;
                case '/':
                    if (b == 0) {
                        printf("Error: Divisor is zero!\n");
                        DestroyIntStack(&numStack);
                        return -1;
                    }
                    PushIntStack(&numStack, a / b);
                    break;
                default:
                    printf("Error: Invalid operator %c!\n", postfix[i]);
                    DestroyIntStack(&numStack);
                    return -1;
            }
            i++;
        }
    }

    // 栈顶即为最终结果
    PopIntStack(&numStack, &num);
    // 校验：若栈非空，说明表达式非法
    if (!IsIntStackEmpty(numStack)) {
        printf("Postfix expression is invalid!\n");
        num = -1;
    }

    DestroyIntStack(&numStack);
    return num;
}


/**********************************************************
 * 测试模块：统一测试所有栈应用功能
 *********************************************************/

/**
 * @brief 测试括号匹配功能
 * @return void
 * @note 覆盖4种典型场景：匹配、类型不匹配、左括号多、右括号多
 */
void TestBracketMatch() {
    printf("\n==================== 括号匹配测试 ====================\n");
    const char *testCases[] = {
            "({[]})",    // 匹配
            "([)]",      // 类型不匹配
            "({[]}",     // 左括号多
            "({[]}))",   // 右括号多
            "a+b*(c-d)", // 含其他字符的合法表达式
            NULL
    };

    for (int i = 0; testCases[i] != NULL; i++) {
        int res = IsBracketMatch(testCases[i]);
        printf("字符串: \"%s\" → 括号%s\n", testCases[i], res ? "匹配" : "不匹配");
    }
}

/**
 * @brief 测试进制转换功能
 * @return void
 * @note 覆盖二进制、八进制、十六进制，包含0、常规数、边界数（255）
 */
void TestDecimalToAnyBase() {
    printf("\n==================== 进制转换测试 ====================\n");
    DecimalToAnyBase(13, 2);   // 13 → 1101
    DecimalToAnyBase(13, 8);   // 13 → 15
    DecimalToAnyBase(13, 16);  // 13 → D
    DecimalToAnyBase(0, 2);    // 0 → 0
    DecimalToAnyBase(255, 16); // 255 → FF
    DecimalToAnyBase(100, 7);  // 100 → 202（测试非2/8/16进制）
}

/**
 * @brief 测试表达式求值功能
 * @return void
 * @note 覆盖常规表达式、含括号表达式、多位数表达式
 */
void TestExpressionEval() {
    printf("\n==================== 表达式求值测试 ====================\n");
    const char *infixCases[] = {
            "3+4*2-1",      // 常规表达式
            "(3+4)*2-1",    // 含括号
            "12+3*4-5",     // 多位数
            "100/2+5*8",    // 混合运算
            NULL
    };

    char postfix[200];  // 存储后缀表达式的缓冲区
    for (int i = 0; infixCases[i] != NULL; i++) {
        memset(postfix, 0, sizeof(postfix));  // 清空缓冲区
        InfixToPostfix(infixCases[i], postfix);
        int res = PostfixEval(postfix);
        printf("中缀表达式: %s\n", infixCases[i]);
        printf("后缀表达式: %s\n", postfix);
        printf("计算结果: %d\n\n", res);
    }

    // 测试除数为0的异常场景
    memset(postfix, 0, sizeof(postfix));
    InfixToPostfix("10/0+5", postfix);
    printf("中缀表达式: 10/0+5\n");
    printf("后缀表达式: %s\n", postfix);
    int res = PostfixEval(postfix);
    printf("计算结果: %d（除数为0，出错）\n", res);
}


/**********************************************************
 * 主函数：程序入口，调用所有测试模块
 *********************************************************/
int main() {
    INIT_UTF8_CONSOLE();
    printf("==================== 栈应用综合测试 ====================\n");
    // 依次执行所有测试
    TestBracketMatch();
    TestDecimalToAnyBase();
    TestExpressionEval();
    printf("\n==================== 测试结束 ====================\n");
    return 0;
}