//=========================================================
// 
// "UIアーム処理"	[UIarm.cpp]
// Author : KikuchiMina
// 
//=========================================================
#include "main.h"
#include "input.h"
#include "UIarm.h"

//=========================================================
// マクロ定義
//=========================================================
#define UI_ARM_SIZE			100			// UIの縦サイズ
#define UI_DRAW_START_X		1110.0f		// UIの描画始めX
#define UI_SPACE			200.0f		// UIのX間の距離
#define UI_TEX_SIZE			0.5f		// テクスチャのサイズ

//=========================================================
// グローバル変数
//=========================================================
LPDIRECT3DTEXTURE9		g_pTextureUIarm = {};		// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9	g_pVtxBuffUIarm = NULL;		// 頂点バッファへのポインタ
D3DXVECTOR3				g_UIarmPos;					// UIアームの位置
D3DXVECTOR3				g_UIarmMove;				// UIアームの移動量
int g_nDirectionPlayer;								// プレイ方法　// 0:1人 * 1:2人

//=========================================================
// UIアームの初期化処理
//=========================================================
void InitUIarm(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();		// デバイスへのポインタの取得
	VERTEX_2D* pVtx;								// 頂点情報へのポインタ

	//-------------------------------------
	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice, "data\\TEXTURE\\TapToStart.png", &g_pTextureUIarm);

	//-------------------------------------
	// 頂点バッファの作成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
								D3DUSAGE_WRITEONLY,
								FVF_VERTEX_2D,
								D3DPOOL_MANAGED,
								&g_pVtxBuffUIarm,
								NULL);

	//-------------------------------------
	// 初期化
	g_UIarmPos = D3DXVECTOR3(UI_DRAW_START_X, 570.0f, 0.0f);
	g_UIarmMove = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_nDirectionPlayer = 0;
	
	//-------------------------------------
	// 頂点バッファをロックし、頂点情報へのポインタを獲得
	g_pVtxBuffUIarm->Lock(0, 0, (void**)&pVtx, 0);

	//位置の設定
	pVtx[0].pos.x = g_UIarmPos.x;
	pVtx[1].pos.x = g_UIarmPos.x + UI_ARM_SIZE;
	pVtx[2].pos.x = g_UIarmPos.x;
	pVtx[3].pos.x = g_UIarmPos.x + UI_ARM_SIZE;
	pVtx[0].pos.y = g_UIarmPos.y;
	pVtx[1].pos.y = g_UIarmPos.y;
	pVtx[2].pos.y = g_UIarmPos.y + UI_ARM_SIZE;
	pVtx[3].pos.y = g_UIarmPos.y + UI_ARM_SIZE;
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
	pVtx[1].tex = D3DXVECTOR2(UI_TEX_SIZE, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(UI_TEX_SIZE, 1.0f);

	//-------------------------------------
	// 頂点バッファをアンロック
	g_pVtxBuffUIarm->Unlock();
}

//=========================================================
// UIアームの終了処理
//=========================================================
void UninitUIarm(void)
{
	//-------------------------------------
	// テクスチャの破棄
	//-------------------------------------
	if (g_pTextureUIarm != NULL)
	{
		g_pTextureUIarm->Release();
		g_pTextureUIarm = NULL;
	}

	//-------------------------------------
	// 頂点バッファの破棄
	//-------------------------------------
	if (g_pVtxBuffUIarm != NULL)
	{
		g_pVtxBuffUIarm->Release();
		g_pVtxBuffUIarm = NULL;
	}
}

//=========================================================
// UIアームの更新処理
//=========================================================
void UpdateUIarm(void)
{
	VERTEX_2D* pVtx;								// 頂点情報へのポインタ

	if (g_nDirectionPlayer == 0)
	{
		g_UIarmPos.x = UI_DRAW_START_X;
	}
	else if (g_nDirectionPlayer == 1)
	{
		g_UIarmPos.x = 500.0f;
	}

	//-------------------------------------
	// 頂点バッファをロックし、頂点情報へのポインタを獲得
	g_pVtxBuffUIarm->Lock(0, 0, (void**)&pVtx, 0);

	if (GetKeyboardTrigger(DIK_I) == true)
	{
		// テクスチャ座標の設定
		pVtx[0].tex.x -= UI_TEX_SIZE;
		pVtx[1].tex.x -= UI_TEX_SIZE;
		pVtx[2].tex.x -= UI_TEX_SIZE;
		pVtx[3].tex.x -= UI_TEX_SIZE;
	}

	if (GetKeyboardTrigger(DIK_O) == true)
	{
		// テクスチャ座標の設定
		pVtx[0].tex.x += UI_TEX_SIZE;
		pVtx[1].tex.x += UI_TEX_SIZE;
		pVtx[2].tex.x += UI_TEX_SIZE;
		pVtx[3].tex.x += UI_TEX_SIZE;
	}

	//-------------------------------------
	// 頂点バッファをアンロック
	g_pVtxBuffUIarm->Unlock();
}

//=========================================================
// UIアームの描画処理
//=========================================================
void DrawUIarm(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ・取得

	//-------------------------------------
	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0,
							g_pVtxBuffUIarm,			// 頂点バッファへのポインタ
							0,
							sizeof(VERTEX_2D));			// 頂点情報構造体のサイズ

	//-------------------------------------
	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, g_pTextureUIarm);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,	// プリミティブの種類
							0,					// 描画する最初の頂点インデックス
							2);					// プリミティブ(ポリゴン)数
}