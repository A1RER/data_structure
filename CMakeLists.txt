cmake_minimum_required(VERSION 3.10)
project(data_structure)  # 项目名称，可自定义

# 强制编译器用 UTF-8 处理源码和输出字符串（汉化版也能识别）
if(CMAKE_C_COMPILER_ID MATCHES "GNU")
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# 添加头文件搜索路径（关键步骤）
include_directories(include)  # 如果放在 include 文件夹下
# 若直接放在根目录，则用 include_directories(.)

# 设置 C 标准（根据需要调整，如 C99、C11）
set(CMAKE_C_STANDARD 99)

# 编译 TCP 相关代码（program251113tcp/main.c）
add_executable(tcp_program  # 生成的可执行文件名，如 tcp_program
        program251113tcp/main.c
        # 若有其他依赖文件（如 .h 或 .c），在此处添加路径
)

# 编译通话记录相关代码（programTel/main.c）
add_executable(tel_program  # 生成的可执行文件名，如 tel_program
        programTel/main.c
        # 若有其他依赖文件（如 .h 或 .c），在此处添加路径
)

add_executable(BinarySortTree_program

        programBST/main.c
)

add_executable(HuffmanCode_program
        ProgramHC/main.c
)

add_executable(Review_program
        programReview_singleNode/main.c
)

add_executable(ProRev_TwinNode
        ProRev_TwinNode/main.c
)

add_executable(programBefPre
        program251210/main.c
)

add_executable(P_Dijkstra
        P_Dijkstra/main.c
)

add_executable(ProPrim
        ProPrim/main.c
)

# 确保C编译器以UTF-8处理源码
if(CMAKE_C_COMPILER_ID MATCHES "GNU")
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# 若需要链接外部库（如网络库），针对每个可执行程序添加链接命令
# 例如 TCP 可能需要链接 socket 库（Windows 下是 ws2_32）：
if(WIN32)
    target_link_libraries(tcp_program ws2_32)  # Windows 下 TCP 需链接的库
endif()