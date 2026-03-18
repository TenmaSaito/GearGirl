//==================================================================================
//
// DirectXのゲーム関連用のcppファイル [game.cpp]
// Author : TENMA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "billboard.h"
#include "camera.h"
#include "common_fade.h"
#include "debugproc.h"
#include "dialog.h"
#include "effect.h"
#include "fade.h"
#include "field.h"
#include "game.h"
#include "gimmick.h"
#include "guide.h"
#include "judgeEnd.h"
#include "input.h"
#include "item.h"
#include "itemEffector.h"
#include "light.h"
#include "lineEffect.h"
#include "mathUtil.h"
#include "map.h"
#include "mesh.h"
#include "meshOrbit.h"
#include "model.h"
#include "modeldata.h"
#include "motion.h"
#include "player.h"
#include "parabola.h"
#include "particle.h"
#include "pause.h"
#include "prompt.h"
#include "sound.h"
#include "skybox.h"
#include "Texture.h"
#include "thread.h"
#include "timer.h"
#include "UIarm.h"
#include "UImenu.h"
#include "UIplayer.h"
#include "UIcollect.h"
#include "UIGet.h"
#include "UIShop.h"
#include "wall.h"
#include "2Dpolygon.h"
#include "3Dmodel.h"

#include "Color_defs.h"

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
//*** 定数変数 ***
//**********************************************************************************
const float g_fRadiusShopStart = 1.0f;		// ショップのシリンダー開始半径
const float g_fRadiusShopFinish = 150.0f;	// 最大半径
const float g_fHeightShopFinish = 1500.0f;	// 最大の高さ
const int g_nStartLerpTime = 3600;			// Lerp変換開始秒数

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
int g_nIdxShopPrompt = -1;		// 店前のプロンプトインデックス
int g_nIdxShopCylinder = -1;	// 店のシリンダー
float g_fShopCylinderS = 0.0f;	// Lerp変換変数
int g_nCounterGame = 0;			// ゲームのカウンター
bool g_bPause = false;			//ポーズ状態のON/OFF
bool g_bEnableUI = true;		// UIの描画状態
ObserveEnding g_obEnding;		// エンディングへの移行状態

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

	/*** ビルボードの初期化 ***/
	InitBillboard();

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

	/*** 壁の初期化 ***/
	InitWall();

	/*** メッシュの初期化 ***/
	InitMesh();

	/*** メッシュオービットの初期化 ***/
	InitMeshOrbit();

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

	/*** UIプレイヤー初期化 ***/
	InitUIplayer();

	/*** UIコレクト初期化 ***/
	InitUIcollect();

	/*** UIゲット初期化 ***/
	InitUIGet();

	/*** UIショップの初期化 ***/
	InitUIShop();

	/*** エフェクトの初期化 ***/
	InitEffect();

	/*** ラインエフェクトの初期化 ***/
	InitLineEffect();

	/*** パーティクルの初期化 ***/
	InitParticle();

	/*** マップの初期化 ***/
	InitMap(D3DXVECTOR3(150, 150, 0.0f), D3DXVECTOR2(180, 180), 1045.0f);

	/*** ギミックの初期化 ***/
	InitGimmick();

	/*** ダイアログの初期化 ***/
	InitDialog();

	/*** アイテムエフェクターの初期化 ***/
	InitItemEffector();

	/*** ガイドの初期化 ***/
	InitGuide();

	/*** スカイボックスの初期化 ***/
	InitSkybox();

	// スカイボックスの設置
	SetSkybox(SKYBOX::NORMAL);

	//==========================================
	/*** モデルのスクリプト読み込み ***/
	LoadModel();

	g_nCounterGame = 0;
	IDX_TEXTURE Tex;
	LoadTexture("data/TEXTURE/UIprompt/promptTalk.png", &Tex);
	g_nIdxShopPrompt = SetPrompt(D3DXVECTOR3(1463, 116, -455), D3DXVECTOR2(10.0f, 5.0f), Tex, true);
	SetEnablePrompt(true, g_nIdxShopPrompt);

	SetEnableMap(true);

	LoadTexture("data/TEXTURE/NINGEN01.png", &Tex);
	SetBillboard(D3DXVECTOR3(1450, 100, -445), 15, 25, Tex);

	SetDialog();

	g_nIdxShopCylinder = SetMeshCylinder(D3DXVECTOR3(1463, 100, -455), VECNULL, CONVERSION_A(COLOR_WHITE, 0.5f), g_fRadiusShopStart, g_fRadiusShopStart, 2, 16);
	SetEnableMeshCylinder(g_nIdxShopCylinder, false);
	g_fShopCylinderS = 0.0f;
	g_bEnableUI = true;
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitGame(void)
{
	/*** Aの終了 ***/

	/*** ビルボードの終了 ***/
	UninitBillboard();

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

	/*** 壁の終了 ***/
	UninitWall();

	/*** メッシュの終了 ***/
	UninitMesh();

	/*** メッシュオービットの終了 ***/
	UninitMeshOrbit();

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

	/*** UIプレイヤー終了 ***/
	UninitUIplayer();

	/*** UIコレクト終了 ***/
	UninitUIcollect();

	/*** UIショップの終了 ***/
	UninitUIShop();

	/*** ギミックの終了 ***/
	UninitGimmick();

	/*** エフェクトの終了 ***/
	UninitEffect();

	/*** ラインエフェクトの終了 ***/
	UninitLineEffect();

	/*** パーティクルの終了 ***/
	UninitParticle();

	/*** マップの終了 ***/
	UninitMap();

	/*** ダイアログの終了 ***/
	UninitDialog();

	/*** アイテムエフェクターの終了 ***/
	UninitItemEffector();

	/*** ガイドの終了 ***/
	UninitGuide();

	/*** UIゲット終了 ***/
	UninitUIGet();

	/*** スカイボックスの終了 ***/
	UninitSkybox();
}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateGame(void)
{
	//ポーズ状態のON/OFFを切り替え
	if ((GetKeyboardTrigger(DIK_P) == true
		|| GetDualInput(JOYKEY_START, DUAL_JOYPAD | DUAL_OR | DUAL_TRIGGER,
			JOYKEY_START, DUAL_JOYPAD | DUAL_DUAL | DUAL_TRIGGER))
		&& g_obEnding.bAlreadyEnd == false
		&& IsEnableItemPut() == false
		&& IsTutorialreveal() == false)
	{
		g_bPause = g_bPause ? false : true;
	}

	// UIの描画状態の切り替え
	if (GetKeyboardTrigger(DIK_U))
	{
		g_bEnableUI = !g_bEnableUI;
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
		if (IsTutorialreveal() == false)
		{// チュートリアルが表示されていない場合
			/*** Aの更新 ***/

			/*** カメラの更新 ***/
			UpdateCamera();

			/*** ビルボードの更新 ***/
			UpdateBillboard();

			/*** 3Dモデルの更新 ***/
			Update3DModel();

			/*** 2Dポリゴンの更新 ***/
			Update2DPolygon();

			/*** 床の更新 ***/
			UpdateField();

			/*** 壁の更新 ***/
			UpdateWall();

			/*** メッシュの更新 ***/
			UpdateMesh();

			/*** メッシュオービットの更新 ***/
			UpdateMeshOrbit();

			/*** モデルの更新 ***/
			UpdateModel();

			/*** プレイヤーの更新 ***/
			UpdatePlayer();

			/*** アイテムの更新 ***/
			if (IsEndDialog() == true && GetCommonFade() == FADE_NONE)
			{
				UpdateItem();
			}

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

			/*** UIプレイヤーの更新 ***/
			UpdateUIplayer();

			/*** UIコレクトの更新 ***/
			UpdateUIcollect();

			/*** UIゲット更新 ***/
			UpdateUIGet();

			/*** UIショップの更新 ***/
			UpdateUIShop();

			/*** パーティクルの更新 ***/
			UpdateParticle();

			/*** エフェクトの更新 ***/
			UpdateEffect();

			/*** ラインエフェクトの更新 ***/
			UpdateLineEffect();

			/*** マップの更新 ***/
			UpdateMap();

			/*** ダイアログの更新 ***/
			UpdateDialog();

			/*** アイテムエフェクターの更新 ***/
			UpdateItemEffector();

			/*** ガイドの更新 ***/
			UpdateGuide();

			/*** スカイボックスの更新 ***/
			UpdateSkybox();

			if (IsEndDialog() == true && GetCommonFade() == FADE_NONE)
			{
				AddTimer(-1);
			}

			if ((GetKeyboardTrigger(DIK_5)
				|| GetJoypadTrigger(0, JOYKEY_BACK))
				&& IsEndDialog() == true 
				&& GetCommonFade() == FADE_NONE
				&& IsEnableItemPut() == false)
			{
				SetEnableUImenu(!GetEnableUImenu(), 0);
			}

			g_nCounterGame++;
		}

		/*** ギミックの更新 ***/
		UpdateGimmick();

		if (GetTimer() <= g_nStartLerpTime && g_fShopCylinderS < 1.0f)
		{
			g_fShopCylinderS += 0.01f;
			SetEnableMeshCylinder(g_nIdxShopCylinder, true);
			SetRadiusMeshCylinder(&GetMeshCylinder()[g_nIdxShopCylinder], Lerp(g_fRadiusShopStart, g_fRadiusShopFinish, g_fShopCylinderS));
			SetHeightMeshCylinder(&GetMeshCylinder()[g_nIdxShopCylinder], Lerp(g_fRadiusShopStart, g_fHeightShopFinish, g_fShopCylinderS));
		}
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

		/*** スカイボックスの描画 ***/
		DrawSkybox();

		/*** 床の描画  ***/
		DrawField();

		/*** 床の描画  ***/
		DrawWall();

		/*** モデルの描画 ***/
		DrawModel();

		/*** ビルボードの描画 ***/
		DrawBillboard();

		/*** プレイヤーの描画 ***/
		DrawPlayer();

		/*** アイテムの更新 ***/
		if (IsEndDialog() == true && GetCommonFade() == FADE_NONE)
		{
			/*** アイテムの描画 ***/
			DrawItem();
		}

		/*** メッシュオービットの描画 ***/
		DrawMeshOrbit();

		/*** ギミックの描画 ***/
		DrawGimmick();

		/*** プロンプトの描画 ***/
		DrawPrompt();

		/*** エフェクトの描画 ***/
		DrawEffect();

		/*** ラインエフェクトの描画 ***/
		DrawLineEffect();

		/*** 3Dモデルの描画 ***/
		Draw3DModel();

		/*** メッシュの描画 ***/
		if ((nCntDraw == PLAYERTYPE_MOUSE 
			|| (GetNumPlayer() == 1 && GetActivePlayer() == PLAYERTYPE_MOUSE)))
		{ // ネズミの場合のみZFuncを無効に
			SetEnableZFunction(pDevice, false);
			DrawMeshSphere();
			SetEnableZFunction(pDevice, true);
			DrawMeshCylinder();
		}
		else
		{
			DrawMesh();
		}

		/*** フォグをクリア ***/
		CleanFog();
	}

	// ビューポートを設定
	pDevice->SetViewport(&viewport);

	// VERTEX_2D ============================================
	/*** Aの描画 ***/

	/*** UIアームの描画 ***/
	if (IsEndDialog() == true
		&& GetCommonFade() == FADE_NONE
		&& g_bPause == false
		&& IsEnableItemPut() == false
		&& g_bEnableUI == true
		&& GetActivePlayer() == PLAYERTYPE_GIRL)
	{
		DrawUIarm();
	}

	if (g_bPause == false
		&& IsEnableItemPut() == false
		&& GetNumPlayer() != 2
		&& g_bEnableUI == true)
	{
		/*** UIプレイヤーの描画 ***/
		DrawUIplayer();
	}

	if (g_bPause == false
		&& IsEnableItemPut() == false
		&& g_bEnableUI == true)
	{
		/*** UIメニュー描画 ***/
		DrawUImenu();
	}

	/*** マップの描画 ***/
	if (IsEndDialog() == true
		&& GetCommonFade() == FADE_NONE
		&& GetActivePlayer() == PLAYERTYPE_GIRL
		&& g_bPause == false
		&& IsEnableItemPut() == false
		&& g_bEnableUI == true)
	{
		DrawMap();
	}

	if (g_bPause == false
		&& g_bEnableUI == true)
	{
		/*** 2Dポリゴンの描画 ***/
		Draw2DPolygon();
	}

	if (g_bPause == false
		&& IsEndDialog() == false)
	{
		/*** ダイアログの描画 ***/
		DrawDialog();
	}

	/*** UIアイテム描画 ***/
	if (g_bPause == false
		&& g_bEnableUI == true)
	{
		DrawUIItem();
	}

	/*** UIアイテム描画 ***/
	if (IsEndDialog() == true
		&& g_bPause == false
		&& IsEnableItemPut() == false
		&& g_bEnableUI == true)
	{
		/*** UIコレクトの更新 ***/
		DrawUIcollect();
	}

	/*** タイマーの描画 ***/
	if (g_bPause == false
		&& IsEndDialog() == true
		&& GetCommonFade() == FADE_NONE
		&& g_bEnableUI == true)
	{
		DrawTimer();
	}

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
// --- UIの描画状態 ---
//==================================================================================
bool GetEnableUI(void)
{
	return g_bEnableUI;
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

//==================================================================================
// --- ポーズ状態取得 ---
//==================================================================================
bool GetEnablePause(void)
{
	return g_bPause;
}