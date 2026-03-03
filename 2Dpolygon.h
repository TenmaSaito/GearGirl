//================================================================================================================
//
// DirectXのポリゴン表示用ヘッダファイル [2Dpolygon.h]
// Author : TENMA
//
//================================================================================================================
#ifndef _2DPOLYGON_H_
#define _2DPOLYGON_H_

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
using IDX_2DPOLYGON = int;

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************
void Init2DPolygon(void);
void Uninit2DPolygon(void);
void Update2DPolygon(void);
void Draw2DPolygon(void);

int Set2DPolygon(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR2 size, int nIdTexture = -1, D3DXCOLOR col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
void SetPosition2DPolygon(IDX_2DPOLYGON nId2DPolygon, D3DXVECTOR3 pos);
void SetColor2DPolygon(IDX_2DPOLYGON nId2DPolygon, D3DXCOLOR col);
void SetSize2DPolygon(IDX_2DPOLYGON nId2DPolygon, D3DXVECTOR2 size);
void Destroy2DPolygon(IDX_2DPOLYGON nId2DPolygon);
void SetEnable2DPolygon(IDX_2DPOLYGON Idx2DPolygon, bool bEnable);
#endif