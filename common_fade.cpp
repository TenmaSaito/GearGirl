//==================================================================================
//
// DirectXの通常フェード処理 [common_fade.cpp]
// Author : TENMA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "common_fade.h"

// 汎用インクルード
#include "mathUtil.h"
#include "param.h"
USE_UTIL;
USE_PARAM;

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************

//**********************************************************************************
//*** 通常フェード構造体 ***
//**********************************************************************************
STRUCT()
{
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff;	// 頂点バッファのポインタ
	FADE type;			// フェードの種類
	D3DXCOLOR col;		// ポリゴンの色
	int nWaitIn;		// フェードアウト後の待機時間
	float fAlphaOut;	// フェードアウトのα値の増減値
	float fAlphaIn;		// フェードインのα値の増減値
} CommonFade, *LPCOMMONFADE;

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
CommonFade g_commonFade;

//==================================================================================
// --- フェードの初期化 ---
//==================================================================================
void InitCommonFade(void)
{
	/*** デバイス取得！ ***/
	AUTODEVICE9 AD9;
	LPDIRECT3DDEVICE9 pDevice = AD9.pDevice;	// デバイス省略版
	LPCOMMONFADE pCommonFade = &g_commonFade;	// データへのポインタ

	// メモリの自動初期化！
	AutoZeroMemory(g_commonFade);

	// データ設定！
	pCommonFade->col = CParamColor::INV_NONE;	// 黒の透明色に！
	pCommonFade->type = FADE_NONE;				// 未フェード状態に設定！

	/*** バッファ作成！ ***/
	pDevice->CreateVertexBuffer(CREATE_2DPOLYGON(pCommonFade->pVtxBuff));

	/*** 頂点設定！ ***/
	VERTEX_2D *pVtx = NULL;

	// 頂点バッファをロック！
	pCommonFade->pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 画面全体ポリゴンに設定！
	SetFullScreenPolygon(pVtx);

	// 頂点バッファをアンロック！
	pCommonFade->pVtxBuff->Unlock();
}

//==================================================================================
// --- フェードの終了 ---
//==================================================================================
void UninitCommonFade(void)
{
	LPCOMMONFADE pCommonFade = &g_commonFade;	// データへのポインタ

	/*** 頂点バッファの破棄 ***/
	RELEASE(pCommonFade->pVtxBuff);
}

//==================================================================================
// --- フェードの更新 ---
//==================================================================================
void UpdateCommonFade(void)
{
	LPCOMMONFADE pCommonFade = &g_commonFade;	// データへのポインタ
	VERTEX_2D *pVtx;							// 頂点情報へのポインタ

	/*** フェードの更新 ***/
	if (pCommonFade->type != FADE_NONE)
	{
		if (pCommonFade->type == FADE_IN)
		{// フェードイン
			if (pCommonFade->nWaitIn > 0)
			{
				pCommonFade->nWaitIn--;
				return;
			}

			pCommonFade->col.a -= pCommonFade->fAlphaIn;	// α値を低下
			if (pCommonFade->col.a <= 0.0f)
			{// フェードイン終了
				pCommonFade->col.a = 0.0f;					// α値を0にリセット
				pCommonFade->type = FADE_NONE;				// フェードを解除
			}
		}
		else if (pCommonFade->type == FADE_OUT)
		{// フェードアウト
			pCommonFade->col.a += pCommonFade->fAlphaOut;	// α値を増加
			if (pCommonFade->col.a >= 1.0f)
			{// フェードアウト終了
				pCommonFade->col.a = 1.0f;					// α値を1に設定
				pCommonFade->type = FADE_IN;				// フェードアウトからフェードインへ移行
			}
		}

		/*** 頂点バッファの設定 ***/
		pCommonFade->pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		/*** 頂点カラー設定 ***/
		SetDefaultColor(pVtx, pCommonFade->col);

		/*** 頂点バッファの設定を終了 ***/
		pCommonFade->pVtxBuff->Unlock();
	}
}

//==================================================================================
// --- フェードの描画 ---
//==================================================================================
void DrawCommonFade(void)
{
	/*** デバイスの取得 ***/
	AUTODEVICE9 AD9;
	LPDIRECT3DDEVICE9 pDevice = AD9.pDevice;	// デバイス省略版
	LPCOMMONFADE pCommonFade = &g_commonFade;	// データへのポインタ

	// 描画の必要がない場合即時終了
	if (pCommonFade->type == FADE_NONE) return;

	// ポリゴンの描画！
	DrawPolygon(pDevice,
		pCommonFade->pVtxBuff,
		NULL,
		sizeof(VERTEX_2D),
		FVF_VERTEX_2D,
		1);
}

//==================================================================================
// --- フェードの設置 ---
//==================================================================================
void SetCommonFade(int nCounterFadeOut,
	int nWaitFadeIn,
	int nCounterFadeIn,
	D3DXCOLOR col)
{
	LPCOMMONFADE pCommonFade = &g_commonFade;	// データへのポインタ！

	/*** フェード設定 ***/
	pCommonFade->type = FADE_OUT;				// フェードアウトに設定！
	pCommonFade->col = col;						// フェードの色を設定！
	pCommonFade->nWaitIn = nWaitFadeIn;			// フェードアウト誤の待機時間を設定！
	pCommonFade->fAlphaIn = (1.0f / (float)nCounterFadeIn);		// フェードインの減数係数を計算！
	pCommonFade->fAlphaOut = (1.0f / (float)nCounterFadeOut);	// フェードアウトの減数係数を計算！
}

//==================================================================================
// --- フェードの取得 ---
//==================================================================================
FADE GetCommonFade(void)
{
	LPCOMMONFADE pCommonFade = &g_commonFade;	// データへのポインタ！

	return pCommonFade->type;
}