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
#include "input.h"
#include "player.h"
#include "debugproc.h"
#include "Color_defs.h"

USE_UTIL;

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
void _3DVibration(ItemEffector *pEffector);
void FloatNormalize(float *pInOut, float min, float max);

//**********************************************************************************
//*** 定数変数 ***
//**********************************************************************************
const D3DXVECTOR3 g_posEffectUI = D3DXVECTOR3();		// UIの表示位置
const D3DXVECTOR2 g_sizeEffectUI = D3DXVECTOR2();		// UIのサイズ
const float g_fStartRadius = 10.0f;						// 開始時半径
const float g_fIncreaseRadius = 0.1f;					// 半径増加係数
const float g_fIncreaseRadiusFinish = 5.0f;				// 終了時半径増加係数
const float g_fDecreaseAlphaFinish = 0.003f;			// 終了時α値減少係数	
const WORD g_wStartVib = 65500;							// バイブレーションの初期値
const int g_nCountView = 160;							// 透視可能時間

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
ItemEffector g_aEffector[ITEMTYPE_MAX];		// エフェクターの種類
int g_nCounterEffector = 0;					// 透過エフェクトが使用できる時間
int g_nCounterCoolDown = 0;					// 透過エフェクトのクールタイム
int g_nIdxPlayerEffectSphere;				// プレイヤーのスフィアインデックス
float g_fRadiusPlayerEffectSphere;			// ネズミのスフィア半径
D3DXCOLOR g_colPlayerEffectSphere;			// 色

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitItemEffector(void)
{
	// 初期化
	AutoZeroMemory(g_aEffector);
	g_nCounterEffector = 0;
	g_nCounterCoolDown = 0;
	g_nIdxPlayerEffectSphere = -1;

	Player* pMouse = &GetPlayer()[PLAYERTYPE_MOUSE];
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
	ItemEffector* pEffector = &g_aEffector[0];
	Player* pMouse = &GetPlayer()[PLAYERTYPE_MOUSE];

	_3DVibration(pEffector);

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

//==================================================================================
// --- アイテムエフェクトの立体振動システム ---
//==================================================================================
void _3DVibration(ItemEffector *pEffector)
{
	// 変数
	Player *pPlayer = GetPlayer();					// プレイヤーの位置取得用
	Player *pMouse = &pPlayer[PLAYERTYPE_MOUSE];	// ネズミのポインタ
	float rotLeft = pMouse->rot.y + D3DX_HALFPI,	// プレイヤーの左
		  rotRight = pMouse->rot.y - D3DX_HALFPI;	// プレイヤーの右
	float fLength = 1000000.0f;						// アイテムとプレイヤーの距離
	ITEMTYPE type = ITEMTYPE_MAX;					// 対象のアイテムの種類

	// アイテムの中で最もネズミに近いアイテムを取得
	for (int nCntItem = 0; nCntItem < ITEMTYPE_MAX; nCntItem++)
	{
		// アイテムが取得済みの場合、スキップ
		if (pEffector[nCntItem].bFinish == true) continue;

		D3DXVECTOR3 diff = pMouse->pos - pEffector[nCntItem].pos;
		float fPTPLength = D3DXVec3Length(&diff);		// 2点間の距離を求める

		// 二点間の距離を判定
		if (fPTPLength < fLength)
		{ // 基準以下の場合、基準を更新しアイテムの種類を保存
			fLength = fPTPLength;
			type = (ITEMTYPE)nCntItem;
		}
	}

	// 判定されなかった場合終了
	if (type == ITEMTYPE_MAX)
	{
		return;
	}

	// バイブレーション可能距離外の場合終了
	if (fLength >= MAXWORD)
	{
		return;
	}

	// アイテムへの角度
	float fAngle = GetPosToPos(pEffector[type].pos, pMouse->pos);

	// 左の角度との差異
	float fDestLeft = rotLeft - fAngle;			// 角度の差分を計算
	fDestLeft = RepairRot(fDestLeft);			// 角度の修正
	fDestLeft = fabsf(fDestLeft);				// 角度の絶対値を求める
	FloatNormalize(&fDestLeft, 0.0f, D3DX_PI);	// 角度の正規化
	int nLeft = (g_wStartVib) - ((g_wStartVib) * fDestLeft);		// 正規化した値から振動の値を求める
	if (nLeft <= 10000)
	{
		nLeft = 0;
	}

	// 右の角度との差異
	float fDestRight = rotRight - fAngle;		// 角度の差分を計算
	fDestRight = RepairRot(fDestRight);			// 角度の修正
	fDestRight = fabsf(fDestRight);				// 角度の絶対値を求める
	FloatNormalize(&fDestRight, 0.0f, D3DX_PI);	// 角度の正規化
	int nRight = (g_wStartVib) - ((g_wStartVib) * fDestRight);	// 正規化した値から振動の値を求める
	if (nRight <= 10000)
	{
		nRight = 0;
	}

	if (GetNumPlayer() == 2 && GetJoypadPress(PLAYERTYPE_MOUSE, JOYKEY_RB))
	{ // 2Pプレイの場合、B長押し時ネズミ側のコントローラーにのみ振動
		SetVibration(PLAYERTYPE_MOUSE, nLeft, nRight, INT_INFINITY);
	}
	else if (GetActivePlayer() == PLAYERTYPE_MOUSE && GetJoypadPress(0, JOYKEY_RB))
	{ // 1Pプレイの場合、B長押し時ネズミ操作時のみ振動
		SetVibration(0, nLeft, nRight, INT_INFINITY);
	}
	else
	{ // 長押しをしていない場合、バイブレーションをリセット
		SetVibration(0, 0, 0, INT_INFINITY);
		SetVibration(1, 0, 0, INT_INFINITY);
	}

	PrintDebugProc("L %d : R %d\n", nLeft, nRight);
}

//==================================================================================
// --- 浮動小数の正規化処理 ---
//==================================================================================
void FloatNormalize(float *pInOut, float min, float max)
{
	*pInOut = (*pInOut - min) / (max - min);
}

//==================================================================================
// --- 透過処理の判定 ---
//==================================================================================
bool IsCouldViewItemEffectorByMouse(void)
{
	return (g_nCounterEffector > 0) ? true : false;
}

//==================================================================================
// --- 透過処理の設定 ---
//==================================================================================
void SetViewItemEffectorByMouse(void)
{
	// 無し
}