//==================================================================================
//
// アイテム獲得時演出のcppファイル [UIGet.cpp]
// Author : TENMA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "UIGet.h"

#include "mathUtil.h"
#include "Texture.h"
#include "2Dpolygon.h"
#include "Color_defs.h"
#include "item.h"

USE_UTIL;

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************

//**********************************************************************************
//*** 獲得情報構造体 ***
//**********************************************************************************
STRUCT()
{
	D3DXCOLOR col;		// 色
	float s;			// Lerp変換
	int sign;			// 符号
	int nCounter;		// カウンター
	IDX_2DPOLYGON poly;	// ポリゴンインデックス
	bool bEnable;		// 描画状態
	bool bFinish;		// 描画終了フラグ
} UIGet;

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************

//**********************************************************************************
//*** 定数変数 ***
//**********************************************************************************
const D3DXVECTOR3 g_posBefore = D3DXVECTOR3(WINDOW_MID.x, WINDOW_MID.y + 100.0f, 0.0f);	// UIの開始位置
const D3DXVECTOR3 g_posAfter = D3DXVECTOR3(WINDOW_MID.x, WINDOW_MID.y + 150.0f, 0.0f);	// UIの最終位置
const D3DXVECTOR2 g_sizeEffectUI = D3DXVECTOR2(400, 140);	// UIのサイズ

const char *g_apUIGetTexture[ITEMTYPE_MAX] =		// 各取得時のテクスチャのパス
{
	"data/TEXTURE/GetUI/Get_Screw.png",
	"data/TEXTURE/GetUI/Get_SmallGear.png",
	"data/TEXTURE/GetUI/Get_BigGear.png",
	"data/TEXTURE/GetUI/Get_Shaft.png",
	"data/TEXTURE/GetUI/Get_Spring.png",
	"data/TEXTURE/GetUI/Get_OldScrew.png",
	"data/TEXTURE/GetUI/Get_RustedSmallGear.png",
	"data/TEXTURE/GetUI/Get_RustedBigGear.png",
	"data/TEXTURE/GetUI/Get_BentShaft.png",
	"data/TEXTURE/GetUI/Get_DistortedSpring.png",
};

const int g_nCountFadeOutUI = 300;		// 獲得UIが消えるまでの待機時間
const float g_fIncreaseS = 0.05f;		// 増加係数

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
UIGet g_aUIGet[ITEMTYPE_MAX] = {};		// 取得時UIの情報

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitUIGet(void)
{
	// 初期化
	AutoZeroMemory(g_aUIGet);

	for (int nCntUI = 0; nCntUI < ITEMTYPE_MAX; nCntUI++)
	{
		IDX_TEXTURE tex;
		LoadTexture(g_apUIGetTexture[nCntUI], &tex);
		g_aUIGet[nCntUI].poly = Set2DPolygon(g_posBefore, VECNULL, g_sizeEffectUI, tex);
		SetEnable2DPolygon(g_aUIGet[nCntUI].poly, false);
		SetColor2DPolygon(g_aUIGet[nCntUI].poly, COLOR_INV);
		g_aUIGet[nCntUI].bEnable = false;
		g_aUIGet[nCntUI].bFinish = false;
		g_aUIGet[nCntUI].nCounter = g_nCountFadeOutUI;
	}
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitUIGet(void)
{

}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateUIGet(void)
{
	P_ITEM pItem = GetItem();

	for (int nCntItem = 0; nCntItem < ITEMTYPE_MAX; nCntItem++, pItem++)
	{
		if (pItem->bGet == true)
		{
			UIGet* pUIGet = &g_aUIGet[pItem->type];
			if (pUIGet->bFinish == true) continue;

			if (pUIGet->bEnable == false)
			{ // 初獲得状態なら
				SetEnable2DPolygon(pUIGet->poly, true);
				SetPriority2DPolygon(pUIGet->poly);
				pUIGet->bEnable = true;
				pUIGet->sign = 1;
			}
			else
			{ // 獲得後
				if (pUIGet->sign == 1 && pUIGet->s >= 1.0f)
				{
					pUIGet->s = 1.0f;
					pUIGet->nCounter--;
					if (pUIGet->nCounter <= 0)
					{
						pUIGet->sign *= -1;
					}
				}
				else if (pUIGet->sign == -1 && pUIGet->s <= 0.0f)
				{
					pUIGet->bFinish = true;
					SetEnable2DPolygon(pUIGet->poly, false);
				}
				else
				{
					pUIGet->s += g_fIncreaseS * pUIGet->sign;
					SetPosition2DPolygon(pUIGet->poly, GetPTPLerp(g_posBefore, g_posAfter, pUIGet->s));
					SetColor2DPolygon(pUIGet->poly, GetColLerp(COLOR_INV, COLOR_WHITE, pUIGet->s));
				}
			}
		}
	}
}