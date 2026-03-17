//==================================================================================================================================
//
//			リザルトの処理 [result.h]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
#ifndef _RESULT_H_
#define _RESULT_H_	// 2重インクルード防止

//==============================================================
// ヘッダーインクルード
#include "main.h"
#include "endomacro.h"

//=========================================================================================
// マクロ定義
//=========================================================================================
#define MAX_RESULT			(32)			// 表示物の最大数
#define MAX_RESULT_TEX		(8)				// テクスチャの最大数
#define MAX_NUMSCORE		(5)				// スコアを表示する数
#define MAX_SCOREDIGIT		(3)				// スコアの桁数
#define SCOREMOVE			(79)			// 取ったスコアが動くフレーム数

//**************************************************************
// 操作
#define KEY_ENTER			DIK_RETURN		// キーボード操作で次へ
#define KEY_BACK			DIK_A			// 前の画面へ
#define JOY_ENTER			JOYKEY_A		// コントローラー操作で次へ


//==============================================================
// リザルトテクスチャ
//==============================================================
typedef enum
{
	RESULTTEX_BG = 0,						// 背景
	RESULTTEX_RESULT,						// RESULT
	RESULTTEX_SCORE,						// SCORE
	RESULTTEX_ITEMCOUNT,					// 納品数
	RESULTTEX_RANK,							// RANKING
	RESULTTEX_RANKNUM,						// 順位
	RESULTTEX_NUM,							// 数字
	RESULTTEX_MAX
}RESULTTEX;

//==============================================================
// リザルトモード
//==============================================================
typedef enum
{
	RESULT_SCORE = 0,						// クリア画面
	RESULT_RANGING,							// ランキング画面
	RESULT_MAX
}RESULTMODE;

//==============================================================
// リザルト情報構造体
//==============================================================
typedef struct RESULT
{
	vec3		pos;
	vec3		size;
	vec3		rot;
	colX		col;
	float		fAngle;
	float		fLength;
	int			nTex;
	int			nTexPos;	// 数字のテクスチャなら何番か
	bool		bUse;
}RESULT;
POINTER(RESULT, P_RESULT);

//=========================================================================================
// プロトタイプ宣言
//=========================================================================================
void InitResult(void);
void UninitResult(void);
void UpdateResult(void);
void DrawResult(void);
P_RESULT GetResult(void);
P_RESULT GetRanking(void);
void SetResultMode(RESULTMODE mode);
P_RESULT SetResult(vec3 pos, vec3 size, vec3 rot, colX col, int nTex, int nTexPos, P_RESULT pResult);

#endif // !_RESULT_H_