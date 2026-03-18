//==================================================================================
//
// ショップでのアイテム提出時のcppファイル [UIShop.cpp]
// Author : TENMA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "UIShop.h"

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
	IDX_TEXTURE aTex[ITEMTYPE_MAX];		// 各アイテムのテクスチャインデックス
	IDX_2DPOLYGON poly;		// ポリゴンインデックス
	ITEMTYPE selected;		// 選択されているアイテム
	bool bEnable;			// 描画状態
} UIShop;

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************

//**********************************************************************************
//*** 定数変数 ***
//**********************************************************************************
const D3DXVECTOR3 g_posUIShop = D3DXVECTOR3(WINDOW_MID.x, WINDOW_MID.y + 250.0f, 0.0f);	// UIの位置
const D3DXVECTOR2 g_sizeUIShop = D3DXVECTOR2(500, 110);	// UIのサイズ

const char* g_apUIShopTexture[ITEMTYPE_MAX] =		// 各取得時のテクスチャのパス
{
	"data/TEXTURE/ShopUI/Shop_Screw.png",
	"data/TEXTURE/ShopUI/Shop_SmallGear.png",
	"data/TEXTURE/ShopUI/Shop_BigGear.png",
	"data/TEXTURE/ShopUI/Shop_Shaft.png",
	"data/TEXTURE/ShopUI/Shop_Spring.png",
	"data/TEXTURE/ShopUI/Shop_OldScrew.png",
	"data/TEXTURE/ShopUI/Shop_RustedSmallGear.png",
	"data/TEXTURE/ShopUI/Shop_RustedBigGear.png",
	"data/TEXTURE/ShopUI/Shop_BentShaft.png",
	"data/TEXTURE/ShopUI/Shop_DistortedSpring.png",
};

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
UIShop g_UIShop = {};		// UIShopの情報

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitUIShop(void)
{
	// 初期化
	AutoZeroMemory(g_UIShop);

	for (int nCntUI = 0; nCntUI < ITEMTYPE_MAX; nCntUI++)
	{
		// テクスチャの読み込み及びインデックスの取得
		LoadTexture(g_apUIShopTexture[nCntUI], &g_UIShop.aTex[nCntUI]);
	}

	// 2Dポリゴンの設置
	g_UIShop.poly = Set2DPolygon(g_posUIShop, VECNULL, g_sizeUIShop);
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitUIShop(void)
{

}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateUIShop(void)
{
	// 提出状態ではない
	if (!IsEnableItemPut())
	{
		SetEnable2DPolygon(g_UIShop.poly, false);
		return;
	}

	// 選択中アイテムがない場合、終了
	if (GetNumSelect() == -1)
	{
		SetEnable2DPolygon(g_UIShop.poly, false);
		SetTexture2DPolygon(g_UIShop.poly, -1);
		return;
	}
	
	// 提出状態の時
	SetEnable2DPolygon(g_UIShop.poly, true);
	SetTexture2DPolygon(g_UIShop.poly, g_UIShop.aTex[GetNumSelect()]);
}