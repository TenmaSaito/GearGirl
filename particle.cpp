//==================================================================================
//
// DirectXのパーティクル処理 [particle.cpp]
// Author : KOMATSU SHOTA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include "player.h"
#include "particle.h"

//*************************************************************************************************
//*** マクロ定義 ***
//*************************************************************************************************
#define MAX_PARTICLE (128) // パーティクルの最大数
#define FVF_VERTEX_3D (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1) // 頂点フォーマット（XYZ座標 + 色 + テクスチャ）

//*************************************************************************************************
//*** 頂点構造体 ***
//*************************************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;	// 3D位置
	D3DCOLOR col;		// 頂点カラー
	D3DXVECTOR2 tex;	// テクスチャ座標
} VERTEX_3D;

//*************************************************************************************************
//*** パーティクル構造体 ***
//*************************************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;	// 現在位置
	D3DXVECTOR3 move;	// 毎フレームの移動量
	D3DXVECTOR3 col;	// 色
	float fRadius;		// サイズ（半径）
	int nLife;			// 残り寿命（フレーム数）
	bool bUse;			// 使用中フラグ
	int nLifeMax;		// 寿命最大値
} Particle;

//*************************************************************************************************
//*** グローバル変数 ***
//*************************************************************************************************
LPDIRECT3DTEXTURE9 g_pTextureParticle = NULL;		 // テクスチャ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffParticle = NULL;	 // 頂点バッファ
Particle g_aParticle[MAX_PARTICLE];					 // パーティクル配列

// トランスフォーム行列（カメラビュー・射影・ワールド）
D3DXMATRIX g_mtxView, g_mtxProj, g_mtxWorld;

//================================================================================================================
// --- パーティクルの初期化処理 ---
//================================================================================================================
void InitParticle(void)
{
	int nCntParticle;

	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// テクスチャ読み込み
	D3DXCreateTextureFromFile(pDevice,"DATA\\TEXTURE\\effect000.jpg", &g_pTextureParticle);

	// パーティクルの初期化
	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		g_aParticle[nCntParticle].pos = D3DXVECTOR3(0, 0, 0);
		g_aParticle[nCntParticle].move = D3DXVECTOR3(0, 0, 0);
		g_aParticle[nCntParticle].nLife = 0;
		g_aParticle[nCntParticle].bUse = false;
	}

	// 頂点バッファ作成（4頂点 × パーティクル数）
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4 * MAX_PARTICLE,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffParticle,
		NULL);
}

//================================================================================================================
// --- パーティクルの終了処理 ---
//================================================================================================================
void UninitParticle(void)
{
	if (g_pTextureParticle)
	{
		g_pTextureParticle->Release();
		g_pTextureParticle = NULL;
	}
	if (g_pVtxBuffParticle)
	{
		g_pVtxBuffParticle->Release();
		g_pVtxBuffParticle = NULL;
	}
}

//================================================================================================================
// --- パーティクルの更新処理 ---
//================================================================================================================
void UpdateParticle(void)
{
	VERTEX_3D* pVtx;

	g_pVtxBuffParticle->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		// 使用中のパーティクルのみ更新
		if (g_aParticle[nCntParticle].bUse)
		{
			// 位置を移動量分だけ更新
			g_aParticle[nCntParticle].pos += g_aParticle[nCntParticle].move;

			// 寿命を1減らす
			g_aParticle[nCntParticle].nLife--;

			//========================================
			// halfSize = パーティクルの半サイズ
			// right = 横方向ベクトル
			// up = 縦方向ベクトル
			// vertexIndex = 4頂点の番号を表す
			//========================================

			// ビルボード用のカメラ方向ベクトルを取得
			D3DXVECTOR3 right(g_mtxView._11, g_mtxView._21, g_mtxView._31);		// 横方向ベクトル
			D3DXVECTOR3 up(g_mtxView._12, g_mtxView._22, g_mtxView._32);		// 縦方向ベクトル

			D3DXVECTOR3 center = g_aParticle[nCntParticle].pos;					// パーティクルの中心座標
			float halfSize = g_aParticle[nCntParticle].fRadius;					// 半サイズ

			// 4つの頂点位置をカメラに向けて配置
			pVtx[0].pos = center - right * halfSize + up * halfSize; // 左上
			pVtx[1].pos = center + right * halfSize + up * halfSize; // 右上
			pVtx[2].pos = center - right * halfSize - up * halfSize; // 左下
			pVtx[3].pos = center + right * halfSize - up * halfSize; // 右下

			// パーティクルが持つ色情報を取得
			D3DXVECTOR3 col = g_aParticle[nCntParticle].col;

			// 寿命に応じたアルファを計算
			float alpha = (float)g_aParticle[nCntParticle].nLife / g_aParticle[nCntParticle].nLifeMax;
			alpha = max(0.0f, min(1.0f, alpha)); //0?1の範囲に制限

			// 各頂点に色とテクスチャ座標を設定
			for (int vertexIndex = 0; vertexIndex < 4; vertexIndex++)
			{
				// 色 + アルファ
				pVtx[vertexIndex].col = D3DCOLOR_COLORVALUE(col.x, col.y, col.z, alpha);

				pVtx[vertexIndex].tex = D3DXVECTOR2((vertexIndex % 2), (vertexIndex / 2));
			}

			pVtx += 4;

			// 寿命が尽きたら削除
			if (g_aParticle[nCntParticle].nLife <= 0)
			{
				g_aParticle[nCntParticle].bUse = false;
			}
		}
		else
		{
			pVtx += 4;
		}
	}

	g_pVtxBuffParticle->Unlock();
}

//================================================================================================================
// --- パーティクルの描画処理 ---
//================================================================================================================
void DrawParticle(void)
{
	int nCntParticle;

	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// トランスフォーム行列の設定（カメラビュー、射影、ワールド）
	pDevice->SetTransform(D3DTS_VIEW, &g_mtxView);
	pDevice->SetTransform(D3DTS_PROJECTION, &g_mtxProj);
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorld);

	// 頂点バッファとテクスチャを設定して描画
	pDevice->SetStreamSource(0, g_pVtxBuffParticle, 0, sizeof(VERTEX_3D));
	pDevice->SetFVF(FVF_VERTEX_3D);
	pDevice->SetTexture(0, g_pTextureParticle);

	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if (g_aParticle[nCntParticle].bUse)
		{
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntParticle * 4, 2); // 4頂点で2枚三角形
		}
	}
}

//================================================================================================================
// --- パーティクルの設定処理 ---
//================================================================================================================
void SetParticle(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXVECTOR3 col, float fRadius, int nLife)
{
	int nCntParticle;
	VERTEX_3D* pVtx;

	g_pVtxBuffParticle->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if (!g_aParticle[nCntParticle].bUse)
		{
			// パーティクルの初期値設定
			g_aParticle[nCntParticle].pos = pos;
			g_aParticle[nCntParticle].move = move;
			g_aParticle[nCntParticle].col = col;
			g_aParticle[nCntParticle].fRadius = fRadius;
			g_aParticle[nCntParticle].nLife = nLife;
			g_aParticle[nCntParticle].nLifeMax = nLife;
			g_aParticle[nCntParticle].bUse = true;
			break;
		}
		pVtx += 4;
	}

	g_pVtxBuffParticle->Unlock();
}
