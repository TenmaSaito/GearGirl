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

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define ENDLINE_NORMAL		(3)		// 3個以上完璧でノーマルエンド
#define ENDLINE_HAPPY		(5)		// 全て完璧でスーパーハッピーエンド
#define CONTRAINDICATION_LINE	(3)		// 不具合のあるパーツを一定数渡すと則ゲームオーバーになるライン
#define DOPPEL_LINE			(2)		// 同じ種類のパーツが一定回数渡されるとゲームオーバー

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
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
void JudgmentEnding(ITEMTYPE *pIn, UINT size)
{
	ENDTYPE result = ENDTYPE_BAD;		// エンディングタイプ
	ITEMTYPE aTypeTrue[ITEMTYPE_MAX];	// アイテムの種類
	UINT nCntTrue = 0;					// 正しいアイテムの数
	ITEMTYPE aTypeFalse[ITEMTYPE_MAX];	// アイテムの種類
	UINT nCntFalse = 0;					// 間違ったアイテムの数

	// 値保存
	for (UINT nCntItem = 0; nCntItem < size; nCntItem++)
	{
		if (pIn[nCntItem] < 5)
		{
			aTypeTrue[nCntTrue] = pIn[nCntItem];
			nCntTrue++;
		}
		else
		{
			aTypeFalse[nCntFalse] = pIn[nCntItem];
			nCntFalse++;
		}
	}

	// バッドエンド判定[1]
	if (nCntFalse >= CONTRAINDICATION_LINE)
	{
		result = ENDTYPE_BAD;
		SetEndingType(result);
		return;
	}

	bool aSuffer[DOPPEL_LINE];
	int nCountTrue = 0;

	// バッドエンド判定[2]
	for (UINT nCntItem = 0; nCntItem < nCntTrue; nCntItem++)
	{
		for (UINT nCntItemFalse = 0; nCntItemFalse < nCntFalse; nCntItemFalse++)
		{
			if (aTypeTrue[nCntItem] == aTypeFalse[nCntItemFalse])
			{
				aSuffer[nCountTrue] = true;
				nCountTrue++;
				if (nCountTrue >= DOPPEL_LINE)
				{
					result = ENDTYPE_BAD;
					SetEndingType(result);
					return;
				}
			}
		}
	}

	// ノーマル,ハッピーエンド判定
	if (nCntTrue >= ENDLINE_HAPPY)
	{
		result = ENDTYPE_HAPPY;
		SetEndingType(result);
		return;
	}
	else
	{
		result = ENDTYPE_NORMAL;
		SetEndingType(result);
		return;
	}

	// 例外
	result = ENDTYPE_BAD;
	SetEndingType(result);
}