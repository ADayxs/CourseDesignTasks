cmake_minimum_required(VERSION 4.0)
project(CourseDesignTasks C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ========== 公共核心库：AST + 序列化 + 编辑距离 ==========
add_library(core STATIC
        src/ast.c
        src/ast_parser.c
        src/ast_serial.c
        src/edit_distance.c
)

target_include_directories(core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# （可选）警告
if (MSVC)
    target_compile_options(core PRIVATE /W4)
else()
    target_compile_options(core PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ========== 1) AST 验证程序（入口 src/main.c） ==========
add_executable(ast_demo
        tests/test_ast.c
)
target_link_libraries(ast_demo PRIVATE core)

# ========== 2) 最终程序（入口 src/main.c） ==========
# 为了不影响当前构建：如果文件还没放进来，就先不生成该 target
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c")
    add_executable(final_app
            src/main.c
    )
    target_link_libraries(final_app PRIVATE core)
endif()
