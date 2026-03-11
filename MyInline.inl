//==================================================================================
//
// 自作インラインライブラリヘッダファイル [MyInline.inl]
// Author : TENMA
// DATE	: 2026/03/09
//
//==================================================================================
#ifndef _MYINLINE_INL_		// インクルードガード
#define _MYINLINE_INL_

//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include <math.h>
#include "Vector_defs.h"

//==================================================================================
//
// インライン計算関数
//
//==================================================================================
namespace MyInl
{
	//------------------------------------------------------------------------------
	/*** 単純計算関連 ***/
	// --- int --- //
	// 最大値
	__forceinline int Max(int x, int y)
	{
		return (x >= y) ? x : y;
	}

	// 最小値
	__forceinline int Min(int x, int y)
	{
		return (x >= y) ? y : x;
	}

	// クランプ
	__forceinline int Clamp(int x, int clampMin, int clampMax)
	{
		return (x > clampMax) ? clampMax
			: ((x < clampMin) ? clampMin
				: x);
	}

	// 二乗
	__forceinline int Square(int x)
	{
		return x * x;
	}

	// 線形補間
	__forceinline int Lerp(int start, int end, float s)
	{
		return start + ((end - start) * s);
	}

	// 上下判定
	__forceinline int Step(int y, int x)
	{
		return (x >= y) ? 1 : 0;
	}

	// 符号を返す
	__forceinline int Sign(int x)
	{
		return (x > 0) ? 1
			: ((x < 0) ? -1
				: 0);
	}

	// --- float --- //
	// 最大値
	__forceinline float Max(float x, float y)
	{
		return (x >= y) ? x : y;
	}

	// 最小値
	__forceinline float Min(float x, float y)
	{
		return (x >= y) ? y : x;
	}

	// クランプ
	__forceinline float Clamp(float x, float clampMin, float clampMax)
	{
		return (x > clampMax) ? clampMax
			: ((x < clampMin) ? clampMin
				: x);
	}

	// 二乗
	__forceinline float Square(float x)
	{
		return x * x;
	}

	// 線形補間
	__forceinline float Lerp(float start, float end, float s)
	{
		return start + ((end - start) * s);
	}

	// 上下判定
	__forceinline float Step(float y, float x)
	{
		return (x >= y) ? 1.0f : 0.0f;
	}

	// 絶対値変換済み上下判定
	__forceinline float StepAbs(float y, float x)
	{
		return (x >= fabsf(y)) ? 1.0f : 0.0f;
	}

	// 符号を返す
	__forceinline float Sign(float x)
	{
		return (x > 0.0f) ? 1.0f
			: ((x < 0.0f) ? -1.0f
				: 0.0f);
	}

	// 角度修正
	__forceinline float RepairRot(float fRot)
	{
		return (fRot > D3DX_PI) ? fRot -= D3DX_PI * 2.0f :
			((fRot <= -D3DX_PI) ? fRot += D3DX_PI * 2.0f :
				fRot);
	}

	// --- VECTOR3 --- //
	// 円の弧の座標
	__forceinline D3DXVECTOR3 Arc(float radius, float radian, D3DXVECTOR3 offset = VECNULL)
	{
		return D3DXVECTOR3(offset.x + cosf(RepairRot(radian)) * radius,
			offset.y + sinf(RepairRot(radian)) * radius,
			offset.z + 0.0f);
	}

	// ランダムな角度の取得
	__forceinline D3DXVECTOR3 GetRandomRadian(void)
	{
		return D3DXVECTOR3(
			RepairRot((FLOAT)(rand() % 628 - 314) * 0.01f),
			RepairRot((FLOAT)(rand() % 628 - 314) * 0.01f),
			RepairRot((FLOAT)(rand() % 628 - 314) * 0.01f));
	}

	// --- EVERY ---//
	// 判定
	template <typename T>
	inline bool IsArray(T* pArray, int num, T Compare)
	{
		for (int nCntCom = 0; nCntCom < num; nCntCom++)
		{
			if (pArray[nCntCom] == Compare)
			{
				return true;
			}
		}

		return false;
	}

	template<typename Func, typename... Args>
	void SetFunction(Func function, Args... args)
	{
		function(args...);
	}


	template<typename Return, typename Func, typename... Args>
	Return (*FunctionPointer[50])(Args...);

	template<typename Return, typename Func, typename... Args>
	Return SetFunction_(Func function, Args... args)
	{
		return function(args...);
	}
	//-----------------------------------------------------------------------------
}

//==================================================================================
//
// NAND関連
//
//==================================================================================
typedef enum
{
	INTERMEDIARY = 0	// 仲介列挙(意図的に使用しない)
} Intermediary;

//==================================================================================
// --- NAND構造体(仲介役) ---
//==================================================================================
struct NAND
{
	bool nand;		// 変数の一時代入先

	// コンストラクタ
	NAND(bool Elem)
	{
		nand = Elem;
	}
};

//==================================================================================
// --- NAND計算 1 ---
//==================================================================================
inline NAND operator *(bool a, Intermediary)
{
	return NAND(a);
}

//==================================================================================
// --- NAND計算 2 ---
//==================================================================================
inline bool operator *(NAND b, bool a)
{
	return (!(b.nand & a));
}

//==================================================================================
// --- 計算マクロ定義 ---
//==================================================================================
#define $ *INTERMEDIARY*		// NANDの記号
#define NAND $					// 明示的表現版

#endif