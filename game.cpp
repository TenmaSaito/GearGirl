//==================================================================================
//
// DirectXのゲーム関連用のcppファイル [game.cpp]
// Author : TENMA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "camera.h"
#include "debugproc.h"
#include "effect.h"
#include "fade.h"
#include "field.h"
#include "game.h"
#include "gimmick.h"
#include "judgeEnd.h"
#include "input.h"
#include "item.h"
#include "light.h"
#include "mathUtil.h"
#include "map.h"
#include "mesh.h"
#include "model.h"
#include "modeldata.h"
#include "motion.h"
#include "player.h"
#include "parabola.h"
#include "particle.h"
#include "pause.h"
#include "prompt.h"
#include "sound.h"
#include "Texture.h"
#include "thread.h"
#include "timer.h"
#include "UIarm.h"
#include "UImenu.h"
#include "2Dpolygon.h"
#include "3Dmodel.h"

using namespace MyMathUtil;

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************

//**********************************************************************************
//*** エンディング移行管理構造体 ***
//**********************************************************************************
STRUCT()
{
	bool bAlreadyEnd;	// エンディングへの移行状態
	int nCountWait;		// エンディングへの移行待機時間
}ObserveEnding;

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
int g_nIdxShopPrompt = -1;	// 店前のプロンプトインデックス
int g_nCounterGame = 0;	// ゲームのカウンター
bool g_bPause = false;	//ポーズ状態のON/OFF
ObserveEnding g_obEnding;			// エンディングへの移行状態

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitGame(void)
{
	/*** Aの初期化 ***/

	//===========================================
	// 初期化
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

	/*** 2Dポリゴンの初期化 ***/
	Init2DPolygon();

	/*** 床の初期化 ***/
	InitField();

	/*** メッシュの初期化 ***/
	InitMesh();

	/*** モデルの初期化 ***/
	InitModel();

	/*** アイテムの初期化 ***/
	InitItem();

	/*** ライトの初期化 ***/
	InitLight();

	/*** プレイヤーの初期化 ***/
	InitPlayer();

	/*** ポーズの初期化 ***/
	InitPause();

	/*** プロンプト初期化 ***/
	InitPrompt();

	/*** タイマー初期化 ***/
	InitTimer(false);

	/*** UIアーム初期化 ***/
	InitUIarm();

	/*** UIメニュー初期化 ***/
	InitUImenu();

	/*** ギミックの初期化 ***/
	InitGimmick();

	/*** エフェクトの初期化 ***/
	InitEffect();

	/*** パーティクルの初期化 ***/
	InitParticle();

	/*** 放物線の初期化 ***/
	InitParabola();

	/*** マップの初期化 ***/
	InitMap(D3DXVECTOR3(150.0f, 550.0f, 0.0f), D3DXVECTOR2(256, 256), 500.0f);

	//==========================================
	/*** モデルのスクリプト読み込み ***/
	LoadModel();

	g_nCounterGame = 0;
	IDX_TEX Tex;
	LoadTexture("data/TEXTURE/TestPrompt.png", &Tex);
	g_nIdxShopPrompt = SetPrompt(D3DXVECTOR3(1463, 116, -455), D3DXVECTOR2(10.0f, 5.0f), Tex, true);
	SetEnablePrompt(true, g_nIdxShopPrompt);

	SetParticle(D3DXVECTOR3(1463, 116, -455), CParamColor::RED, -CParamVector::V3ONE * 0.5f, CParamVector::V3ONE, 5, 10.0f, 1000, 10, false);

	SetEnableMap(true);
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

	/*** 2Dポリゴンの終了 ***/
	Uninit2DPolygon();

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

	/*** UIメニュー終了 ***/
	UninitUImenu();

	/*** ギミックの終了 ***/
	UninitGimmick();

	/*** エフェクトの終了 ***/
	UninitEffect();

	/*** パーティクルの終了 ***/
	UninitParticle();

	/*** 放物線の終了 ***/
	UninitParabola();

	/*** マップの終了 ***/
	UninitMap();
}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateGame(void)
{
	//ポーズ状態のON/OFFを切り替え
	if ((GetKeyboardTrigger(DIK_P) == true
		|| GetDualInput(JOYKEY_BACK, DUAL_JOYPAD | DUAL_OR | DUAL_TRIGGER,
			JOYKEY_BACK, DUAL_JOYPAD | DUAL_DUAL | DUAL_TRIGGER))
		&& g_obEnding.bAlreadyEnd == false)
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

		/*** 2Dポリゴンの更新 ***/
		Update2DPolygon();

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

		/*** UIメニュー更新 ***/
		UpdateUImenu();

		/*** ギミックの更新 ***/
		UpdateGimmick();

		/*** パーティクルの更新 ***/
		UpdateParticle();

		/*** エフェクトの更新 ***/
		UpdateEffect();

		/*** 放物線の更新 ***/
		UpdateParabola();

		/*** マップの更新 ***/
		UpdateMap();

		if (g_nCounterGame % 60 == 0)
		{
			AddTimer(-1);
		}

		if (GetKeyboardTrigger(DIK_5))
		{
			SetEnableUImenu(!GetEnableUImenu(), 0);
		}

		g_nCounterGame++;
	}

	PrintDebugProc("NumPlayer %d  ActivePlayer %d  CameraNum %d", GetNumPlayer(), GetActivePlayer(), GetCameraNum());

	// リザルト画面移行処理
	if (g_obEnding.bAlreadyEnd == true)
	{
		g_obEnding.nCountWait--;
		if (g_obEnding.nCountWait <= 0
			&& GetFade() == FADE_NONE)
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
	/*** デバイスの取得 ***/
	AUTODEVICE9 Auto;							// デバイス自動解放システム
	LPDIRECT3DDEVICE9 pDevice = Auto.pDevice;	// 自動解放システムを介してデバイスを取得
	D3DVIEWPORT9 viewport;

	// 現在のビューポートを取得
	pDevice->GetViewport(&viewport);

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

		/*** ギミックの描画 ***/
		DrawGimmick();

		/*** プロンプトの描画 ***/
		DrawPrompt();

		/*** エフェクトの描画 ***/
		DrawEffect();

		/*** 放物線の描画 ***/
		DrawParabola();

		/*** フォグをクリア ***/
		CleanFog();
	}

	// ビューポートを設定
	pDevice->SetViewport(&viewport);

	/*** マップの描画 ***/
	DrawMap();

	// VERTEX_2D ============================================
	/*** Aの描画 ***/

	/*** タイマーの描画 ***/
	DrawTimer();

	/*** UIアームの描画 ***/
	DrawUIarm();
	
	/*** UIメニュー描画 ***/
	DrawUImenu();

	/*** 2Dポリゴンの描画 ***/
	Draw2DPolygon();

	/*** UIアイテム描画 ***/
	DrawUIItem();

	//ポーズ状態がONの時
	if (g_bPause == true)
	{
		/*** ポーズの描画 ***/
		DrawPause();
	}
}

//==================================================================================
// --- ポーズの設定 ---
//==================================================================================
void SetEnablePause(bool bPouse)
{
	g_bPause = bPouse;
}

//==================================================================================
// --- 描画 ---
//==================================================================================
void SetGameEnding(int nCountWait)
{
	if (g_obEnding.bAlreadyEnd == true) return;

	g_obEnding.bAlreadyEnd = true;		// エンディング移行開始
	g_obEnding.nCountWait = nCountWait;	// エンディング移行待機時間
}

//==================================================================================
// --- ショップのインデックス取得 ---
//==================================================================================
int GetIdxShopPrompt(void)
{
	return g_nIdxShopPrompt;
}