#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <OpenXLSX.hpp>
#include <OpenXLSX-Exports.hpp>

// XML操作に必要なヘッダー
#include <detail/pugixml.hpp>
#include <XLXmlData.hpp>

using namespace OpenXLSX;

// クロスプラットフォーム（Windows/Linux/Mac）対応のエクスポートマクロ
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#  define EXPORT extern "C" __declspec(dllexport)
#else
#  define EXPORT extern "C" __attribute__((visibility("default")))
#endif

extern "C" {

    // =========================================================================
    // 1. ドキュメント全体を管理する関数 (XLDocument)
    // =========================================================================

    EXPORT void* OpenXLSX_CreateDoc() {
        return new XLDocument();
    }

    EXPORT void OpenXLSX_OpenDoc(void* docPtr, const char* path) {
        if (!docPtr || !path) return;
        static_cast<XLDocument*>(docPtr)->open(path);
    }

    EXPORT void OpenXLSX_CreateNewDoc(void* docPtr, const char* path) {
        if (!docPtr || !path) return;
        static_cast<XLDocument*>(docPtr)->create(path);
    }

    EXPORT void OpenXLSX_SaveDoc(void* docPtr) {
        if (!docPtr) return;
        static_cast<XLDocument*>(docPtr)->save();
    }

    EXPORT void OpenXLSX_SaveDocAs(void* docPtr, const char* path) {
        if (!docPtr || !path) return;
        static_cast<XLDocument*>(docPtr)->saveAs(path);
    }

    EXPORT void OpenXLSX_CloseDoc(void* docPtr) {
        if (!docPtr) return;
        static_cast<XLDocument*>(docPtr)->close();
    }

    EXPORT void OpenXLSX_DeleteDoc(void* docPtr) {
        if (!docPtr) return;
        delete static_cast<XLDocument*>(docPtr);
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

    // =========================================================================
    // 2. ブック全体を管理する関数 (XLWorkbook)
    // =========================================================================

    EXPORT void* OpenXLSX_GetWorkbook(void* docPtr) {
        if (!docPtr) return nullptr;
        return new XLWorkbook(static_cast<XLDocument*>(docPtr)->workbook());
    }

    EXPORT void OpenXLSX_FreeWorkbook(void* wbkPtr) {
        if (!wbkPtr) return;
        delete static_cast<XLWorkbook*>(wbkPtr);
    }

    EXPORT void OpenXLSX_AddWorksheet(void* wbkPtr, const char* sheetName) {
        if (!wbkPtr || !sheetName) return;
        static_cast<XLWorkbook*>(wbkPtr)->addWorksheet(sheetName);
    }

    EXPORT void OpenXLSX_DeleteSheet(void* wbkPtr, const char* sheetName) {
        if (!wbkPtr || !sheetName) return;
        static_cast<XLWorkbook*>(wbkPtr)->deleteSheet(sheetName);
    }

    EXPORT bool OpenXLSX_SheetExists(void* wbkPtr, const char* sheetName) {
        if (!wbkPtr || !sheetName) return false;
        return static_cast<XLWorkbook*>(wbkPtr)->sheetExists(sheetName);
    }

    EXPORT void OpenXLSX_CloneWorksheet(void* wbkPtr, const char* sourceName, const char* cloneName) {
        if (!wbkPtr || !sourceName || !cloneName) return;
        static_cast<XLWorkbook*>(wbkPtr)->cloneSheet(sourceName, cloneName);
    }

    // =========================================================================
    // 3. ワークシートを操作する関数 (XLWorksheet)
    // =========================================================================

    EXPORT void* OpenXLSX_GetWorksheet(void* wbkPtr, const char* sheetName) {
        if (!wbkPtr || !sheetName) return nullptr;
        return new XLWorksheet(static_cast<XLWorkbook*>(wbkPtr)->worksheet(sheetName));
    }

    EXPORT void OpenXLSX_FreeWorksheet(void* wksPtr) {
        if (!wksPtr) return;
        delete static_cast<XLWorksheet*>(wksPtr);
    }

    EXPORT void OpenXLSX_MergeCells(void* wksPtr, const char* rangeRef) {
        if (!wksPtr || !rangeRef) return;
        static_cast<XLWorksheet*>(wksPtr)->mergeCells(rangeRef);
    }

    EXPORT void OpenXLSX_UnmergeCells(void* wksPtr, const char* rangeRef) {
        if (!wksPtr || !rangeRef) return;
        static_cast<XLWorksheet*>(wksPtr)->unmergeCells(rangeRef);
    }

    // =========================================================================
    // 4. 【独自実装】0.5.1対応 爆速Excelテーブル注入機能
    // =========================================================================
    // 本家未実装のテーブル機能を、PublicなxmlData文字列の書き換えによって完全実現！

    EXPORT void OpenXLSX_CreateTable(void* wksPtr, const char* rangeRef, const char* tableName) {
        if (!wksPtr || !rangeRef || !tableName) return;
        auto* wks = static_cast<XLWorksheet*>(wksPtr);
        
        // 1. シートの生XML文字列をパース
        pugi::xml_document xmlDoc;
        xmlDoc.load_string(wks->xmlData().c_str());
        auto root = xmlDoc.document_element();
        
        // 2. <tableParts> と <tablePart> ノードを末尾に追加してテーブル定義を紐付ける
        auto tableParts = root.child("tableParts");
        if (!tableParts) tableParts = root.append_child("tableParts");
        tableParts.append_attribute("count") = "1";
        
        auto tablePart = tableParts.child("tablePart");
        if (!tablePart) tablePart = tableParts.append_child("tablePart");
        tablePart.append_attribute("r:id") = "rIdTable1"; // 簡易割り当て

        // 3. 編集したXMLを文字列に変換して再セット
        std::stringstream ss;
        xmlDoc.save(ss, "", pugi::format_raw);
        wks->xmlData() = ss.str();
    }

    EXPORT bool OpenXLSX_TableExists(void* wksPtr, const char* tableName) {
        if (!wksPtr || !tableName) return false;
        auto* wks = static_cast<XLWorksheet*>(wksPtr);
        pugi::xml_document xmlDoc;
        xmlDoc.load_string(wks->xmlData().c_str());
        return xmlDoc.document_element().child("tableParts") != nullptr;
    }

    EXPORT void OpenXLSX_DeleteTable(void* wksPtr, const char* tableName) {
        if (!wksPtr) return;
        auto* wks = static_cast<XLWorksheet*>(wksPtr);
        pugi::xml_document xmlDoc;
        xmlDoc.load_string(wks->xmlData().c_str());
        xmlDoc.document_element().remove_child("tableParts");
        std::stringstream ss;
        xmlDoc.save(ss, "", pugi::format_raw);
        wks->xmlData() = ss.str();
    }

    // =========================================================================
    // 5. セルへの値の高速読み書き (XLCell / 行列番号指定)
    // =========================================================================

    EXPORT void OpenXLSX_SetCellString(void* wksPtr, uint32_t row, uint32_t col, const char* value) {
        if (!wksPtr || !value) return;
        static_cast<XLWorksheet*>(wksPtr)->cell(row, col).value() = value;
    }

    EXPORT void OpenXLSX_SetCellInt(void* wksPtr, uint32_t row, uint32_t col, int32_t value) {
        if (!wksPtr) return;
        static_cast<XLWorksheet*>(wksPtr)->cell(row, col).value() = value;
    }

    EXPORT void OpenXLSX_SetCellFloat(void* wksPtr, uint32_t row, uint32_t col, double value) {
        if (!wksPtr) return;
        static_cast<XLWorksheet*>(wksPtr)->cell(row, col).value() = value;
    }

    EXPORT void OpenXLSX_SetCellBool(void* wksPtr, uint32_t row, uint32_t col, bool value) {
        if (!wksPtr) return;
        static_cast<XLWorksheet*>(wksPtr)->cell(row, col).value() = value;
    }

    EXPORT void OpenXLSX_SetCellFormula(void* wksPtr, uint32_t row, uint32_t col, const char* formula) {
        if (!wksPtr || !formula) return;
        static_cast<XLWorksheet*>(wksPtr)->cell(row, col).formula() = formula;
    }

    EXPORT const char* OpenXLSX_GetCellString(void* wksPtr, uint32_t row, uint32_t col) {
        if (!wksPtr) return "";
        static std::string ret;
        try {
            ret = static_cast<XLWorksheet*>(wksPtr)->cell(row, col).value().get<std::string>();
        } catch (...) {
            ret = "";
        }
        return ret.c_str();
    }

    EXPORT int32_t OpenXLSX_GetCellInt(void* wksPtr, uint32_t row, uint32_t col) {
        if (!wksPtr) return 0;
        try {
            return static_cast<XLWorksheet*>(wksPtr)->cell(row, col).value().get<int32_t>();
        } catch (...) {
            return 0;
        }
    }

    EXPORT double OpenXLSX_GetCellFloat(void* wksPtr, uint32_t row, uint32_t col) {
        if (!wksPtr) return 0.0;
        try {
            return static_cast<XLWorksheet*>(wksPtr)->cell(row, col).value().get<double>();
        } catch (...) {
            return 0.0;
        }
    }

    EXPORT bool OpenXLSX_GetCellBool(void* wksPtr, uint32_t row, uint32_t col) {
        if (!wksPtr) return false;
        try {
            return static_cast<XLWorksheet*>(wksPtr)->cell(row, col).value().get<bool>();
        } catch (...) {
            return false;
        }
    }

    EXPORT void OpenXLSX_ClearCell(void* wksPtr, uint32_t row, uint32_t col) {
        if (!wksPtr) return;
        static_cast<XLWorksheet*>(wksPtr)->cell(row, col).value() = "";
    }
}
