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
#include "effect.h"
#include "mathUtil.h"

//*************************************************************************************************
//*** マクロ定義 ***
//*************************************************************************************************
#define MAX_PARTICLE	(128)	// パーティクルの最大数
#define SPD_MAX			(300)	// エフェクトの最大速度

//*************************************************************************************************
//*** パーティクル構造体 ***
//*************************************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;			// 現在位置
	D3DXVECTOR3 vecMin, vecMax;	// ベクトルの範囲
	D3DXCOLOR col;				// 色
	float fRadiusEffect;		// 出現エフェクトサイズ(半径)
	int nRadius;				// 出現ゲートサイズ（半径）
	int nLife;					// 残り寿命（フレーム数）
	int nCntEffect;				// 一フレームに発生するエフェクトの数
	bool bUse;					// 使用中フラグ
	bool bGravity;				// エフェクトの自由落下の有無
	bool bVisible;				// 限定的に見えるものか
} Particle;

//*************************************************************************************************
//*** グローバル変数 ***
//*************************************************************************************************
Particle g_aParticle[MAX_PARTICLE];					 // パーティクル配列

//================================================================================================================
// --- パーティクルの初期化処理 ---
//================================================================================================================
void InitParticle(void)
{
	ZeroMemory(&g_aParticle[0], sizeof(g_aParticle));
}

//================================================================================================================
// --- パーティクルの終了処理 ---
//================================================================================================================
void UninitParticle(void)
{
	
}

//================================================================================================================
// --- パーティクルの更新処理 ---
//================================================================================================================
void UpdateParticle(void)
{
	Particle* pParticle = &g_aParticle[0];
	D3DXVECTOR3 pos;
	D3DXVECTOR3 move;
	D3DXCOLOR col;
	float fSpeed;
	int nLife;

	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++, pParticle++)
	{
		if (pParticle->bUse == false)
		{
			continue;
		}

		for (int nCntEffect = 0; nCntEffect < pParticle->nCntEffect; nCntEffect++)
		{
			pos.x = pParticle->pos.x + ((float)(rand() % (pParticle->nRadius * 200) - pParticle->nRadius * 100) * 0.01f);
			pos.y = pParticle->pos.y + ((float)(rand() % (pParticle->nRadius * 200) - pParticle->nRadius * 100) * 0.01f);
			pos.z = pParticle->pos.z + ((float)(rand() % (pParticle->nRadius * 200) - pParticle->nRadius * 100) * 0.01f);
			
			D3DXVECTOR3 vecMx = pParticle->vecMax * 100.0f;
			D3DXVECTOR3 vecMn = pParticle->vecMin * 100.0f;

			move.x = ((float)(rand() % (int)vecMx.x) + vecMn.x);
			move.y = ((float)(rand() % (int)vecMx.y) + vecMn.y);
			move.z = ((float)(rand() % (int)vecMx.z) + vecMn.z);

			move *= 0.01f;

			fSpeed = (float)(rand() % SPD_MAX) * 0.01f + 0.1f;
			col = pParticle->col;
			nLife = rand() % 100;

			if (nLife > 30)
			{
				nLife = 30;
			}

			SetEffect(pos, col, move, pParticle->fRadiusEffect, pParticle->fRadiusEffect, fSpeed, nLife, pParticle->bGravity, pParticle->bVisible);
		}

		pParticle->nLife--;

		if (pParticle->nLife <= 0)
		{
			pParticle->bUse = false;
		}
	}
}

//================================================================================================================
// --- パーティクルの設定処理 ---
//================================================================================================================
void SetParticle(D3DXVECTOR3 pos, D3DXCOLOR col, D3DXVECTOR3 vecMin, D3DXVECTOR3 vecMax, int nRadius, float fRadiusEffect, int nLife, int nFrameEffect, bool bUseGravity, bool bVisible)
{
	Particle *pParticle = &g_aParticle[0];

	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++, pParticle++)
	{
		if (pParticle->bUse == false)
		{
			// パーティクルの初期値設定
			pParticle->pos = pos;
			pParticle->vecMax = vecMax;
			pParticle->vecMin = vecMin;
			pParticle->col = col;
			pParticle->fRadiusEffect = fRadiusEffect;
			pParticle->nRadius = nRadius;
			pParticle->nLife = nLife;
			pParticle->nCntEffect = nFrameEffect;
			pParticle->bUse = true;
			pParticle->bGravity = bUseGravity;
			pParticle->bVisible = bVisible;
			break;
		}
	}
}
