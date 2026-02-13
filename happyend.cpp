//==================================================================================
//
// DirectXのハッピーエンド用のcppファイル [happyend.cpp]
// Author : Shu Tanaka
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "happyend.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define TEX_HAPPYEND		(1)		// ハッピーエンドの際に使うテクスチャ数	

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
LPDIRECT3DTEXTURE9	g_pTextureHappyend[TEX_HAPPYEND] = {};		// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9	g_pVtxBuffHappyend = NULL;	// 頂点バッファへのポインタ

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitHappyEnd(void)
{
	/*** デバイスの取得 ***/
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	VERTEX_2D* pVtx;					// 頂点情報へのポインタ

		// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\happyend.png",
		&g_pTextureHappyend[0]);

	/*** 頂点バッファの生成 ***/
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * TEX_HAPPYEND,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffHappyend,
		NULL);

	/*** 頂点バッファの設定 ***/
	g_pVtxBuffHappyend->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntHappyend = 0; nCntHappyend < TEX_HAPPYEND; nCntHappyend++)
	{
		/*** 頂点座標の設定の設定 ***/
		pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(SCREEN_WIDTH, 0.0f, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(0.0f, SCREEN_HEIGHT, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);

		/*** rhwの設定 ***/
		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

		/*** 頂点カラー設定 ***/
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		/*** テクスチャ座標の設定 ***/
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += 4;
	}

	/*** 頂点バッファの設定を終了 ***/
	g_pVtxBuffHappyend->Unlock();

	// デバイスの破棄
	EndDevice();
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitHappyEnd(void)
{
	// テクスチャの破棄
	for (int nCntHappyend = 0; nCntHappyend < TEX_HAPPYEND; nCntHappyend++)
	{
		if (g_pTextureHappyend[nCntHappyend] != NULL)
		{
			g_pTextureHappyend[nCntHappyend]->Release();
			g_pTextureHappyend[nCntHappyend] = NULL;
		}
	}

	if (g_pVtxBuffHappyend != NULL)
	{
		g_pVtxBuffHappyend->Release();
		g_pVtxBuffHappyend = NULL;
	}
}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateHappyEnd(void)
{

}

//==================================================================================
// --- 描画 ---
//==================================================================================
void DrawHappyEnd(void)
{
	LPDIRECT3DDEVICE9 pDevice;		// デバイスへのポインタ

// デバイスの取得
	pDevice = GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffHappyend, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntHappyend = 0; nCntHappyend < TEX_HAPPYEND; nCntHappyend++)
	{
		// テクスチャの設定
		pDevice->SetTexture(0, g_pTextureHappyend[nCntHappyend]);

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0 + (4 * nCntHappyend), 2);
	}

	// デバイスの破棄
	EndDevice();
}