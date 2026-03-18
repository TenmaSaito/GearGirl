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
#include "common_fade.h"
#include "dialog.h"
#include "Color_defs.h"

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
const float g_fLengthPlayer = 15.0f;		// プレイヤーとの距離
const float g_fHeightModel = 15.0f;			// 基本の高さ		
const D3DXVECTOR3 g_posShop = D3DXVECTOR3(1463, 100, -455);				// 店の位置

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
Guide g_guide = {};		// ガイドの情報

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitGuide(void)
{
	// 初期化
	AutoZeroMemory(g_guide);

	// モデル読み込み
	IDX_MODELDATA md;
	LoadModelData(g_pGuideModelPath, &md);

	// モデル設置
	g_guide.Idx3DModel = Set3DModel(VECNULL, VECNULL, md);

	SetEnable3DModel(g_guide.Idx3DModel, true);
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitGuide(void)
{
	// 特になし
}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateGuide(void)
{
	Player *pPlayer = GetPlayer();
	LPGIMMICK pGimmick = GetGimmick();
	float fLength = 1000000.0f;			// ギミックとプレイヤーの距離
	GIMMICKTYPE type = GIMMICKTYPE_MAX;	// 最も近いギミック

	for (int nCntGimmick = 0; nCntGimmick < GIMMICKTYPE_MAX; nCntGimmick++)
	{
		if (pGimmick[nCntGimmick].bClear == true) continue;

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
	if (TUTORIAL_NOW)
	{
		type = GIMMICKTYPE_MAX;
	}

	if (type == GIMMICKTYPE_MAX)
	{
		fAngle = GetPosToPos(g_posShop, pPlayer->pos);
		SetColor3DModel(g_guide.Idx3DModel, D3DXCOLOR(0, 1, 0, 1), false);
	}
	else
	{
		SetColor3DModel(g_guide.Idx3DModel, COLOR_UNUSED, false);
	}

	D3DXVECTOR3 pos;
	pos.x = pPlayer->pos.x + sinf(fAngle) * g_fLengthPlayer;
	pos.y = pPlayer->pos.y + g_fHeightModel;
	pos.z = pPlayer->pos.z + cosf(fAngle) * g_fLengthPlayer;

	SetPosition3DModel(g_guide.Idx3DModel, pos);
	SetRotation3DModel(g_guide.Idx3DModel, VEC_Y(fAngle));
}