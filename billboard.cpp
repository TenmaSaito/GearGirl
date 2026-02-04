//================================================================================================================
//
// DirectXのデバッグ表示のcppファイル [billboard.cpp]
// Author : KOMATSU SHOTA
//
//================================================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include "billboard.h"
#include "input.h"

//*************************************************************************************************
//*** マクロ定義 ***
//*************************************************************************************************
#define MAX_TEXTURE (3)	// テクスチャの最大数

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
//LPDIRECT3DTEXTURE9 g_pTextureBillboard = NULL;			// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffBillboard = NULL;		// 頂点バッファへのポインタ
D3DXVECTOR3 g_posBillboard;
D3DXMATRIX g_mtxWorldBillboard;
LPDIRECT3DTEXTURE9 g_pTextures[MAX_TEXTURE];

BILLBOARD g_billboard;

//================================================================================================================
// --- ビルボードの初期化処理 ---
//================================================================================================================
void InitBillboard(void)
{
	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice, "data\\TEXTURE\\000.png", &g_pTextures[0]);	// テクスチャ1
	D3DXCreateTextureFromFile(pDevice, "data\\TEXTURE\\001.png", &g_pTextures[1]);	// テクスチャ2
	D3DXCreateTextureFromFile(pDevice, "data\\TEXTURE\\002.png", &g_pTextures[2]);	// テクスチャ3

	// 頂点バッファの読み込み
	pDevice->CreateVertexBuffer({ sizeof(VERTEX_3D) * 4 },
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffBillboard,
		NULL);

	VERTEX_3D* pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックして、頂点情報へのポインタを取得
	g_pVtxBuffBillboard->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-25.0f, 100.0f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(25.0f, 100.0f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-25.0f, 0.0f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(25.0f, 0.0f, 0.0f);

	// 法線ベクトルの設定
	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	// 頂点カラー
	pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[1].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[2].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファをアンロックする
	g_pVtxBuffBillboard->Unlock();
}

//================================================================================================================
// --- ビルボードの終了処理 ---
//================================================================================================================
void UninitBillboard(void)
{
	int nCntTexture;

	// テクスチャの破棄
	for (nCntTexture = 0; nCntTexture < MAX_TEXTURE; nCntTexture++)
	{
		if (g_pTextures[nCntTexture] != NULL)
		{
			g_pTextures[nCntTexture]->Release();
			g_pTextures[nCntTexture] = NULL;
		}
	}

	//// テクスチャの破棄
	//if (g_pTextureBillboard != NULL)
	//{
	//	g_pTextureBillboard->Release();
	//	g_pTextureBillboard = NULL;
	//}

	// バッファの破棄
	if (g_pVtxBuffBillboard != NULL)
	{
		g_pVtxBuffBillboard->Release();
		g_pVtxBuffBillboard = NULL;
	}
}

//================================================================================================================
// --- ビルボードの更新処理 ---
//================================================================================================================
void UpdateBillboard(void)
{
	
}

//================================================================================================================
// --- ビルボードの描画処理 ---
//================================================================================================================
void DrawBillboard(void)
{
	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// 計算用マトリックス
	D3DXMATRIX mtxRot, mtxTrans;

	// ビューマトリックス取得用
	D3DXMATRIX mtxView;

	// Zテストを無効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&g_mtxWorldBillboard);

	// ビューマトリックスを取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	// ポリゴンをカメラに対して正面に向ける
	D3DXMatrixInverse(&g_mtxWorldBillboard, NULL, &mtxView);
	g_mtxWorldBillboard._41 = 0.0f;
	g_mtxWorldBillboard._42 = 0.0f;
	g_mtxWorldBillboard._43 = 0.0f;

	// 向きを反映
	/*D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotBillboard.y, g_rotBillboard.x, g_rotBillboard.z);*/
	/*D3DXMatrixMultiply(&g_mtxWorldBillboard, &g_mtxWorldBillboard, &mtxRot);*/

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, g_posBillboard.x, g_posBillboard.y, g_posBillboard.z);
	D3DXMatrixMultiply(&g_mtxWorldBillboard, &g_mtxWorldBillboard, &mtxTrans);

	// ビルボードのテクスチャIDを使ってテクスチャを設定
	if (g_billboard.texID >= 0 && g_billboard.texID < MAX_TEXTURE)
	{
		pDevice->SetTexture(0, g_pTextures[g_billboard.texID]);
	}

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldBillboard);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffBillboard, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定............................................
	pDevice->SetFVF(FVF_VERTEX_3D);

	//// テクスチャの設定
	//pDevice->SetTexture(0, g_pTextureBillboard);

	// ポリゴン描写
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// Zテストを有効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

}

void SetBillboard(D3DXVECTOR3 pos, int texID)
{
	g_posBillboard = pos;
	g_billboard.texID = texID;
}