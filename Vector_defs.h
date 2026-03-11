//==================================================================================
//
// VECTOR関連マクロ定義ヘッダファイル [Vector_defs.h]
// Author : TENMA
// DATE : 2026/03/10
//
//==================================================================================
#ifndef _VECTOR_DEFS_H_		// インクルードガード
#define _VECTOR_DEFS_H_

//**********************************************************************************
//*** インクルード関連 ***
//**********************************************************************************
#include "d3dx9math.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define VECNULL						(D3DXVECTOR3(0, 0, 0))				// 省略版VECTORNULL
#define VECFILL(elem)				(D3DXVECTOR3(elem, elem, elem))		// FILLVECTOR

#define EXTRACT_X(vec)				(D3DXVECTOR3(vec.x, 0, 0))			// X単体に変換
#define EXTRACT_Y(vec)				(D3DXVECTOR3(0, vec.y, 0))			// Y単体に変換
#define EXTRACT_Z(vec)				(D3DXVECTOR3(0, 0, vec.z))			// Z単体に変換

#define EXTRACT_XY(vec)				(D3DXVECTOR3(vec.x, vec.y, 0))		// XY平面に変換
#define EXTRACT_XZ(vec)				(D3DXVECTOR3(vec.x, 0, vec.z))		// XZ平面に変換
#define EXTRACT_YZ(vec)				(D3DXVECTOR3(0, vec.y, vec.z))		// YZ平面に変換

#define CONVERSION_X(vec, x)		(D3DXVECTOR3(x, vec.y, vec.z))		// X要素のみ変換
#define CONVERSION_Y(vec, y)		(D3DXVECTOR3(vec.x, y, vec.z))		// Y要素のみ変換
#define CONVERSION_Z(vec, z)		(D3DXVECTOR3(vec.x, vec.y, z))		// Z要素のみ変換

#define CONVERSION_XY(vec, x, y)	(D3DXVECTOR3(x, y, vec.z))			// XY要素を変換
#define CONVERSION_XZ(vec, x, z)	(D3DXVECTOR3(x, vec.y, z))			// XZ要素を変換
#define CONVERSION_YZ(vec, y, z)	(D3DXVECTOR3(vec.x, y, z))			// YZ要素を変換

#define VEC_X(x)					CONVERSION_X(VECNULL, x)			// Xのみ変更
#define VEC_Y(y)					CONVERSION_Y(VECNULL, y)			// Yのみ変更
#define VEC_Z(z)					CONVERSION_Z(VECNULL, z)			// Zのみ変更

#define VEC_XY(x, y)				CONVERSION_XY(VECNULL, x, y)		// XYを変更
#define VEC_XZ(x, z)				CONVERSION_XZ(VECNULL, x, z)		// XZを変更
#define VEC_YZ(y, z)				CONVERSION_YZ(VECNULL, y, z)		// YZを変更

#define ADDITION_XY(vec, elem)		(D3DXVECTOR3(vec.x + elem, vec.y + elem, vec.z))			// XYに特定の値を加算
#define ADDITION_XZ(vec, elem)		(D3DXVECTOR3(vec.x + elem, vec.y, vec.z + elem))			// XZに特定の値を加算
#define ADDITION_YZ(vec, elem)		(D3DXVECTOR3(vec.x, vec.y + elem, vec.z + elem))			// YZに特定の値を加算
#define ADDITION_XYZ(vec, elem)		(D3DXVECTOR3(vec.x + elem, vec.y + elem, vec.z + elem))		// 特定の値を加算

#define HIVECTOR3(vec)				(D3DXVECTOR4(vec, 0.0f))			// D3DXVECTOR4に昇格変換
#define HIVECTOR2(vec)				(D3DXVECTOR3(vec.x, vec.y, 0.0f))	// D3DXVECTOR3に昇格変換
#define LOVECTOR3(vec)				(D3DXVECTOR3(vec.x, vec.y, vec.z))	// D3DXVECTOR3に降格変換
#define LOVECTOR2(vec)				(D3DXVECTOR2(vec.x, vec.y))			// D3DXVECTOR2に降格変換

#endif // !_VECTOR_DEFS_H_