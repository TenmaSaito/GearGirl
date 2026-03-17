//==================================================================================
//
// DirectXのエンド分岐用のcppファイル [judgeEnd.cpp]
// Author : TENMA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "judgeEnd.h"
#include "Result.h"
#include "timer.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define ENDLINE_NORMAL		(3)		// 3個以上完璧でノーマルエンド
#define ENDLINE_HAPPY		(5)		// 全て完璧でスーパーハッピーエンド
#define CONTRAINDICATION_LINE	(3)	// 不具合のあるパーツを一定数渡すと則ゲームオーバーになるライン
#define DOPPEL_LINE			(2)		// 同じ種類のパーツが一定回数渡されるとゲームオーバー

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
int g_nScoreJudge;	// マルチエンディング判別用
int g_nItemScore;	// アイテムによる取得スコア
int g_nTimeScore;	// クリアタイムによる取得スコア
int g_nTotalScore;	// スコア合計値

#if FALSE
//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitJudgeEnd(void)
{

}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitJudgeEnd(void)
{

}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateJudgeEnd(void)
{

}

//==================================================================================
// --- 描画 ---
//==================================================================================
void DrawJudgeEnd(void)
{

}
#endif
//==================================================================================
// --- エンディング判定 ---
//==================================================================================
int JudgmentEnding(ITEMTYPE *pIn, UINT size)
{
	ENDTYPE result = ENDTYPE_BAD;		// エンディングタイプ
	ITEMTYPE aType[ITEMTYPE_MAX];		// 総合のアイテム種類
	ITEMTYPE aTypeTrue[ITEMTYPE_MAX];	// アイテムの種類
	UINT nCntTrue = 0;					// 正しいアイテムの数
	ITEMTYPE aTypeFalse[ITEMTYPE_MAX];	// アイテムの種類
	UINT nCntFalse = 0;					// 間違ったアイテムの数

	// 現在のスコアをリセット
	g_nScoreJudge = 0;
	g_nItemScore = 0;

	// 値保存
	for (UINT nCntItem = 0; nCntItem < size; nCntItem++)
	{
		if (pIn[nCntItem] < 5)
		{
			aTypeTrue[nCntTrue] = pIn[nCntItem];
			nCntTrue++;
			g_nScoreJudge++;
		}
		else
		{
			aTypeFalse[nCntFalse] = pIn[nCntItem];
			nCntFalse++;
		}

		aType[nCntItem] = pIn[nCntItem];
	}

	// === スコア計算 === //
	g_nItemScore += nCntFalse * 400;	// 最大で2000点
	g_nItemScore += nCntTrue * 4000;	// 最大で20000点

	if (nCntTrue == 5)
	{// すべて正しいアイテムならば追加点
		g_nItemScore += 10000;
	}

	// かかった時間を取得
	int nClearTime = GetTimer();

	// 基準値からクリアタイムを引いた値をスコアに加算
	g_nTimeScore +=  30000 - nClearTime;

	// アイテムスコアとタイムスコアを合算
	g_nTotalScore = g_nItemScore + g_nTimeScore;	// 最大で60000点(不可能ではある)


	// バッドエンド判定[1]
	if (nCntFalse >= CONTRAINDICATION_LINE)
	{
		result = ENDTYPE_BAD;
		SetEndingType(result);
		return ENDTYPE_BAD;
	}

	int nCountTrue = 0;

	// バッドエンド判定[2]
	for (UINT nCntItem = 0; nCntItem < size; nCntItem++)
	{
		for (UINT nCntItemCover = nCntItem; nCntItemCover < size; nCntItemCover++)
		{
			if (nCntItem == nCntItemCover) continue;

			if (aType[nCntItem] == aType[nCntItemCover])
			{
				nCountTrue++;
				if (nCountTrue >= DOPPEL_LINE)
				{
					result = ENDTYPE_BAD;
					SetEndingType(result);
					return ENDTYPE_BAD;
				}
			}
		}
	}

	// ノーマル,ハッピーエンド判定
	if (nCntTrue >= ENDLINE_HAPPY && nCountTrue == 0)
	{
		result = ENDTYPE_HAPPY;
		SetEndingType(result);
		return ENDTYPE_HAPPY;
	}
	else
	{
		result = ENDTYPE_NORMAL;
		SetEndingType(result);
		return ENDTYPE_NORMAL;
	}

	// 例外
	result = ENDTYPE_BAD;
	SetEndingType(result);

	return ENDTYPE_BAD;
}