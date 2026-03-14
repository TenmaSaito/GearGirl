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
#define UI_TEX_SIZE			0.25f		// テクスチャのサイズ

// =================================================
// アームUIの構造体
typedef struct
{
	D3DXVECTOR3				pos;					// UIアームの位置
	D3DXVECTOR3				move;					// UIアームの移動量
}ArmUI;

//=========================================================
// グローバル変数
//=========================================================
LPDIRECT3DTEXTURE9		g_pTextureUIarm = {};		// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9	g_pVtxBuffUIarm = NULL;		// 頂点バッファへのポインタ
int g_nDirectionPlayer;								// プレイ方法　// 0:1人 * 1:2人
ArmUI g_ArmUI;

//=========================================================
// UIアームの初期化処理
//=========================================================
void InitUIarm(void)
{
	/*** デバイスの取得 ***/
	AUTODEVICE9 Auto;							// デバイス自動解放システム
	LPDIRECT3DDEVICE9 pDevice = Auto.pDevice;	// 自動解放システムを介してデバイスを取得
	VERTEX_2D* pVtx;								// 頂点情報へのポインタ

	//-------------------------------------
	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice, "data\\TEXTURE\\armui.png", &g_pTextureUIarm);

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
	g_ArmUI.pos = D3DXVECTOR3(UI_DRAW_START_X, 570.0f, 0.0f);
	g_ArmUI.move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_nDirectionPlayer = 0;
	
	//-------------------------------------
	// 頂点バッファをロックし、頂点情報へのポインタを獲得
	g_pVtxBuffUIarm->Lock(0, 0, (void**)&pVtx, 0);

	//位置の設定
	pVtx[0].pos = D3DXVECTOR3(g_ArmUI.pos.x, g_ArmUI.pos.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(g_ArmUI.pos.x + UI_ARM_SIZE, g_ArmUI.pos.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(g_ArmUI.pos.x, g_ArmUI.pos.y + UI_ARM_SIZE, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(g_ArmUI.pos.x + UI_ARM_SIZE, g_ArmUI.pos.y + UI_ARM_SIZE, 0.0f);

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
		g_ArmUI.pos.x = UI_DRAW_START_X;
	}
	else if (g_nDirectionPlayer == 1)
	{
		g_ArmUI.pos.x = 500.0f;
	}

	//-------------------------------------
	// 頂点バッファをロックし、頂点情報へのポインタを獲得
	g_pVtxBuffUIarm->Lock(0, 0, (void**)&pVtx, 0);

	if (GetKeyboardTrigger(DIK_9) || GetJoypadTrigger(0, JOYKEY_RB))
	{
		// テクスチャ座標の設定
		pVtx[0].tex.x -= UI_TEX_SIZE;
		pVtx[1].tex.x -= UI_TEX_SIZE;
		pVtx[2].tex.x -= UI_TEX_SIZE;
		pVtx[3].tex.x -= UI_TEX_SIZE;
	}
	else if (GetKeyboardTrigger(DIK_0) == true)
	{
		// テクスチャ座標の設定
		pVtx[0].tex.x += UI_TEX_SIZE;
		pVtx[1].tex.x += UI_TEX_SIZE;
		pVtx[2].tex.x += UI_TEX_SIZE;
		pVtx[3].tex.x += UI_TEX_SIZE;
	}

	// === 最初に戻す === //
	if (pVtx[0].tex.x == UI_TEX_SIZE * 3)
	{
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(UI_TEX_SIZE, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(UI_TEX_SIZE, 1.0f);
	}
	else if (pVtx[1].tex.x == 0.0f)
	{
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(UI_TEX_SIZE, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(UI_TEX_SIZE, 1.0f);
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
	/*** デバイスの取得 ***/
	AUTODEVICE9 Auto;							// デバイス自動解放システム
	LPDIRECT3DDEVICE9 pDevice = Auto.pDevice;	// 自動解放システムを介してデバイスを取得

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