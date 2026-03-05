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
#include "common_fade.h"
#include "sound.h"

// 汎用インクルード群
#include "Texture.h"
#include "2Dpolygon.h"
#include "mathUtil.h"
#include "param.h"

// 通常インクルード群
#include "game.h"
#include "player.h"
#include "MessageLog.h"
#include "prompt.h"

USE_PARAM;

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define DIALOG_NUM		(2)									// ダイアログボックスの数
#define LOG_POS		D3DXVECTOR3(125.0f, 510.0f, 0.0f)		// ログの位置
#define LOG_WIDTH	(1000.0f)	// ログの幅
#define LOG_HEIGHT	(250.0f)	// ログの高さ
#define SHOP_POS	D3DXVECTOR3(1450.0f, 100.0f, -475.0f)	// 店前の座標

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
//*** ログのフェーズの種類 ***
//**********************************************************************************
ENUM()
{
	LOGPHASE_FIRST,
	LOGPHASE_TUTORIAL,
	LOGPHASE_MAX
} LOGPHASE;

// 列挙型のインクリメント,デクリメント定義
START_UNABLE
UNABLE_THISFILE(26812)
ENUM_INCREMENT_STOP(LOGPHASE, LOGPHASE_FIRST, LOGPHASE_MAX)
ENUM_DECREMENT_STOP(LOGPHASE, LOGPHASE_FIRST, LOGPHASE_MAX)
END_UNABLE

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
	LOGTYPE type;				// タイプ
} Log;	

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************
LPMESSAGELOG GetMessageLogPointer(void);
void NowLog(void);
void NextLog(void);
void FirstLog(void);
void TutorialLog(void);
void MovableTutorial(void);

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
	"data/TEXTURE/tutorial_frame.png",
};

Log g_apLog[] =		// ログの情報
{
	{L"......街に着いた。", LOGTYPE_GIRL},
	{L"ん？あんな所に奇妙なお店がある。", LOGTYPE_GIRL},
	{L"行ってみよう。", LOGTYPE_GIRL},
};

Log g_apTutorialLog[] =		// チュートリアルログの情報
{
	{L"よぉ。嬢ちゃん。少しお願い聞いてくれねぇか。", LOGTYPE_OLDMAN},
	{L"この街のシンボルの時計台が壊れちまって、\n直さなきゃならないんだ。", LOGTYPE_OLDMAN},
	{L"その為に街から時計台のパーツを集めてきてほしい。\nお礼は弾むぜ。", LOGTYPE_OLDMAN},
	{L"......分かった。", LOGTYPE_GIRL},
};

ID_LOG g_CurrentID = 0;		// 現在のログのID
ID_LOG g_MaxID;				// ログの最大数
ID_LOG g_MaxTutorialID;		// チュートリアルログの最大数
LOGPHASE g_logPhase;		// 現在のログフェーズ
LPMESSAGELOG g_pLog = NULL;	// メッセージログへのポインタ
bool g_bIsEndTutorial;		// チュートリアルの終了判定
bool g_bIsShowAnyDialog;	// 何か一つでもダイアログが表示されているか
bool g_bIsMovableTutorial;	// チュートリアルの動ける状態か
IDX_2DPOLYGON g_IdxLeftStick;	// 左スティックのチュートリアルポリゴン
IDX_2DPOLYGON g_IdxRightStick;	// 右スティックのチュートリアルポリゴン

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitDialog(void)
{
	USE_UTIL;
	// 初期化
	memcpy(&g_aDialog[0], &g_aDialogInfo[0], sizeof(Dialog) * DIALOG_NUM);
	g_CurrentID = 0;
	g_logPhase = LOGPHASE_FIRST;
	g_bIsEndTutorial = false;
	g_bIsShowAnyDialog = false;
	g_bIsMovableTutorial = false;
	g_IdxLeftStick = -1;
	g_IdxRightStick = -1;

	// 値をコピー
	for (int nCntDialog = 0; nCntDialog < DIALOG_NUM; nCntDialog++)
	{
		g_aDialog[nCntDialog].BoxMid = D3DXVECTOR3(CParamVector::WINMID.x, CParamVector::WINMID.y * 1.5f, 0.0f);
	}

	// ログの最大数を保存
	g_MaxID = sizeof g_apLog / sizeof(Log);
	g_MaxTutorialID = sizeof g_apTutorialLog / sizeof(Log);

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

	if (GetKeyboardTrigger(DIK_BACK)
		|| GetJoypadTrigger(0, JOYKEY_START)
		|| GetJoypadTrigger(1, JOYKEY_START))
	{ // チュートリアルスキップ
		g_bIsEndTutorial = true;
		g_bIsShowAnyDialog = false;
		g_bIsMovableTutorial = false;
	}

	if (g_bIsMovableTutorial == true)
	{
		MovableTutorial();
	}
	else if (g_bIsEndTutorial == false)
	{
		NowLog();

		// メッセージログの更新
		UpdateMessageLog(&g_pLog, sizeof(g_pLog));
	}
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
	g_bIsShowAnyDialog = true;
}

//==================================================================================
// --- ログのポインタを取得 ---
//==================================================================================
LPMESSAGELOG GetMessageLogPointer(void)
{
	return g_pLog;
}

//==================================================================================
// --- ログの文章を更新 ---
//==================================================================================
void NowLog(void)
{
	switch (g_logPhase)
	{
	// 最初のログ
	case LOGPHASE_FIRST:
		FirstLog();
		break;

	// チュートリアル中のログ
	case LOGPHASE_TUTORIAL:
		TutorialLog();
		break;

	default:
		return;
	}
}

//==================================================================================
// --- ログのインクリメント ---
//==================================================================================
void NextLog(void)
{
	g_logPhase++;
	g_CurrentID = 0;
}

//==================================================================================
// --- 最初のログ ---
//==================================================================================
void FirstLog(void)
{
	Log* pLogInfo = &g_apLog[g_CurrentID];
	const LPMESSAGELOG pMessageLog = GetMessageLogPointer();

	if (GetKeyboardTrigger(DIK_RETURN)
		|| GetJoypadTrigger(0, JOYKEY_A)
		|| GetJoypadTrigger(1, JOYKEY_A))
	{
		if (g_CurrentID >= g_MaxID)
		{ // ログを流し終わった時、ゲーム開始
			g_bIsShowAnyDialog = false;
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

//==================================================================================
// --- チュートリアルのログ ---
//==================================================================================
void TutorialLog(void)
{
	Player* pPlayer = GetPlayer();		// プレイヤーの取得
	if (GetCommonFade() == FADE_IN
		&& pPlayer->pos != SHOP_POS)
	{
		pPlayer->pos = SHOP_POS;
		pPlayer++;
		pPlayer->pos = SHOP_POS;
	}

	if (GetCommonFade() != FADE_NONE)
	{
		return;
	}

	Log* pLogInfo = &g_apTutorialLog[g_CurrentID];
	const LPMESSAGELOG pMessageLog = GetMessageLogPointer();

	if (g_CurrentID == 0)
	{
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
	else if (GetKeyboardTrigger(DIK_RETURN)
		|| GetJoypadTrigger(0, JOYKEY_A)
		|| GetJoypadTrigger(1, JOYKEY_A))
	{
		if (g_CurrentID >= g_MaxID)
		{ // ログを流し終わった時、ゲーム開始
			g_bIsEndTutorial = true;
			SetEnable2DPolygon(g_aDialog[0].polygon, false);
			SetEnable2DPolygon(g_aDialog[1].polygon, false);
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

//==================================================================================
// --- 行動可能なチュートリアル中処理 ---
//==================================================================================
void MovableTutorial(void)
{
	if (IsDispPrompt(GetIdxShopPrompt()))
	{
		// 店前でAもしくはENTERを押したら
		if (GetKeyboardTrigger(DIK_RETURN)
			|| GetJoypadTrigger(PLAYERTYPE_GIRL, JOYKEY_A))
		{ // 移動中チュートリアル終了 -> ログ表示
			g_bIsMovableTutorial = false;
			NextLog();
		}
	}

	if (g_IdxLeftStick == -1)
	{

	}

	if (g_IdxRightStick == -1)
	{

	}
}

//==================================================================================
// --- ログの表示判定 ---
//==================================================================================
bool IsShowAnyDialog(void)
{
	return g_bIsShowAnyDialog;
}

//==================================================================================
// --- ログの終了判定 ---
//==================================================================================
bool IsEndDialog(void)
{
	return g_bIsEndTutorial;
}