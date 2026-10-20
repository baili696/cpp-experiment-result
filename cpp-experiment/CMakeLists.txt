cmake_minimum_required(VERSION 3.5)

PROJECT(cpp-training VERSION 0.1.0)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

# 告警策略：GCC/Clang 沿用原有的严格开关；
# MSVC 不支持 -Wall/-Werror/-Wnon-virtual-dtor 等 GNU 开关，
# 其告警开关改由各子目标分别设置（见 src/CMakeLists.txt）。
IF(MSVC)
    # 源码使用 UTF-8 编码（含中文注释），必须显式告知 MSVC，
    # 否则在 GBK(936) 代码页下会被误解析而报出一堆语法错误；
    # 同时把 CMake 默认的 /W3 提升为 /W4。
    STRING(REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
ELSE()
    SET(CMAKE_CXX_FLAGS "-Wno-deprecated-declarations -Wall -Werror -Wnon-virtual-dtor -Woverloaded-virtual")
ENDIF()

ENABLE_TESTING()
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(tests)
