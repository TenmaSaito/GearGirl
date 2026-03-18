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
const char *g_apGuideModelPath[PLAYERTYPE_MAX] =			// ガイド矢印モデルのパス
{
	"data/MODEL/Guide/GuideArrow.x",
	"data/MODEL/Guide/GuideArrow_Smaller.x",
};	
const float g_aLengthPlayer[PLAYERTYPE_MAX] = { 15.0f, 0 };			// プレイヤーとの距離
const float g_aHeightModel[PLAYERTYPE_MAX] = { 15.0f, 4.5f };		// 基本の高さ		
const D3DXVECTOR3 g_posShop = D3DXVECTOR3(1463, 100, -455);			// 店の位置

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
Guide g_aGuide[PLAYERTYPE_MAX] = {};		// ガイドの情報

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitGuide(void)
{
	// 初期化
	AutoZeroMemory(g_aGuide);

	// モデル読み込み
	IDX_MODELDATA md;
	LoadModelData(g_apGuideModelPath[PLAYERTYPE_GIRL], &md);

	// モデル設置
	g_aGuide[PLAYERTYPE_GIRL].Idx3DModel = Set3DModel(VECNULL, VECNULL, md);

	LoadModelData(g_apGuideModelPath[PLAYERTYPE_MOUSE], &md);

	// モデル設置
	g_aGuide[PLAYERTYPE_MOUSE].Idx3DModel = Set3DModel(VECNULL, VECNULL, md);

	SetEnable3DModel(g_aGuide[PLAYERTYPE_GIRL].Idx3DModel, true);
	SetEnable3DModel(g_aGuide[PLAYERTYPE_MOUSE].Idx3DModel, true);
	SetZFunc3DModel(g_aGuide[PLAYERTYPE_GIRL].Idx3DModel, false);
	SetZFunc3DModel(g_aGuide[PLAYERTYPE_MOUSE].Idx3DModel, false);
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
	Guide *pGuide = &g_aGuide[0];
	GC_DATA *pGimmick = GetGCData();

	for (int nCntPlayer = 0; nCntPlayer < PLAYERTYPE_MAX; nCntPlayer++, pPlayer++, pGuide++)
	{
		GIMMICKTYPE type = GIMMICKTYPE_MAX;	// 最も近いギミック
		float fLength = 1000000.0f;			// ギミックとプレイヤーの距離
		float fAngle = 0.0f;				// 角度
		D3DXCOLOR col = COLOR_WHITE;		// 矢印の色

		for (int nCntGimmick = 0; nCntGimmick < GC_MAX; nCntGimmick++)
		{
			if (pGimmick[nCntGimmick].bEnable == false) continue;

			D3DXVECTOR3 posGimmick = GetGimmick()[pGimmick[nCntGimmick].typeDetection].pos;
			D3DXVECTOR3 diff = pPlayer->pos - posGimmick;
			float fPTPLength = D3DXVec3Length(&diff);		// 2点間の距離を求める

			if (fPTPLength < fLength)
			{
				col = pGimmick[nCntGimmick].col;
				fAngle = GetPosToPos(posGimmick, pPlayer->pos);
				fLength = fPTPLength;
				type = (GIMMICKTYPE)nCntGimmick;
			}
		}

		if (TUTORIAL_NOW)
		{
			type = GIMMICKTYPE_MAX;
		}

		if (type == GIMMICKTYPE_MAX)
		{
			fAngle = GetPosToPos(g_posShop, pPlayer->pos);
			col = COLOR_UNUSED;
		}

		D3DXVECTOR3 pos;
		pos.x = pPlayer->pos.x + sinf(fAngle) * g_aLengthPlayer[nCntPlayer];
		pos.y = pPlayer->pos.y + g_aHeightModel[nCntPlayer];
		pos.z = pPlayer->pos.z + cosf(fAngle) * g_aLengthPlayer[nCntPlayer];

		SetPosition3DModel(pGuide->Idx3DModel, pos);
		SetRotation3DModel(pGuide->Idx3DModel, VEC_Y(fAngle));
		SetColor3DModel(pGuide->Idx3DModel, col, false);
	}
}

//==================================================================================
// --- 描画フラグ変更 ---
//==================================================================================
void SetEnableGuide(bool bEnable)
{
	SetEnable3DModel(g_aGuide[PLAYERTYPE_GIRL].Idx3DModel, bEnable);
	SetEnable3DModel(g_aGuide[PLAYERTYPE_MOUSE].Idx3DModel, bEnable);
}

//==================================================================================
// --- 描画フラグ取得 ---
//==================================================================================
bool GetEnableGuide(void)
{
	return GetEnable3DModel(g_aGuide[PLAYERTYPE_GIRL].Idx3DModel);
}