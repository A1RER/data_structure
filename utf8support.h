#ifndef UTF8_SUPPORT_H
#define UTF8_SUPPORT_H

#include <windows.h>

// 初始化控制台UTF-8（输入+输出）
static inline void init_console_utf8() {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
}

// 定义宏，一键初始化（懒人版）
#define INIT_UTF8_CONSOLE() init_console_utf8()

#endif // UTF8_SUPPORT_H