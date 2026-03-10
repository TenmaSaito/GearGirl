//==================================================================================
//
// VECTOR関連マクロ定義ヘッダファイル [Vector_defs.h]
// Author : TENMA
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
#define VECNULL					D3DXVECTOR3(0.0f,0.0f,0.0f)			// 省略版vectorNull

#define EXTRACT_YZ(vec)			(D3DXVECTOR3(0, vec.y, vec.z))		// YZ平面に変換
#define EXTRACT_XZ(vec)			(D3DXVECTOR3(vec.x, 0, vec.z))		// XZ平面に変換
#define EXTRACT_XY(vec)			(D3DXVECTOR3(vec.x, vec.y, 0))		// XY平面に変換

#define CONVERSION_X(vec, x)	(D3DXVECTOR3(x, vec.y, vec.z))		// X要素のみ変換
#define CONVERSION_Y(vec, y)	(D3DXVECTOR3(vec.x, y, vec.z))		// Y要素のみ変換
#define CONVERSION_Z(vec, z)	(D3DXVECTOR3(vec.x, vec.y, z))		// Z要素のみ変換

#define VEC_X(x)				CONVERSION_X(VECNULL, x)			// Xのみ変更
#define VEC_Y(y)				CONVERSION_Y(VECNULL, y)			// Yのみ変更
#define VEC_Z(z)				CONVERSION_Z(VECNULL, z)			// Zのみ変更

#define VEC_XY(x, y)			D3DXVECTOR3(x, y, 0.0f)				// XYを変更
#define VEC_XZ(x, z)			D3DXVECTOR3(x, 0.0f, z)				// XZを変更
#define VEC_YZ(y, z)			D3DXVECTOR3(0.0f, y, z)				// YZを変更

#endif // !_VECTOR_DEFS_H_