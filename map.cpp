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
#include "2Dpolygon.h"
#include "Texture.h"
#include "param.h"
#include "MyCol.h"

USE_UTIL;
USE_PARAM;

// 通常インクルード群
#include "game.h"
#include "camera.h"
#include "tCamera.h"
#include "player.h"
#include "field.h"
#include "UImenu.h"
#include "dialog.h"
#include "gimmick.h"
#include "item.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_ANGLE			(45.0f)		// 視野角
#define MAP_MAGNI				(0.6f)		// サイズの倍率

//**********************************************************************************
//*** マップカメラ構造体 ***
//**********************************************************************************
typedef TCamera MapCamera;
typedef MapCamera* LPMAPCAMERA, * PMAPCAMERA;

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
STRUCT(MapIcon)
{
	LPDIRECT3DVERTEXBUFFER9 pVtx;	// 頂点バッファへのポインタ
	IDX_TEXTURE tex;				// テクスチャインデックス
	D3DXVECTOR3 pos;				// 位置
	D3DXVECTOR3 rot;				// 角度
	D3DXVECTOR2 size;				// サイズ
	D3DXCOLOR col;					// 色
	D3DXMATRIX mtxWorld;			// ワールドマトリックス
	float s;						// Lerp変換用変数
	bool bReverse;					// Lerp反転
	bool bUse;
} MapIcon;

//**********************************************************************************
//*** プレイヤーアイコン構造体 ***
//**********************************************************************************
STRUCT(PlayerIcon) PARENT(MapIcon)
{
	LPDIRECT3DVERTEXBUFFER9 pVtxArrow;	// 頂点バッファへのポインタ
	IDX_TEXTURE texArrow;		// テクスチャインデックス
	D3DXVECTOR3 rotArrow;		// 矢印の向き
	D3DXMATRIX mtxWorldArrow;	// ワールドマトリックス
	D3DXVECTOR2 sizeArrow;		// 矢印の大きさ
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

void DrawMapIcon(void);
void DrawPlayerIcon(void);

MapIcon* GetMapIcon(void);
PlayerIcon* GetPlayerIcon(void);

// ユーザー定義のリテラル演算子(テスト)
D3DXVECTOR3 operator"" _v3(long double val)
{
	return D3DXVECTOR3(val, val, val);
}

D3DXVECTOR3 operator"" _v3(unsigned long long val)
{
	return D3DXVECTOR3(val, val, val);
}

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
MapCamera g_mapCamera;		// マップのカメラ
Map g_map;					// マップの情報
D3DVIEWPORT9 g_vpDef;		// 基本のビューポート
D3DVIEWPORT9 g_vpBefore;	// 変更前のビューポート
MapIcon g_aMapIcon[MAPICONTYPE_MAX];		// マップアイコンの情報
PlayerIcon g_aPlayerIcon[PLAYERTYPE_MAX];	// プレイヤーアイコンの情報
D3DRECT g_mapRect;			// マップサイズ
IDX_2DPOLYGON g_IdxUIMap;	// マップUIのインデックス

//**********************************************************************************
//*** 定数変数 ***
//**********************************************************************************
MapCamera MiniMap =	// ミニマップの基本情報
{
	{},
	{},
	{},
	D3DXVECTOR3(1370.1f, 3050.0f, -365.0f),	// 視点
	D3DXVECTOR3(1370, 0, -365),		// 注視点
	D3DXVECTOR3(0, 1, 0),			// 上方向ベクトル
	D3DXVECTOR3(0, 0, 0),			// 角度
	2550,							// 距離
	45,								// 視野角
	1,								// 最短
	20000,							// 最長距離
	0
};

// マップアイコンの情報
const MapIcon g_aMapIconInfo[MAPICONTYPE_MAX] =
{
	// NULL, -1, 位置, 角度, サイズ, 色, NULL, 0.0f, false, true;
	{NULL, -1, D3DXVECTOR3(1450, 100, -425), 0_v3, D3DXVECTOR2(250, 250), CParamColor::WHITE, CParamEx::MTX_IDENTITY, 0, false, true},
	{NULL, -1, D3DXVECTOR3(573, 100, -900), VECNULL, D3DXVECTOR2(250, 250), CParamColor::WHITE, CParamEx::MTX_IDENTITY, 0, false, true},
	{NULL, -1, D3DXVECTOR3(535, 100, -630), VECNULL, D3DXVECTOR2(250, 250), CParamColor::WHITE, CParamEx::MTX_IDENTITY, 0, false, true},
	{NULL, -1, D3DXVECTOR3(1695, 100, 460), VECNULL, D3DXVECTOR2(250, 250), CParamColor::WHITE, CParamEx::MTX_IDENTITY, 0, false, true},
	{NULL, -1, D3DXVECTOR3(1100, 100, 110), VECNULL, D3DXVECTOR2(250, 250), CParamColor::WHITE, CParamEx::MTX_IDENTITY, 0, false, true},
};

// キャラクターアイコンのサイズ
const D3DXVECTOR2 g_aPlayerIconSize[] =
{
	D3DXVECTOR2(100, 100),		// キャラクターアイコン
	D3DXVECTOR2(160, 160),		// 矢印アイコン
};

// UIのサイズ
const D3DXVECTOR2 g_UIMapSize = D3DXVECTOR2(200, 85);

// マップアイコンのテクスチャパス
const char* g_apMapIconTexture[MAPICONTYPE_MAX] =
{
	// 店のアイコン
	"data/TEXTURE/shopicon_blackbg.png",

	// ギミックアイコン(共通でも可)
	"data/TEXTURE/gimmickicon.png",
	"data/TEXTURE/gimmickicon.png",
	"data/TEXTURE/gimmickicon.png",
	"data/TEXTURE/gimmickicon.png",
};

// プレイヤーアイコンのテクスチャパス
const char* g_apPlayerIconTexture[PLAYERTYPE_MAX] =
{
	// 少女のアイコン
	"data/TEXTURE/GirlIcon.png",

	// ネズミのアイコン
	"data/TEXTURE/MouseIcon.png",
};

const char* g_pArrowIconTexture = "data/TEXTURE/IconArrow.png";		// 矢印のアイコン
const char* g_pUIMapTexture = "data/TEXTURE/UI_MAP.png";			// マップのUI

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
	g_mapCamera.vp = g_vpBefore = CalcViewPort(mid, size);
	g_mapCamera.fN = zn;
	g_mapCamera.fF = zf;
	g_mapCamera.fAngle = DEFAULT_ANGLE;
	g_mapCamera.rot = CParamVector::V3NULL;
	g_mapCamera.fLength = fLength;

	InitIcon();

	IDX_TEXTURE tex;

	LoadTexture(g_pUIMapTexture, &tex);
	g_IdxUIMap = Set2DPolygon(D3DXVECTOR3(mid.x, mid.y + size.y * MAP_MAGNI, 0.0f), VECNULL, g_UIMapSize, tex);
	SetEnable2DPolygon(g_IdxUIMap, false);
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitMap(void)
{
	// 各バッファ解放
	UninitIcon();
}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateMap(void)
{
	if (!GetEnableUI() || IsEnableItemPut() == false)
	{ // UI非表示の場合、無効にする
		SetEnable2DPolygon(g_IdxUIMap, false);
	}
	else
	{
		SetEnable2DPolygon(g_IdxUIMap, true);
	}

	// マップ無効中は即時終了
	if (g_map.bEnable == false) return;

	// マップカメラの取得
	LPMAPCAMERA pMCam = GetMapCamera();

	if (GetActivePlayer() == PLAYERTYPE_MOUSE)
	{
		SetEnable2DPolygon(g_IdxUIMap, false);
		return;
	}

	if (g_map.bEnable == true && IsEndDialog() && UIMENU_OFF)
	{
		SetEnable2DPolygon(g_IdxUIMap, true);
	}
	else
	{
		SetEnable2DPolygon(g_IdxUIMap, false);
	}

	if (GetEnableUImenu() == true)
	{
		if (g_mapCamera.vp.X == g_vpBefore.X)
		{
			g_mapCamera.vp = CalcViewPort(VEC_XY(WINDOW_MID.x, 250.0f), V2(400, 400));
			SetEnable2DPolygon(g_IdxUIMap, false);

			// 視点の位置を変更
			pMCam->posR.x = MiniMap.posR.x;
			pMCam->posR.y = MiniMap.posR.y;
			pMCam->posR.z = MiniMap.posR.z;

			pMCam->posV.x = MiniMap.posV.x;
			pMCam->posV.y = MiniMap.posV.y;
			pMCam->posV.z = MiniMap.posV.z;

			// デバイスの取得
			AUTODEVICE9 Auto;		// デバイスの自動解放用変数
			HRESULT hr;

			hr = Auto.pDevice->Clear(1, &g_mapRect,
				(D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET),
				D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);
		}

		return;
	}

	if (g_mapCamera.vp.X != g_vpBefore.X)
	{
		g_mapCamera.vp = CalcViewPort(VEC_XY(g_vpBefore.X + g_vpBefore.Width * 0.5f, g_vpBefore.Y + g_vpBefore.Height * 0.5f), V2(g_vpBefore.Width, g_vpBefore.Height));
		SetEnable2DPolygon(g_IdxUIMap, true);

		// デバイスの取得
		AUTODEVICE9 Auto;		// デバイスの自動解放用変数
		HRESULT hr;

		hr = Auto.pDevice->Clear(1, &g_mapRect,
			(D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET),
			D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);
	}

	Player* pPlayer = GetPlayer();			// プレイヤーの取得

	pMCam->posR = D3DXVECTOR3(pPlayer->pos.x, pMCam->posR.y, pPlayer->pos.z);				// 注視点位置をプレイヤーの位置に変更

	// 各情報から視点の位置を逆算
	pMCam->posV.x = pMCam->posR.x + 0.001f;
	pMCam->posV.y = pMCam->posR.y + pMCam->fLength;
	pMCam->posV.z = pMCam->posR.z;

	UpdateIcon();
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

	// アイコン描画
	DrawIcon();

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

	g_mapRect = { (long)vp.X, (long)vp.Y, (long)(vp.X + vp.Width), (long)(vp.Y + vp.Height) };

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

	hr = Auto.pDevice->Clear(1, &g_mapRect,
		(D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET),
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
	MapIcon* pMIcon = &g_aMapIcon[0];
	PlayerIcon* pPIcon = &g_aPlayerIcon[0];
	IDX_TEXTURE tex;
	VERTEX_3D* pVtx;

	// アイコンの作成
	for (int nCntIcon = 0; nCntIcon < MAPICONTYPE_MAX; nCntIcon++, pMIcon++)
	{
		// データコピー
		*pMIcon = g_aMapIconInfo[nCntIcon];

		// バッファ作成
		AD9.pDevice->CreateVertexBuffer(CREATE_3DPOLYGON(pMIcon->pVtx));

		// 頂点設定
		pMIcon->pVtx->Lock(0, 0, (void**)&pVtx, 0);

		// 各種設定
		SetPolygonSize(pVtx, pMIcon->size, false);		// 頂点座標設定
		SetPolygonNormal(pVtx, D3DXVECTOR3(0, 1, 0));	// 法線設定
		SetDefaultColor(pVtx);							// 頂点カラー設定
		SetDefaultTexture(pVtx);						// テクスチャ座標設定

		pMIcon->rot.y = -D3DX_HALFPI;

		// 頂点設定終了
		pMIcon->pVtx->Unlock();

		// テクスチャ読み込み
		LoadTexture(g_apMapIconTexture[nCntIcon], &pMIcon->tex);
	}

	// プレイヤーアイコン及び矢印アイコンの作成
	for (int nCntIcon = 0; nCntIcon < PLAYERTYPE_MAX; nCntIcon++, pPIcon++)
	{
		// データコピー
		pPIcon->rot = VEC_Y(RepairRot(-CParamFloat::HALFPI));
		pPIcon->size = g_aPlayerIconSize[0];
		pPIcon->sizeArrow = g_aPlayerIconSize[1];

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
		LoadTexture(g_apPlayerIconTexture[nCntIcon], &pPIcon->tex);
		LoadTexture(g_pArrowIconTexture, &pPIcon->texArrow);
	}
}

//==================================================================================
// --- アイコンの終了 ---
//==================================================================================
void UninitIcon(void)
{
	MapIcon* pMIcon = &g_aMapIcon[0];
	PlayerIcon* pPIcon = &g_aPlayerIcon[0];

	/*** マップアイコン解放 ***/
	for (int nCntRelease = 0; nCntRelease < MAPICONTYPE_MAX; nCntRelease++, pMIcon++)
	{
		RELEASE(pMIcon->pVtx);
	}

	/*** プレイヤーアイコン解放 ***/
	for (int nCntRelease = 0; nCntRelease < PLAYERTYPE_MAX; nCntRelease++, pPIcon++)
	{
		RELEASE(pPIcon->pVtx);
		RELEASE(pPIcon->pVtxArrow);
	}
}

//==================================================================================
// --- アイコンの更新 ---
//==================================================================================
void UpdateIcon(void)
{
	PlayerIcon* pPIcon = GetPlayerIcon();	// プレイヤーアイコン
	Player* pPlayer = GetPlayer();			// プレイヤー

	for (int nCntIcon = 0; nCntIcon < PLAYERTYPE_MAX; nCntIcon++, pPIcon++, pPlayer++)
	{
		D3DXVECTOR3 pos = pPlayer->pos, rot = pPlayer->rot;		// ポインタ省略

		pPIcon->pos = EXTRACT_XZ(pos);							// アイコン位置をプレイヤー座標に置換
		pPIcon->rotArrow = VEC_Y(InverseRot(rot.y));			// 矢印アイコンの向きをプレイヤーの向きに置換
	}

	MapIcon* pMapIcon = GetMapIcon();	// マップアイコン

	Gimmick* pGimmick = GetGimmick();	// ギミックの情報

	// === ギミッククリアで対応しているマップアイコンを消去する === //
	for (int nCntGimmick = 0; nCntGimmick < GIMMICKTYPE_MAX; nCntGimmick++, pGimmick++)
	{// ギミック数分まわす
		for (int nCntMapIcon = 0; nCntMapIcon < MAPICONTYPE_MAX; nCntMapIcon++, pMapIcon++)
		{// マップアイコン分回す
			if (pGimmick->bClear == true)
			{// クリア済みなら
				switch (pGimmick->myType)
				{
					// でかボタン
				case GIMMICKTYPE_BIGBUTTON:
					g_aMapIcon[MAPICONTYPE_LBUTTON].bUse = false;
					break;
					// ちびボタン
				case GIMMICKTYPE_SMALLBUTTON:
					g_aMapIcon[MAPICONTYPE_SBUTTON].bUse = false;

					break;
					// 倒木
				case GIMMICKTYPE_FALLENTREE:
					g_aMapIcon[MAPICONTYPE_TREE].bUse = false;

					break;
					// 銅像(バルブ)
				case GIMMICKTYPE_STATUE:
					g_aMapIcon[MAPICONTYPE_FOUNTAIN].bUse = false;

					break;
					// それ以外
				default:
					break;
				}
			}
		}
	}
}

//==================================================================================
// --- アイコンの描画 ---
//==================================================================================
void DrawIcon(void)
{
	AUTODEVICE9 AD9;

	// Zテスト無効
	SetEnableZFunction(AD9.pDevice, false);

	// マップアイコン表示
	DrawMapIcon();

	// プレイヤーアイコン表示
	DrawPlayerIcon();

	// Zテスト有効
	SetEnableZFunction(AD9.pDevice, true);
}

//==================================================================================
// --- マップアイコンの描画 ---
//==================================================================================
void DrawMapIcon(void)
{
	/*** デバイスの取得 ***/
	AUTODEVICE9 Auto;							// デバイス自動解放システム
	LPDIRECT3DDEVICE9 pDevice = Auto.pDevice;	// 自動解放システムを介してデバイスを取得

	MapIcon* pMIcon = &g_aMapIcon[0];

	// 計算用マトリックス
	D3DXMATRIX mtxRot, mtxTrans;

	for (int nCntMapIcon = 0; nCntMapIcon < MAPICONTYPE_MAX; nCntMapIcon++, pMIcon++)
	{
		if (pMIcon->bUse == true)
		{
			/*** ワールドマトリックスの初期化 ***/
			D3DXMatrixIdentity(&pMIcon->mtxWorld);

			/*** 向きを反映 (※ 位置を反映する前に必ず行うこと！) ***/
			D3DXMatrixRotationYawPitchRoll(&mtxRot,
				pMIcon->rot.y,		// Y軸回転
				pMIcon->rot.x,		// X軸回転
				pMIcon->rot.z);		// Z軸回転

			D3DXMatrixMultiply(&pMIcon->mtxWorld, &pMIcon->mtxWorld, &mtxRot);

			/*** 位置を反映 (※ 向きを反映したのちに行うこと！) ***/
			D3DXMatrixTranslation(&mtxTrans,
				pMIcon->pos.x,
				pMIcon->pos.y,
				pMIcon->pos.z);

			D3DXMatrixMultiply(&pMIcon->mtxWorld, &pMIcon->mtxWorld, &mtxTrans);

			/*** ワールドマトリックスの設定 ***/
			pDevice->SetTransform(D3DTS_WORLD, &pMIcon->mtxWorld);

			/*** 頂点バッファをデータストリームに設定 ***/
			pDevice->SetStreamSource(0, pMIcon->pVtx, 0, sizeof(VERTEX_3D));

			/*** 頂点フォーマットの設定 ***/
			pDevice->SetFVF(FVF_VERTEX_3D);

			/*** テクスチャの設定 ***/
			pDevice->SetTexture(0, GetTexture(pMIcon->tex));

			/*** ポリゴンの描画 ***/
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,		// プリミティブの種類
				0,											// 描画する最初の頂点インデックス
				2);											// 描画するプリミティブの数
		}

	}
}

//==================================================================================
// --- プレイヤーアイコンの描画 ---
//==================================================================================
void DrawPlayerIcon(void)
{
	/*** デバイスの取得 ***/
	AUTODEVICE9 Auto;							// デバイス自動解放システム
	LPDIRECT3DDEVICE9 pDevice = Auto.pDevice;	// 自動解放システムを介してデバイスを取得

	PlayerIcon* pPIcon = &g_aPlayerIcon[0];

	// 計算用マトリックス
	D3DXMATRIX mtxRot, mtxTrans;

	/*** アルファテストを有効にする ***/
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// アルファテストを有効
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// 基準値よりも大きい場合にZバッファに書き込み
	pDevice->SetRenderState(D3DRS_ALPHAREF, 60);				// 基準値

	for (int nCntMapIcon = 0; nCntMapIcon < PLAYERTYPE_MAX; nCntMapIcon++, pPIcon++)
	{
		/*** ワールドマトリックスの初期化 ***/
		D3DXMatrixIdentity(&pPIcon->mtxWorld);

		/*** 向きを反映 (※ 位置を反映する前に必ず行うこと！) ***/
		D3DXMatrixRotationYawPitchRoll(&mtxRot,
			pPIcon->rot.y,		// Y軸回転
			pPIcon->rot.x,		// X軸回転
			pPIcon->rot.z);		// Z軸回転

		D3DXMatrixMultiply(&pPIcon->mtxWorld, &pPIcon->mtxWorld, &mtxRot);

		/*** 位置を反映 (※ 向きを反映したのちに行うこと！) ***/
		D3DXMatrixTranslation(&mtxTrans,
			pPIcon->pos.x,
			pPIcon->pos.y,
			pPIcon->pos.z);

		D3DXMatrixMultiply(&pPIcon->mtxWorld, &pPIcon->mtxWorld, &mtxTrans);

		/*** ワールドマトリックスの設定 ***/
		pDevice->SetTransform(D3DTS_WORLD, &pPIcon->mtxWorld);

		/*** 頂点バッファをデータストリームに設定 ***/
		pDevice->SetStreamSource(0, pPIcon->pVtx, 0, sizeof(VERTEX_3D));

		/*** 頂点フォーマットの設定 ***/
		pDevice->SetFVF(FVF_VERTEX_3D);

		/*** テクスチャの設定 ***/
		pDevice->SetTexture(0, GetTexture(pPIcon->tex));

		/*** ポリゴンの描画 ***/
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,		// プリミティブの種類
			0,											// 描画する最初の頂点インデックス
			2);											// 描画するプリミティブの数

		// ******** 矢印アイコン ********
		/*** ワールドマトリックスの初期化 ***/
		D3DXMatrixIdentity(&pPIcon->mtxWorldArrow);

		/*** 向きを反映 (※ 位置を反映する前に必ず行うこと！) ***/
		D3DXMatrixRotationYawPitchRoll(&mtxRot,
			pPIcon->rotArrow.y,		// Y軸回転
			pPIcon->rotArrow.x,		// X軸回転
			pPIcon->rotArrow.z);	// Z軸回転

		D3DXMatrixMultiply(&pPIcon->mtxWorldArrow, &pPIcon->mtxWorldArrow, &mtxRot);

		/*** 位置を反映 (※ 向きを反映したのちに行うこと！) ***/
		D3DXMatrixTranslation(&mtxTrans,
			pPIcon->pos.x,
			pPIcon->pos.y + 0.1f,
			pPIcon->pos.z);

		D3DXMatrixMultiply(&pPIcon->mtxWorldArrow, &pPIcon->mtxWorldArrow, &mtxTrans);

		/*** ワールドマトリックスの設定 ***/
		pDevice->SetTransform(D3DTS_WORLD, &pPIcon->mtxWorldArrow);

		/*** 頂点バッファをデータストリームに設定 ***/
		pDevice->SetStreamSource(0, pPIcon->pVtxArrow, 0, sizeof(VERTEX_3D));

		/*** 頂点フォーマットの設定 ***/
		pDevice->SetFVF(FVF_VERTEX_3D);

		/*** テクスチャの設定 ***/
		pDevice->SetTexture(0, GetTexture(pPIcon->texArrow));

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

//==================================================================================
// --- マップアイコンの取得 ---
//==================================================================================
MapIcon* GetMapIcon(void)
{
	return &g_aMapIcon[0];
}

//==================================================================================
// --- プレイヤーアイコンの取得 ---
//==================================================================================
PlayerIcon* GetPlayerIcon(void)
{
	return &g_aPlayerIcon[0];
}