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
    add_subdirectory(third-party/yoga)

    # 创建yoga的别名，方便其他target使用
    add_library(yoga::yoga ALIAS yogacore)
else()
    # 查找已安装的yoga库
    find_package(yoga REQUIRED)
endif()

# 查找GLFW
pkg_check_modules(GLFW REQUIRED glfw3)

# Skia 配置
set(SKIA_DIR ${CMAKE_SOURCE_DIR}/third-party/skia)
set(SKIA_OUT ${SKIA_DIR}/out/Release-x64)

# 包含 Skia 定义
if(EXISTS ${SKIA_OUT}/defines.cmake)
    include(${SKIA_OUT}/defines.cmake)
endif()

# 将src中所有文件编译为一个library
file(GLOB_RECURSE LITEDUI_SOURCES "src/*.cpp")
add_library(litedui ${LITEDUI_SOURCES})
target_include_directories(litedui PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${SKIA_DIR}
)
target_link_libraries(litedui PUBLIC 
    yoga::yoga 
    ${GLFW_LIBRARIES}
    ${SKIA_OUT}/libskparagraph.a
    ${SKIA_OUT}/libskshaper.a
    ${SKIA_OUT}/libskunicode_core.a
    ${SKIA_OUT}/libskunicode_icu.a
    ${SKIA_OUT}/libskia.a
    ${SKIA_OUT}/libpng.a
    ${SKIA_OUT}/libzlib.a
    ${SKIA_OUT}/libharfbuzz.a
    ${SKIA_OUT}/libicu.a
    ${SKIA_OUT}/libskcms.a
    ${SKIA_OUT}/libwebp.a
    ${SKIA_OUT}/libwuffs.a
    ${SKIA_OUT}/libjpeg.a
    fontconfig
    freetype
    GL
    EGL
    pthread
    dl
)
target_include_directories(litedui PUBLIC ${GLFW_INCLUDE_DIRS})

# 示例程序
add_subdirectory(examples)
