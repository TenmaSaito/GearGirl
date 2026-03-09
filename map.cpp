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
#include "Texture.h"
#include "param.h"
#include "MyCol.h"

USE_UTIL;
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
//*** マップ上のアイコンの種類 ***
//**********************************************************************************
typedef enum
{
	MAPICONTYPE_SHOP = 0,		// 店のアイコン
	MAPICONTYPE_SBUTTON,		// 小さなボタン
	MAPICONTYPE_LBUTTON,		// 大きなボタン
	MAPICONTYPE_TREE,			// 倒木エリア
	MAPICONTYPE_FOUNTAIN,		// 噴水
	MAPICONTYPE_MAX	
} MAPICONTYPE;

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
//*** マップアイコン構造体 ***
//**********************************************************************************
STRUCT()
{
	LPDIRECT3DVERTEXBUFFER9 pVtx;	// 頂点バッファへのポインタ
	IDX_TEXTURE tex;				// テクスチャインデックス
	D3DXVECTOR3 pos;				// 位置
	D3DXVECTOR2 size;				// サイズ
	D3DXCOLOR col;					// 色
	float s;						// Lerp変換用変数
	bool bReverse;					// Lerp反転
} MapIcon;

//**********************************************************************************
//*** プレイヤーアイコン構造体 ***
//**********************************************************************************
STRUCT(PlayerIcon) PARENT(MapIcon)
{
	LPDIRECT3DVERTEXBUFFER9 pVtxArrow;	// 頂点バッファへのポインタ
	IDX_TEXTURE texArrow;	// テクスチャインデックス
	D3DXVECTOR3 rotArrow;	// 矢印の向き
	D3DXVECTOR2 sizeArrow;	// 矢印の大きさ
} PlayerIcon;

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************
D3DVIEWPORT9 CalcViewPort(V3 posMid, V2 size);
LPMAPCAMERA GetMapCamera(void);
void SetMapCamera(void);
void EndMapCamera(void);

void InitIcon(void);
void UninitIcon(void);
void UpdateIcon(void);
void DrawIcon(void);

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
MapCamera g_mapCamera;		// マップのカメラ
Map g_map;					// マップの情報
D3DVIEWPORT9 g_vpDef;		// 基本のビューポート
const MapCamera MiniMap =	// ミニマップの基本情報
{
	{},
	{},
	{},
	D3DXVECTOR3(1770, 3575, -580),	// 視点
	D3DXVECTOR3(1770, 0, -580),		// 注視点
	D3DXVECTOR3(0, 1, 0),			// 上方向ベクトル
	D3DXVECTOR3(0, 0, 0),			// 角度
	3575,							// 距離
	45,								// 視野角
	1,								// 最短
	20000,							// 最長距離
	0
};

MapIcon g_aMapIcon[MAPICONTYPE_MAX];		// マップアイコンの情報
PlayerIcon g_aPlayerIcon[PLAYERTYPE_MAX];	// プレイヤーアイコンの情報

// マップアイコンのテクスチャパス
const char *g_apMapIconTexture[MAPICONTYPE_MAX] =
{
	// 店のアイコン
	"data/TEXTURE/.png",

	// ギミックアイコン(共通でも可)
	"data/TEXTURE/.png",
	"data/TEXTURE/.png",
	"data/TEXTURE/.png",
	"data/TEXTURE/.png",
};

// プレイヤーアイコンのテクスチャパス
const char *g_apPlayerIconTexture[PLAYERTYPE_MAX] =
{
	// 少女のアイコン
	"data/TEXTURE/GirlIcon.png",

	// ネズミのアイコン
	"data/TEXTURE/MouseIcon.png",
};

// 矢印のアイコン
const char *g_pArrowIconTexture = "data/TEXTURE/ArrowIcon.png";

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

	g_mapCamera = MiniMap;
	g_mapCamera.vp = CalcViewPort(mid, size);
	g_mapCamera.fN = zn;
	g_mapCamera.fF = zf;
	g_mapCamera.fAngle = DEFAULT_ANGLE;
	g_mapCamera.rot = CParamVector::V3NULL;
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

	if (GetKeyboardPress(DIK_UP))
	{
		g_mapCamera.posR.z += 10.0f;
		g_mapCamera.posV.z += 10.0f;
	}
	else if (GetKeyboardPress(DIK_DOWN))
	{
		g_mapCamera.posR.z -= 10.0f;
		g_mapCamera.posV.z -= 10.0f;
	}
	else if (GetKeyboardPress(DIK_LEFT))
	{
		g_mapCamera.posR.x -= 10.0f;
		g_mapCamera.posV.x -= 10.0f;
	}
	else if (GetKeyboardPress(DIK_RIGHT))
	{
		g_mapCamera.posR.x += 10.0f;
		g_mapCamera.posV.x += 10.0f;
	}

	// マップカメラの取得
	LPMAPCAMERA pMCam = GetMapCamera();

	// 現在のプレイヤータイプ
	PlayerType Type = PLAYERTYPE_GIRL;

	if (GetNumPlayer() == 1)
	{
		Type = (PlayerType)GetActivePlayer();
	}

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

//==================================================================================
// --- アイコンの初期化 ---
//==================================================================================
void InitIcon(void)
{
	// デバイスの取得
	AUTODEVICE9 AD9;		// デバイスの自動解放用変数
	MapIcon *pMIcon = &g_aMapIcon[0];
	PlayerIcon *pPIcon = &g_aPlayerIcon[0];
	IDX_TEXTURE tex;
	VERTEX_3D *pVtx;

	// アイコンの作成
	for (int nCntIcon = 0; nCntIcon < MAPICONTYPE_MAX; nCntIcon++, pMIcon++)
	{
		// バッファ作成
		AD9.pDevice->CreateVertexBuffer(CREATE_3DPOLYGON(pMIcon->pVtx));

		// 頂点設定
		pMIcon->pVtx->Lock(0, 0, (void**)&pVtx, 0);

		// 各種設定
		SetPolygonSize(pVtx, pMIcon->size, false);		// 頂点座標設定
		SetPolygonNormal(pVtx, D3DXVECTOR3(0, 1, 0));	// 法線設定
		SetDefaultColor(pVtx);							// 頂点カラー設定
		SetDefaultTexture(pVtx);						// テクスチャ座標設定

		// 頂点設定終了
		pMIcon->pVtx->Unlock();

		// テクスチャ読み込み
		LoadTexture(g_apMapIconTexture[nCntIcon], &tex);
	}

	// プレイヤーアイコン及び矢印アイコンの作成
	for (int nCntIcon = 0; nCntIcon < PLAYERTYPE_MAX; nCntIcon++, pPIcon++)
	{
		// バッファ作成
		AD9.pDevice->CreateVertexBuffer(CREATE_3DPOLYGON(pPIcon->pVtx));
		AD9.pDevice->CreateVertexBuffer(CREATE_3DPOLYGON(pPIcon->pVtxArrow));

		// ******** プレイヤーアイコン ********
		// 頂点設定
		pPIcon->pVtx->Lock(0, 0, (void**)&pVtx, 0);

		// 各種設定
		SetPolygonSize(pVtx, pPIcon->size, false);		// 頂点座標設定
		SetPolygonNormal(pVtx, D3DXVECTOR3(0, 1, 0));	// 法線設定
		SetDefaultColor(pVtx);							// 頂点カラー設定
		SetDefaultTexture(pVtx);						// テクスチャ座標設定

		// 頂点設定終了
		pPIcon->pVtx->Unlock();

		// ******** 矢印アイコン ********
		// 頂点設定
		pPIcon->pVtxArrow->Lock(0, 0, (void**)&pVtx, 0);

		// 各種設定
		SetPolygonSize(pVtx, pPIcon->sizeArrow, false);	// 頂点座標設定
		SetPolygonNormal(pVtx, D3DXVECTOR3(0, 1, 0));	// 法線設定
		SetDefaultColor(pVtx);							// 頂点カラー設定
		SetDefaultTexture(pVtx);						// テクスチャ座標設定

		// 頂点設定終了
		pPIcon->pVtxArrow->Unlock();

		// テクスチャ読み込み
		LoadTexture(g_apPlayerIconTexture[nCntIcon], &tex);
		LoadTexture(g_pArrowIconTexture, &tex);
	}
}

//==================================================================================
// --- アイコンの終了 ---
//==================================================================================
void UninitIcon(void)
{

}

//==================================================================================
// --- アイコンの更新 ---
//==================================================================================
void UpdateIcon(void)
{

}

//==================================================================================
// --- アイコンの描画 ---
//==================================================================================
void DrawIcon(void)
{

}