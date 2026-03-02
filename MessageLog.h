//=================================================================================
//
// DirectXのメッセージログヘッダファイル [MessageLog.h]
// Author : TENMA
// 
// Info : UNICODE,ANSIの両方で利用可
//=================================================================================
//*********************************************************************************
//*** インクルードファイル ***
//*********************************************************************************
// main.hが存在する場合
#if __has_include("main.h")
	#include "main.h"
#endif

// pch.hが存在する場合
#if __has_include("pch.h")
#include "pch.h"
#endif

// Windowsのヘッダを読み込んでいない場合
#ifndef _WINDOWS_
	#if __has_include(<windows.h>)
		#include <windows.h>
	#else
		#error "You can not use this header."			// windows.h使用不可時、エラー
	#endif
#endif

// DirectXのヘッダを読み込んでいない場合
#ifndef __D3DX9_H__
	#if __has_include("d3dx9.h")
		#include "d3dx9.h"
		#define DIRECTINPUT_VERSION		(0x0800)	// ビルド時の警告対処用マクロ
		#pragma comment(lib,"d3d9.lib")				// 描画処理に必要
		#pragma comment(lib,"d3dx9.lib")			// d3d9.libの拡張ライブラリ
		#pragma comment(lib,"dxguid.lib")			// DirectXコンポーネント(部品)使用に必要
	#else
		#error "You can not use this header."			// d3dx9.h使用不可時、エラー
	#endif
#endif

// wcharのヘッダを読み込んでいない場合
#ifndef _INC_WCHAR
	#if __has_include(<wchar.h>)
		#include <wchar.h>
	#else
		#error "You can not use this header."			// wchar使用不可時、エラー
	#endif
#endif

// stdioのヘッダを読み込んでいない場合
#ifndef _INC_STDIO
	#if __has_include(<stdio.h>)
		#include <stdio.h>
	#else
		#error "You can not use this header."			// stdio使用不可時、エラー
	#endif
#endif

// stdlibのヘッダを読み込んでいない場合
#ifndef _INC_STDLIB
	#if __has_include(<stdio.h>)
		#include <stdlib.h>
	#else
		#error "You can not use this header."			// stdlib使用不可時、エラー
	#endif
#endif

// stringのヘッダを読み込んでいない場合
#ifndef _INC_STRING
	#if __has_include(<string.h>)
		#include <string.h>
	#else
		#error "You can not use this header."			// string使用不可時、エラー
	#endif
#endif

//*********************************************************************************
//*** マクロ定義 ***
//*********************************************************************************
#ifndef FPS
	#define FPS		60
#endif

#ifndef RELEASE
	#define RELEASE(p)		{ if ((p) != NULL) {(p)->Release(); (p) = nullptr; } }	// メモリ確保ポインタのRelease用マクロ
#endif

#define DEF_FONTSIZE					(35)							// フォントの基準サイズ
#define DEF_FONT						TEXT("HG丸ｺﾞｼｯｸM-PRO")			// 基準フォント
#define SAFE_DELETE(buffer)				do{ if(buffer != nullptr){delete buffer; buffer = nullptr;}}while(0)		// 動的バッファの解放
#define SAFE_DELETEARRAY(buffer)		do{ if(buffer != nullptr){delete[] buffer; buffer = nullptr;}}while(0)		// 動的配列バッファの解放
#define MESSAGELOG_FLAG					(DT_LEFT | DT_TOP | DT_WORDBREAK)					// メッセージログのフラグ
#define SECOND(nCount)					(nCount / FPS)					// 1秒判定
#define IFSECOND(nCount, nSecond)		(nCount % nSecond == 0)			// フレーム判定省略	
#define CMP_SUCCESS(cmp)				(cmp == 0)						// strcmp,strncmpでの成功判定
#define CMP_FAILED(cmp)					(cmp != 0)						// strcmp,strncmpでの失敗判定
#define STR_SUCCESS(str)				(str != NULL)					// strstrでの成功判定
#define STR_FAILED(str)					(str == NULL)					// strstrでの失敗判定
#define SIZEOFWCHAR(size)				(sizeof(size) / 2)				// ワイド文字列使用時のsizeof
#define SAFE_FUNC(pointer, func)		do{ if(pointer != nullptr){ pointer##->##func; } }while(0)		// ポインタのNULLCHECKを含んだポインタ参照型関数呼び出し
#define KEEP_USING						-INT_MAX						// 現在の値を使用
#define KEEP_USING_COL					(D3DXCOLOR(0.0000001f, 1, 1, 1))// 現在の色を使用
#ifdef UNICODE
#define DEF_WIDE						true
#else
#define DEF_WIDE						false
#endif

// 基本作成ログ
#ifdef UNICODE
#define CreateMessageLog CreateMessageLogW						
#else // UNICODE
#define CreateMessageLog CreateMessageLogA
#endif // !UNICODE

//*********************************************************************************
//*** 文字列共用体の定義 ***
//*********************************************************************************
typedef union
{
	LPSTR pStrA;		// セリフの文字列
	LPWSTR pStrW;		// セリフの文字列(ワイド)
} String;

//*********************************************************************************
//*** 文字列の種類の定義 ***
//*********************************************************************************
typedef enum
{
	STRINGTYPE_ANSI = 0,
	STRINGTYPE_UNICODE,
	STRINGTYPE_MAX
}STRINGTYPE;

//*********************************************************************************
//*** メッセージログの定義 ***
//*********************************************************************************
typedef struct MessageLog
{
private:
	LPD3DXFONT pFontMessage;	// フォントバッファ
	LPSTR pDrawString;			// 文字列
	LPWSTR pDrawStringW;		// 文字列
	INT nMaxString;				// 最大文字数
	INT nCntDrawString;			// 表示中の文字数
	INT nDrawSecond;			// 表示を進める秒数
	INT nCountMessageLog;		// 秒数カウント用
	INT nWaitSecond;			// 待機時間
	INT nWaitByte;				// 待機する配列番号
	INT nDrawNumOtherByte;		// バイト数が異なる文字の個数
	INT nDrawOtherByte;			// 異なる文字のバイト
	INT *pDrawInt;				// 異なる文字の存在する配列番号
	RECT rect;					// メッセージボックスの大きさ
	D3DXCOLOR DrawCol;			// メッセージボックスに表示する文字列の色
	BOOL bDrawJapanise;			// 日本語(2バイト文字)か
	BOOL bUseWCHAR;				// ワイド文字列を使用するか

public:

	// コンストラクタ
	MessageLog();

	// デストラクタ
	~MessageLog();

	// 解放
	void Uninit(void);

	// 更新
	void Update(void);

	// 描画
	void Draw(void);

	// メッセージログの新規作成(引数は下部の関数を参照)
	bool SetMessageLogA(LPDIRECT3DDEVICE9 pDevice, D3DXVECTOR3 pos, FLOAT fWidth, FLOAT fHeight,
		INT nFontSize, LPSTR pStr, INT nNumMessage, INT nSecond, D3DXCOLOR Col, BOOL bJapanise);

	// 異なるバイト数の文字列を使用できるメッセージログの新規作成(引数は下部の関数を参照)
	bool SetMessageLogExA(LPDIRECT3DDEVICE9 pDevice, D3DXVECTOR3 pos, FLOAT fWidth, FLOAT fHeight,
		INT nFontSize, LPSTR pStr, INT nNumMessage, INT nSecond, D3DXCOLOR Col, BOOL bJapanise,
		INT nNumOtherByte, INT nOtherByte, INT *pInt);

	// ワイド文字列を使用したメッセージログの新規作成(引数は下部の関数を参照)
	bool SetMessageLogW(LPDIRECT3DDEVICE9 pDevice, D3DXVECTOR3 pos, FLOAT fWidth, FLOAT fHeight,
		INT nFontSize, LPWSTR pStrW, INT nNumMessage, INT nSecond, D3DXCOLOR Col);

	/// <summary>
	/// 文字列更新の一時待機
	/// </summary>
	/// <param name="nByte">待機する配列番号</param>
	/// <param name="nSecond">待機時間</param>
	void WaitForString(INT nByte, INT nSecond);

	/// <summary>
	/// 文字列の即時表示
	/// </summary>
	void ImmeString(void);

	/// <summary>
	/// メッセージログの文字列の変更
	/// </summary>
	/// <param name="pStrA">変更後の文字列</param>
	/// <param name="nNumMessage">文字列の配列サイズ</param>
	/// <param name="nSecond">一文字あたりにかかる秒数</param>
	/// <param name="Col">文字の色</param>
	/// <param name="bJapanise">日本語か</param>
	/// <param name="bReset">表示文字数をリセットするか</param>
	bool ReplaceStringA(LPSTR pStrA, INT nNumMessage, INT nSecond,
		D3DXCOLOR Col, BOOL bJapanise, BOOL bReset);

	/// <summary>
	/// メッセージログの文字列の変更
	/// </summary>
	/// <param name="pStrW">変更後の文字列</param>
	/// <param name="nNumMessage">文字列の配列サイズ</param>
	/// <param name="nSecond">一文字あたりにかかる秒数</param>
	/// <param name="Col">文字の色</param>
	/// <param name="bReset">表示文字数をリセットするか</param>
	bool ReplaceStringW(LPWSTR pStrW, INT nNumMessage, INT nSecond,
		D3DXCOLOR Col, BOOL bReset);

#ifdef UNICODE
#define ReplaceString ReplaceStringW
#else
#define ReplaceString ReplaceStringA
#endif

	/// <summary>
	/// メッセージログへの文字列の追加
	/// </summary>
	/// <param name="pStrA">追加する文字列</param>
	/// <param name="nNumMessage">文字列の配列サイズ</param>
	/// <returns>成功判定</returns>
	bool AddStringA(LPSTR pStrA, INT nNumMessage);

	/// <summary>
	/// メッセージログへの文字列の追加
	/// </summary>
	/// <param name="pStrW">追加する文字列</param>
	/// <param name="nNumMessage">文字列の配列サイズ</param>
	/// <returns>成功判定</returns>
	bool AddStringW(LPWSTR pStrW, INT nNumMessage);

#ifdef UNICODE
#define AddString AddStringW
#else
#define AddString AddStringA
#endif

	/// <summary>
	/// 表示位置の変更
	/// </summary>
	/// <param name="pos">左上座標</param>
	/// <param name="fWidth">幅</param>
	/// <param name="fHeight">高さ</param>
	void SetPos(D3DXVECTOR3 pos, float fWidth, float fHeight);

	/// <summary>
	/// フォントサイズ変更
	/// </summary>
	/// <param name="pDevice">Direct3Dデバイス</param>
	/// <param name="nFontSize">フォントサイズ</param>
	/// <returns>成功判定</returns>
	bool SetSize(LPDIRECT3DDEVICE9 pDevice, INT nFontSize);

	/// <summary>
	/// フォントの更新スピード
	/// </summary>
	/// <param name="nUpdateSpeed">変更後のスピード</param>
	/// <returns>成功判定</returns>
	bool SetSpeed(INT nUpdateSpeed);

	/// <summary>
	/// int型の配列の比較
	/// </summary>
	/// <param name="pArray">intの配列の先頭アドレス</param>
	/// <param name="nTarget">比較対象</param>
	/// <param name="nSize">配列の大きさ</param>
	/// <returns>正誤判定</returns>
	bool CompairArray(int* pArray, int nTarget, int nSize);
} MessageLog;

typedef MessageLog *LPMESSAGELOG, *PMESSAGELOG;

//*********************************************************************************
//*** プロトタイプ宣言 ***
//*********************************************************************************
void InitMessageLog(LPMESSAGELOG* lpLog, size_t LogSize);
void UninitMessageLog(LPMESSAGELOG* lpLog, size_t LogSize);
void UpdateMessageLog(LPMESSAGELOG* lpLog, size_t LogSize);
void DrawMessageLog(LPMESSAGELOG* lpLog, size_t LogSize);

/// <summary>
/// メッセージログの削除及び新規作成(1 or 2バイト文字固定)
/// </summary>
/// <param name="pDevice">Direct3Dデバイス</param>
/// <param name="pos">ログの設置位置(左上座標)</param>
/// <param name="fWidth">ログの幅(幅を超えると自動改行)</param>
/// <param name="fHeight">ログの高さ(下に伸びる)</param>
/// <param name="nFontSize">フォントのサイズ</param>
/// <param name="pStr">文字列</param>
/// <param name="nNumMessage">文字列のサイズ</param>
/// <param name="nSecond">一文字あたりにかかる秒数</param>
/// <param name="Col">文字の色</param>
/// <param name="bJapanise">日本語かどうか(2バイト文字か)</param>
/// <param name="pMessageLog">LPMESSAGELOGへのポインタ</param>
void CreateMessageLogA(LPDIRECT3DDEVICE9 pDevice, D3DXVECTOR3 pos, FLOAT fWidth, FLOAT fHeight,
	INT nFontSize, LPSTR pStr, INT nNumMessage, INT nSecond, D3DXCOLOR Col, BOOL bJapanise,
	LPMESSAGELOG *pMessageLog);

/// <summary>
/// メッセージログの削除及び新規作成(バイト動的)
/// </summary>
/// <param name="pDevice">Direct3Dデバイス</param>
/// <param name="pos">ログの設置位置(左上座標)</param>
/// <param name="fWidth">ログの幅(幅を超えると自動改行)</param>
/// <param name="fHeight">ログの高さ(下に伸びる)</param>
/// <param name="nFontSize">フォントのサイズ</param>
/// <param name="pStr">文字列</param>
/// <param name="nNumMessage">文字列のサイズ</param>
/// <param name="nSecond">一文字あたりにかかる秒数</param>
/// <param name="Col">文字の色</param>
/// <param name="pMessageLog">LPMESSAGELOGへのポインタ</param>
void CreateMessageLogW(LPDIRECT3DDEVICE9 pDevice, D3DXVECTOR3 pos, FLOAT fWidth, FLOAT fHeight,
	INT nFontSize, LPWSTR pStr, INT nNumMessage, INT nSecond, D3DXCOLOR Col,
	LPMESSAGELOG* pMessageLog);

/// <summary>
/// メッセージログの削除及び新規作成(ワイド文字列)
/// </summary>
/// <param name="pDevice">Direct3Dデバイス</param>
/// <param name="pos">ログの設置位置(左上座標)</param>
/// <param name="fWidth">ログの幅(幅を超えると自動改行)</param>
/// <param name="fHeight">ログの高さ(下に伸びる)</param>
/// <param name="nFontSize">フォントのサイズ</param>
/// <param name="pStr">文字列</param>
/// <param name="nNumMessage">文字列のサイズ</param>
/// <param name="nSecond">一文字あたりにかかる秒数</param>
/// <param name="Col">文字の色</param>
/// <param name="bJapanise">日本語かどうか(2バイト文字か)</param>
/// <param name="nNumOtherByte">バイト数が異なる文字の数</param>
/// <param name="nOtherByte">異なるバイトのサイズ</param>
/// <param name="pInt">異なる文字の配列番号を示した配列へのポインタ</param>
/// <param name="pMessageLog">LPMESSAGELOGへのポインタ</param>
void CreateMessageLogExA(LPDIRECT3DDEVICE9 pDevice, D3DXVECTOR3 pos, FLOAT fWidth, FLOAT fHeight,
	INT nFontSize, LPSTR pStr, INT nNumMessage, INT nSecond, D3DXCOLOR Col, BOOL bJapanise,
	INT nNumOtherByte, INT nOtherByte, INT *pInt, LPMESSAGELOG *pMessageLog);

/// <summary>
/// メッセージログの削除
/// </summary>
/// <param name="pMessageLog">LPMESSAGELOGへのポインタ</param>
void DeleteMessageLog(LPMESSAGELOG *pLog);