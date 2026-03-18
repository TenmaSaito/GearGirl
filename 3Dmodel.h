//==================================================================================
//
// DirectXの3Dモデル配置用ヘッダファイル [3Dmodel.h]
// Author : TENMA
//
//==================================================================================
#ifndef _3DMODEL_H_
#define _3DMODEL_H_

//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "main.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define MAX_MAT			(25)	// マテリアルの最大数

//**********************************************************************************
//*** 型宣言 ***
//**********************************************************************************
typedef int IDX_3DMODEL;		// 3Dモデルのインデックス

//**********************************************************************************
//*** モデル情報構造体の定義 ***
//**********************************************************************************
STRUCT(_3DMODEL)
{
	D3DXCOLOR col;			// 色
	D3DXVECTOR3 pos;		// 3Dモデルの位置
	D3DXVECTOR3 rot;		// 3Dモデルの向き
	D3DXMATRIX mtxWorld;	// ワールドマトリックス
	int nIdx3Dmodel;		// モデルデータのインデックス
	bool bUse;				// 格納状況
	bool bEnable;			// 描画するかどうか
	bool bAlpha;			// α値を変更するか
	bool bZFunc;			// Zテストを行うか
} _3DMODEL;

typedef struct _3DMODEL *P3DMODEL, *LP3DMODEL;

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************
void Init3DModel(void);
void Uninit3DModel(void);
void Update3DModel(void);
void Draw3DModel(void);

IDX_3DMODEL Set3DModel(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nIdxModelData);
void SetPosition3DModel(IDX_3DMODEL Idx, D3DXVECTOR3 pos);
void SetRotation3DModel(IDX_3DMODEL Idx, D3DXVECTOR3 rot);
void SetEnable3DModel(IDX_3DMODEL Idx, bool bEnable); 
void SetColor3DModel(IDX_3DMODEL Idx, D3DXCOLOR col, bool bAlpha);
void SetZFunc3DModel(IDX_3DMODEL Idx, bool bEnable);
LP3DMODEL Get3DModel(IDX_3DMODEL nIdxModel);
#endif