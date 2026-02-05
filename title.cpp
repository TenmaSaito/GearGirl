//==================================================================================
//
// DirectXのタイトル関連用のcppファイル [title.cpp]
// Author : TENMA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "title.h"
#include "input.h"
#include "fade.h"
#include "2Dpolygon.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define MAX_TITLE			TITLE_MENU_MAX		// タイトルの種類
#define HEIGHT_SIZE_TITLE	50.0f 				// 縦サイズ
#define WIDTH_SIZE_TITLE	300.0f				// 横サイズ
#define LINE_SPACE_TITLE	5.0f				// 行間隔
#define WIDTH_CENTER_LINE	SCREEN_WIDTH / 2	// 横中央線

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************
LPDIRECT3DTEXTURE9		g_apTextureTitle[MAX_TITLE] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9	g_pVtxBuffTitle = NULL;				// 頂点バッファへのポインタ
D3DXVECTOR3				g_posTitle[MAX_TITLE];				// 位置
int nTitleData;
int nCntData = 100;

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
int g_nIdx2DPolygon;		// 設置した2Dポリゴンのインデックス

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitTitle(void)
{
	/*** Aの初期化 ***/

	/*** 2Dポリゴンの初期化 ***/
	Init2DPolygon();

	/*** 2Dポリゴンの設置(例) ***/
	g_nIdx2DPolygon = Set2DPolygon(WINDOW_MID, D3DXVECTOR3_NULL, D3DXVECTOR2(SCREEN_WIDTH, SCREEN_HEIGHT));

	LPDIRECT3DDEVICE9 pDevice = GetDevice();		// デバイスへのポインタの取得
	VERTEX_2D* pVtx;								// 頂点情報へのポインタ

	//-------------------------------------
	// テクスチャの読み込み
	for (int nCnrTex = 0; nCnrTex < MAX_TITLE; nCnrTex++)
	{
		D3DXCreateTextureFromFile(pDevice, c_apFilenameTitle[nCnrTex], &g_apTextureTitle[nCnrTex]);
	}

	//-------------------------------------
	// 頂点バッファの作成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_TITLE,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffTitle,
		NULL);

	//-------------------------------------
	// 計算
	float fData = (TITLE_MENU_OUTGAME * HEIGHT_SIZE_TITLE + (TITLE_MENU_OUTGAME - 1) * LINE_SPACE_TITLE);		// (始める + 設定 + ゲームをやめる) * 高さ + 行間
	float fDrawStart = SCREEN_HEIGHT - fData;					// 画面下から↑これ
	float fDrawStartHalf = SCREEN_HEIGHT - fData / 2;			// 全体の半分
	float fData2 = 2 * HEIGHT_SIZE_TITLE + LINE_SPACE_TITLE;	// 1PLAY + 2PLAY * 高さ + 行間
	float fDataPlayer = fDrawStartHalf - fData2 / 2;

	//-------------------------------------
	// 初期化
	nTitleData = 0;
	for (int nCntTitle = 0; nCntTitle < MAX_TITLE; nCntTitle++)
	{
		g_posTitle[nCntTitle] = D3DXVECTOR3(WIDTH_CENTER_LINE, 0.0f, 0.0f);
	}

	g_posTitle[TITLE_MENU_TAP].y = fDrawStartHalf - HEIGHT_SIZE_TITLE / 2;
	g_posTitle[TITLE_MENU_START].y = 0 * (LINE_SPACE_TITLE + HEIGHT_SIZE_TITLE) + fDrawStart;		// 間隔, 描画始め, サイズ
	g_posTitle[TITLE_MENU_OPTION].y = 1 * (LINE_SPACE_TITLE + HEIGHT_SIZE_TITLE) + fDrawStart;		// 間隔, 描画始め, サイズ
	g_posTitle[TITLE_MENU_OUTGAME].y = 2 * (LINE_SPACE_TITLE + HEIGHT_SIZE_TITLE) + fDrawStart;		// 間隔, 描画始め, サイズ
	g_posTitle[TITLE_MENU_ONEPLAY].y = 0 * (LINE_SPACE_TITLE + HEIGHT_SIZE_TITLE) + fDataPlayer;	// 間隔, 描画始め, サイズ
	g_posTitle[TITLE_MENU_TWOPLAY].y = 1 * (LINE_SPACE_TITLE + HEIGHT_SIZE_TITLE) + fDataPlayer;	// 間隔, 描画始め, サイズ

	//-------------------------------------
	// 頂点バッファをロックし、頂点情報へのポインタを獲得
	g_pVtxBuffTitle->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntTitle = 0; nCntTitle < MAX_TITLE; nCntTitle++)
	{
		//位置の設定
		pVtx[0].pos.x = g_posTitle[nCntTitle].x - WIDTH_SIZE_TITLE * 0.5f;
		pVtx[1].pos.x = g_posTitle[nCntTitle].x + WIDTH_SIZE_TITLE * 0.5f;
		pVtx[2].pos.x = g_posTitle[nCntTitle].x - WIDTH_SIZE_TITLE * 0.5f;
		pVtx[3].pos.x = g_posTitle[nCntTitle].x + WIDTH_SIZE_TITLE * 0.5f;
		pVtx[0].pos.y = g_posTitle[nCntTitle].y;
		pVtx[1].pos.y = g_posTitle[nCntTitle].y;
		pVtx[2].pos.y = g_posTitle[nCntTitle].y + HEIGHT_SIZE_TITLE;
		pVtx[3].pos.y = g_posTitle[nCntTitle].y + HEIGHT_SIZE_TITLE;
		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.z = 0.0f;

		// rhwの設定
		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

		// 頂点カラーの設定
		pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);
		pVtx[1].col = D3DCOLOR_RGBA(255, 255, 255, 255);
		pVtx[2].col = D3DCOLOR_RGBA(255, 255, 255, 255);
		pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);
		pVtx += 4;
	}

	//-------------------------------------
	// 頂点バッファをアンロック
	g_pVtxBuffTitle->Unlock();
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitTitle(void)
{
	/*** Aの終了 ***/

	/*** 2Dポリゴンの終了 ***/
	Uninit2DPolygon();

	//-------------------------------------
	// テクスチャの破棄
	//-------------------------------------
	for (int nCnrTex = 0; nCnrTex < MAX_TITLE; nCnrTex++)
	{
		if (g_apTextureTitle[nCnrTex] != NULL)
		{
			g_apTextureTitle[nCnrTex]->Release();
			g_apTextureTitle[nCnrTex] = NULL;
		}
	}

	//-------------------------------------
	// 頂点バッファの破棄
	//-------------------------------------
	if (g_pVtxBuffTitle != NULL)
	{
		g_pVtxBuffTitle->Release();
		g_pVtxBuffTitle = NULL;
	}
}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateTitle(void)
{
	VERTEX_2D* pVtx;			// 頂点情報へのポインタ
	FADE pFade = GetFade();		// フェード情報の取得
	nCntData++;

	//-------------------------------------
	// 頂点バッファをロックし、頂点情報へのポインタを獲得
	g_pVtxBuffTitle->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標を進める
	pVtx += TITLE_MENU_TAP * 4;
	pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[1].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[2].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);

	if (GetJoypadPress(0, JOYKEY_START) == true
		|| GetJoypadPress(1, JOYKEY_START) == true
		|| GetKeyboardPress(DIK_RETURN) == true)
	{
		nCntData = 0;
	}
	if (nCntData < 60)
	{
		//位置の設定
		pVtx[0].pos.x -= 5.0f;
		pVtx[1].pos.x -= 5.0f;
		pVtx[2].pos.x -= 5.0f;
		pVtx[3].pos.x -= 5.0f;

		if (pVtx[0].pos.x < WIDTH_CENTER_LINE - 300.0f)
		{
			pVtx[0].pos.x = WIDTH_CENTER_LINE - 300.0f;
			pVtx[2].pos.x = WIDTH_CENTER_LINE - 300.0f;

			//テクスチャ座標の設定
			pVtx[0].tex = D3DXVECTOR2(1.0f - (pVtx[1].pos.x - pVtx[0].pos.x) / WIDTH_SIZE_TITLE, 0.0f);
			pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
			pVtx[2].tex = D3DXVECTOR2(1.0f - (pVtx[1].pos.x - pVtx[0].pos.x) / WIDTH_SIZE_TITLE, 1.0f);
			pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);
			//pVtx[1].tex = D3DXVECTOR2((pVtx[1].pos.x - pVtx[0].pos.x) / WIDTH_SIZE_TITLE, 0.0f);
		}
	}

	//頂点バッファをアンロック
	g_pVtxBuffTitle->Unlock();

	// モード変更
	if (GetKeyboardTrigger(DIK_RETURN)
		|| GetJoypadTrigger(0, JOYKEY_A)
		|| GetJoypadTrigger(0, JOYKEY_START))
	{
		if (GetFade() == FADE_NONE)
		{
			SetFade(MODE_GAME);
		}
	}

	/*** Aの更新 ***/

	/*** 2Dポリゴンの更新 ***/
	Update2DPolygon();
}

//==================================================================================
// --- 描画 ---
//==================================================================================
void DrawTitle(void)
{
	// VERTEX_3D ============================================
	/*** Aの描画 ***/

	// VERTEX_2D ============================================
	/*** Aの描画 ***/

	/*** 2Dポリゴンの描画 ***/
	//Draw2DPolygon();

	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ・取得

	//-------------------------------------
	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0,
		g_pVtxBuffTitle,			// 頂点バッファへのポインタ
		0,
		sizeof(VERTEX_2D));			// 頂点情報構造体のサイズ

	//-------------------------------------
	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntTitle = 0; nCntTitle < MAX_TITLE; nCntTitle++)
	{
		// テクスチャの設定
		pDevice->SetTexture(0, g_apTextureTitle[nCntTitle]);

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,		// プリミティブの種類
			nCntTitle * 4,			// 描画する最初の頂点インデックス
			2);						// プリミティブ(ポリゴン)数
	}
}