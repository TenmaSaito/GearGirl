//==================================================================================
//
// ガイドのcppファイル [guide.cpp]
// Author : TENMA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "guide.h"

#include "player.h"
#include "gimmick.h"
#include "3Dmodel.h"
#include "modeldata.h"
#include "mathUtil.h"

USE_UTIL;

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************

//**********************************************************************************
//*** ガイド構造体 ***
//**********************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;			// 位置
	D3DXVECTOR3 rot;			// 角度
	float fRadius;				// 半径
	IDX_3DMODEL Idx3DModel;		// モデルのインデックス
} Guide;

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************

//**********************************************************************************
//*** 定数変数 ***
//**********************************************************************************
const char *g_pGuideModelPath = "data/MODEL/Guide/GuideArrow.x";	// ガイド矢印モデルのパス
const float g_fLength

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
Guide g_guide = {};		// ガイドの情報

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitItemEffector(void)
{
	// 初期化
	AutoZeroMemory(g_guide);

	// モデル読み込み
	IDX_MODELDATA md;
	LoadModelData(g_pGuideModelPath, &md);

	// モデル設置
	g_guide.Idx3DModel = Set3DModel(VECNULL, VECNULL, md);

	SetEnable3DModel(g_guide.Idx3DModel, false);
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitItemEffector(void)
{
	// 特になし
}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateItemEffector(void)
{
	Player *pPlayer = GetPlayer();
	LPGIMMICK pGimmick = GetGimmick();
	float fLength = 1000000.0f;		// ギミックとプレイヤーの距離
	GIMMICKTYPE type;				// 最も近いギミック

	for (int nCntGimmick = 0; nCntGimmick < GIMMICKTYPE_MAX; nCntGimmick++)
	{
		if (pGimmick[nCntGimmick].myType == GIMMICKTYPE_TUNNEL
			|| pGimmick[nCntGimmick].myType == GIMMICKTYPE_CLOSEDDOOR)
		{
			continue;
		}

		D3DXVECTOR3 diff = pPlayer->pos - pGimmick[nCntGimmick].pos;
		float fPTPLength = D3DXVec3Length(&diff);		// 2点間の距離を求める

		if (fPTPLength < fLength)
		{
			fLength = fPTPLength;
			type = (GIMMICKTYPE)nCntGimmick;
		}
	}

	float fAngle = GetPosToPos(pGimmick[type].pos, pPlayer->pos);

	D3DXVECTOR3 pos;
}