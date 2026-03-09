//================================================================================================================
//
// DirectXのゲームフィールド表示用ヘッダファイル [field.h]
// Author : TENMA
//
//================================================================================================================
#ifndef _FIELD_H_
#define _FIELD_H_

//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "main.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************

//**********************************************************************************
//*** 型宣言 ***
//**********************************************************************************
typedef int IDX_FIELD;		// フィールドインデックス

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************
void InitField(void);
void UninitField(void);
void UpdateField(void);
void DrawField(void);

IDX_FIELD SetField(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXVECTOR3 rot, float fWidth, float fDepth, int nIndexTexture, int nXblock, int nZblock, D3DCULL type);
bool CollisionFloor(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, float fHeight);
void SetEnableField(IDX_FIELD field, bool bEnable);
void SetColorField(IDX_FIELD field, D3DXCOLOR col);
void SetPositionField(IDX_FIELD field, D3DXVECTOR3 pos);
void DestroyField(IDX_FIELD field);
#endif