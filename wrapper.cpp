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

    // ドキュメントを新規作成する
    EXPORT void* OpenXLSX_CreateDoc() {
        return new XLDocument();
    }

    // 既存のExcelファイルを開く（ご質問の機能です！）
    EXPORT void OpenXLSX_OpenDoc(void* docPtr, const char* path) {
        if (!docPtr || !path) return;
        auto* doc = static_cast<XLDocument*>(docPtr);
        doc->open(path);
    }

    // ドキュメントを上書き保存して閉じる
    EXPORT void OpenXLSX_SaveAndClose(void* docPtr) {
        if (!docPtr) return;
        auto* doc = static_cast<XLDocument*>(docPtr);
        doc->save();
        doc->close();
        delete doc;
    }

    // ドキュメントを別名で保存して閉じる
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

    // 内部のWorkbookオブジェクトへの参照を取得する
    EXPORT void* OpenXLSX_GetWorkbook(void* docPtr) {
        if (!docPtr) return nullptr;
        auto* doc = static_cast<XLDocument*>(docPtr);
        return &(doc->workbook());
    }

    // 新しいワークシートを追加する
    EXPORT void OpenXLSX_AddWorksheet(void* wbkPtr, const char* sheetName) {
        if (!wbkPtr || !sheetName) return;
        auto* wbk = static_cast<XLWorkbook*>(wbkPtr);
        wbk->addWorksheet(sheetName);
    }

    // 指定した名前のシートを削除する
    EXPORT void OpenXLSX_DeleteSheet(void* wbkPtr, const char* sheetName) {
        if (!wbkPtr || !sheetName) return;
        auto* wbk = static_cast<XLWorkbook*>(wbkPtr);
        wbk->deleteSheet(sheetName);
    }

    // 指定した名前のシートが存在するか確認する
    EXPORT bool OpenXLSX_SheetExists(void* wbkPtr, const char* sheetName) {
        if (!wbkPtr || !sheetName) return false;
        auto* wbk = static_cast<XLWorkbook*>(wbkPtr);
        return wbk->sheetExists(sheetName);
    }


    // ==========================================
    // 3. ワークシートを操作する関数 (XLWorksheet)
    // ==========================================

    // シート名（Worksheet）を指定して開く
    EXPORT void* OpenXLSX_GetWorksheet(void* wbkPtr, const char* sheetName) {
        if (!wbkPtr || !sheetName) return nullptr;
        auto* wbk = static_cast<XLWorkbook*>(wbkPtr);
        // ポインタとして安全に扱うため、newしてインスタンスをコピーして返します
        return new XLWorksheet(wbk->worksheet(sheetName));
    }

    // ワークシートオブジェクトを解放する
    EXPORT void OpenXLSX_FreeWorksheet(void* wksPtr) {
        if (!wksPtr) return;
        auto* wks = static_cast<XLWorksheet*>(wksPtr);
        delete wks;
    }

    // セルのマージ（結合）を設定する
    EXPORT void OpenXLSX_MergeCells(void* wksPtr, const char* rangeRef) {
        if (!wksPtr || !rangeRef) return;
        auto* wks = static_cast<XLWorksheet*>(wksPtr);
        wks->mergeCells(rangeRef);
    }

    // セルのマージ（結合）を解除する
    EXPORT void OpenXLSX_UnmergeCells(void* wksPtr, const char* rangeRef) {
        if (!wksPtr || !rangeRef) return;
        auto* wks = static_cast<XLWorksheet*>(wksPtr);
        wks->unmergeCells(rangeRef);
    }

    // ウィンドウ枠の固定（FreezePanes）を設定する (XML直接書換の爆速版)
    EXPORT void OpenXLSX_FreezePanes(void* wksPtr, uint32_t row, uint32_t col) {
        if (!wksPtr) return;
        auto* wks = static_cast<XLWorksheet*>(wksPtr);
        auto root = wks->xmlData().xmlDocument().document_element();
        auto sheetViews = root.child("sheetViews");
        if (!sheetViews) sheetViews = root.prepend_child("sheetViews");
        auto sheetView = sheetViews.child("sheetView");
        if (!sheetView) sheetView = sheetViews.append_child("sheetView");
        
        sheetView.append_attribute("tabSelected") = "1";
        auto pane = sheetView.append_child("pane");
        pane.append_attribute("ySplit") = std::to_string(row).c_str();
        pane.append_attribute("xSplit") = std::to_string(col).c_str();
        pane.append_attribute("topLeftCell") = "A2";                  
        pane.append_attribute("activePane") = "bottomLeft";
        pane.append_attribute("state") = "frozen";                    
    }

    // オートフィルタを設定する (XML直接書換の爆速版)
    EXPORT void OpenXLSX_SetAutoFilter(void* wksPtr, const char* rangeRef) {
        if (!wksPtr || !rangeRef) return;
        auto* wks = static_cast<XLWorksheet*>(wksPtr);
        auto root = wks->xmlData().xmlDocument().document_element();
        auto autoFilter = root.child("autoFilter");
        if (!autoFilter) autoFilter = root.append_child("autoFilter");
        autoFilter.append_attribute("ref") = rangeRef;
    }


    // ==========================================
    // 4. セルへの値の読み書きを行う関数 (XLCell / 行列番号指定)
    // ==========================================

    // セルに文字列を書き込む（激速の行列番号指定版）
    EXPORT void OpenXLSX_SetCellString(void* wksPtr, uint32_t row, uint32_t col, const char* value) {
        if (!wksPtr || !value) return;
        auto* wks = static_cast<XLWorksheet*>(wksPtr);
        wks->cell(row, col).value() = value;
    }

    // セルに数値を書き込む
    EXPORT void OpenXLSX_SetCellInt(void* wksPtr, uint32_t row, uint32_t col, int32_t value) {
        if (!wksPtr) return;
        auto* wks = static_cast<XLWorksheet*>(wksPtr);
        wks->cell(row, col).value() = value;
    }

    // セルの文字列を読み込む
    EXPORT const char* OpenXLSX_GetCellString(void* wksPtr, uint32_t row, uint32_t col) {
        if (!wksPtr) return "";
        auto* wks = static_cast<XLWorksheet*>(wksPtr);
        static std::string ret;
        ret = wks->cell(row, col).value().get<std::string>();
        return ret.c_str();
    }
}
