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
#define MAX_PARTICLE (128) // パーティクルの最大数

//*************************************************************************************************
//*** パーティクル構造体 ***
//*************************************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;			// 現在位置
	D3DXCOLOR col;				// 色
	float fRadius;				// 出現ゲートサイズ（半径）
	int nLife;					// 残り寿命（フレーム数）
	int nSpeedMax, nSpeedMin;	// 速度の最大値最小値
	int nCntEffect;				// 一フレームに発生するエフェクトの数
	bool bUse;					// 使用中フラグ
} Particle;

typedef D3DXVECTOR3 位置, 向き, ベクトル;
typedef D3DXCOLOR 色;
#define 無 NULL
#define 初期化 ZeroMemory

//*************************************************************************************************
//*** グローバル変数 ***
//*************************************************************************************************
Particle g_aParticle[MAX_PARTICLE];					 // パーティクル配列

//================================================================================================================
// --- パーティクルの初期化処理 ---
//================================================================================================================
void InitParticle(void)
{
	初期化(&g_aParticle[0], sizeof(g_aParticle));

	foreach(Particle, var, g_aParticle)
	{
		var.bUse = false;
	}
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
	float fRadius;
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
			pos.x = pParticle->pos.x + ((float)(rand() % 100 - 50) * 0.1f);
			pos.y = pParticle->pos.y + ((float)(rand() % 100 - 50) * 0.1f);

			move.z = (float)((rand() % 50) * 0.5f) + 0.1f;
			fRadius = (float)(rand() % 50 - 25) + 0.1f;
			fSpeed = (float)(rand() % pParticle->nSpeedMax - pParticle->nSpeedMin) + 0.1f;
			col = pParticle->col;
			nLife = rand() % 100;

			SetEffect(pos, col, move, fRadius, fRadius, fSpeed, nLife);
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
void SetParticle(位置 pos, 色 col, float fRadius, int nLife, int nFrameEffect, int nSpeedMax, int nSpeedMin, bool bUseGravity)
{
	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if (!g_aParticle[nCntParticle].bUse)
		{
			// パーティクルの初期値設定
			g_aParticle[nCntParticle].pos = pos;
			g_aParticle[nCntParticle].col = col;
			g_aParticle[nCntParticle].fRadius = fRadius;
			g_aParticle[nCntParticle].nLife = nLife;
			g_aParticle[nCntParticle].bUse = true;
			break;
		}
	}
}
