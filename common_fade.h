//================================================================================================================
//
// DirectXの通常フェード用ヘッダファイル [common_fade.h]
// Author : TENMA
//
//================================================================================================================
#ifndef _COMMON_FADE_H_
#define _COMMON_FADE_H_

//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include "fade.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define NORMAL_FADECOUNT	(100)								// 通常のフェード時間
#define NORMAL_FADE			D3DXCOLOR(0.0f,0.0f,0.0f,0.0f)		// 通常の暗転フェード

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************
void InitCommonFade(void);
void UninitCommonFade(void);
void UpdateCommonFade(void);
void DrawCommonFade(void);

/// <summary>
/// モード移行しないただのフェード再生
/// </summary>
/// <param name="nCounterFadeOut">フェードアウトにかかる時間</param>
/// <param name="nWaitFadeIn">フェードインするまでの待機時間</param>
/// <param name="nCounterFadeIn">フェードインにかかる時間</param>
void SetCommonFade(int nCounterFadeOut = NORMAL_FADECOUNT,
	int nWaitFadeIn = NORMAL_FADECOUNT,
	int nCounterFadeIn = NORMAL_FADECOUNT,
	D3DXCOLOR col = D3DXCOLOR(0, 0, 0, 0));

FADE GetCommonFade(void);
#endif
