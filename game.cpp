//==================================================================================
//
// DirectXのゲーム関連用のcppファイル [game.cpp]
// Author : TENMA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "game.h"
#include "input.h"
#include "camera.h"
#include "fade.h"
#include "model.h"
#include "modeldata.h"
#include "Texture.h"
#include "3Dmodel.h"
#include "field.h"
#include "player.h"
#include "pause.h"
#include "light.h"
#include "thread.h"
#include "motion.h"
#include "mesh.h"
#include "item.h"
#include "debugproc.h"
#include "prompt.h"
#include "timer.h"
#include "UIarm.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
int g_nIdx3DModel;		// 設置した3Dモデルのインデックス
int g_nIdxCamera;		// 設置したカメラのインデックス
int g_nCounterGame = 0;	// ゲームのカウンター
bool g_bPause = false;	//ポーズ状態のON/OFF

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitGame(void)
{
	/*** Aの初期化 ***/

	/*** カメラの初期化 ***/
	InitCamera();

	/*** モデルデータの初期化 ***/
	InitModelData();

	/*** テクスチャの初期化 ***/
	InitTexture();

	/*** モーションの初期化 ***/
	InitMotion();

	/*** 3Dモデルの初期化 ***/
	Init3DModel();

	/*** 床の初期化 ***/
	InitField();

	/*** メッシュの初期化 ***/
	InitMesh();

	/*** モデルの初期化 ***/
	InitModel();

	/*** プレイヤーの初期化 ***/
	InitPlayer();

	/*** アイテムの初期化 ***/
	InitItem();

	/*** ライトの初期化 ***/
	InitLight();

	/*** モデルのスクリプト読み込み ***/
	LoadModel();

	/*** ポーズの初期化 ***/
	InitPause();

	/*** プロンプト初期化 ***/
	InitPrompt();

	/*** タイマー初期化 ***/
	InitTimer(false);

	/*** UIアーム初期化 ***/
	InitUIarm();

	g_nCounterGame = 0;
	int nIdxPrompt, Tex;
	LoadTexture("data/TEXTURE/TestPrompt.png", &Tex);
	nIdxPrompt = SetPrompt(D3DXVECTOR3(100, 100, 200), D3DXVECTOR2(50.0f, 20.0f), Tex, true);
	SetEnablePrompt(true, nIdxPrompt);
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitGame(void)
{
	/*** Aの終了 ***/

	/*** カメラの終了 ***/
	UninitCamera();

	/*** モデルデータの終了 ***/
	UninitModelData();

	/*** テクスチャの終了 ***/
	UninitTexture();

	/*** モーションの終了 ***/
	UninitMotion();

	/*** 3Dモデルの終了 ***/
	Uninit3DModel();

	/*** 床の終了 ***/
	UninitField();

	/*** メッシュの終了 ***/
	UninitMesh();

	/*** モデルの終了 ***/
	UninitModel();

	/*** プレイヤーの終了 ***/
	UninitPlayer();

	/*** アイテムの終了 ***/
	UninitItem();

	/*** ライトの終了 ***/
	UninitLight();

	/*** ポーズの終了 ***/
	UninitPause();

	/*** プロンプトの終了 ***/
	UninitPrompt();

	/*** タイマーの終了 ***/
	UninitTimer();

	/*** UIアームの終了 ***/
	UninitUIarm();
}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateGame(void)
{
	//ポーズ状態のON/OFFを切り替え
	if (GetKeyboardTrigger(DIK_P) == true
		|| GetJoypadTrigger(0, JOYKEY_BACK) == true
		|| GetJoypadTrigger(1, JOYKEY_BACK) == true)
	{
		g_bPause = g_bPause ? false : true;
	}
	//ポーズ状態がONの時
	if (g_bPause == true)
	{
		//ポーズの更新処理
		UpdatePause();
	}
	//ポーズ状態がOFFの時
	else
	{
		/*** Aの更新 ***/

		/*** カメラの更新 ***/
		UpdateCamera();

		/*** 3Dモデルの更新 ***/
		Update3DModel();

		/*** 床の更新 ***/
		UpdateField();

		/*** メッシュの更新 ***/
		UpdateMesh();

		/*** モデルの更新 ***/
		UpdateModel();

		/*** プレイヤーの更新 ***/
		UpdatePlayer();

		/*** アイテムの更新 ***/
		UpdateItem();

		/*** ライトの更新 ***/
		UpdateLight();

		/*** プロンプトの更新 ***/
		UpdatePrompt();

		/*** タイマーの更新 ***/
		UpdateTimer();

		/*** UIアームの更新 ***/
		UpdateUIarm();

		if (g_nCounterGame % 60 == 0)
		{
			AddTimer(-1);
		}

		g_nCounterGame++;
	}

	PrintDebugProc("NumPlayer %d  ActivePlayer %d  CameraNum %d", GetNumPlayer(), GetActivePlayer(), GetCameraNum());

	// モード変更
	if (GetKeyboardTrigger(DIK_BACK)
		|| GetJoypadTrigger(0, JOYKEY_START))
	{
		if (GetFade() == FADE_NONE)
		{
			SetFade(MODE_RESULT);
		}
	}
}

//==================================================================================
// --- 描画 ---
//==================================================================================
void DrawGame(void)
{
	// カメラの数分だけ描画
	for (int nCntDraw = 0; nCntDraw < GetCameraNum(); nCntDraw++)
	{		
		/*** カメラの設置 ***/
		SetCamera();

		// VERTEX_3D ============================================
		/*** Aの描画 ***/

		/*** 床の描画  ***/
		DrawField();

		/*** 3Dモデルの描画 ***/
		Draw3DModel();

		/*** モデルの描画 ***/
		DrawModel();

		/*** プレイヤーの描画 ***/
		DrawPlayer();

		/*** アイテムの描画 ***/
		DrawItem();

		/*** メッシュの描画 ***/
		DrawMesh();

		/*** プロンプトの描画 ***/
		DrawPrompt();

		/*** フォグをクリア ***/
		CleanFog();
	}

	// VERTEX_2D ============================================
	/*** Aの描画 ***/

	/*** タイマーの描画 ***/
	DrawTimer();

	/*** UIアームの描画 ***/
	DrawUIarm();

	//ポーズ状態がONの時
	if (g_bPause == true)
	{
		/*** ポーズの描画 ***/
		DrawPause();
	}
}

//==================================================================================
// ポーズの設定
//==================================================================================
void SetEnablePause(bool bPouse)
{
	g_bPause = bPouse;
}