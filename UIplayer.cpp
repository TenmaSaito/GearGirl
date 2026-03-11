//==================================================================================
//
// DirectXのPlayerのUI表示処理 [UIplayer.cpp]
// Author : TENMA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "UIplayer.h"

#include "Texture.h"
#include "mathUtil.h"

#include "UImenu.h"
#include "player.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define PATH_MOUSE		"data/TEXTURE/PlayerIcon_Mouse.png"	// ネズミのテクスチャパス
#define PATH_GIRL		"data/TEXTURE/PlayerIcon_Girl.png"	// 少女のテクスチャパス
#define DEF_UI_SIZE		(D3DXVECTOR2(130, 130))				// ポリゴンの基本サイズ

//**********************************************************************************
//*** プロンプト構造体 ***
//**********************************************************************************
typedef struct
{
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff;	// 頂点バッファへのポインタ
	D3DXVECTOR3 pos;					// 位置
	D3DXVECTOR3 rot;					// 角度
	D3DXVECTOR2 size;					// サイズ
	IDX_TEXTURE texMouse;				// ネズミのテクスチャインデックス
	IDX_TEXTURE texGirl;				// 少女のテクスチャインデックス
	float s;							// Lerp変換用
	float fAngle;						// 回転用
	bool bUse;							// 使用状況
	bool bEnable;						// UIとして描画するか
} UIplayer, * LPUIPLAYER;

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
UIplayer g_playerUI;

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitUIplayer(void)
{
	LPUIPLAYER pUIPlayer = &g_playerUI;

	ZeroMemory(&g_playerUI, sizeof(g_playerUI));

	// 初期設定
	pUIPlayer->pos = GetPositionUImenu();
	pUIPlayer->size = DEF_UI_SIZE;
	pUIPlayer->s = 0.001f;

	/*** デバイスの取得 ***/
	AUTODEVICE9 Auto;							// デバイス自動解放システム
	LPDIRECT3DDEVICE9 pDevice = Auto.pDevice;	// 自動解放システムを介してデバイスを取得

	// 頂点生成
	pDevice->CreateVertexBuffer(CREATE_2DPOLYGON(pUIPlayer->pVtxBuff));

	// テクスチャの読み込み
	if (FAILED(LoadTexture(PATH_MOUSE, &pUIPlayer->texMouse)))
	{
		OutputDebugString(TEXT("テクスチャの読み込みに失敗しました......"));
		return;
	}

	if (FAILED(LoadTexture(PATH_GIRL, &pUIPlayer->texGirl)))
	{
		OutputDebugString(TEXT("テクスチャの読み込みに失敗しました......"));
		return;
	}

	VERTEX_2D* pVtx = nullptr;

	// 頂点設定
	pUIPlayer->pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);
	if (pVtx == nullptr)
	{
		OutputDebugString(TEXT("頂点バッファへのアクセスに失敗しました？！"));
		return;
	}

	// 頂点位置を設定
	MyMathUtil::SetPolygonPos(pVtx, pUIPlayer->pos, pUIPlayer->size);

	// 座標変換用値を設定
	MyMathUtil::SetPolygonRHW(pVtx);

	// ポリゴンカラーを設定
	MyMathUtil::SetDefaultColor(pVtx);

	// ポリゴンテクスチャを設定
	MyMathUtil::SetDefaultTexture(pVtx);

	pUIPlayer->pVtxBuff->Unlock();

	pUIPlayer->bUse = true;
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitUIplayer(void)
{
	LPUIPLAYER pUIPlayer = &g_playerUI;

	RELEASE(pUIPlayer->pVtxBuff);
}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateUIplayer(void)
{
	LPUIPLAYER pUIPlayer = &g_playerUI;

	if (GetEnableUImenu() == true && pUIPlayer->bEnable == true)
	{
		pUIPlayer->bEnable = false;
	}
	else if (GetEnableUImenu() == false && pUIPlayer->bEnable == false)
	{
		pUIPlayer->bEnable = true;
	}
}

//==================================================================================
// --- 描画 ---
//==================================================================================
void DrawUIplayer(void)
{
	/*** デバイスの取得 ***/
	AUTODEVICE9 pAuto;							// デバイス自動解放システム
	LPDIRECT3DDEVICE9 pDevice = pAuto.pDevice;	// 自動解放システムを介してデバイスを取得
	LPUIPLAYER pUIPlayer = &g_playerUI;

	if (pUIPlayer->bUse == true)
	{
		/*** アルファテストを有効にする ***/
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// アルファテストを有効
		pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// 基準値よりも大きい場合にZバッファに書き込み
		pDevice->SetRenderState(D3DRS_ALPHAREF, 60);				// 基準値

		if (pUIPlayer->bUse & pUIPlayer->bEnable)
		{
			/*** 頂点バッファをデータストリームに設定 ***/
			pDevice->SetStreamSource(0, pUIPlayer->pVtxBuff, 0, sizeof(VERTEX_2D));

			/*** 頂点フォーマットの設定 ***/
			pDevice->SetFVF(FVF_VERTEX_2D);

			/*** 現在のプレイヤーのテクスチャの設定 ***/
			if (GetActivePlayer() == PLAYERTYPE_GIRL)
			{
				pDevice->SetTexture(0, GetTexture(pUIPlayer->texGirl));
			}
			else
			{
				pDevice->SetTexture(0, GetTexture(pUIPlayer->texMouse));
			}

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
void SetEnableUIplayer(bool bDisp, int nIdxUIplayer)
{
	LPUIPLAYER pUIPlayer = &g_playerUI;

	pUIPlayer->bEnable = bDisp;
}

//==================================================================================
// --- メニュー画面のオンオフ取得 ---
//==================================================================================
bool GetEnableUIplayer(void)
{
	LPUIPLAYER pUIPlayer = &g_playerUI;

	return pUIPlayer->bEnable;
}