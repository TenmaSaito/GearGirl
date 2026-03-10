//==================================================================================
//
// DirectXの棒状エフェクト処理 [lineEffect.cpp]
// Author : KOMATSU SHOTA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include"main.h"
#include"camera.h"
#include"lineEffect.h"

#include "mathUtil.h"

USE_UTIL;

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define	MAX_EFFECT	(1024)	// 棒状エフェクトの最大数

//**********************************************************************************
//*** 棒状エフェクト構造体の定義 ***
//**********************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;	// 位置
	D3DXVECTOR3 rot;	// 角度
	D3DXVECTOR3 vec;	// 速度
	D3DXCOLOR col;		// 色
	D3DXMATRIX mtxWorld; // ワールドマトリックス
	float fSpeed;		// 速度
	int nLife;			// 寿命
	bool bUse;			// 使用しているかどうか
} LineEffect;

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
LPDIRECT3DTEXTURE9 g_pTextureLineEffect = NULL;			// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffLineEffect = NULL;	// 頂点バッファへのポインタ
LineEffect g_aLineEffect[MAX_EFFECT];					// 棒状エフェクトの情報

//**********************************************************************************
//*** 定数変数 ***
//**********************************************************************************
const char *g_LineEffectTexture = "data/TEXTURE/Line_Effect.jpg";	// テクスチャパス
const float g_fDecelerationCoefficient = 0.01f;						// 減速係数

//==================================================================================
// --- 棒状エフェクトの初期化処理 ---
//==================================================================================
void InitLineEffect(void)
{
	int nCntEffect;

	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice, g_LineEffectTexture, &g_pTextureLineEffect);

	// 弾の情報の初期化
	AutoZeroMemory(g_aLineEffect);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4 * MAX_EFFECT,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffLineEffect,
		NULL);

	VERTEX_3D *pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックして、頂点情報へのポインタを取得
	g_pVtxBuffLineEffect->Lock(0, 0, (void**)&pVtx, 0);

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
	g_pVtxBuffLineEffect->Unlock();

	EndDevice();
}

//==================================================================================
// --- 棒状エフェクトの終了処理 ---
//==================================================================================
void UninitLineEffect(void)
{
	// テクスチャの破棄
	RELEASE(g_pTextureLineEffect);

	// バッファの破棄
	RELEASE(g_pVtxBuffLineEffect);
}

//==================================================================================
// --- 棒状エフェクトの更新処理 ---
//==================================================================================
void UpdateLineEffect(void)
{
	LineEffect *pEffect = &g_aLineEffect[0];

	for (int nCntEffect = 0; nCntEffect < MAX_EFFECT; nCntEffect++, pEffect++)
	{
		if (pEffect->bUse == true)
		{// 弾が使用されている
			//弾の位置更新
			pEffect->pos.x += pEffect->vec.x * pEffect->fSpeed;
			pEffect->pos.z += pEffect->vec.z * pEffect->fSpeed;

			// 慣性を掛ける
			pEffect->vec.x += (0.0f - pEffect->vec.x) * g_fDecelerationCoefficient;
			pEffect->vec.z += (0.0f - pEffect->vec.z) * g_fDecelerationCoefficient;

			pEffect->nLife -= 1;

			//寿命のカウントダウン
			if (pEffect->nLife < 0)
			{
				//使用していない状態にする
				pEffect->bUse = false;
			}
		}
	}
}

//==================================================================================
// --- 棒状エフェクトの描画処理 ---
//==================================================================================
void DrawLineEffect(void)
{
	int nCntEffect;

	LineEffect *pEffect = &g_aLineEffect[0];
	D3DXMATRIX mtxRot, mtxTrans;		// 計算用マトリックス
	AUTODEVICE9 AD9;
	LPDIRECT3DDEVICE9 pDevice = AD9.pDevice;	// デバイスへのポインタ

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffLineEffect, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, g_pTextureLineEffect);

	// αブレンディングを加算合計に設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// Zテストを設定
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);

	for (nCntEffect = 0; nCntEffect < MAX_EFFECT; nCntEffect++, pEffect++)
	{
		if (pEffect->bUse == true)
		{// 弾が使用されている
			/*** ワールドマトリックスの初期化 ***/
			D3DXMatrixIdentity(&pEffect->mtxWorld);

			/*** 向きを反映 (※ 位置を反映する前に必ず行うこと！) ***/
			D3DXMatrixRotationYawPitchRoll(&mtxRot,
				pEffect->rot.y,		// Y軸回転
				pEffect->rot.x,		// X軸回転
				pEffect->rot.z);	// Z軸回転

			D3DXMatrixMultiply(&pEffect->mtxWorld, &pEffect->mtxWorld, &mtxRot);

			/*** 位置を反映 (※ 向きを反映したのちに行うこと！) ***/
			D3DXMatrixTranslation(&mtxTrans,
				pEffect->pos.x,
				pEffect->pos.y,
				pEffect->pos.z);

			D3DXMatrixMultiply(&pEffect->mtxWorld, &pEffect->mtxWorld, &mtxTrans);

			/*** ワールドマトリックスの設定 ***/
			pDevice->SetTransform(D3DTS_WORLD, &pEffect->mtxWorld);

			// ポリゴン描写
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntEffect * 4, 2);
		}
	}

	// aブレンディングを元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// Zテストを再設定
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
}

//==================================================================================
// --- 棒状エフェクトの設定処理 ---
//==================================================================================
void SetLineEffect(D3DXVECTOR3 pos, D3DXCOLOR col, UNUSEDROLL D3DXVECTOR3 rot, float Width, float Height, float speed, int nLife)
{
	int nCntEffect;
	VERTEX_3D *pVtx;
	LineEffect *pEffect = &g_aLineEffect[0];

	// 頂点バッファをロックして、頂点情報へのポインタを取得
	g_pVtxBuffLineEffect->Lock(0, 0, (void**)&pVtx, 0);

	for (nCntEffect = 0; nCntEffect < MAX_EFFECT; nCntEffect++, pEffect++)
	{// 弾が使用されていない
		if (pEffect->bUse == false)
		{
			D3DXVECTOR3 vecNor, vec;

			// 角度からベクトルへの変換
			vec.x = pos.x - sinf(rot.y) * sinf(rot.x) * speed;
			vec.y = pos.y - cosf(rot.x) * speed;
			vec.z = pos.z - cosf(rot.y) * sinf(rot.x) * speed;

			// ベクトルの正規化
			D3DXVec3Normalize(&vecNor, &vec);

			// 頂点項目の設定
			pEffect->pos = pos;
			pEffect->rot = CONVERSION_Y(rot, RepairRot(rot.y + D3DX_HALFPI));
			pEffect->col = col;
			pEffect->fSpeed = speed;
			pEffect->nLife = nLife;
			pEffect->bUse = true;	// 使用している状態にする

			// 頂点座標の設定
			pVtx[0].pos = D3DXVECTOR3(-Width * 0.5f, Height * 0.5f, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(Width * 0.5f, Height * 0.5f, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(-Width * 0.5f, -Height, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(Width * 0.5f, -Height * 0.5f, 0.0f);

			// 色の適応
			for (int nCntcol = 0; nCntcol < 4; nCntcol++)
			{
				pVtx[nCntcol].col = col;
			}

			break;	// ここでfor文を抜ける
		}

		pVtx += 4;	// 頂点データ
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffLineEffect->Unlock();
}