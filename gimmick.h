//==================================================================================
//
// DirectXのギミック処理 [gimmick.h]
// Author : TENMA
//
//==================================================================================
#ifndef _GIMMICK_H_		// IncludeGuard
#define _GIMMICK_H_

//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include "player.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************

//**********************************************************************************
//*** ギミックタイプ ***
//**********************************************************************************
ENUM()
{
	GIMMICKTYPE_BIGBUTTON = 0,	// でかボタン
	GIMMICKTYPE_SMALLBUTTON,	// ちびボタン
	GIMMICKTYPE_FALLENTREE,		// 倒木
	GIMMICKTYPE_CLOSEDDOOR,		// 閉まっている扉
	GIMMICKTYPE_MAX
}GIMMICKTYPE;

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************
void InitGimmick(void);
void UninitGimmick(void);
void UpdateGimmick(void);
void DrawGimmick(void);

void SetGimmick(D3DXVECTOR3 pos, D3DXVECTOR3 rot, GIMMICKTYPE type);
void ClearGimmick(GIMMICKTYPE type);
bool IsClearGimmick(GIMMICKTYPE type);
bool CollisionGimmick(D3DXVECTOR3 *pPos, D3DXVECTOR3 *pPosOld, D3DXVECTOR3 *pMove, Player *pPlayer);
#endif	// !_GIMMICK_H_