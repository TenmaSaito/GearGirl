//==================================================================================
//
// DirectXのダイアログのソースファイル [dialog.cpp]
// Author : TENMA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "dialog.h"

// 基本インクルード群
#include "input.h"
#include "fade.h"
#include "sound.h"

// 汎用インクルード群
#include "Texture.h"
#include "2Dpolygon.h"
#include "mathUtil.h"
#include "param.h"

// 通常インクルード群
#include "tutorial.h"
#include "MessageLog.h"

USE_PARAM;

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define DIALOG_NUM		(2)						// ダイアログボックスの数
#define LOG_POS		D3DXVECTOR3(125.0f, 510.0f, 0.0f)	// ログの位置
#define LOG_WIDTH	(1000.0f)	// ログの幅
#define LOG_HEIGHT	(250.0f)	// ログの高さ

//**********************************************************************************
//*** 型宣言 ***
//**********************************************************************************
typedef int ID_LOG;				// ログの順番

//**********************************************************************************
//*** ログの人物の種類 ***
//**********************************************************************************
ENUM()
{
	LOGTYPE_GIRL,
	LOGTYPE_OLDMAN,
	LOGTYPE_MAX
} LOGTYPE;

//**********************************************************************************
//*** ダイアログボックス構造体 ***
//**********************************************************************************
STRUCT()
{
	D3DXVECTOR3 BoxMid;		// ボックスの中心
	D3DXVECTOR2 BoxSize;	// ボックスのサイズ
	IDX_TEX tex;			// テクスチャのインデックス
	IDX_2DPOLYGON polygon;	// ポリゴンのインデックス
} Dialog;

//**********************************************************************************
//*** ログ構造体 ***
//**********************************************************************************
STRUCT()
{
	const wchar_t *pLog;		// ログの内容
	LOGTYPE type;		// タイプ
} Log;	

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************
LPMESSAGELOG GetMessageLogPointer(void);

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
Dialog g_aDialog[DIALOG_NUM];
const Dialog g_aDialogInfo[DIALOG_NUM] =		// ダイアログの情報
{
	{D3DXVECTOR3(CParamVector::WINMID.x, CParamVector::WINMID.y * 1.75f, 0.0f), D3DXVECTOR2(1280.0f, 300.0f), -1, -1},
	{D3DXVECTOR3(CParamVector::WINMID.x, CParamVector::WINMID.y * 1.75f, 0.0f), D3DXVECTOR2(1280.0f, 300.0f), -1, -1},
};

const char *g_apDialogTexture[DIALOG_NUM] =		// テクスチャパス
{
	"data/TEXTURE/tutorial_frame.png",
	"data/TEXTURE/.jpg",
};

Log g_apLog[] =		// ログの情報
{
	{L"......街に着いた。", LOGTYPE_GIRL},
	{L"一先ず、壊れた体を修理したい。", LOGTYPE_GIRL},
	{L"どこかに修理してくれる人は......", LOGTYPE_GIRL},
	{L"ん？あんな所に奇妙なお店がある。", LOGTYPE_GIRL},
	{L"......行ってみよう。", LOGTYPE_GIRL},
};

ID_LOG g_CurrentID = 0;		// 現在のログのID
ID_LOG g_MaxID;				// ログの最大数
LPMESSAGELOG g_pLog = NULL;		// メッセージログへのポインタ

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitDialog(void)
{
	// 初期化
	memcpy(&g_aDialog[0], &g_aDialogInfo[0], sizeof(Dialog) * DIALOG_NUM);
	g_CurrentID = 0;

	// 値をコピー
	for (int nCntDialog = 0; nCntDialog < DIALOG_NUM; nCntDialog++)
	{
		g_aDialog[nCntDialog].BoxMid = D3DXVECTOR3(CParamVector::WINMID.x, CParamVector::WINMID.y * 1.5f, 0.0f);
	}

	// ログの最大数を保存
	g_MaxID = sizeof g_apLog / sizeof(Log);

	// メッセージログの初期化
	InitMessageLog(&g_pLog, sizeof(g_pLog));
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitDialog(void)
{
	// メッセージログの破棄
	UninitMessageLog(&g_pLog, sizeof(g_pLog));
}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateDialog(void)
{
	Log *pLogInfo = &g_apLog[g_CurrentID];
	const LPMESSAGELOG pMessageLog = GetMessageLogPointer();

	if (GetKeyboardTrigger(DIK_RETURN)
		|| GetJoypadTrigger(0, JOYKEY_A)
		|| GetJoypadTrigger(1, JOYKEY_A))
	{
		if (GetFade() == FADE_NONE)
		{
			if (g_CurrentID >= g_MaxID)
			{ // ログを流し終わった時、ゲーム開始

			}
			else
			{ // ログを進める
				USE_LIB;
				WString string;
				string.AddPointer(pLogInfo->pLog, wcslen(pLogInfo->pLog) + 1);
				g_CurrentID++;

				SetEnable2DPolygon(g_aDialog[pLogInfo->type].polygon, true);
				SetEnable2DPolygon(g_aDialog[(pLogInfo->type + 1) % LOGTYPE_MAX].polygon, false);
				pMessageLog->ReplaceStringW(string.GetHead(),
					string.GetVectorNum(),
					KEEP_USING,
					KEEP_USING_COL,
					TRUE);
			}
		}
	}
	else if (GetKeyboardTrigger(DIK_BACK)
		|| GetJoypadTrigger(0, JOYKEY_START)
		|| GetJoypadTrigger(1, JOYKEY_START))
	{ // チュートリアルスキップ
		if (GetFade() == FADE_NONE)
		{
			if (g_CurrentID < g_MaxID)
			{
				USE_LIB;

				WString testString = L"ちょっ、未だ話してる最中だからスキップなんてしないで。";

				pMessageLog->ReplaceStringW(testString.GetHead(),
					testString.GetVectorNum(),
					KEEP_USING,
					KEEP_USING_COL,
					TRUE);
			}
		}
	}

	// メッセージログの更新
	UpdateMessageLog(&g_pLog, sizeof(g_pLog));
}

//==================================================================================
// --- 描画 ---
//==================================================================================
void DrawDialog(void)
{
	// メッセージログの描画
	DrawMessageLog(&g_pLog, sizeof(g_pLog));
}

//==================================================================================
// --- 設置 ---
//==================================================================================
void SetDialog(void)
{
	USE_LIB;
	AUTODEVICE9 Auto;
	WString testString = L"";

	// メッセージログの作成
	CreateMessageLogW(Auto.pDevice,
		LOG_POS,
		LOG_WIDTH,
		LOG_HEIGHT,
		DEF_FONTSIZE,
		testString.GetHead(),
		testString.GetVectorNum(),
		5,
		CParamColor::WHITE,
		&g_pLog);

	Log* pLogInfo = &g_apLog[g_CurrentID];
	const LPMESSAGELOG pMessageLog = GetMessageLogPointer();

	for (int nCntDialog = 0; nCntDialog < DIALOG_NUM; nCntDialog++)
	{
		// テクスチャの読み込み
		LoadTexture(g_apDialogTexture[nCntDialog], &g_aDialog[nCntDialog].tex);

		// 2Dポリゴンの設置
		g_aDialog[nCntDialog].polygon = Set2DPolygon(g_aDialog[nCntDialog].BoxMid,
			CParamVector::V3NULL,
			g_aDialog[nCntDialog].BoxSize,
			g_aDialog[nCntDialog].tex);

		SetEnable2DPolygon(g_aDialog[nCntDialog].polygon, false);
	}

	WString string;
	string.AddPointer(pLogInfo->pLog, wcslen(pLogInfo->pLog) + 1);

	SetEnable2DPolygon(g_aDialog[pLogInfo->type].polygon, true);
	SetEnable2DPolygon(g_aDialog[(pLogInfo->type + 1) % LOGTYPE_MAX].polygon, false);
	pMessageLog->ReplaceStringW(string.GetHead(),
		string.GetVectorNum(),
		KEEP_USING,
		KEEP_USING_COL,
		TRUE);

	g_CurrentID++;
}

//==================================================================================
// --- ログのポインタを取得 ---
//==================================================================================
LPMESSAGELOG GetMessageLogPointer(void)
{
	return g_pLog;
}