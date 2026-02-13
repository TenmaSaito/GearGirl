//==================================================================================
//
// DirectXのノーマルエンド用のcppファイル [normalend.cpp]
// Author : Shu Tanaka
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "normalend.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define TEX_NORMALEND		(1)		// バッドエンドの際に使うテクスチャ数	

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
LPDIRECT3DTEXTURE9	g_pTextureNormalend[TEX_NORMALEND] = {};		// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9	g_pVtxBuffNormalend = NULL;	// 頂点バッファへのポインタ

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitNormalEnd(void)
{
	/*** デバイスの取得 ***/
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	VERTEX_2D* pVtx;					// 頂点情報へのポインタ

		// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\normalend.png",
		&g_pTextureNormalend[0]);

	/*** 頂点バッファの生成 ***/
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * TEX_NORMALEND,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffNormalend,
		NULL);

	/*** 頂点バッファの設定 ***/
	g_pVtxBuffNormalend->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntNormalend = 0; nCntNormalend < TEX_NORMALEND; nCntNormalend++)
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
	g_pVtxBuffNormalend->Unlock();

	// デバイスの破棄
	EndDevice();
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitNormalEnd(void)
{
	// テクスチャの破棄
	for (int nCntNormalend = 0; nCntNormalend < TEX_NORMALEND; nCntNormalend++)
	{
		if (g_pTextureNormalend[nCntNormalend] != NULL)
		{
			g_pTextureNormalend[nCntNormalend]->Release();
			g_pTextureNormalend[nCntNormalend] = NULL;
		}
	}

	if (g_pVtxBuffNormalend != NULL)
	{
		g_pVtxBuffNormalend->Release();
		g_pVtxBuffNormalend = NULL;
	}
}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateNormalEnd(void)
{

}

//==================================================================================
// --- 描画 ---
//==================================================================================
void DrawNormalEnd(void)
{
	LPDIRECT3DDEVICE9 pDevice;		// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffNormalend, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntNormalend = 0; nCntNormalend < TEX_NORMALEND; nCntNormalend++)
	{
		// テクスチャの設定
		pDevice->SetTexture(0, g_pTextureNormalend[nCntNormalend]);

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0 + (4 * nCntNormalend), 2);
	}
	// デバイスの破棄
	EndDevice();
}