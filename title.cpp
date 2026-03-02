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
#include "title.h"
#include "titleSelect.h"
#include "modeldata.h"
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
#define MOVE_TIME			(MOVE_INTERVAL + 10)		// 揺れた座標に居続ける時間

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
int g_nIdxTrain;
int g_nCounterTrain;

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitTitle(void)
{
	/*** モデルデータの初期化 ***/
	InitModelData();

	/*** ライトの初期化 ***/
	InitLight();

	/*** 選択肢の初期化 ***/
	InitTitleSelect();

	/*** カメラの初期化 ***/
	InitTCamera();

	/*** 3Dモデルの初期化 ***/
	Init3DModel();

	/*** スカイボックスの初期化 ***/
	InitSkybox();

	// カメラの追加
	AddTCamera(D3DXVECTOR3(-35.0f, 30.0f, 0.0f), VEC_Y(CParamFloat::HALFPI), 100.0f);

	// モデルの読み込み
	int nIdxModel;
	LoadModelData("data/MODEL/Building/train.x", &nIdxModel);

	// モデルの設置
	g_nIdxTrain = Set3DModel(CParamVector::V3NULL, CParamVector::V3NULL, nIdxModel);

	// スカイボックスの設置
	SetSkybox(SKYBOX::NORMAL);

	g_nCounterTrain = 0;
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitTitle(void)
{
	/*** モデルデータの終了 ***/
	UninitModelData();

	/*** スカイボックスの終了 ***/
	UninitSkybox();

	/*** 3Dモデルの終了 ***/
	Uninit3DModel();

	/*** ライトの終了 ***/
	UninitLight();

	/*** 選択肢の終了 ***/
	UninitTitleSelect();

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

	/*** カメラの更新 ***/
	UpdateTCamera();

	/*** スカイボックスの更新 ***/
	UpdateSkybox();

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
}