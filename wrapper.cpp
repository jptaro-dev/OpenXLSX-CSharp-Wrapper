#include <iostream>
#include <string>
#include <vector>
#include <OpenXLSX.hpp>
#include <OpenXLSX-Exports.hpp>

using namespace OpenXLSX;

// Windows環境とそれ以外（Linux/Mac）でエクスポート用のキーワードを自動で切り替える
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#  define EXPORT extern "C" __declspec(dllexport)
#else
#  define EXPORT extern "C" __attribute__((visibility("default")))
#endif

extern "C" {

    // ==========================================
    // 1. ドキュメント全体を管理する関数 (XLDocument)
    // ==========================================

    EXPORT void* OpenXLSX_CreateDoc() {
        return new XLDocument();
    }

    EXPORT void OpenXLSX_OpenDoc(void* docPtr, const char* path) {
        if (!docPtr || !path) return;
        auto* doc = static_cast<XLDocument*>(docPtr);
        doc->open(path);
    }

    EXPORT void OpenXLSX_SaveAndClose(void* docPtr) {
        if (!docPtr) return;
        auto* doc = static_cast<XLDocument*>(docPtr);
        doc->save();
        doc->close();
        delete doc;
    }

    EXPORT void OpenXLSX_SaveAsAndClose(void* docPtr, const char* path) {
        if (!docPtr || !path) return;
        auto* doc = static_cast<XLDocument*>(docPtr);
        doc->saveAs(path);
        doc->close();
        delete doc;
    }


    // ==========================================
    // 2. ブック全体を管理する関数 (XLWorkbook)
    // ==========================================

    EXPORT void* OpenXLSX_GetWorkbook(void* docPtr) {
        if (!docPtr) return nullptr;
        auto* doc = static_cast<XLDocument*>(docPtr);
        return new XLWorkbook(doc->workbook());
    }

    EXPORT void OpenXLSX_FreeWorkbook(void* wbkPtr) {
        if (!wbkPtr) return;
        auto* wbk = static_cast<XLWorkbook*>(wbkPtr);
        delete wbk;
    }

    EXPORT void OpenXLSX_AddWorksheet(void* wbkPtr, const char* sheetName) {
        if (!wbkPtr || !sheetName) return;
        auto* wbk = static_cast<XLWorkbook*>(wbkPtr);
        wbk->addWorksheet(sheetName);
    }

    EXPORT void OpenXLSX_DeleteSheet(void* wbkPtr, const char* sheetName) {
        if (!wbkPtr || !sheetName) return;
        auto* wbk = static_cast<XLWorkbook*>(wbkPtr);
        wbk->deleteSheet(sheetName);
    }

    EXPORT bool OpenXLSX_SheetExists(void* wbkPtr, const char* sheetName) {
        if (!wbkPtr || !sheetName) return false;
        auto* wbk = static_cast<XLWorkbook*>(wbkPtr);
        return wbk->sheetExists(sheetName);
    }


    // ==========================================
    // 3. ワークシートを操作する関数 (XLWorksheet)
    // ==========================================

    EXPORT void* OpenXLSX_GetWorksheet(void* wbkPtr, const char* sheetName) {
        if (!wbkPtr || !sheetName) return nullptr;
        auto* wbk = static_cast<XLWorkbook*>(wbkPtr);
        return new XLWorksheet(wbk->worksheet(sheetName));
    }

    EXPORT void OpenXLSX_FreeWorksheet(void* wksPtr) {
        if (!wksPtr) return;
        auto* wks = static_cast<XLWorksheet*>(wksPtr);
        delete wks;
    }

    EXPORT void OpenXLSX_MergeCells(void* wksPtr, const char* rangeRef) {
        if (!wksPtr || !rangeRef) return;
        auto* wks = static_cast<XLWorksheet*>(wksPtr);
        wks->mergeCells(rangeRef);
    }

    EXPORT void OpenXLSX_UnmergeCells(void* wksPtr, const char* rangeRef) {
        if (!wksPtr || !rangeRef) return;
        auto* wks = static_cast<XLWorksheet*>(wksPtr);
        wks->unmergeCells(rangeRef);
    }


    // ==========================================
    // 4. セルへの値の読み書きを行う関数 (XLCell)
    // ==========================================

    EXPORT void OpenXLSX_SetCellString(void* wksPtr, uint32_t row, uint32_t col, const char* value) {
        if (!wksPtr || !value) return;
        auto* wks = static_cast<XLWorksheet*>(wksPtr);
        wks->cell(row, col).value() = value;
    }

    EXPORT void OpenXLSX_SetCellInt(void* wksPtr, uint32_t row, uint32_t col, int32_t value) {
        if (!wksPtr) return;
        auto* wks = static_cast<XLWorksheet*>(wksPtr);
        wks->cell(row, col).value() = value;
    }

    EXPORT const char* OpenXLSX_GetCellString(void* wksPtr, uint32_t row, uint32_t col) {
        if (!wksPtr) return "";
        auto* wks = static_cast<XLWorksheet*>(wksPtr);
        static std::string ret;
        ret = wks->cell(row, col).value().get<std::string>();
        return ret.c_str();
    }
}
