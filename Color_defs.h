//==================================================================================
//
// COLOR関連マクロ定義ヘッダファイル [Color_defs.h]
// Author : TENMA
// DATE : 2026/03/10
//
//==================================================================================
#ifndef _COLOR_DEFS_H_		// インクルードガード
#define _COLOR_DEFS_H_

//**********************************************************************************
//*** インクルード関連 ***
//**********************************************************************************
#include "d3dx9math.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define COLOR_WHITE				(D3DXCOLOR(1, 1, 1, 1))					// 基本色
#define COLOR_NONE				(D3DXCOLOR(0, 0, 0, 0))					// 無色
#define COLOR_INV				(D3DXCOLOR(1, 1, 1, 0))					// 透明
#define COLOR_UNUSED			(D3DXCOLOR(-1, -1, -1, -1))				// 未使用

#define COLOR_FILL(elem)		(D3DXCOLOR(elem, elem, elem, elem))		// 指定色

#define CONVERSION_A(col, a)	(D3DXCOLOR(col.r, col.g, col.b, a))		// alpha値のみ変換

#define EXTRACT_R(col)			(D3DXCOLOR(col.r, 0, 0, 0))				// r値のみに変換
#define EXTRACT_G(col)			(D3DXCOLOR(0, col.g, 0, 0))				// g値のみに変換
#define EXTRACT_B(col)			(D3DXCOLOR(0, 0, col.b, 0))				// b値のみに変換

#define EXTRACT_RA(col)			(CONVERSION_A(EXTRACT_R(col), col.a))	// ra値のみに変換
#define EXTARCT_GA(col)			(CONVERSION_A(EXTRACT_G(col), col.a))	// ga値のみに変換
#define EXTARCT_BA(col)			(CONVERSION_A(EXTRACT_B(col), col.a))	// ba値のみに変換

#define EXTRACT_RGA(col)		(D3DXCOLOR(col.r, col.g, 0, col.a))		// rga値のみに変換
#define EXTRACT_RBA(col)		(D3DXCOLOR(col.r, 0, col.b, col.a))		// rba値のみに変換
#define EXTRACT_GBA(col)		(D3DXCOLOR(0, col.g, col.b, col.a))		// gba値のみに変換

#define CONVERSION_R(col, r)	(D3DXCOLOR(r, col.g, col.b, col.a))		// r値のみ変更
#define CONVERSION_G(col, g)	(D3DXCOLOR(col.r, g, col.b, col.a))		// g値のみ変更
#define CONVERSION_B(col, b)	(D3DXCOLOR(col.r, col.g, b, col.a))		// b値のみ変更

#endif