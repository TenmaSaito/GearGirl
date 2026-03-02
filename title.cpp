//==================================================================================
//
// DirectXのタイトル関連用のcppファイル [title.cpp]
// Author : TENMA SAITO
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "light.h"
#include "param.h"
#include "tCamera.h"
#include "Texture.h"
#include "title.h"
#include "titleSelect.h"
#include "tLogo.h"
#include "modeldata.h"
#include "2Dpolygon.h"
#include "3Dmodel.h"
#include "mathUtil.h"
#include "skybox.h"

// 名前空間の省略
using namespace MyMathUtil;
using namespace Constants;

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define MOVE_INTERVAL		(360)		// 電車が揺れる間隔
#define MOVE_TIME			(MOVE_INTERVAL + 10)				// 揺れた座標に居続ける時間
#define LOGOTEX_PATH		"data/TEXTURE/GearGirl_Logo.png"	// ロゴのテクスチャパス

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
int g_nIdxTrain;
int g_nCounterTrain;
const D3DXVECTOR3 g_aPosTLogo[2] =	// ロゴの位置 
{
	D3DXVECTOR3(0, 0, 0),
	D3DXVECTOR3(250, 150, 0)
};
const D3DXVECTOR2 g_sizeTLogo = D3DXVECTOR2(475, 270);	// ロゴのサイズ

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitTitle(void)
{
	/*** モデルデータの初期化 ***/
	InitModelData();

	/*** テクスチャの初期化 ***/
	InitTexture();

	/*** ライトの初期化 ***/
	InitLight();

	/*** 選択肢の初期化 ***/
	InitTitleSelect();

	/*** カメラの初期化 ***/
	InitTCamera();

	/*** 3Dモデルの初期化 ***/
	Init3DModel();

	/*** 2Dポリゴンの初期化 ***/
	Init2DPolygon();

	/*** スカイボックスの初期化 ***/
	InitSkybox();

	/*** ロゴの初期化 ***/
	InitTLogo();

	// カメラの追加
	AddTCamera(D3DXVECTOR3(-35.0f, 30.0f, 0.0f), VEC_Y(CParamFloat::HALFPI), 100.0f);

	// モデルの読み込み
	int nIdxModel;
	LoadModelData("data/MODEL/Building/train.x", &nIdxModel);

	// モデルの設置
	g_nIdxTrain = Set3DModel(CParamVector::V3NULL, CParamVector::V3NULL, nIdxModel);

	// スカイボックスの設置
	SetSkybox(SKYBOX::NORMAL);

	// ロゴテクスチャの読み込み
	IDX_TEX tex;
	LoadTexture(LOGOTEX_PATH, &tex);

	// ロゴの設置
	SetTLogo(g_aPosTLogo[0], g_aPosTLogo[1], g_sizeTLogo, tex);

	g_nCounterTrain = 0;
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitTitle(void)
{
	/*** モデルデータの終了 ***/
	UninitModelData();

	/*** テクスチャの終了 ***/
	UninitTexture();

	/*** 2Dポリゴンの終了 ***/
	Uninit2DPolygon();

	/*** スカイボックスの終了 ***/
	UninitSkybox();

	/*** 3Dモデルの終了 ***/
	Uninit3DModel();

	/*** ライトの終了 ***/
	UninitLight();

	/*** 選択肢の終了 ***/
	UninitTitleSelect();

	/*** ロゴの終了 ***/
	UninitTLogo();

	/*** カメラの終了 ***/
	UninitTCamera();
}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateTitle(void)
{
	/*** ライトの更新 ***/
	UpdateLight();

	/*** 3Dモデルの更新 ***/
	Update3DModel();

	/*** 選択肢の更新 ***/
	UpdateTitleSelect();

	/*** 2Dポリゴンの更新 ***/
	Update2DPolygon();

	/*** カメラの更新 ***/
	UpdateTCamera();

	/*** スカイボックスの更新 ***/
	UpdateSkybox();

	/*** ロゴの更新 ***/
	UpdateTLogo();

	// モデルの座標更新
	if (g_nCounterTrain > MOVE_INTERVAL)
	{
		if (Step(g_nCounterTrain, MOVE_TIME))
		{
			LP3DMODEL pModel;
			pModel = Get3DModel(g_nIdxTrain);
			if (pModel)
			{
				pModel->pos.y = CParamVector::V3NULL.y + 0.05f;
			}
		}
		else
		{
			g_nCounterTrain = 0;
		}
	}
	else
	{
		LP3DMODEL pModel;
		pModel = Get3DModel(g_nIdxTrain);
		if (pModel)
		{
			if (g_nCounterTrain % 60 >= 30)
			{
				pModel->pos.y = CParamVector::V3NULL.y + 0.02f;
			}
			else
			{
				pModel->pos.y = CParamVector::V3NULL.y;
			}
		}
	}

	g_nCounterTrain++;
}

//==================================================================================
// --- 描画 ---
//==================================================================================
void DrawTitle(void)
{
	/*** カメラの設置 ***/
	SetTCamera();

	/*** スカイボックスの描画 ***/
	DrawSkybox();

	/*** 3Dモデルの描画 ***/
	Draw3DModel();

	/*** 選択肢の描画 ***/
	DrawTitleSelect();

	/*** 2Dポリゴンの描画 ***/
	Draw2DPolygon();
}