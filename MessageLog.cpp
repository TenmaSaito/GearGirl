//=================================================================================
//
// DirectXのメッセージログcppファイル [MessageLog.cpp]
// Author : TENMA
//
//=================================================================================
//*********************************************************************************
//*** インクルードファイル ***
//*********************************************************************************
#include "MessageLog.h"

//==================================================================================
//--- 初期化 ---
//==================================================================================
void InitMessageLog(LPMESSAGELOG *lpLog, size_t LogSize)
{
	if (lpLog == nullptr) return;

	int nNumMessage = LogSize / sizeof(LPMESSAGELOG);

	for (int nCntMessage = 0; nCntMessage < nNumMessage; nCntMessage++, lpLog++)
	{
		(*lpLog) = NULL;
	}
}

//==================================================================================
//--- 終了 ---
//==================================================================================
void UninitMessageLog(LPMESSAGELOG *lpLog, size_t LogSize)
{
	if (lpLog == nullptr) return;

	int nNumMessage = LogSize / sizeof(LPMESSAGELOG);

	for (int nCntMessage = 0; nCntMessage < nNumMessage; nCntMessage++, lpLog++)
	{
		SAFE_FUNC((*lpLog), Uninit());
		SAFE_DELETE(*lpLog);
	}
}

//==================================================================================
//--- 更新 ---
//==================================================================================
void UpdateMessageLog(LPMESSAGELOG *lpLog, size_t LogSize)
{
	if (lpLog == nullptr) return;

	int nNumMessage = LogSize / sizeof(LPMESSAGELOG);

	for (int nCntMessage = 0; nCntMessage < nNumMessage; nCntMessage++, lpLog++)
	{
		SAFE_FUNC((*lpLog), Update());
	}
}

//==================================================================================
//--- 描画 ---
//==================================================================================
void DrawMessageLog(LPMESSAGELOG *lpLog, size_t LogSize)
{
	if (lpLog == nullptr) return;

	int nNumMessage = LogSize / sizeof(LPMESSAGELOG);

	for (int nCntMessage = 0; nCntMessage < nNumMessage; nCntMessage++, lpLog++)
	{
		SAFE_FUNC((*lpLog), Draw());
	}
}

//==================================================================================
//--- コンストラクタ ---
//==================================================================================
MessageLog::MessageLog() : pFontMessage(nullptr), pDrawString(nullptr), pDrawStringW(nullptr),
nMaxString(0), nCntDrawString(0), nDrawSecond(0), nCountMessageLog(0), nWaitSecond(0),
nWaitByte(0), nDrawNumOtherByte(0), nDrawOtherByte(0), pDrawInt(), rect(), DrawCol(),
bDrawJapanise(true), bUseWCHAR(DEF_WIDE)
{
}

//==================================================================================
//--- デストラクタ ---
//==================================================================================
MessageLog::~MessageLog()
{
	if (this != nullptr)
	{
		Uninit();
	}
}

//==================================================================================
//--- 破棄 ---
//==================================================================================
void MessageLog::Uninit(void)
{
	RELEASE(pFontMessage);

	SAFE_DELETEARRAY(pDrawString);

	SAFE_DELETEARRAY(pDrawStringW);

	SAFE_DELETEARRAY(pDrawInt);
}

//==================================================================================
//--- 更新 ---
//==================================================================================
void MessageLog::Update(void)
{
	if (nWaitSecond > 0 && nWaitByte != -1)
	{
		if (nWaitByte == nCntDrawString)
		{
			nWaitSecond--;
			return;
		}
	}

	if (IFSECOND(nCountMessageLog, nDrawSecond))
	{
		if (nMaxString - 1 > nCntDrawString)
		{
			if (bUseWCHAR)
			{
				nCntDrawString += 1;

				if (nCntDrawString > nMaxString) nCntDrawString = nMaxString - 1;
			}
			else
			{
				if (bDrawJapanise)
				{
					if (pDrawString == nullptr && pDrawStringW == nullptr) return;

					if (CMP_FAILED(strncmp(pDrawString + nCntDrawString, "\n", 1)))
					{
						nCntDrawString += 2;
					}
					else
					{
						if (CompairArray(pDrawInt, nCntDrawString, nDrawNumOtherByte))
						{
							nCntDrawString += nDrawOtherByte;
						}
						else
						{
							nCntDrawString++;
						}
					}
				}
				else
				{
					nCntDrawString++;
				}

				if (nCntDrawString > nMaxString) nCntDrawString = nMaxString - 1;
			}
		}
		else
		{
			if (nCntDrawString > nMaxString) nCntDrawString = nMaxString - 1;
		}
	}

	nCountMessageLog++;
}

//==================================================================================
//--- 描画 ---
//==================================================================================
void MessageLog::Draw(void)
{
	if (pFontMessage != nullptr)
	{
		if (bUseWCHAR && pDrawStringW != nullptr)
		{
			HRESULT hr;

			hr = pFontMessage->DrawTextW(NULL,
				pDrawStringW,
				nCntDrawString,
				&rect,
				MESSAGELOG_FLAG,
				DrawCol);

			if (FAILED(hr))
			{
				OutputDebugString(TEXT("あれっ？何故でしょう...描画が出来ません！"));
			}
		}
		else if (pDrawString != nullptr)
		{
			HRESULT hr;

			hr = pFontMessage->DrawTextA(NULL,
				pDrawString,
				nCntDrawString,
				&rect,
				MESSAGELOG_FLAG,
				DrawCol);

			if (FAILED(hr))
			{
				OutputDebugString(TEXT("あれっ？何故でしょう...描画が出来ません！"));
			}
		}
	}
}

//==================================================================================
//--- メッセージログの新規作成 ---
//==================================================================================
bool MessageLog::SetMessageLogA(LPDIRECT3DDEVICE9 pDevice, D3DXVECTOR3 pos, FLOAT fWidth, FLOAT fHeight,
	INT nFontSize, LPSTR pStr, INT nNumMessage, INT nSecond, D3DXCOLOR Col, BOOL bJapanise)
{
	if (pDevice == nullptr)
	{// NULLチェック
		OutputDebugString(TEXT("pDeviceがNULLじゃダメじゃないですか！"));
		return false;
	}

	RELEASE(pFontMessage);

	HRESULT hr;

	hr = D3DXCreateFont(pDevice,		// フォント作成
		nFontSize,
		0,
		0,
		0,
		FALSE,
		SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		DEF_FONT,
		&pFontMessage);

	if (FAILED(hr))
	{
		OutputDebugString(TEXT("error"));
		return false;
	}

	// メッセージログのサイズを保存
	rect.left = pos.x;
	rect.top = pos.y;
	rect.right = pos.x + fWidth;
	rect.bottom = pos.y + fHeight;

	SAFE_DELETEARRAY(pDrawString);				// 文字列破棄
	nMaxString = nNumMessage;					// 最大文字数設定
	pDrawString = new CHAR[nMaxString];			// メモリ確保
	if (pDrawString == nullptr)
	{ // NULLチェック
		OutputDebugString(TEXT("想定...外...です...メモリが...とれないなんて..."));
		return false;
	}

	strcpy((char*)pDrawString, pStr);			// 文字列保存

	nDrawSecond = nSecond;						// 一文字あたりにかかる秒数を保存
	DrawCol = Col;								// 文字の色を設定
	bDrawJapanise = bJapanise;					// 日本語化を判定

	// Exの値を初期化
	nWaitSecond = -1;
	nWaitByte = -1;
	nDrawNumOtherByte = 0;
	nDrawOtherByte = 0;
	SAFE_DELETEARRAY(pDrawInt);

	bUseWCHAR = false;							// ワイド文字列未使用に設定

	return true;
}

//==================================================================================
//--- メッセージログの新規作成(バイトの区別あり) ---
//==================================================================================
bool MessageLog::SetMessageLogExA(LPDIRECT3DDEVICE9 pDevice, D3DXVECTOR3 pos, FLOAT fWidth, FLOAT fHeight,
	INT nFontSize, LPSTR pStr, INT nNumMessage, INT nSecond, D3DXCOLOR Col, BOOL bJapanise,
	INT nNumOtherByte, INT nOtherByte, INT* pInt)
{
	if (pDevice == nullptr)
	{// NULLチェック
		OutputDebugString(TEXT("pDeviceがNULLじゃダメじゃないですか！"));
		return false;
	}

	RELEASE(pFontMessage);

	HRESULT hr;

	hr = D3DXCreateFont(pDevice,		// フォント作成
		nFontSize,
		0,
		0,
		0,
		FALSE,
		SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		DEF_FONT,
		&pFontMessage);

	if (FAILED(hr))
	{
		OutputDebugString(TEXT("error"));
		return false;
	}

	// メッセージログのサイズを保存
	rect.left = pos.x;
	rect.top = pos.y;
	rect.right = pos.x + fWidth;
	rect.bottom = pos.y + fHeight;

	SAFE_DELETEARRAY(pDrawString);				// 文字列破棄
	nMaxString = nNumMessage;					// 最大文字数設定
	pDrawString = new CHAR[nMaxString];			// メモリ確保
	if (pDrawString == nullptr)
	{ // NULLチェック
		OutputDebugString(TEXT("想定...外...です...メモリが...とれないなんて..."));
		return false;
	}

	strcpy((char*)pDrawString, pStr);			// 文字列保存

	nDrawSecond = nSecond;						// 一文字あたりにかかる秒数を保存
	DrawCol = Col;								// 文字の色を設定
	bDrawJapanise = bJapanise;					// 日本語化を判定

	nDrawNumOtherByte = nNumOtherByte;			// 異なるバイト数の文字の数を保存
	nDrawOtherByte = nOtherByte;				// 異なるバイトを保存
	SAFE_DELETEARRAY(pDrawInt);					// 配列破棄
	pDrawInt = new INT[nDrawNumOtherByte];		// 新規に確保
	if (pDrawInt == nullptr) return false;
	memcpy(pDrawInt, pInt, sizeof(pDrawInt) * nDrawNumOtherByte);	// バイトの異なる配列番号を保存

	bUseWCHAR = false;							// ワイド文字列未使用に設定

	return true;
}

//==================================================================================
//--- メッセージログの新規作成(ワイド文字列) ---
//==================================================================================
bool MessageLog::SetMessageLogW(LPDIRECT3DDEVICE9 pDevice, D3DXVECTOR3 pos, FLOAT fWidth, FLOAT fHeight,
	INT nFontSize, LPWSTR pStrW, INT nNumMessage, INT nSecond, D3DXCOLOR Col)
{
	if (pDevice == nullptr)
	{// NULLチェック
		OutputDebugString(TEXT("pDeviceがNULLじゃダメじゃないですか！"));
		return false;
	}

	RELEASE(pFontMessage);

	HRESULT hr;

	hr = D3DXCreateFont(pDevice,		// フォント作成
		nFontSize,
		0,
		0,
		0,
		FALSE,
		SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		DEF_FONT,
		&pFontMessage);

	if (FAILED(hr))
	{
		OutputDebugString(TEXT("error"));
		return false;
	}

	// メッセージログのサイズ保存
	rect.left = pos.x;
	rect.top = pos.y;
	rect.right = pos.x + fWidth;
	rect.bottom = pos.y + fHeight;

	// 文字列を指定、保存
	SAFE_DELETEARRAY(pDrawStringW);				// 文字列破棄
	nMaxString = nNumMessage;					// 最大文字数設定
	pDrawStringW = new WCHAR[nMaxString];		// メモリ確保
	if (pDrawStringW == nullptr)
	{ // NULLチェック
		OutputDebugString(TEXT("想定...外...です...メモリが...とれないなんて..."));
		return false;
	}

	wcscpy(pDrawStringW, pStrW);				// 文字列保存

	// 各種設定を保存							  
	nDrawSecond = nSecond;						// 一文字あたりにかかる秒数を保存
	DrawCol = Col;								// 文字の色を設定
	bDrawJapanise = TRUE;						// 2バイト文字列に設定

	// Exの値を初期化
	nWaitSecond = -1;
	nWaitByte = -1;
	nDrawNumOtherByte = 0;
	nDrawOtherByte = 0;
	SAFE_DELETEARRAY(pDrawInt);

	bUseWCHAR = true;				// ワイド文字列使用に指定

	return true;
}

//==================================================================================
//--- 文字列待機 ---
//==================================================================================
void MessageLog::WaitForString(INT nByte, INT nSecond)
{
	nWaitByte = nByte;
	nWaitSecond = nSecond;
}

//==================================================================================
//--- 文字列即時表示機 ---
//==================================================================================
void MessageLog::ImmeString(void)
{
	nCntDrawString = nMaxString - 1;
}

//==================================================================================
//--- 文字列の置き換え(W->A, A->A) ---
//==================================================================================
bool MessageLog::ReplaceStringA(LPSTR pStrA, INT nNumMessage, INT nSecond,
	D3DXCOLOR Col, BOOL bJapanise, BOOL bReset)
{
	SAFE_DELETEARRAY(pDrawString);			// 元の文字列を破棄
	SAFE_DELETEARRAY(pDrawStringW);			// 元の文字列を破棄
	nMaxString = nNumMessage;				// 新規の文字列のサイズを保存
	pDrawString = new CHAR[nMaxString];		// 新規の文字列分メモリ確保
	if (pDrawString == nullptr)
	{ // NULLチェック
		OutputDebugString(TEXT("想定...外...です...メモリが...とれないなんて..."));
		return false;
	}

	strcpy((char*)pDrawString, pStrA);		// 文字列をコピー

	if (nSecond != KEEP_USING)
	{
		nDrawSecond = nSecond;					// 秒数を保存
	}

	if (Col != KEEP_USING_COL)
	{
		DrawCol = Col;							// 描画色を保存
	}

	if (bJapanise != KEEP_USING)
	{
		bDrawJapanise = bJapanise;				// 日本語かを保存
	}

	if (bReset)
	{
		nCntDrawString = 0;						// 描画する文字数をリセット
	}
	else if (nCntDrawString >= nNumMessage)
	{
		nCntDrawString = nNumMessage;			// 最大数が減った場合、減少
	}

	bUseWCHAR = false;

	return true;
}

//==================================================================================
//--- 文字列の置き換え(A->W, W->W) ---
//==================================================================================
bool MessageLog::ReplaceStringW(LPWSTR pStrW, INT nNumMessage, INT nSecond,
	D3DXCOLOR Col, BOOL bReset)
{
	SAFE_DELETEARRAY(pDrawString);			// 元の文字列を破棄
	SAFE_DELETEARRAY(pDrawStringW);			// 元の文字列を破棄
	nMaxString = nNumMessage;				// 新規の文字列のサイズを保存
	pDrawStringW = new WCHAR[nMaxString];	// 新規の文字列分メモリ確保
	if (pDrawStringW == nullptr)
	{ // NULLチェック
		OutputDebugString(TEXT("想定...外...です...メモリが...とれないなんて..."));
		return false;
	}

	wcscpy(pDrawStringW, pStrW);			// 新規の文字列にコピー

	if (nSecond != KEEP_USING)
	{
		nDrawSecond = nSecond;					// 秒数を保存
	}
	
	if (Col != KEEP_USING_COL)
	{
		DrawCol = Col;							// 描画色を保存
	}

	if (bReset)
	{
		nCntDrawString = 0;						// 描画する文字数をリセット
	}
	else if (nCntDrawString >= nNumMessage)
	{
		nCntDrawString = nNumMessage;			// 最大数が減った場合、減少
	}

	bUseWCHAR = true;

	return true;
}

//==================================================================================
//--- 文字列の追加(A+A) ---
//==================================================================================
bool MessageLog::AddStringA(LPSTR pStrA, INT nNumMessage)
{
	if (bUseWCHAR == TRUE) return false;			// 現在の文字列がワイド文字列であれば追加不可
	if (pStrA == nullptr) return false;

	INT nSize = nMaxString - 1;						// 元のサイズを保存
	CHAR* pStrCopy = new CHAR[nMaxString];			// 元の文字列分メモリを確保
	if (pStrCopy == nullptr)
	{ // NULLチェック
		OutputDebugString(TEXT("想定...外...です...メモリが...とれないなんて..."));
		return false;
	}

	strcpy((char*)pStrCopy, (char*)pDrawString);	// 文字列を保存

	SAFE_DELETEARRAY(pDrawString);					// 元の文字列を破棄
	nMaxString = nSize + nNumMessage;				// 追加する文字列分を足した配列数を保存
	pDrawString = new CHAR[nMaxString];				// 新規配列を確保
	if (pDrawString == nullptr)
	{ // NULLチェック
		SAFE_DELETEARRAY(pStrCopy);					// 確保していた文字列保存用メモリを解放
		OutputDebugString(TEXT("想定...外...です...メモリが...とれないなんて..."));
		return false;
	}

	strcpy((char*)pDrawString, pStrCopy);			// 保存しておいた文字列をコピー
	strcat((char*)pDrawString, pStrA);				// 追加する文字列を追加

	SAFE_DELETEARRAY(pStrCopy);						// 確保していた文字列保存用メモリを解放

	return true;
}

//==================================================================================
//--- 文字列の追加(W+W) ---
//==================================================================================
bool MessageLog::AddStringW(LPWSTR pStrW, INT nNumMessage)
{
	if (bUseWCHAR == FALSE) return false;			// 現在の文字列がANSI文字列なら追加不可
	if (pStrW == nullptr) return false;

	INT nSize = nMaxString - 1;						// 元のサイズを保存
	WCHAR* pStrCopy = new WCHAR[nMaxString];		// 元の文字列分メモリを確保
	if (pStrCopy == nullptr)
	{ // NULLチェック
		OutputDebugString(TEXT("想定...外...です...メモリが...とれないなんて..."));
		return false;
	}

	wcscpy(pStrCopy, pDrawStringW);					// 文字列を保存

	SAFE_DELETEARRAY(pDrawStringW);					// 元の文字列を破棄
	nMaxString = nSize + nNumMessage;				// 追加する文字列分を足した配列数を保存
	pDrawStringW = new WCHAR[nMaxString];			// 新規配列を確保
	if (pDrawStringW == nullptr)
	{ // NULLチェック
		SAFE_DELETEARRAY(pStrCopy);					// 確保していた文字列保存用メモリを解放
		OutputDebugString(TEXT("想定...外...です...メモリが...とれないなんて..."));
		return false;
	}

	wcscpy(pDrawStringW, pStrCopy);					// 保存しておいた文字列をコピー
	wcscat(pDrawStringW, pStrW);					// 追加する文字列を追加

	SAFE_DELETEARRAY(pStrCopy);						// 確保していた文字列保存用メモリを解放

	return true;
}

//==================================================================================
//--- 文字列の移動 ---
//==================================================================================
void MessageLog::SetPos(D3DXVECTOR3 pos, float fWidth, float fHeight)
{
	float fWidthDef, fHeightDef;

	if (fWidth == KEEP_USING)
	{ // そのままの値を使う場合
		fWidthDef = rect.right - rect.left;
	}
	else
	{
		fWidthDef = fWidth;
	}

	if (fHeight == KEEP_USING)
	{ // そのままの値を使う場合
		fHeightDef = rect.bottom - rect.top;
	}
	else
	{
		fHeightDef = fHeight;
	}

	// メッセージログのサイズを保存
	rect.left = pos.x;
	rect.top = pos.y;
	rect.right = pos.x + fWidthDef;
	rect.bottom = pos.y + fHeightDef;
}

//==================================================================================
//--- テキストサイズの変更 ---
//==================================================================================
bool MessageLog::SetSize(LPDIRECT3DDEVICE9 pDevice, INT nFontSize)
{
	if (pDevice == nullptr)
	{// NULLチェック
		OutputDebugString(TEXT("pDeviceがNULLじゃダメじゃないですか！"));
		return false;
	}

	if (nFontSize == KEEP_USING)
	{// フォントサイズを引き続き使用する場合無視
		OutputDebugString(TEXT("はぁ...それってする意味あるんですか？"));
		return false;
	}

	RELEASE(pFontMessage);

	HRESULT hr;

	hr = D3DXCreateFont(pDevice,		// フォント作成
		nFontSize,
		0,
		0,
		0,
		FALSE,
		SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		DEF_FONT,
		&pFontMessage);

	if (FAILED(hr))
	{
		OutputDebugString(TEXT("フォント作成に失敗...！不覚！"));
		return false;
	}

	return true;
}

//==================================================================================
//--- 更新速度変更 ---
//==================================================================================
bool MessageLog::SetSpeed(INT nUpdateSpeed)
{
	if (nUpdateSpeed == KEEP_USING)
	{// フォントサイズを引き続き使用する場合無視
		OutputDebugString(TEXT("はぁ...それってする意味あるんですか？"));
		return false;
	}

	nDrawSecond = nUpdateSpeed;

	return true;
}

//==================================================================================
//--- int型配列の比較 ---
//==================================================================================
bool MessageLog::CompairArray(int* pArray, int nTarget, int nSize)
{
	if (pArray == nullptr) return false;	// NULLチェック

	for (int nCntCmp = 0; nCntCmp < nSize; nCntCmp++)
	{ // 比較し、正しければtrue
		if (pArray[nCntCmp] == nTarget) return true;
	}

	return false;
}

//==================================================================================
//--- メッセージログの破棄及び新規作成(バイト固定) ---
//==================================================================================
void CreateMessageLogA(LPDIRECT3DDEVICE9 pDevice, D3DXVECTOR3 pos, FLOAT fWidth, FLOAT fHeight,
	INT nFontSize, LPSTR pStr, INT nNumMessage, INT nSecond, D3DXCOLOR Col, BOOL bJapanise,
	LPMESSAGELOG *pMessageLog)
{
	if (pDevice == nullptr) return;
	if (pStr == nullptr) return;
	if (pMessageLog == nullptr) return;

	// 以前のメッセージログを破棄
	SAFE_DELETE(*pMessageLog);

	// 新規でメッセージログを確保
	*pMessageLog = new MessageLog;
	if (*pMessageLog == nullptr)
	{ // NULLチェック
		OutputDebugString(TEXT("不覚...メモリ確保に失敗しました..."));
		return;
	}

	// メッセージログを作成
	(*pMessageLog)->SetMessageLogA(pDevice,
		pos,
		fWidth,
		fHeight,
		nFontSize,
		pStr,
		nNumMessage,
		nSecond, 
		Col, 
		bJapanise);
}

//==================================================================================
//--- メッセージログの破棄及び新規作成(バイト動的) ---
//==================================================================================
void CreateMessageLogExA(LPDIRECT3DDEVICE9 pDevice, D3DXVECTOR3 pos, FLOAT fWidth, FLOAT fHeight,
	INT nFontSize, LPSTR pStr, INT nNumMessage, INT nSecond, D3DXCOLOR Col, BOOL bJapanise,
	INT nNumOtherByte, INT nOtherByte, INT* pInt, LPMESSAGELOG* pMessageLog)
{
	if (pDevice == nullptr) return;
	if (pStr == nullptr) return;
	if (pMessageLog == nullptr) return;

	// 以前のメッセージログを破棄
	SAFE_DELETE(*pMessageLog);

	// 新規でメッセージログを確保
	(*pMessageLog) = new MessageLog;
	if (*pMessageLog == nullptr)
	{ // NULLチェック
		OutputDebugString(TEXT("不覚...メモリ確保に失敗しました..."));
		return;
	}

	// メッセージログを作成
	(*pMessageLog)->SetMessageLogExA(pDevice,
		pos,
		fWidth,
		fHeight,
		nFontSize,
		pStr,
		nNumMessage,
		nSecond,
		Col,
		bJapanise,
		nNumOtherByte,
		nOtherByte,
		pInt);
}

//==================================================================================
//--- メッセージログの破棄及び新規作成(ワイド文字列) ---
//==================================================================================
void CreateMessageLogW(LPDIRECT3DDEVICE9 pDevice, D3DXVECTOR3 pos, FLOAT fWidth, FLOAT fHeight,
	INT nFontSize, LPWSTR pStr, INT nNumMessage, INT nSecond, D3DXCOLOR Col,
	LPMESSAGELOG *pMessageLog)
{
	if (pDevice == nullptr) return;
	if (pStr == nullptr) return;
	if (pMessageLog == nullptr) return;

	// 以前のメッセージログを破棄
	SAFE_DELETE(*pMessageLog);

	// 新規でメッセージログを確保
	*pMessageLog = new MessageLog;
	if (*pMessageLog == nullptr)
	{ // NULLチェック
		OutputDebugString(TEXT("不覚...メモリ確保に失敗しました..."));
		return;
	}

	// メッセージログを作成
	(*pMessageLog)->SetMessageLogW(pDevice,
		pos,
		fWidth,
		fHeight,
		nFontSize,
		pStr,
		nNumMessage,
		nSecond,
		Col);
}

//==================================================================================
//--- メッセージログの破棄 ---
//==================================================================================
void DeleteMessageLog(LPMESSAGELOG *pLog)
{
	if (pLog == nullptr) return;
	SAFE_DELETE(*pLog);
}