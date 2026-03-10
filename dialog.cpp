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
#include "field.h"

USE_PARAM;

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define DIALOG_NUM		(2)										// ダイアログボックスの数
#define LOG_POS			D3DXVECTOR3(125.0f, 510.0f, 0.0f)		// ログの位置
#define LOG_WIDTH		(1000.0f)	// ログの幅
#define LOG_HEIGHT		(250.0f)	// ログの高さ
#define SHOP_POS		D3DXVECTOR3(1450.0f, 100.0f, -475.0f)	// 店前の座標
#define ANIMTEX_NUM		(3)			// 動くテクスチャの数
#define START_ANIMTEX	(3)			// アニメーションテクスチャの開始ログID

//**********************************************************************************
//*** 型宣言 ***
//**********************************************************************************
typedef int ID_LOG;				// ログの順番

//**********************************************************************************
//*** ログの人物の種類 ***
//**********************************************************************************
typedef enum
{
	LOGTYPE_GIRL,		// 少女
	LOGTYPE_OLDMAN,		// お爺さん
	LOGTYPE_MAX
} LOGTYPE;

//**********************************************************************************
//*** ログのフェーズの種類 ***
//**********************************************************************************
typedef enum
{
	LOGPHASE_FIRST,		// 開始直後のログ
	LOGPHASE_TUTORIAL,	// チュートリアル中のログ
	LOGPHASE_MAX
} LOGPHASE;

// 列挙型のインクリメント,デクリメント定義
START_UNABLE
UNABLE_THISFILE(26812)
UNABLE_THISFILE(26495)
ENUM_INCREMENT_STOP(LOGPHASE, LOGPHASE_FIRST, LOGPHASE_MAX)
ENUM_DECREMENT_STOP(LOGPHASE, LOGPHASE_FIRST, LOGPHASE_MAX)

//**********************************************************************************
//*** チュートリアルテクスチャの種類 ***
//**********************************************************************************
typedef enum
{
	TUTORIAL_TEX_LEFTSTICK = 0,		// 左スティックの説明
	TUTORIAL_TEX_RIGHTSTICK,		// 右スティックの説明
	TUTORIAL_TEX_GEAR,				// ギアの画像
	TUTORIAL_TEX_SCREW,				// ネジの画像
	TUTORIAL_TEX_UNKNOWN,			// ?の画像
	TUTORIAL_TEX_ARROW,				// 地面に写るガイド矢印
	TUTORIAL_TEX_MAX
} TUTORIAL_TEX;

//**********************************************************************************
//*** ダイアログボックス構造体 ***
//**********************************************************************************
STRUCT()
{
	D3DXVECTOR3 BoxMid;		// ボックスの中心
	D3DXVECTOR2 BoxSize;	// ボックスのサイズ
	IDX_TEXTURE tex;			// テクスチャのインデックス
	IDX_2DPOLYGON polygon;	// ポリゴンのインデックス
} Dialog;

//**********************************************************************************
//*** チュートリアルテクスチャ構造体 ***
//**********************************************************************************
STRUCT()
{
	D3DXVECTOR3 pos;		// 描画位置
	D3DXVECTOR2 size;		// 描画サイズ
} TutorialTex;

//**********************************************************************************
//*** ログ構造体 ***
//**********************************************************************************
STRUCT()
{
	const wchar_t *pLog;		// ログの内容
	LOGTYPE type;				// タイプ
} Log;	

//**********************************************************************************
//*** チュートリアルで動くテクスチャの情報 ***
//**********************************************************************************
STRUCT()
{
	IDX_2DPOLYGON poly;			// ポリゴンインデックス
	ID_LOG showID;				// 描画開始するID(Initで設定)
	D3DXVECTOR3 start, end;		// 動く範囲(Lerpで移動)
	D3DXCOLOR col;				// 色
	float s;					// 座標及び透明度変換係数(Lerpで使用)
	bool bReverse;				// 移動反転
} Tutorial_TexAnim;

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
#pragma region 変数宣言
Dialog g_aDialog[DIALOG_NUM];

// ダイアログの情報
const Dialog g_aDialogInfo[DIALOG_NUM] =		
{
	{D3DXVECTOR3(CParamVector::WINMID.x, CParamVector::WINMID.y * 1.75f, 0.0f), D3DXVECTOR2(1280.0f, 300.0f), -1, -1},
	{D3DXVECTOR3(CParamVector::WINMID.x, CParamVector::WINMID.y * 1.75f, 0.0f), D3DXVECTOR2(1280.0f, 300.0f), -1, -1},
};

#pragma region テクスチャパスまとめ

// テクスチャパス
const char *g_apDialogTexture[DIALOG_NUM] =		
{
	"data/TEXTURE/girl_frame.png",
	"data/TEXTURE/man_frame.png",
};

// チュートリアル中のテクスチャ
const char *g_apTutorialTexture[TUTORIAL_TEX_MAX] =
{
	"data/TEXTURE/LeftStick.png",
	"data/TEXTURE/RightStick.png",

	// ここから動くテクスチャ
	"data/TEXTURE/Item_Gear.png",
	"data/TEXTURE/Item_Screw.png",
	"data/TEXTURE/Item_Unknown.png",
	"data/TEXTURE/Tutorial_Arrow.png",
};

#pragma endregion

// アニメーションテクスチャの動く範囲
const D3DXVECTOR3 g_aMoveTexture[TUTORIAL_TEX_MAX][2] =
{
	{D3DXVECTOR3(), D3DXVECTOR3()},
	{D3DXVECTOR3(), D3DXVECTOR3()},

	// ここから開始
	{D3DXVECTOR3(300, 200, 0), D3DXVECTOR3(300, 250, 0)},
	{D3DXVECTOR3(640, 225, 0), D3DXVECTOR3(640, 175, 0)},
	{D3DXVECTOR3(1000, 225, 0), D3DXVECTOR3(1000, 300, 0)},

	// 終わり
	{D3DXVECTOR3(1550.0f, 100.9999f, -905.0f), D3DXVECTOR3(1550.0f, 100.9999f, -895.0f)},
};

#pragma region ログの情報まとめ

// ログの情報
Log g_apLog[] =		
{
	{L"街に着いた。", LOGTYPE_GIRL},
	{L"ん？あんな所に奇妙なお店がある。", LOGTYPE_GIRL},
	{L"気になるし、行ってみよう。", LOGTYPE_GIRL},
};

// チュートリアルログの情報
Log g_apTutorialLog[] =		
{
	{L"よぉ。嬢ちゃん。少しお願い聞いてくれねぇか。", LOGTYPE_OLDMAN},
	{L"この街のシンボルの時計台が壊れちまって、\n直さなきゃならないんだ。", LOGTYPE_OLDMAN},
	{L"その為に街から時計台のパーツを集めてきてほしい。\nうちのネズミを貸してやる。頼りになるぜ。", LOGTYPE_OLDMAN},
	{L"......分かった。", LOGTYPE_GIRL},
};

#pragma endregion

// チュートリアル中の情報
TutorialTex g_aTutorial[] =
{
	{D3DXVECTOR3(150, 550, 0), D3DXVECTOR2(150, 150)},					// 左スティック
	{D3DXVECTOR3(1130, 550, 0), D3DXVECTOR2(150, 150)},					// 右スティック
	{D3DXVECTOR3(300, 200, 0), D3DXVECTOR2(150, 150)},					// 歯車
	{D3DXVECTOR3(640, 225, 0), D3DXVECTOR2(150, 150)},					// ネジ
	{D3DXVECTOR3(1000, 225, 0), D3DXVECTOR2(150, 150)},					// ？
	{D3DXVECTOR3(1550.0f, 100.9999f, -900.0f), D3DXVECTOR2(250, 125)},	// 矢印
};

ID_LOG g_CurrentID = 0;			// 現在のログのID
ID_LOG g_MaxID;					// ログの最大数
ID_LOG g_MaxTutorialID;			// チュートリアルログの最大数
LOGPHASE g_logPhase;			// 現在のログフェーズ
LPMESSAGELOG g_pLog = NULL;		// メッセージログへのポインタ
bool g_bIsEndTutorial;			// チュートリアルの終了判定
bool g_bIsShowAnyDialog;		// 何か一つでもダイアログが表示されているか
bool g_bIsMovableTutorial;		// チュートリアルの動ける状態か
bool g_bShowMoveTex;			// テクスチャ表示済みか
bool g_bSKipLog;				// 現在再生中のログをスキップ可能か
IDX_2DPOLYGON g_IdxLeftStick;	// 左スティックのチュートリアルポリゴン
IDX_2DPOLYGON g_IdxRightStick;	// 右スティックのチュートリアルポリゴン
IDX_FIELD g_IdxField;			// フィールドのインデックス
Tutorial_TexAnim g_aAnimTex[TUTORIAL_TEX_MAX];	// テクスチャのポリゴン情報

#pragma endregion

#pragma region ダイアログの基本処理

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
	g_bShowMoveTex = false;
	g_bSKipLog = true;
	g_IdxLeftStick = -1;
	g_IdxRightStick = -1;
	g_IdxField = -1;

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

	// アニメーションテクスチャの設定

	// 初期化
	AutoZeroMemory(g_aAnimTex);

	IDX_TEXTURE tex;	// テクスチャインデックス一時保存先

	for (int nCntTex = TUTORIAL_TEX_GEAR; nCntTex < TUTORIAL_TEX_ARROW; nCntTex++)
	{
		LoadTexture(g_apTutorialTexture[nCntTex], &tex);
		g_aAnimTex[nCntTex].poly =
			Set2DPolygon(g_aMoveTexture[nCntTex][0],
				CParamVector::V3NULL, 
				g_aTutorial[nCntTex].size,
				tex);

		SetColor2DPolygon(g_aAnimTex[nCntTex].poly, g_aAnimTex[nCntTex].col);
		SetEnable2DPolygon(g_aAnimTex[nCntTex].poly, false);

		g_aAnimTex[nCntTex].showID = START_ANIMTEX;
		g_aAnimTex[nCntTex].start = g_aMoveTexture[nCntTex][0];
		g_aAnimTex[nCntTex].end = g_aMoveTexture[nCntTex][1];
	}

	// 床
	// テクスチャ読み込み
	LoadTexture(g_apTutorialTexture[TUTORIAL_TEX_ARROW], &tex);

	// 設置
	g_IdxField = SetField(g_aTutorial[TUTORIAL_TEX_ARROW].pos,
		CParamVector::V3NULL,
		VEC_Y(CParamFloat::HALFPI),
		g_aTutorial[TUTORIAL_TEX_ARROW].size.x,
		g_aTutorial[TUTORIAL_TEX_ARROW].size.y,
		tex,
		1,
		1,
		D3DCULL_CCW);

	SetEnableField(g_IdxField, false);					// 描画スキップ

	g_aAnimTex[TUTORIAL_TEX_ARROW].start = g_aMoveTexture[TUTORIAL_TEX_ARROW][0];
	g_aAnimTex[TUTORIAL_TEX_ARROW].end = g_aMoveTexture[TUTORIAL_TEX_ARROW][1];
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
	if (g_bIsEndTutorial == true) return;

	Log *pLogInfo = &g_apLog[g_CurrentID];
	const LPMESSAGELOG pMessageLog = GetMessageLogPointer();

	if (GetKeyboardTrigger(DIK_BACK)
		|| GetJoypadTrigger(0, JOYKEY_START)
		|| GetJoypadTrigger(1, JOYKEY_START))
	{ // チュートリアルスキップ
		SetCommonFade(30, 30, 30);
		g_bIsEndTutorial = true;
		g_bIsShowAnyDialog = false;
		g_bIsMovableTutorial = false;

		SetEnable2DPolygon(g_aDialog[0].polygon, false);
		SetEnable2DPolygon(g_aDialog[1].polygon, false);

		for (int nCntTex = TUTORIAL_TEX_GEAR; nCntTex < TUTORIAL_TEX_ARROW; nCntTex++)
		{
			SetEnable2DPolygon(g_aAnimTex[nCntTex].poly, false);
		}

		SetEnableField(g_IdxField, false);

		Destroy2DPolygon(g_IdxLeftStick);
		Destroy2DPolygon(g_IdxRightStick);
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

	if (g_logPhase == LOGPHASE_TUTORIAL && g_CurrentID >= START_ANIMTEX)
	{
		if (g_bShowMoveTex == false)
		{
			for (int nCntTex = TUTORIAL_TEX_GEAR; nCntTex < TUTORIAL_TEX_ARROW; nCntTex++)
			{
				SetEnable2DPolygon(g_aAnimTex[nCntTex].poly, true);
			}

			g_bShowMoveTex = true;
		}

		D3DXVECTOR3 pos;
		D3DXCOLOR col;

		for (int nCntTex = TUTORIAL_TEX_GEAR; nCntTex < TUTORIAL_TEX_ARROW; nCntTex++)
		{
			if (g_aAnimTex[nCntTex - 1].col.a <= 0.75f && nCntTex != TUTORIAL_TEX_GEAR) continue;

			g_aAnimTex[nCntTex].s += (0.005f + (nCntTex * 0.0008f)) * ((g_aAnimTex[nCntTex].bReverse) ? -1 : 1);

			if (g_aAnimTex[nCntTex].s <= 0.0f)
			{
				g_aAnimTex[nCntTex].s = 0.0f;
				g_aAnimTex[nCntTex].bReverse = !g_aAnimTex[nCntTex].bReverse;
			}
			else if (g_aAnimTex[nCntTex].s >= 1.0f)
			{
				g_aAnimTex[nCntTex].s = 1.0f;
				g_aAnimTex[nCntTex].bReverse = !g_aAnimTex[nCntTex].bReverse;
			}

			USE_UTIL;
			pos = GetPTPLerp(g_aAnimTex[nCntTex].start, g_aAnimTex[nCntTex].end, g_aAnimTex[nCntTex].s);

			if (g_aAnimTex[nCntTex].col != CParamColor::WHITE)
			{
				g_aAnimTex[nCntTex].col = GetColLerp(CParamColor::INV_WHITE, CParamColor::WHITE, g_aAnimTex[nCntTex].s);
			}

			SetPosition2DPolygon(g_aAnimTex[nCntTex].poly, pos);
			SetColor2DPolygon(g_aAnimTex[nCntTex].poly, g_aAnimTex[nCntTex].col);
		}
	}

	D3DXVECTOR3 pos;
	D3DXCOLOR col;

	g_aAnimTex[TUTORIAL_TEX_ARROW].s += 0.005f * ((g_aAnimTex[TUTORIAL_TEX_ARROW].bReverse) ? -1 : 1);

	if (g_aAnimTex[TUTORIAL_TEX_ARROW].s <= 0.0f)
	{
		g_aAnimTex[TUTORIAL_TEX_ARROW].s = 0.0f;
		g_aAnimTex[TUTORIAL_TEX_ARROW].bReverse = !g_aAnimTex[TUTORIAL_TEX_ARROW].bReverse;
	}
	else if (g_aAnimTex[TUTORIAL_TEX_ARROW].s >= 1.0f)
	{
		g_aAnimTex[TUTORIAL_TEX_ARROW].s = 1.0f;
		g_aAnimTex[TUTORIAL_TEX_ARROW].bReverse = !g_aAnimTex[TUTORIAL_TEX_ARROW].bReverse;
	}

	USE_UTIL;
	pos = GetPTPLerp(g_aAnimTex[TUTORIAL_TEX_ARROW].start, g_aAnimTex[TUTORIAL_TEX_ARROW].end, g_aAnimTex[TUTORIAL_TEX_ARROW].s);

	SetPositionField(g_IdxField, pos);

	g_bSKipLog = !g_pLog->IsEndMessageLog();
}

//==================================================================================
// --- 描画 ---
//==================================================================================
void DrawDialog(void)
{
	// メッセージログの描画
	DrawMessageLog(&g_pLog, sizeof(g_pLog));
}

#pragma endregion

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
	g_bIsMovableTutorial = false;
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
		if (g_bSKipLog == true)
		{
			g_pLog->ImmeString();
			g_bSKipLog = false;
			return;
		}

		if (g_CurrentID >= g_MaxID)
		{ // ログを流し終わった時、ゲーム開始
			g_bIsShowAnyDialog = false;
			g_bIsMovableTutorial = true;
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

			g_bSKipLog = true;
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
		if (g_bSKipLog == true)
		{
			g_pLog->ImmeString();
			g_bSKipLog = false;
			return;
		}

		if (g_CurrentID >= g_MaxTutorialID)
		{ // ログを流し終わった時、ゲーム開始
			g_bIsEndTutorial = true;
			g_bIsShowAnyDialog = false;
			g_bIsMovableTutorial = false;
			SetEnable2DPolygon(g_aDialog[0].polygon, false);
			SetEnable2DPolygon(g_aDialog[1].polygon, false);
			SetCommonFade(40, 40, 30);

			for (int nCntTex = TUTORIAL_TEX_GEAR; nCntTex < TUTORIAL_TEX_MAX; nCntTex++)
			{
				SetEnable2DPolygon(g_aAnimTex[nCntTex].poly, false);
			}

			SetEnableField(g_IdxField, false);
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

			g_bSKipLog = true;
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
			g_bIsShowAnyDialog = true;
			Destroy2DPolygon(g_IdxLeftStick);
			Destroy2DPolygon(g_IdxRightStick);
			NextLog();
		}
	}

	if (g_IdxLeftStick == -1)
	{
		IDX_TEXTURE tex;

		// テクスチャ読み込み
		LoadTexture(g_apTutorialTexture[0], &tex);

		// 2Dポリゴンを設置
		g_IdxLeftStick = Set2DPolygon(g_aTutorial[0].pos, CParamVector::V3NULL, g_aTutorial[0].size, tex);

		// ログ及びダイアログを不可視に設定
		const LPMESSAGELOG pMessageLog = GetMessageLogPointer();

		USE_LIB;
		WString string = L"";

		SetEnable2DPolygon(g_aDialog[0].polygon, false);
		SetEnable2DPolygon(g_aDialog[1].polygon, false);
		pMessageLog->ReplaceStringW(string.GetHead(),
			string.GetVectorNum(),
			KEEP_USING,
			KEEP_USING_COL,
			TRUE);

		SetEnableField(g_IdxField, true);
	}

	if (g_IdxRightStick == -1)
	{
		IDX_TEXTURE tex;

		// テクスチャ読み込み
		LoadTexture(g_apTutorialTexture[1], &tex);

		// 2Dポリゴンを設置
		g_IdxRightStick = Set2DPolygon(g_aTutorial[1].pos, CParamVector::V3NULL, g_aTutorial[1].size, tex);
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

END_UNABLE