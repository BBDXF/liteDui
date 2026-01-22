# skia demo

add_executable(02_skia_demo main.cpp)

# Skia 路径
set(SKIA_DIR ${CMAKE_SOURCE_DIR}/third-party/skia)
set(SKIA_OUT ${SKIA_DIR}/out/Release-x64)

# 包含 Skia 定义
include(${SKIA_OUT}/defines.cmake)

# 头文件路径
target_include_directories(02_skia_demo PRIVATE ${SKIA_DIR})

# 链接 Skia 库
target_link_libraries(02_skia_demo PRIVATE
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
    pthread
    dl
)
