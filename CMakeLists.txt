cmake_minimum_required(VERSION 3.15)
project(OpenXLSX_CSharp_Wrapper LANGUAGES C CXX)

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

# 3. すべてのソースファイルを全回収
file(GLOB_RECURSE OPENXLSX_CPP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/OpenXLSX/*.cpp")
file(GLOB_RECURSE OPENXLSX_C_SOURCES   "${CMAKE_CURRENT_SOURCE_DIR}/OpenXLSX/*.c")

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

# 6. 環境固有の設定（Linux/MacのC言語厳格化エラーへの対策を追加）
if(WIN32)
    target_compile_definitions(OpenXLSX_Wrapper PRIVATE EXPORT_API)
    target_compile_options(OpenXLSX_Wrapper PRIVATE /permissive- /Zc:twoPhase-)
else()
    # Ubuntu（GCC）と macOS（Clang）で古いCコード（miniz）のビルドを通すための魔法のフラグ
    # 警告をエラーとして扱わず、古いC言語の記述を許可します
    target_compile_options(OpenXLSX_Wrapper PRIVATE 
        $<$<COMPILE_LANGUAGE:C>:-w -fpermissive>
    )
endif()
