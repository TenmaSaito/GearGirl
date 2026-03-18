//================================================================================================================
//
// DirectXの壁表示用ヘッダファイル [wall.h]
// Author : TENMA
//
//================================================================================================================
#ifndef _WALL_H_
#define _WALL_H_

//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include "input.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************
void InitWall(void);
void UninitWall(void);
void UpdateWall(void);
void DrawWall(void);

int SetWall(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidth, float fHeight, int nIndexTexture, int nXblock, int nZblock, D3DXCOLOR col = D3DXCOLOR_NULL, D3DCULL type = D3DCULL_CCW, bool IsStatic = true);
void CollisionWall(D3DXVECTOR3 *pPos, D3DXVECTOR3 *pPosOld, D3DXVECTOR3 *pMove);
void ReflectWall(D3DXVECTOR3 *pPos, D3DXVECTOR3 *pPosOld, D3DXVECTOR3 *pMove);
void SetEnableWall(int nIdx, bool bDisp);	// 壁の非表示、非表示切り替え

#endif