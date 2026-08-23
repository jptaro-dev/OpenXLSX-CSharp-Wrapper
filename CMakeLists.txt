cmake_minimum_required(VERSION 3.15)
project(OpenXLSX_CSharp_Wrapper LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. 不足しているエクスポート用マクロを適切に定義してダミー生成
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/OpenXLSX-Exports.hpp" "
#ifndef OPENXLSX_EXPORT
#define OPENXLSX_EXPORT
#endif
")

# 2. OpenXLSXの全ソースファイルをリストアップ
file(GLOB_RECURSE OPENXLSX_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenXLSX/sources/*.cpp"
)

# 3. ラッパー用の動的ライブラリ（DLL/so/dylib）を定義
add_library(OpenXLSX_Wrapper SHARED 
    wrapper.cpp
    ${OPENXLSX_SOURCES}
)

# 4. インクルードパスの指定
target_include_directories(OpenXLSX_Wrapper PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenXLSX"
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenXLSX/headers"
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenXLSX/headers/detail"
    "${CMAKE_CURRENT_BINARY_DIR}"
)

# 5. Windows環境用のエクスポート定義
if(WIN32)
    target_compile_definitions(OpenXLSX_Wrapper PRIVATE EXPORT_API)
endif()
