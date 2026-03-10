//==================================================================================
//
// 自作カラー関数ヘッダファイル [MyCol.h]
// Author : TENMA
//
//==================================================================================
#ifndef _MYCOL_H_		// インクルードガード
#define _MYCOL_H_		// インクルードガード

#include "main.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define USE_COL			using namespace MyLib;				// カラー関数使用宣言
#define DEF_COL			D3DXCOLOR(1, 1, 1, 1)				// デフォルトカラー
#define D3DCOLOR_NULL	(D3DCOLOR_RGBA(255, 255, 255, 255)) // D3DCOLOR_NULL

//==================================================================================
// 
// --- 自作カラー関数 ---
// 
//==================================================================================
namespace MyLib
{
	//------------------------------------------------------------
	// --- 定数カラー設定 --- 
	//------------------------------------------------------------
	inline void SetConstantTexColor(D3DXCOLOR col)
	{
		// デバイス取得
		AUTODEVICE9 AD9;

		// テクスチャに追加乗算
		// 定数カラーを設定
		AD9.pDevice->SetTextureStageState(0, D3DTSS_CONSTANT, col);

		// 先程設定した定数カラーを使って計算
		// Result(ポリゴンの色) = COLORARG1(テクスチャの色) * CONSTANT[New!](設定されている定数カラー)
		AD9.pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CONSTANT);
	}

	//------------------------------------------------------------
	// --- 定数カラー設定終了 --- 
	//------------------------------------------------------------
	inline void EndConstantTexColor(void)
	{
		// デバイス取得
		AUTODEVICE9 AD9;

		// テクスチャの追加乗算終了
		// 定数カラーの値を既定値に戻す
		AD9.pDevice->SetTextureStageState(0, D3DTSS_CONSTANT, 0xFFFFFFFF);

		// 計算に使う値を頂点カラーに設定
		// Result = COLORARG1 * COLORARG2(頂点カラー)
		AD9.pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	}
}
#endif