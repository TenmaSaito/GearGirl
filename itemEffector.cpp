//==================================================================================
//
// アイテムの演出のcppファイル [itemEffector.cpp]
// Author : TENMA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "itemEffector.h"
#include "item.h"
#include "mesh.h"
#include "mathUtil.h"
#include "Color_defs.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define FINISH_LINE			(1000.0f)		// 終了半径
#define ALPHA_ATTENUATION	(0.01f)			// α値の縮小係数

//**********************************************************************************
//*** エフェクターの定義 ***
//**********************************************************************************
STRUCT()
{
	D3DXCOLOR col;			// 色
	D3DXVECTOR3 pos;		// 位置
	float fRadius;			// 半径
	int nIdxSphere;			// メッシュスフィアのインデックス
	bool bUse;				// 使用状態
	bool bFinish;			// 終了判定
} ItemEffector;

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************
void UpdateFinishEffector(ItemEffector *pEffector);
void UpdateNormalEffector(ItemEffector *pEffector);

//**********************************************************************************
//*** 定数変数 ***
//**********************************************************************************
const D3DXVECTOR3 g_posEffectUI = D3DXVECTOR3();		// UIの表示位置
const D3DXVECTOR2 g_sizeEffectUI = D3DXVECTOR2();		// UIのサイズ
const float g_fStartRadius = 10.0f;						// 開始時半径
const float g_fIncreaseRadius = 0.1f;					// 半径増加係数
const float g_fIncreaseRadiusFinish = 5.0f;				// 終了時半径増加係数
const float g_fDecreaseAlphaFinish = 0.003f;			// 終了時α値減少係数		

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
ItemEffector g_aEffector[ITEMTYPE_MAX];					// エフェクターの種類

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitItemEffector(void)
{
	USE_UTIL;

	AutoZeroMemory(g_aEffector);
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitItemEffector(void)
{

}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateItemEffector(void)
{
	ItemEffector *pEffector = &g_aEffector[0];

	for (int nCntEffector = 0; nCntEffector < ITEMTYPE_MAX; nCntEffector++, pEffector++)
	{
		if (pEffector->bUse)
		{
			if (pEffector->bFinish)
			{ // 終了更新
				UpdateFinishEffector(pEffector);
			}
			else
			{ // 通常更新
				UpdateNormalEffector(pEffector);
			}
		}
	}
}

//==================================================================================
// --- アイテムエフェクトの設置 ---
//==================================================================================
IDX_EFFECTOR SetItemEffector(D3DXVECTOR3 pos, D3DXCOLOR col)
{
	ItemEffector *pEffector = &g_aEffector[0];
	IDX_EFFECTOR error = -1;

	for (int nCntEffector = 0; nCntEffector < ITEMTYPE_MAX; nCntEffector++, pEffector++)
	{
		if (pEffector->bUse == false)
		{
			pEffector->col = col;
			pEffector->pos = pos;
			pEffector->fRadius = g_fStartRadius;

			// メッシュスフィアの設置
			pEffector->nIdxSphere = SetMeshSphere(pos, VECNULL, g_fStartRadius, 16, 16);
			SetColorMeshSphere(&GetMeshSphere()[pEffector->nIdxSphere], col);
			pEffector->bUse = true;

			error = nCntEffector;
			break;
		}
	}

	return error;
}

//==================================================================================
// --- アイテムエフェクトの終了 ---
//==================================================================================
void SetGetEffect(IDX_EFFECTOR Idx)
{
	if (Idx < 0 || Idx >= ITEMTYPE_MAX) return;

	ItemEffector *pEffector = &g_aEffector[Idx];
	pEffector->bFinish = true;
	pEffector->col.a = 0.5f;
}

//==================================================================================
// --- アイテムエフェクトの位置変更 ---
//==================================================================================
void SetPositionItemEffector(IDX_EFFECTOR Idx, D3DXVECTOR3 pos)
{
	if (Idx < 0 || Idx >= ITEMTYPE_MAX) return;

	ItemEffector* pEffector = &g_aEffector[Idx];
	pEffector->pos = pos;

	// メッシュスフィアに適用
	SetPositionMesh(GetMeshSphere(), pos, pEffector->nIdxSphere, GetNumMeshSphere());
}

//==================================================================================
// --- アイテムエフェクトの終了時更新処理 ---
//==================================================================================
void UpdateFinishEffector(ItemEffector *pEffector)
{
	if (pEffector == nullptr) return;

	// 半径縮小
	pEffector->fRadius += g_fIncreaseRadiusFinish;
	pEffector->col.a -= g_fDecreaseAlphaFinish;
	if (pEffector->fRadius >= FINISH_LINE)
	{
		pEffector->bUse = false;
		ReleaseMesh(GetMeshSphere(), pEffector->nIdxSphere, GetNumMeshSphere());
		return;
	}

	// 各種変数を適用
	SetColorMeshSphere(&GetMeshSphere()[pEffector->nIdxSphere], pEffector->col);
	SetRadiusMeshSphere(&GetMeshSphere()[pEffector->nIdxSphere], pEffector->fRadius);
}

//==================================================================================
// --- アイテムエフェクトの通常時更新処理 ---
//==================================================================================
void UpdateNormalEffector(ItemEffector *pEffector)
{
	if (pEffector == nullptr) return;

	pEffector->col.a -= ALPHA_ATTENUATION;
	if (pEffector->col.a <= 0.0f)
	{ // 透明になったら、半径縮小し再度不透明に
		pEffector->col.a = 1.0f;
		pEffector->fRadius = g_fStartRadius;
		return;
	}

	// 半径縮小
	pEffector->fRadius += g_fIncreaseRadius;

	// 各種変数を適用
	SetColorMeshSphere(&GetMeshSphere()[pEffector->nIdxSphere], pEffector->col);
	SetRadiusMeshSphere(&GetMeshSphere()[pEffector->nIdxSphere], pEffector->fRadius);
}