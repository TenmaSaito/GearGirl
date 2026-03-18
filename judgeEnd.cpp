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
int JudgmentEnding(ITEMTYPE* pIn, UINT size)
{
	// === 変数を宣言 === //
	ENDTYPE result = ENDTYPE_BAD;		// エンディングタイプ
	ITEMTYPE aType[ITEMTYPE_MAX];		// 総合のアイテム種類
	ITEMTYPE aTypeTrue[ITEMTYPE_MAX];	// アイテムの種類
	UINT nCntTrue = 0;					// 正しいアイテムの数
	ITEMTYPE aTypeFalse[ITEMTYPE_MAX];	// アイテムの種類
	UINT nCntFalse = 0;					// 間違ったアイテムの数
	int nGetHighMag = 0;				// 高倍率アイテムをとった個数を保管

	// 現在のスコアをリセット
	g_nScoreJudge = 0;
	g_nItemScore = 0;

	// 値保存
	for (UINT nCntItem = 0; nCntItem < size; nCntItem++)
	{// 提出した回数分回す
		if (pIn[nCntItem] < ITEMTYPE_SPRING_TRUE)
		{// trueアイテムの処理
			aTypeTrue[nCntTrue] = pIn[nCntItem];
			nCntTrue++;
			g_nScoreJudge++;
		}
		else
		{// falseアイテムの処理
			aTypeFalse[nCntFalse] = pIn[nCntItem];
			nCntFalse++;
		}

		// 用意したローカル変数に情報を代入
		aType[nCntItem] = pIn[nCntItem];

		// 特定のアイテムは特殊倍率に
		if (pIn[nCntItem] == ITEMTYPE_SCREW_TRUE)
		{// 高級なネジ
			nGetHighMag++;	// 個数を増やす
			nCntTrue--;
		}

		if (pIn[nCntItem] == ITEMTYPE_SCREW_FALSE)
		{// ノーマルネジ
			// 個数調整
			nCntFalse--;
			nCntTrue++;
		}
	}

	// === 変数宣言 === //
	float fInfMag = 1.0f + (((float)nCntTrue * 3) / 10.0f);	// 倍率を保存(最大で2.5倍)
	float fSupMag = 1.0f + ((float)nCntFalse / 10.0f);		// 倍率を保存(最大で1.5倍)
	float fHighMag = 1.0f + ((float)nGetHighMag * 5) / 10.0f;	// 高倍率アイテム取得フラグが立っているときの特殊倍率

	float fTotalMag = fInfMag + fSupMag + fHighMag;

	// 残った時間を取得
	g_nTimeScore = GetTimer();

	// === スコア計算 === //
	// クリアタイム(残ったフレーム数)に倍率を掛けて、スコアに加算
	g_nTimeScore = g_nTimeScore * fTotalMag;

	if (nCntTrue == 4 && nGetHighMag == 1)
	{// すべて正しいアイテムならば追加点
		g_nItemScore += 10000;
	}

	// アイテムスコアとタイムスコアを合算
	g_nTotalScore = g_nItemScore + g_nTimeScore;	// 最大で60000点(不可能ではある)

	if (size == 0)
	{// アイテムを獲得せず提出すると0点に
		g_nTotalScore = 0;
	}

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

//==================================================================================
// --- 獲得スコアを渡す ---
//==================================================================================
int GetScore(void)
{
	return g_nTotalScore;
}