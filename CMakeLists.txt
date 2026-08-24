cmake_minimum_required(VERSION 3.15)
project(OpenXLSX_CSharp_Wrapper LANGUAGES C CXX) # C言語のサポートを明示

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. OpenXLSX用エクスポートマクロのダミー生成
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/OpenXLSX-Exports.hpp" "
#ifndef OPENXLSX_EXPORT
#define OPENXLSX_EXPORT
#endif
")

# 2. 不足している miniz_export.h をダミー生成
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/miniz_export.h" "
#ifndef MINIZ_EXPORT
#define MINIZ_EXPORT
#endif
")

# 3. リポジトリ内のすべてのC++ファイル(.cpp)とCファイル(.c)を確実に全回収する
file(GLOB_RECURSE OPENXLSX_CPP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/OpenXLSX/*.cpp")
file(GLOB_RECURSE OPENXLSX_C_SOURCES   "${CMAKE_CURRENT_SOURCE_DIR}/OpenXLSX/*.c")

# 全ソースを一本のリストにまとめる
set(ALL_SOURCES 
    wrapper.cpp
    ${OPENXLSX_CPP_SOURCES}
    ${OPENXLSX_C_SOURCES}
)

# 4. ラッパー用の動的ライブラリ定義
add_library(OpenXLSX_Wrapper SHARED ${ALL_SOURCES})

# 5. インクルードパスの指定
target_include_directories(OpenXLSX_Wrapper PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenXLSX"
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenXLSX/headers"
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenXLSX/headers/detail"
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenXLSX/sources/utilities"
    "${CMAKE_CURRENT_BINARY_DIR}"
)

# 6. Windows環境用の設定 (MSVC対策)
if(WIN32)
    target_compile_definitions(OpenXLSX_Wrapper PRIVATE EXPORT_API)
    target_compile_options(OpenXLSX_Wrapper PRIVATE /permissive- /Zc:twoPhase-)
endif()
