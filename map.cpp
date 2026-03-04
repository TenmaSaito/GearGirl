//==================================================================================
//
// DirectXのマップのソースファイル [map.cpp]
// Author : TENMA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "map.h"

// 基本インクルード群
#include "input.h"

// 汎用インクルード群
#include "mathUtil.h"
#include "param.h"

USE_PARAM;

// 通常インクルード群
#include "camera.h"
#include "tCamera.h"
#include "player.h"
#include "field.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_ANGLE			(45.0f)		// 視野角

//**********************************************************************************
//*** マップカメラ構造体 ***
//**********************************************************************************
typedef TCamera MapCamera;
typedef MapCamera *LPMAPCAMERA, *PMAPCAMERA;

//**********************************************************************************
//*** マップ構造体 ***
//**********************************************************************************
STRUCT()
{
	D3DXVECTOR3 mapPos;		// マップの位置
	D3DXVECTOR2 mapSize;	// マップのサイズ
	bool bEnable;			// 有効かどうか
} Map;

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************
D3DVIEWPORT9 CalcViewPort(V3 posMid, V2 size);
LPMAPCAMERA GetMapCamera(void);
void SetMapCamera(void);
void EndMapCamera(void);

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
MapCamera g_mapCamera;		// マップのカメラ
Map g_map;					// マップの情報
D3DVIEWPORT9 g_vpDef;		// 基本のビューポート

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitMap(D3DXVECTOR3 mid, D3DXVECTOR2 size, float fLength, float zn, float zf)
{
	// 自作ユーティリティ使用宣言
	USE_UTIL;

	// 初期化
	AutoZeroMemory(g_map);
	AutoZeroMemory(g_mapCamera);

	g_mapCamera.vp = CalcViewPort(mid, size);
	g_mapCamera.fN = zn;
	g_mapCamera.fF = zf;
	g_mapCamera.fAngle = DEFAULT_ANGLE;
	g_mapCamera.rot = CParamVector::V3NULL;
	g_mapCamera.fLength = fLength;
	g_mapCamera.vecU = VEC_Y(1.0f);
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitMap(void)
{

}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateMap(void)
{
	// マップ無効中は即時終了
	if (g_map.bEnable == false) return;

	// 自作ユーティリティ使用宣言
	USE_UTIL;

	if (GetKeyboardPress(DIK_T))
	{
		g_mapCamera.fLength += 1.0f;
	}
	else if(GetKeyboardPress(DIK_B))
	{
		g_mapCamera.fLength -= 1.0f;
	}

	// マップカメラの取得
	LPMAPCAMERA pMCam = GetMapCamera();

	// 現在のプレイヤータイプ
	PlayerType Type = PLAYERTYPE_GIRL;

	if (GetNumPlayer() == 1)
	{
		Type = (PlayerType)GetActivePlayer();
	}

	D3DXVECTOR3 rot;
	rot.y = GetPosToPos(GetPlayer()[Type].pos, GetCamera()[Type].posV);

	// カメラの位置をプレイヤーの位置と同期
	pMCam->posR = GetPlayer()[Type].pos;

	// 各情報から視点の位置を逆算
	pMCam->posV.x = pMCam->posR.x + 0.001f;
	pMCam->posV.y = pMCam->posR.y + pMCam->fLength;
	pMCam->posV.z = pMCam->posR.z;
}

//==================================================================================
// --- 描画 ---
//==================================================================================
void DrawMap(void)
{
	// マップ無効中は即時終了
	if (g_map.bEnable == false) return;

	// マップカメラの設置
	SetMapCamera();

	// 地面の描画
	DrawField();

	// プレイヤーの描画
	DrawPlayer();

	// マップカメラの破棄
	EndMapCamera();
}

//==================================================================================
// --- 有効化 ---
//==================================================================================
void SetEnableMap(bool bEnable)
{
	g_map.bEnable = bEnable;
}

//==================================================================================
// --- ビューポート計算 ---
//==================================================================================
START_UNABLE
UNABLE_THISFILE(FLOATTOINT)
D3DVIEWPORT9 CalcViewPort(V3 posMid, V2 size)
{
	D3DVIEWPORT9 vp;

	vp.X = posMid.x - (size.x * 0.5f);
	vp.Y = posMid.y - (size.y * 0.5f);

	vp.Width = size.x;
	vp.Height = size.y;

	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;

	return vp;
}
END_UNABLE

//==================================================================================
// --- マップカメラ取得 ---
//==================================================================================
LPMAPCAMERA GetMapCamera(void)
{
	return &g_mapCamera;
}

//==================================================================================
// --- マップカメラ設置開始 ---
//==================================================================================
void SetMapCamera(void)
{
	// マップカメラの取得
	LPMAPCAMERA pMCam = GetMapCamera();

	// デバイスの取得
	AUTODEVICE9 Auto;		// デバイスの自動解放用変数
	HRESULT hr;

	hr = Auto.pDevice->Clear(0, NULL,
		(D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	// ビューポート取得
	hr = Auto.pDevice->GetViewport(&g_vpDef);

	// ビューポート指定
	hr = Auto.pDevice->SetViewport(&pMCam->vp);

	// プロジェクションマトリックスの初期化
	pMCam->mtxProj = Constants::CParamEx::MTX_IDENTITY;

	// プロジェクションマトリックスを作成
	D3DXMatrixPerspectiveFovLH(&pMCam->mtxProj,
		D3DXToRadian(pMCam->fAngle),						// 視野角
		(float)pMCam->vp.Width / (float)pMCam->vp.Height,	// アスペクト比
		pMCam->fN,											// 最短描画距離
		pMCam->fF);											// 最大描画距離

	// プロジェクションマトリックスを設定
	hr = Auto.pDevice->SetTransform(D3DTS_PROJECTION, &pMCam->mtxProj);

	// ビューマトリックスの初期化
	pMCam->mtxView = Constants::CParamEx::MTX_IDENTITY;

	// ビューマトリックスの作成
	D3DXMatrixLookAtLH(&pMCam->mtxView,
		&pMCam->posV,	// 視点の位置
		&pMCam->posR,	// 注視点の位置
		&pMCam->vecU);	// 上方向ベクトル

	// ビューマトリックスの設定
	hr = Auto.pDevice->SetTransform(D3DTS_VIEW, &pMCam->mtxView);
}

//==================================================================================
// --- マップカメラ設置終了 ---
//==================================================================================
void EndMapCamera(void)
{
	// デバイスの取得
	AUTODEVICE9 Auto;		// デバイスの自動解放用変数

	// ビューポート設定
	Auto.pDevice->SetViewport(&g_vpDef);
}