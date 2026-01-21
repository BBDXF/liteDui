cmake_minimum_required(VERSION 3.16)

project(liteDui LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 启用位置无关代码
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 核心配置1：设置编译类型为 Debug（自动带 -g 符号）
set(CMAKE_BUILD_TYPE Debug)
# 核心配置2：强制关闭优化，保证调试准确性
set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

# 库依赖
find_package(PkgConfig REQUIRED)

# 添加yoga作为外部依赖
option(BUILD_YOGA "Build yoga library from source" ON)
if(BUILD_YOGA)
    # 构建yoga库
    add_subdirectory(third-party/yoga yoga_build)

    # 创建yoga的别名，方便其他target使用
    add_library(yoga::yoga ALIAS yogacore)
else()
    # 查找已安装的yoga库
    find_package(yoga REQUIRED)
endif()

# 查找GLFW
pkg_check_modules(GLFW REQUIRED glfw3)

# 将src中所有文件编译为一个library
file(GLOB_RECURSE LITEDUI_SOURCES "src/*.cpp")
add_library(litedui ${LITEDUI_SOURCES})
target_include_directories(litedui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(litedui PUBLIC yoga::yoga ${GLFW_LIBRARIES})
target_include_directories(litedui PUBLIC ${GLFW_INCLUDE_DIRS})

# 示例程序
add_subdirectory(examples)
