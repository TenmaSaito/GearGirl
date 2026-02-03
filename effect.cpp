//================================================================================================================
//
// DirectXのフェード処理 [effect.cpp]
// Author : KOMATSU SHOTA
//
//================================================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include"main.h"
#include"player.h"
#include"effect.h"

//*************************************************************************************************
//*** マクロ定義 ***
//*************************************************************************************************
#define	MAX_EFFECT	(4096)	// エフェクトの最大数

//*************************************************************************************************
//*** エフェクト構造体の定義 ***
//*************************************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;	// 位置
	D3DXVECTOR3 vec;	// 速度
	D3DXCOLOR col;		// 色
	int nLife;			// 寿命
	bool bUse;			// 使用しているかどうか
	float Width;		// 横
	float Height;		// 縦

	D3DXMATRIX mtxWorld; // ワールドマトリックス
}Effect;

//*************************************************************************************************
//*** グローバル変数 ***
//*************************************************************************************************
LPDIRECT3DTEXTURE9 g_pTextureEffect = NULL;	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffEffect = NULL;	// 頂点バッファへのポインタ
Effect g_aEffect[MAX_EFFECT];	// エフェクトの情報

//================================================================================================================
// --- エフェクトの初期化処理 ---
//================================================================================================================
void InitEffect(void)
{
	int nCntEffect;

	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice, "DATA\\TEXTURE\\effect000.jpg", &g_pTextureEffect);

	// 弾の情報の初期化
	for (nCntEffect = 0; nCntEffect < MAX_EFFECT; nCntEffect++)
	{
		g_aEffect[nCntEffect].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aEffect[nCntEffect].col = D3DXCOLOR(1,1,1,1);	// 白
		g_aEffect[nCntEffect].vec = D3DXVECTOR3(0, 0, 0);	// 黄色
		g_aEffect[nCntEffect].nLife = 100;
		g_aEffect[nCntEffect].bUse = false;	// 使用していない状態にする
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4 * MAX_EFFECT,
								D3DUSAGE_WRITEONLY,
								FVF_VERTEX_3D,
								D3DPOOL_MANAGED,
								&g_pVtxBuffEffect,
								NULL);

	VERTEX_3D* pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックして、頂点情報へのポインタを取得
	g_pVtxBuffEffect->Lock(0, 0, (void**)&pVtx, 0);

	for (nCntEffect = 0; nCntEffect < MAX_EFFECT; nCntEffect++)
	{
		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(-50.0f, 50.0f, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(50.0f, 50.0f, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-50.0f, -50.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(50.0f, -50.0f, 0.0f);

		// rhwの設定
		pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

		// 頂点カラー
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += 4;	// 頂点データ
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffEffect->Unlock();
}

//================================================================================================================
// --- エフェクトの終了処理 ---
//================================================================================================================
void UninitEffect(void)
{
	// テクスチャの破棄
	if (g_pTextureEffect != NULL)
	{
		g_pTextureEffect->Release();
		g_pTextureEffect = NULL;
	}

	// バッファの破棄
	if (g_pVtxBuffEffect != NULL)
	{
		g_pVtxBuffEffect->Release();
		g_pVtxBuffEffect = NULL;
	}
}

//================================================================================================================
// --- エフェクトの更新処理 ---
//================================================================================================================
void UpdateEffect(void)
{
	int nCntEffect;
	VERTEX_3D* pVtx;

	// 頂点バッファをロックして、頂点情報へのポインタを取得
	g_pVtxBuffEffect->Lock(0, 0, (void**)&pVtx, 0);

	for (nCntEffect = 0; nCntEffect < MAX_EFFECT; nCntEffect++)
	{
		if (g_aEffect[nCntEffect].bUse == true)
		{// 弾が使用されている

			 //弾の位置更新
			g_aEffect[nCntEffect].pos.x += g_aEffect[nCntEffect].vec.x;
			g_aEffect[nCntEffect].pos.y += g_aEffect[nCntEffect].vec.y;
			g_aEffect[nCntEffect].pos.z += g_aEffect[nCntEffect].vec.z;

			// 頂点座標の設定
			pVtx[0].pos = D3DXVECTOR3(-g_aEffect[nCntEffect].Width * 0.5f, g_aEffect[nCntEffect].Height * 0.5f, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(g_aEffect[nCntEffect].Width * 0.5f, g_aEffect[nCntEffect].Height * 0.5f, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(-g_aEffect[nCntEffect].Width * 0.5f, -g_aEffect[nCntEffect].Height, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(g_aEffect[nCntEffect].Width * 0.5f, -g_aEffect[nCntEffect].Height * 0.5f, 0.0f);

			 //寿命のカウントダウン
			if (g_aEffect[nCntEffect].nLife < 0)
			{
				 //使用していない状態にする
				g_aEffect[nCntEffect].bUse = false;
			}

			g_aEffect[nCntEffect].Width	-=0.01f;
			g_aEffect[nCntEffect].Height -= 0.01f;
		}

		pVtx += 4;	// 頂点データ
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffEffect->Unlock();
}

//================================================================================================================
// --- エフェクトの描画処理 ---
//================================================================================================================
void DrawEffect(void)
{
	int nCntEffect;

	D3DXMATRIX mtxTrans;		// 計算用マトリックス
	D3DXMATRIX mtxView;			// ビューマトリックスの取得用

	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffEffect, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定............................................
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, g_pTextureEffect);

	// aブレンディングを加算合計に設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);

	// Zテストを設定
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);

	for(nCntEffect = 0; nCntEffect < MAX_EFFECT; nCntEffect++)
	{
		if (g_aEffect[nCntEffect].bUse == true)
		{// 弾が使用されている

			/*** ワールドマトリックスの初期化 ***/
			D3DXMatrixIdentity(&g_aEffect[nCntEffect].mtxWorld);

			/*** カメラのビューマトリックスを取得 ***/
			pDevice->GetTransform(D3DTS_VIEW, &mtxView);

			/*** マトリックスの逆行列を求める (※ 位置を反映する前に必ず行うこと！) ***/
			D3DXMatrixInverse(&g_aEffect[nCntEffect].mtxWorld, NULL, &mtxView);

			/** 逆行列によって入ってしまった位置情報を初期化 **/
			g_aEffect[nCntEffect].mtxWorld._41 = 0.0f;
			g_aEffect[nCntEffect].mtxWorld._42 = 0.0f;
			g_aEffect[nCntEffect].mtxWorld._43 = 0.0f;

			/*** 位置を反映 (※ 向きを反映したのちに行うこと！) ***/
			D3DXMatrixTranslation(&mtxTrans,
				g_aEffect[nCntEffect].pos.x,
				g_aEffect[nCntEffect].pos.y,
				g_aEffect[nCntEffect].pos.z);

			D3DXMatrixMultiply(&g_aEffect[nCntEffect].mtxWorld, &g_aEffect[nCntEffect].mtxWorld, &mtxTrans);

			/*** ワールドマトリックスの設定 ***/
			pDevice->SetTransform(D3DTS_WORLD, &g_aEffect[nCntEffect].mtxWorld);

			// ポリゴン描写
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntEffect*4, 2);
		}
	}

	// aブレンディングを元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// 
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

}

//================================================================================================================
// --- エフェクトの設定処理 ---
//================================================================================================================
void SetEffect(D3DXVECTOR3 pos, D3DXCOLOR col, D3DXVECTOR3 vec, float Width,float Height,float speed,int nLife)
{
	int nCntEffect;
	VERTEX_3D* pVtx;

	// 頂点バッファをロックして、頂点情報へのポインタを取得
	g_pVtxBuffEffect->Lock(0, 0, (void**)&pVtx, 0);

	for (nCntEffect = 0; nCntEffect < MAX_EFFECT; nCntEffect++)
	{// 弾が使用されていない
		if (g_aEffect[nCntEffect].bUse == false)
		{
			D3DXVECTOR3 vecNor = vec;
			// 移動ベクトルの正規化
			D3DXVec3Normalize(&vecNor, &vecNor);

			// 頂点項目の設定
			g_aEffect[nCntEffect].col = col;
			g_aEffect[nCntEffect].nLife = nLife;
			g_aEffect[nCntEffect].Width = Width;
			g_aEffect[nCntEffect].Height = Height;
			g_aEffect[nCntEffect].vec = vecNor * speed;
			g_aEffect[nCntEffect].bUse = true;	// 使用している状態にする

			g_aEffect[nCntEffect].pos = pos;

			// 頂点座標の設定
			pVtx[0].pos = D3DXVECTOR3(-g_aEffect[nCntEffect].Width * 0.5f, g_aEffect[nCntEffect].Height * 0.5f, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(g_aEffect[nCntEffect].Width * 0.5f, g_aEffect[nCntEffect].Height * 0.5f, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(-g_aEffect[nCntEffect].Width * 0.5f, -g_aEffect[nCntEffect].Height, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(g_aEffect[nCntEffect].Width * 0.5f, -g_aEffect[nCntEffect].Height * 0.5f, 0.0f);

			break;	// ここでfor文を抜ける
		}
		pVtx += 4;	// 頂点データ
	}
	// 頂点バッファをアンロックする
	g_pVtxBuffEffect->Unlock();
}