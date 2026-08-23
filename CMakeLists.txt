cmake_minimum_required(VERSION 3.15)
project(OpenXLSX_CSharp_Wrapper LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# 1. 不足している pugixml をGitHubから自動でダウンロードして組み込む
FetchContent_Declare(
  pugixml
  GIT_REPOSITORY https://github.com
  GIT_TAG        v1.14 # 安定版のタグを指定
)
FetchContent_MakeAvailable(pugixml)

# 2. エクスポート用マクロを適切に定義してダミー生成
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/OpenXLSX-Exports.hpp" "
#ifndef OPENXLSX_EXPORT
#define OPENXLSX_EXPORT
#endif
")

# 3. OpenXLSXの全ソースファイルをリストアップ
file(GLOB_RECURSE OPENXLSX_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenXLSX/sources/*.cpp"
)

# 4. ラッパー用の動的ライブラリ定義
add_library(OpenXLSX_Wrapper SHARED 
    wrapper.cpp
    ${OPENXLSX_SOURCES}
)

# 5. 依存関係の紐付け（ダウンロードした pugixml を結合）
target_link_libraries(OpenXLSX_Wrapper PRIVATE pugixml)

# 6. インクルードパスの指定
target_include_directories(OpenXLSX_Wrapper PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenXLSX"
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenXLSX/headers"
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenXLSX/headers/detail"
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenXLSX/sources/utilities"
    "${CMAKE_CURRENT_BINARY_DIR}"
)

# 7. Windows環境固有の設定 (MSVCで標準準拠のC++演算子'not'などを有効化)
if(WIN32)
    target_compile_definitions(OpenXLSX_Wrapper PRIVATE EXPORT_API)
    target_compile_options(OpenXLSX_Wrapper PRIVATE /permissive- /Zc:twoPhase-)
endif()
