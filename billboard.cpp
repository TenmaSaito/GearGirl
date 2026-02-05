//================================================================================================================
//
// DirectXのビルボード表示処理 [billboard.cpp]
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
#define MAX_TEXTURE (3)				// テクスチャの最大数
#define MAX_BILLBOARD	(256)		// ビルボードの最大数

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
//LPDIRECT3DTEXTURE9 g_pTextureBillboard = NULL;		// テクスチャへのポインタ
LPDIRECT3DTEXTURE9 g_apTexture[MAX_TEXTURE];			// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffBillboard = NULL;		// 頂点バッファへのポインタ

BILLBOARD g_aBillboard[MAX_BILLBOARD];

//================================================================================================================
// --- ビルボードの初期化処理 ---
//================================================================================================================
void InitBillboard(void)
{
	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	int nCntBillboard;

	// デバイスの取得
	pDevice = GetDevice();

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice, "data\\TEXTURE\\000.png", &g_apTexture[0]);	// テクスチャ1
	D3DXCreateTextureFromFile(pDevice, "data\\TEXTURE\\001.png", &g_apTexture[1]);	// テクスチャ2
	D3DXCreateTextureFromFile(pDevice, "data\\TEXTURE\\002.png", &g_apTexture[2]);	// テクスチャ3

	// ビルボードの情報の初期化
	for (nCntBillboard = 0; nCntBillboard < MAX_BILLBOARD; nCntBillboard++)
	{
		g_aBillboard[nCntBillboard].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aBillboard[nCntBillboard].fWidth = 100;
		g_aBillboard[nCntBillboard].fHeight = 100;
		g_aBillboard[nCntBillboard].texID = 0;
		g_aBillboard[nCntBillboard].bUse = false;
	}

	// 頂点バッファの読み込み
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4 * MAX_BILLBOARD,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffBillboard,
		NULL);

	VERTEX_3D* pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックして、頂点情報へのポインタを取得
	g_pVtxBuffBillboard->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntBillboard = 0; nCntBillboard < MAX_BILLBOARD; nCntBillboard++)
	{
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

		pVtx += 4;
	}

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
		if (g_apTexture[nCntTexture] != NULL)
		{
			g_apTexture[nCntTexture]->Release();
			g_apTexture[nCntTexture] = NULL;
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

	for (int nCntBillboard = 0; nCntBillboard < MAX_BILLBOARD; nCntBillboard++)
	{
		if (g_aBillboard[nCntBillboard].bUse == true)
		{// ビルボードが使用されている

			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aBillboard[nCntBillboard].mtxWorld);

			// ビューマトリックスを取得
			pDevice->GetTransform(D3DTS_VIEW, &mtxView);

			// ポリゴンをカメラに対して正面に向ける
			D3DXMatrixInverse(&g_aBillboard[nCntBillboard].mtxWorld, NULL, &mtxView);
			g_aBillboard[nCntBillboard].mtxWorld._41 = 0.0f;
			g_aBillboard[nCntBillboard].mtxWorld._42 = 0.0f;
			g_aBillboard[nCntBillboard].mtxWorld._43 = 0.0f;

			// 向きを反映
			/*D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotBillboard.y, g_rotBillboard.x, g_rotBillboard.z);*/
			/*D3DXMatrixMultiply(&g_mtxWorldBillboard, &g_mtxWorldBillboard, &mtxRot);*/

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aBillboard[nCntBillboard].pos.x, g_aBillboard[nCntBillboard].pos.y, g_aBillboard[nCntBillboard].pos.z);
			D3DXMatrixMultiply(&g_aBillboard[nCntBillboard].mtxWorld, &g_aBillboard[nCntBillboard].mtxWorld, &mtxTrans);

			// ビルボードのテクスチャIDを使ってテクスチャを設定
			if (g_aBillboard[nCntBillboard].texID >= 0 && g_aBillboard[nCntBillboard].texID < MAX_TEXTURE)
			{
				pDevice->SetTexture(0, g_apTexture[g_aBillboard[nCntBillboard].texID]);
			}

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aBillboard[nCntBillboard].mtxWorld);

			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_pVtxBuffBillboard, 0, sizeof(VERTEX_3D));

			// 頂点フォーマットの設定............................................
			pDevice->SetFVF(FVF_VERTEX_3D);

			//// テクスチャの設定RIANGLESTRIP, 0, 2);

			/*** ポリゴンの描画 ***/
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,		// プリミティブの種類
				4 * nCntBillboard,							// 描画する最初の頂点インデックス
				2);											// 描画するプリミティブの数
		}
	}

	// Zテストを有効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

//================================================================================================================
// --- ビルボードの設定処理 ---
//================================================================================================================
void SetBillboard(D3DXVECTOR3 pos, float fWidth, float fHeight,int texID)
{
	int nCntBillboard;

	VERTEX_2D* pVtx;

	// 頂点バッファをロックして、頂点情報へのポインタを取得
	g_pVtxBuffBillboard->Lock(0, 0, (void**)&pVtx, 0);


	for (nCntBillboard = 0; nCntBillboard < MAX_BILLBOARD; nCntBillboard++)
	{
		if (g_aBillboard[nCntBillboard].bUse == false)
		{
			g_aBillboard[nCntBillboard].pos = pos;
			g_aBillboard[nCntBillboard].texID = texID;
			g_aBillboard[nCntBillboard].fWidth = fWidth;
			g_aBillboard[nCntBillboard].fHeight = fHeight;

			// 頂点座標の設定
			pVtx[0].pos = D3DXVECTOR3(g_aBillboard[nCntBillboard].pos.x, g_aBillboard[nCntBillboard].pos.y, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(g_aBillboard[nCntBillboard].pos.x + fWidth, g_aBillboard[nCntBillboard].pos.y, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(g_aBillboard[nCntBillboard].pos.x, g_aBillboard[nCntBillboard].pos.y + fHeight, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(g_aBillboard[nCntBillboard].pos.x + fWidth, g_aBillboard[nCntBillboard].pos.y + fHeight, 0.0f);

			g_aBillboard[nCntBillboard].bUse = true;	// 使用している状態にする
			break;	// ここでfor文を抜ける
		}
		pVtx += 4;	// 頂点データ
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffBillboard->Unlock();
}