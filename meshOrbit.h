//==================================================================================
//
// メッシュオービットのヘッダファイル [meshOrbit.h]
// Author : TENMA
// DATE : 2026/03/10
//
//==================================================================================
#ifndef _MESHORBIT_H_		// インクルードガード
#define _MESHORBIT_H_

//**********************************************************************************
//*** インクルード関連 ***
//**********************************************************************************
#include "main.h"
#include "Texture.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define MAX_ORB_ARRAY		(150)		// 配列の最大数

//**********************************************************************************
//*** 型宣言 ***
//**********************************************************************************
typedef int IDX_MESHORBIT;		// メッシュオービットのインデックス

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************
void InitMeshOrbit(void);
void UninitMeshOrbit(void);
void UpdateMeshOrbit(void);
void DrawMeshOrbit(void);

IDX_MESHORBIT SetOrbit(D3DXVECTOR3 offset0, D3DXVECTOR3 offset1, D3DXMATRIX *pMtx, UINT nNumVertices);
void SetColorOrbit(IDX_MESHORBIT IdxMeshOrbit, D3DXCOLOR col);
void SetTextureOrbit(IDX_MESHORBIT IdxMeshOrbit, IDX_TEXTURE tex);
void SetEnableOrbit(IDX_MESHORBIT IdxMeshOrbit, bool bEnable);
void SetOffSetOrbit(IDX_MESHORBIT IdxMeshOrbit, D3DXVECTOR3 offset0, D3DXVECTOR3 offset1);
void SetParentOrbit(IDX_MESHORBIT IdxMeshOrbit, D3DXMATRIX *pMtx);
void DestroyOrbit(IDX_MESHORBIT IdxMeshOrbit);
#endif