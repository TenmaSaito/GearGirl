//==================================================================================
//
// DirectXのタイトル選択肢用のcppファイル [titleSelect.cpp]
// Author : TENMA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "titleSelect.h"
#include "input.h"
#include "2Dpolygon.h"
#include "mathUtil.h"
#include "fade.h"

using namespace MyMathUtil;

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define MAX_TITLE			TITLE_MENU_MAX		// タイトルの種類
#define HEIGHT_SIZE_TITLE	50.0f 				// 縦サイズ
#define WIDTH_SIZE_TITLE	300.0f				// 横サイズ
#define LINE_SPACE_TITLE	5.0f				// 行間隔
#define WIDTH_CENTER_LINE	SCREEN_WIDTH / 2	// 横中央線
#define MENU_ORIGIN			TITLE_MENU_TAP		// 最初の選択肢		
#define PHASE_NUM			3					// 選択の回数
#define COLOR_A(col, a)		D3DXCOLOR(col.r,col.g, col.b, a)	// a値のみ変更

//**********************************************************************************
//*** 選択肢構造体の定義 ***
//**********************************************************************************
typedef struct
{
	LPDIRECT3DTEXTURE9		pTexture;	// テクスチャへのポインタ
	LPDIRECT3DVERTEXBUFFER9	pVtxBuff;	// 頂点バッファへのポインタ
	D3DXVECTOR3 pos;		// 中心位置
	D3DXCOLOR col;			// 色
	float s;				// Lerp変換用
	bool bCanSelect;		// 選択出来るか
	bool bSelect;			// 選択されているか
	bool bShow;				// 描画されているか
} TSelect;

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************
void CreateSelectBuffer(LPDIRECT3DDEVICE9 pDevice, TSelect *pOut, TITLE_MENU type);
void ChangePhase(void);
void ChangeSelect(void);
void ToNextPhase(void);
void ToSetting(void);
void ToQuitGame(void);

//**********************************************************************************
// テクスチャファイル名
//**********************************************************************************
const char* c_apFilenameTitle[MAX_TITLE] =
{
	"data\\TEXTURE\\TitleTapToStart.png",
	"data\\TEXTURE\\TitleStart.png",
	"data\\TEXTURE\\TitleOption.png",
	"data\\TEXTURE\\TitleEnd.png",
	"data\\TEXTURE\\Title1Player.png",
	"data\\TEXTURE\\Title2Player.png",
};

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
TSelect g_aSelect[TITLE_MENU_MAX];	// 選択肢の情報
int g_nSelect;						// 現在の選択肢
int g_nPhaseSelect;					// 現在の選択フェーズ
int g_nPlayerNum;					// プレイヤーの人数

const D3DXVECTOR3 g_aPosSelect[TITLE_MENU_MAX] =		// 各選択肢の初期位置
{
	D3DXVECTOR3(1020, 615, 0),
	D3DXVECTOR3(1020, 560, 0),
	D3DXVECTOR3(1020, 615, 0),
	D3DXVECTOR3(1020, 670, 0),
	D3DXVECTOR3(1020, 587, 0),
	D3DXVECTOR3(1020, 642, 0),
};

const D3DXVECTOR3 g_aPosSelectInvisible[TITLE_MENU_MAX] =		// 各選択肢の見えなくなる位置
{
	D3DXVECTOR3(640, 615, 0),
	D3DXVECTOR3(640, 560, 0),
	D3DXVECTOR3(640, 615, 0),
	D3DXVECTOR3(640, 670, 0),
	D3DXVECTOR3(640, 587, 0),
	D3DXVECTOR3(640, 642, 0),
};

const int g_aPhaseSelect[PHASE_NUM][3] =				// 各選択フェーズにおける選択可能な範囲
{
	{TITLE_MENU_TAP, TITLE_MENU_START, FALSE},			// [0] 範囲の初め
	{TITLE_MENU_START, TITLE_MENU_ONEPLAY, TRUE},		// [1] 範囲の終わり + 1
	{TITLE_MENU_ONEPLAY, TITLE_MENU_MAX, TRUE},			// [2] フェーズ進行後に戻れるか
};

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitTitleSelect(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	TSelect *pSelect = &g_aSelect[0];

	// フェーズリセット
	g_nPhaseSelect = MENU_ORIGIN;

	// 選択肢の作成
	for (int nCntSelect = 0; nCntSelect < TITLE_MENU_MAX; nCntSelect++, pSelect++)
	{
		CreateSelectBuffer(pDevice, pSelect, (TITLE_MENU)nCntSelect);
	}

	EndDevice();
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitTitleSelect(void)
{
	TSelect *pSelect = &g_aSelect[0];
	for (int nCntSelect = 0; nCntSelect < TITLE_MENU_MAX; nCntSelect++, pSelect++)
	{
		RELEASE(pSelect->pVtxBuff);

		RELEASE(pSelect->pTexture);
	}
}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateTitleSelect(void)
{
	VERTEX_2D *pVtx = nullptr;
	TSelect *pSelect = &g_aSelect[0];

	// フェーズ切り替え
	ChangePhase();

	ChangeSelect();

	for (int nCntSelect = 0; nCntSelect < TITLE_MENU_MAX; nCntSelect++, pSelect++)
	{
		if (pSelect->bCanSelect == false && pSelect->bShow == false) continue;

		if (pSelect->bCanSelect == true)
		{
			if (pSelect->s < 1.0f)
			{
				pSelect->s += 0.01f;
				if (pSelect->s >= 1.0f)
				{
					pSelect->s = 1.0f;
					g_aSelect[g_nSelect].col = D3DXCOLOR(0, 1, 0, g_aSelect[g_nSelect].col.a);
				}
			}
		}
		else
		{
			if (pSelect->s > 0.0f)
			{
				pSelect->s -= 0.01f;
				if (pSelect->s <= 0.0f)
				{
					pSelect->s = 0.0f;
					pSelect->bShow = false;
				}
			}
		}

		D3DXVECTOR3 posLerp = GetPTPLerp(pSelect->pos, g_aPosSelectInvisible[nCntSelect], pSelect->s);
		D3DXCOLOR colLerp = GetColLerp(pSelect->col, COLOR_A(pSelect->col, 1.0f), pSelect->s);

		// 頂点バッファの設定
		pSelect->pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);
		if (pVtx == nullptr)
		{
			OutputDebugString(TEXT("頂点バッファの設定に失敗！？"));
		}

		SetPolygonPos(pVtx, posLerp, D3DXVECTOR2(WIDTH_SIZE_TITLE, HEIGHT_SIZE_TITLE));
		SetDefaultColor(pVtx, colLerp);
		SetPolygonRHW(pVtx);
		SetDefaultTexture(pVtx);

		pSelect->pVtxBuff->Unlock();
	}
}

//==================================================================================
// --- 描画 ---
//==================================================================================
void DrawTitleSelect(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	TSelect *pSelect = &g_aSelect[0];

	for (int nCntSelect = 0; nCntSelect < TITLE_MENU_MAX; nCntSelect++, pSelect++)
	{
		if (pSelect->bShow == false) continue;

		DrawPolygon(pDevice,
			pSelect->pVtxBuff,
			pSelect->pTexture,
			sizeof(VERTEX_2D),
			FVF_VERTEX_2D,
			1);
	}

	EndDevice();
}

//==================================================================================
// --- バッファ生成 ---
//==================================================================================
void CreateSelectBuffer(LPDIRECT3DDEVICE9 pDevice, TSelect *pOut, TITLE_MENU type)
{
	if (type < 0 || type >= TITLE_MENU_MAX) return;

	// 頂点バッファ生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4,
		D3DUSAGE_WRITEONLY, 
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&pOut->pVtxBuff,
		NULL);

	// テクスチャバッファ作成
	D3DXCreateTextureFromFile(pDevice,
		c_apFilenameTitle[type],
		&pOut->pTexture);

	// 初期位置を設定
	pOut->pos = g_aPosSelect[type];
	pOut->col = D3DXCOLOR_INV;

	if (type == MENU_ORIGIN)
	{ // 選択肢が初めだったら
		pOut->bCanSelect = true;
		pOut->bSelect = true;
		pOut->bShow = true;
	}
	else
	{
		pOut->bCanSelect = false;
		pOut->bSelect = false;
		pOut->bShow = false;
	}

	VERTEX_2D *pVtx = nullptr;

	// 頂点バッファの設定
	pOut->pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);
	if (pVtx == nullptr)
	{
		OutputDebugString(TEXT("頂点バッファの設定に失敗！？"));
	}

	SetPolygonPos(pVtx, pOut->pos, D3DXVECTOR2(WIDTH_SIZE_TITLE, HEIGHT_SIZE_TITLE));
	SetDefaultColor(pVtx, D3DXCOLOR_INV);
	SetPolygonRHW(pVtx);
	SetDefaultTexture(pVtx);

	pOut->pVtxBuff->Unlock();
}

//==================================================================================
// --- フェーズ切り替え ---
//==================================================================================
void ChangePhase(void)
{
	if (GetKeyboardTrigger(DIK_RETURN))
	{
		if (g_nPhaseSelect < PHASE_NUM - 1)
		{
			if (g_nPhaseSelect == 1)
			{
				switch (g_nSelect)
				{
					// ゲーム開始
				case TITLE_MENU_START:
					ToNextPhase();
					break;

					// 設定
				case TITLE_MENU_OPTION:
					ToSetting();
					break;

					// 退出
				case TITLE_MENU_OUTGAME:
					ToQuitGame();
					break;

				default:
					break;
				}
			}
			else
			{
				ToNextPhase();
			}
		}
		else
		{
			if (g_nSelect == TITLE_MENU_ONEPLAY)
			{
				g_nPlayerNum = 1;
			}
			else if(g_nSelect == TITLE_MENU_TWOPLAY)
			{
				g_nPlayerNum = 2;
			}

			SetFade(MODE_GAME);
		}
	}
	else if (GetKeyboardTrigger(DIK_BACK) && g_nPhaseSelect > 0)
	{
		for (int nCntSelect = g_aPhaseSelect[g_nPhaseSelect][0]; nCntSelect < g_aPhaseSelect[g_nPhaseSelect][1]; nCntSelect++)
		{
			g_aSelect[nCntSelect].bSelect = false;
			g_aSelect[nCntSelect].bCanSelect = false;
			g_aSelect[g_nSelect].col = D3DXCOLOR(1, 1, 1, g_aSelect[g_nSelect].col.a);
		}

		g_nPhaseSelect--;

		for (int nCntSelect = g_aPhaseSelect[g_nPhaseSelect][0]; nCntSelect < g_aPhaseSelect[g_nPhaseSelect][1]; nCntSelect++)
		{
			g_aSelect[nCntSelect].bShow = true;
			g_aSelect[nCntSelect].bSelect = false;
			g_aSelect[nCntSelect].bCanSelect = true;
		}

		g_nSelect = g_aPhaseSelect[g_nPhaseSelect][0];
	}
}

//==================================================================================
// --- フェーズ変更 ---
//==================================================================================
void ToNextPhase(void)
{
	for (int nCntSelect = g_aPhaseSelect[g_nPhaseSelect][0]; nCntSelect < g_aPhaseSelect[g_nPhaseSelect][1]; nCntSelect++)
	{
		g_aSelect[nCntSelect].bSelect = false;
		g_aSelect[nCntSelect].bCanSelect = false;
		g_aSelect[g_nSelect].col = D3DXCOLOR(1, 1, 1, g_aSelect[g_nSelect].col.a);
	}

	g_nPhaseSelect++;

	for (int nCntSelect = g_aPhaseSelect[g_nPhaseSelect][0]; nCntSelect < g_aPhaseSelect[g_nPhaseSelect][1]; nCntSelect++)
	{
		g_aSelect[nCntSelect].bShow = true;
		g_aSelect[nCntSelect].bSelect = false;
		g_aSelect[nCntSelect].bCanSelect = true;
	}

	g_nSelect = g_aPhaseSelect[g_nPhaseSelect][0];
}

//==================================================================================
// --- 設定画面へ移行 ---
//==================================================================================
void ToSetting(void)
{

}

//==================================================================================
// --- ゲームを抜ける ---
//==================================================================================
void ToQuitGame(void)
{
	DestroyWindow(GetHandleWindow());
}

//==================================================================================
// --- 選択肢変更 ---
//==================================================================================
void ChangeSelect(void)
{
	if (GetKeyboardRepeat(DIK_W))
	{
		g_aSelect[g_nSelect].bSelect = false;
		g_aSelect[g_nSelect].col = D3DXCOLOR(1, 1, 1, g_aSelect[g_nSelect].col.a);

		g_nSelect--;
		if (g_aPhaseSelect[g_nPhaseSelect][0] > g_nSelect)
		{
			g_nSelect = g_aPhaseSelect[g_nPhaseSelect][1] - 1;
		}

		g_aSelect[g_nSelect].bSelect = true;
		g_aSelect[g_nSelect].col = D3DXCOLOR(0, 1, 0, g_aSelect[g_nSelect].col.a);
	}
	else if (GetKeyboardRepeat(DIK_S))
	{
		g_aSelect[g_nSelect].bSelect = false;
		g_aSelect[g_nSelect].col = D3DXCOLOR(1, 1, 1, g_aSelect[g_nSelect].col.a);

		g_nSelect++;
		if (g_aPhaseSelect[g_nPhaseSelect][1] <= g_nSelect)
		{
			g_nSelect = g_aPhaseSelect[g_nPhaseSelect][0];
		}

		g_aSelect[g_nSelect].bSelect = true;
		g_aSelect[g_nSelect].col = D3DXCOLOR(0, 1, 0, g_aSelect[g_nSelect].col.a);
	}
}

//==================================================================================
// --- ゲーム開始時のプレイ人数を取得 ---
//==================================================================================
int GetPlayerNum(void)
{
	return g_nPlayerNum;
}