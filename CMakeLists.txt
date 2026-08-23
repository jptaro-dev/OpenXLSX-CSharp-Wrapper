cmake_minimum_required(VERSION 3.15)
project(OpenXLSX_CSharp_Wrapper LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. サブディレクトリの OpenXLSX ライブラリをビルド対象に加える
add_subdirectory(OpenXLSX)

# 2. ラッパー用の動的ライブラリ（DLL / so / dylib）の定義
add_library(OpenXLSX_Wrapper SHARED wrapper.cpp)

# 3. 依存関係の解決（ラッパーから OpenXLSX のヘッダーや本体を紐付ける）
target_link_libraries(OpenXLSX_Wrapper PRIVATE OpenXLSX::OpenXLSX)

# 4. Windows環境で C# から呼び出す際のエクスポート設定 (__declspec(dllexport)用)
if(WIN32)
    target_compile_definitions(OpenXLSX_Wrapper PRIVATE EXPORT_API)
endif()
