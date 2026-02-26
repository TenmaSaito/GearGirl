// =================================================
// 
// 放物線の処理[parabola.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _PARABOLA_H_
#define _PARABOLA_H_

#include "main.h"

// =================================================
// マクロ定義
#define PARABOLA_XSIZE	(1)	// 放物線のXサイズ
#define PARABOLA_YSIZE	(1)	// 放物線のYサイズ
#define PARABOLA_ZSIZE	(1)	// 放物線のZサイズ(奥行き)
#define MAX_PARABOLA	(256)	// 放物線数

// =================================================
// バレットの構造体を定義
typedef struct
{
	D3DXVECTOR3 pos;		// 位置
	D3DXVECTOR3 posOri;		// 発射した位置
	D3DXVECTOR3 posOld;		// 1f前の位置
	D3DXVECTOR3 move;		// 移動量
	D3DXMATRIX mtxWorld;	// ワールドマトリックス
	int nLife;				// 寿命
	int nCounter;			// 処理を行いたい回数用のカウンター
	float Width;			// 幅	
	float Height;			// 高さ
	bool bUse;				// 使用中かどうか
	bool bUseGravity;		// 重力をかけるかどうか
}Parabola;

// =================================================
// プロトタイプ宣言
void InitParabola(void);	// 初期化処理
void UninitParabola(void);	// 終了処理
void UpdateParabola(void);	// 更新処理
void DrawParabola(void);	// 描画処理
void SetParabola(D3DXVECTOR3 pos, D3DXVECTOR3 move, float Width, float Height, bool bUseGravity);	// 放物線の設定
Parabola *GetParabola(void);// 放物線の情報の譲渡

#endif