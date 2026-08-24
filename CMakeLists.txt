cmake_minimum_required(VERSION 3.15)
project(OpenXLSX_CSharp_Wrapper LANGUAGES CXX)

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

# 3. リポジトリ内のすべてのC/C++ソースファイルを「フォルダを問わず」全回収する
# (これにより、headers/detail/ 内などに置いた pugixml.cpp 等も確実にビルドに含まれます)
file(GLOB_RECURSE OPENXLSX_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenXLSX/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenXLSX/*.c"
)

# 4. ラッパー用の動的ライブラリ定義
add_library(OpenXLSX_Wrapper SHARED 
    wrapper.cpp
    ${OPENXLSX_SOURCES}
)

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
