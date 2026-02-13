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

//**********************************************************************************
//*** 選択肢構造体の定義 ***
//**********************************************************************************
typedef struct
{
	LPDIRECT3DTEXTURE9		pTexture;	// テクスチャへのポインタ
	LPDIRECT3DVERTEXBUFFER9	pVtxBuff;	// 頂点バッファへのポインタ
	D3DXVECTOR3 pos;		// 中心位置
	D3DXCOLOR col;			// 色
	bool bCanSelect;		// 選択出来るか
	bool bSelect;			// 選択されているか
	bool bShow;				// 描画されているか
} TSelect;

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************
void CreateSelectBuffer(LPDIRECT3DDEVICE9 pDevice, TSelect *pOut, TITLE_MENU type);

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
int g_nPhaseSelect;					// 現在の選択フェーズ

const D3DXVECTOR3 g_aPosSelect[TITLE_MENU_MAX] =		// 各選択肢の初期位置
{
	D3DXVECTOR3(920, 0, 0),
	D3DXVECTOR3(920, 0, 0),
	D3DXVECTOR3(920, 0, 0),
	D3DXVECTOR3(920, 0, 0),
	D3DXVECTOR3(920, 0, 0),
	D3DXVECTOR3(920, 0, 0),
};

const D3DXVECTOR3 g_aPosSelectInvisible[TITLE_MENU_MAX] =		// 各選択肢の見えなくなる位置
{
	D3DXVECTOR3(0, 0, 0),
	D3DXVECTOR3(0, 0, 0),
	D3DXVECTOR3(0, 0, 0),
	D3DXVECTOR3(0, 0, 0),
	D3DXVECTOR3(0, 0, 0),
	D3DXVECTOR3(0, 0, 0),
};

const int g_aPhaseSelect[PHASE_NUM][2] =				// 各選択フェーズにおける選択可能な範囲
{
	{TITLE_MENU_TAP, TITLE_MENU_TAP},
	{TITLE_MENU_START, TITLE_MENU_OUTGAME},
	{TITLE_MENU_ONEPLAY, TITLE_MENU_TWOPLAY},
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
	SetDefaultColor(pVtx);
	SetPolygonRHW(pVtx);
	SetDefaultTexture(pVtx);

	pOut->pVtxBuff->Unlock();
}