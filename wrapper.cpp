#include <iostream>
#include <string>
#include <OpenXLSX.hpp>
#include <OpenXLSX-Exports.hpp>

using namespace OpenXLSX;

// Windows環境とそれ以外（Linux/Mac）でエクスポート用のキーワードを自動で切り替える
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#  define EXPORT extern "C" __declspec(dllexport)
#else
#  define EXPORT extern "C" __attribute__((visibility("default")))
#endif

// --- Document 階層 ---
EXPORT void* OpenXLSX_CreateDoc(const char* path) {
    auto* doc = new OpenXLSX::XLDocument();
    doc->create(path);
    return doc;
}

EXPORT void OpenXLSX_SaveAndClose(void* docPtr) {
    auto* doc = static_cast<OpenXLSX::XLDocument*>(docPtr);
    doc->save();
    doc->close();
    delete doc;
}

// --- Workbook 階層 ---
EXPORT void* OpenXLSX_GetWorkbook(void* docPtr) {
    auto* doc = static_cast<OpenXLSX::XLDocument*>(docPtr);
    return new OpenXLSX::XLWorkbook(doc->workbook());
}

EXPORT void OpenXLSX_FreeWorkbook(void* wbkPtr) {
    delete static_cast<OpenXLSX::XLWorkbook*>(wbkPtr);
}

// --- Worksheet 階層 ---
EXPORT void* OpenXLSX_GetWorksheet(void* wbkPtr, const char* sheetName) {
    auto* wbk = static_cast<OpenXLSX::XLWorkbook*>(wbkPtr);
    return new OpenXLSX::XLWorksheet(wbk->worksheet(sheetName));
}

EXPORT void OpenXLSX_FreeWorksheet(void* wksPtr) {
    delete static_cast<OpenXLSX::XLWorksheet*>(wksPtr);
}

// --- Cell 操作 ---
EXPORT void OpenXLSX_SetCellString(void* wksPtr, uint32_t row, uint32_t col, const char* value) {
    auto* wks = static_cast<OpenXLSX::XLWorksheet*>(wksPtr);
    wks->cell(row, col).value() = value;
}

EXPORT void OpenXLSX_SetCellInt(void* wksPtr, uint32_t row, uint32_t col, int32_t value) {
    auto* wks = static_cast<OpenXLSX::XLWorksheet*>(wksPtr);
    wks->cell(row, col).value() = value;
}

// --- 表示・便利機能 ---
//EXPORT void OpenXLSX_FreezePanes(void* wksPtr, uint32_t row, uint32_t col) {
//    auto* wks = static_cast<OpenXLSX::XLWorksheet*>(wksPtr);
//    wks->sheetView().freezePanes(row, col);
//}

//EXPORT void OpenXLSX_SetAutoFilter(void* wksPtr, const char* rangeStr) {
//    auto* wks = static_cast<OpenXLSX::XLWorksheet*>(wksPtr);
//    wks->autoFilter().setRange(rangeStr);
//}
