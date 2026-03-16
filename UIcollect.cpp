//==================================================================================
//
// DirectXの目的及び回収数のUI表示処理 [UIcollect.cpp]
// Author : TENMA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "UIcollect.h"

#include "Texture.h"
#include "mathUtil.h"

#include "UImenu.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#if FALSE
#define DRAW_NUM		2		// 二つとも描画
#else
#define DRAW_NUM		1		// 片方のみ描画
#endif

//**********************************************************************************
//*** プロンプト構造体 ***
//**********************************************************************************
typedef struct
{
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff;	// 頂点バッファへのポインタ
	D3DXVECTOR3 pos;					// 位置
	D3DXVECTOR3 rot;					// 角度
	D3DXVECTOR2 size;					// サイズ
	IDX_TEXTURE texInfo;				// 目標のテクスチャインデックス
	IDX_TEXTURE texNum;					// 現在の数のテクスチャインデックス
	bool bUse;							// 使用状況
	bool bEnable;						// UIとして描画するか
} UIcollect, * LPUICOLLECT;

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************

//**********************************************************************************
//*** 定数変数 ***
//**********************************************************************************
const D3DXVECTOR3 g_posMiddleCollectUI = D3DXVECTOR3(1050, 60, 0);		// UIの中心座標
const D3DXVECTOR2 g_sizeCollectUI = D3DXVECTOR2(450, 100);				// ポリゴンの基本サイズ
const char *g_aCollectUITexture[2] =
{
	"data/TEXTURE/GameInfo.png",		// 目標のテクスチャパス
	"data/TEXTURE/Collected.png",		// 現在のテクスチャパス
};

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
UIcollect g_collectUI;

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitUIcollect(void)
{
	LPUICOLLECT pUICollect = &g_collectUI;

	ZeroMemory(&g_collectUI, sizeof(g_collectUI));

	// 初期設定
	pUICollect->pos = g_posMiddleCollectUI;
	pUICollect->size = g_sizeCollectUI;

	/*** デバイスの取得 ***/
	AUTODEVICE9 Auto;							// デバイス自動解放システム
	LPDIRECT3DDEVICE9 pDevice = Auto.pDevice;	// 自動解放システムを介してデバイスを取得

	// 頂点生成
	pDevice->CreateVertexBuffer(CREATE_2DPOLYGON(pUICollect->pVtxBuff));

	// テクスチャの読み込み
	if (FAILED(LoadTexture(g_aCollectUITexture[0], &pUICollect->texInfo)))
	{
		OutputDebugString(TEXT("テクスチャの読み込みに失敗しました......"));
	}

	/*if (FAILED(LoadTexture(g_aCollectUITexture[1], &pUICollect->texNum)))
	{
		OutputDebugString(TEXT("テクスチャの読み込みに失敗しました......"));
	}*/

	VERTEX_2D *pVtx = nullptr;

	// 頂点設定
	pUICollect->pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);
	if (pVtx == nullptr)
	{
		OutputDebugString(TEXT("頂点バッファへのアクセスに失敗しました？！"));
		return;
	}

	// 頂点位置を設定
	MyMathUtil::SetPolygonPos(pVtx, pUICollect->pos, pUICollect->size);

	// 座標変換用値を設定
	MyMathUtil::SetPolygonRHW(pVtx);

	// ポリゴンカラーを設定
	MyMathUtil::SetDefaultColor(pVtx);

	// ポリゴンテクスチャを設定
	MyMathUtil::SetDefaultTexture(pVtx);

	pUICollect->pVtxBuff->Unlock();

	pUICollect->bUse = true;
	pUICollect->bEnable = true;
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitUIcollect(void)
{
	LPUICOLLECT pUICollect = &g_collectUI;

	RELEASE(pUICollect->pVtxBuff);
}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateUIcollect(void)
{
	LPUICOLLECT pUICollect = &g_collectUI;

	if (GetEnableUImenu() == true && pUICollect->bEnable == true)
	{
		pUICollect->bEnable = false;
	}
	else if (GetEnableUImenu() == false && pUICollect->bEnable == false)
	{
		pUICollect->bEnable = true;
	}
}

//==================================================================================
// --- 描画 ---
//==================================================================================
void DrawUIcollect(void)
{
	/*** デバイスの取得 ***/
	AUTODEVICE9 pAuto;							// デバイス自動解放システム
	LPDIRECT3DDEVICE9 pDevice = pAuto.pDevice;	// 自動解放システムを介してデバイスを取得
	LPUICOLLECT pUICollect = &g_collectUI;

	if (pUICollect->bUse == true)
	{
		/*** アルファテストを有効にする ***/
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// アルファテストを有効
		pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// 基準値よりも大きい場合にZバッファに書き込み
		pDevice->SetRenderState(D3DRS_ALPHAREF, 60);				// 基準値

		if (pUICollect->bUse & pUICollect->bEnable)
		{
			/*** 頂点バッファをデータストリームに設定 ***/
			pDevice->SetStreamSource(0, pUICollect->pVtxBuff, 0, sizeof(VERTEX_2D));

			/*** 頂点フォーマットの設定 ***/
			pDevice->SetFVF(FVF_VERTEX_2D);

			/*** テクスチャの設定 ***/
			pDevice->SetTexture(0, GetTexture(pUICollect->texInfo));

			/*** ポリゴンの描画 ***/
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,		// プリミティブの種類
				0,											// 描画する最初の頂点インデックス
				2);											// 描画するプリミティブの数
		}

		/*** アルファテストを無効にする ***/
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);		// アルファテストを無効化
		pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);	// 無条件にZバッファに書き込み
		pDevice->SetRenderState(D3DRS_ALPHAREF, 0);					// 基準値
	}
}

//==================================================================================
// --- メニュー画面のオンオフ ---
//==================================================================================
void SetEnableUIcollect(bool bDisp)
{
	LPUICOLLECT pUICollect = &g_collectUI;

	pUICollect->bEnable = bDisp;
}

//==================================================================================
// --- メニュー画面のオンオフ取得 ---
//==================================================================================
bool GetEnableUIcollect(void)
{
	LPUICOLLECT pUICollect = &g_collectUI;

	return pUICollect->bEnable;
}