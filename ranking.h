//==================================================================================================================================
//
//			リザルトの処理 [ranking.h]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
#ifndef _RANKING_H_
#define _RANKING_H_	// 2重インクルード防止

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
#define MAX_SCOREDIGIT		(5)				// スコアの桁数
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
	RESULTTEX_RESULT = 0,					// RESULT
	RESULTTEX_SCORE,						// SCORE
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
	RESULT_NONE = 0,					// ゼロ
	RESULT_SCORE,						// クリア画面
	RESULT_RANGING,						// ランキング画面
	RESULT_MAX
}RESULTMODE;

//==============================================================
// リザルト切り替え
//==============================================================
typedef enum
{
	RESULTFADE_NONE = 0,
	RESULTFADE_IN,
	RESULTFADE_OUT
}RESULTFADE;

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
void InitRanking(void);
void UninitRanking(void);
void UpdateRanking(void);
void DrawRanking(void);
P_RESULT GetThisScore(void);
P_RESULT GetRanking(void);
P_RESULT GetThisModeRanking(void);
void SetResultMode(RESULTMODE next);
P_RESULT SetResult(vec3 pos, vec3 size, vec3 rot, colX col, int nTex, int nTexPos, P_RESULT pResult);

#endif // !_RANKING_H_