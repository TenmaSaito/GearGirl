//==================================================================================
//
// DirectXのバッドエンド用のcppファイル [badend.cpp]
// Author : Shu Tanaka
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "badend.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define TEX_BADEND		(1)		// バッドエンドの際に使うテクスチャ数	

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
LPDIRECT3DTEXTURE9	g_pTextureBadend[TEX_BADEND] = {};		// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9	g_pVtxBuffBadend = NULL;	// 頂点バッファへのポインタ

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitBadEnd(void)
{
	/*** デバイスの取得 ***/
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	VERTEX_2D* pVtx;					// 頂点情報へのポインタ

		// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\badend.png",
		&g_pTextureBadend[0]);

	/*** 頂点バッファの生成 ***/
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * TEX_BADEND,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffBadend,
		NULL);

	/*** 頂点バッファの設定 ***/
	g_pVtxBuffBadend->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntBadend = 0; nCntBadend < TEX_BADEND; nCntBadend++)
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
	g_pVtxBuffBadend->Unlock();

	EndDevice();
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitBadEnd(void)
{
	// テクスチャの破棄
	for (int nCntBadend = 0; nCntBadend < TEX_BADEND; nCntBadend++)
	{
		if (g_pTextureBadend[nCntBadend] != NULL)
		{
			g_pTextureBadend[nCntBadend]->Release();
			g_pTextureBadend[nCntBadend] = NULL;
		}
	}

	if (g_pVtxBuffBadend != NULL)
	{
		g_pVtxBuffBadend->Release();
		g_pVtxBuffBadend = NULL;
	}
}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateBadEnd(void)
{

}

//==================================================================================
// --- 描画 ---
//==================================================================================
void DrawBadEnd(void)
{
	LPDIRECT3DDEVICE9 pDevice;		// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffBadend, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntBadend = 0; nCntBadend < TEX_BADEND; nCntBadend++)
	{
		// テクスチャの設定
		pDevice->SetTexture(0, g_pTextureBadend[nCntBadend]);

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0 + (4 * nCntBadend), 2);
	}
}