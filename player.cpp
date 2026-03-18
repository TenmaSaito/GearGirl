// =================================================
// 
// プレイヤー処理[mpdel.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include <stdio.h>
#include <d3dx9math.h>
#include "main.h"
#include "camera.h"
#include "common_fade.h"
#include "debugproc.h"
#include "dialog.h"
#include "effect.h"
#include "endomacro.h"
#include "field.h"
#include "game.h"
#include "gimmick.h"
#include "input.h"
#include "item.h"
#include "mesh.h"
#include "meshorbit.h"
#include "mode.h"
#include "model.h"
#include "modeldata.h"
#include "motion.h"
#include "particle.h"
#include "pause.h"
#include "parabola.h"
#include "player.h"
#include "prompt.h"
#include "sound.h"
#include "titleselect.h"
#include "UImenu.h"
#include "MathUtil.h"
#include "wall.h"
#include "2Dpolygon.h"

using namespace MyMathUtil;

// =================================================
// マクロ定義
#define PLAYER_RANGE	(20.0f)		// プレイヤーとアイテムとの当たり判定の距離
#define START_ARMTYPE	(14)		// アームの開始配列番号
#define MAX_PARTS		(17)		// 少女の全パーツ数
#define RHAND_OFFSET	D3DXVECTOR3(0.0f, 0.0f, 0.0f)		// 右手からのオフセット

// =================================================
// 平面投影の構造体
typedef struct
{
	D3DXMATRIX mtxShadow;				// シャドウマトリックス
	D3DLIGHT9 light;					// ライトの情報
	D3DXVECTOR4 posLight;				// ライトの位置
	D3DXVECTOR3 pos, normal;			// 平面の位置、法線
	D3DXPLANE plane;					// 平面情報
} ShadowMatrix;

// =================================================
// プロトタイプ宣言
void ActionPlayer(PlayerType Type, Player* pPlayer);
void UpdateMotion(PlayerType Type);	// モーションのアップデート
void SetMotionType(MOTIONTYPE motionTypeNext, bool bBlend, int nFrameBlend, PlayerType PlayerType);	// モーションの変更
void CheckMotionMove(PlayerType nPlayer, Player* pPlayer);
void MouseKeepUp(void);				// シングルプレイ時ネズミが少女についてくる処理
void UpdateArm(void);				// アームの切り替え処理
void ChangeArmType(ArmType Type);	// アームの変更
void ShotMouse(void);				// ネズミを発射
void MovePlayer(PlayerType nPlayer);	// プレイヤーの移動関数
void JumpPlayer(PlayerType nPlayer);	// プレイヤーのジャンプ関数
void RotRepair(PlayerType nPlayer);	// rotにおける逆回りを防ぐ補正
void DrawNormalPlayer(Player* pPlayer, int nCntModel, LPDIRECT3DDEVICE9 pDevice);	// プレイヤーの通常描画
void DrawShadowPlayer(Player* pPlayer, int nCntModel, LPDIRECT3DDEVICE9 pDevice);	// プレイヤーの影の描画
void CreateShadowMatrix(LPDIRECT3DDEVICE9 pDevice, D3DXMATRIX* pMtxPlayer, ShadowMatrix* pOut);		// シャドウマトリックスの作成
void CalcMatrix(Player* pPlayer);

// =================================================
// グローバル変数
Player g_aPlayer[PLAYERTYPE_MAX];	// プレイヤーの樹応報
ArmType g_armPlayer;				// 少女のアームタイプ
int g_IdxShadowPlayer = -1;			// 使用する影の番号
float g_sinrot = 0;					// sinカーブを用いるとき用の変数
int g_nNumPlayer = 1;				// プレイヤーのアクティブ人数
int g_ActivePlayer = 0;				// 操作しているプレイヤータイプ
int g_Functionkey = 0;
int g_nUseArm = 0;
int	g_nMotionCounter = 0;			// モーションカウンター
bool g_aMovePlayer[PLAYERTYPE_MAX];	// プレイヤーが動いているか
bool g_bShotMouse = false;			// ネズミを発射するフラグ
bool g_bMovable = true;			// 動ける状態かどうか
IDX_MESHORBIT g_nIdxOrbit = -1;		// メッシュオービットのインデックス
IDX_MESHORBIT g_nIdxOrbitSub = -1;	// サブメッシュオービットのインデックス
int g_nKeepUpCounter = 0;			// 一定時間指定の距離に戻れない時に回すカウンター

// =================================================
// 初期化処理
// =================================================
void InitPlayer(void)
{
	LPDIRECT3DDEVICE9 pDevice;		// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// モーション情報のインデックスを格納
	int aIdxMotion[PLAYERTYPE_MAX] = {};

	// 各種変数を初期化(0固定)
	for (int nCntPlayer = 0; nCntPlayer < PLAYERTYPE_MAX; nCntPlayer++)
	{
		g_aPlayer[nCntPlayer].posOri = PLAYER_POSDEF;
		g_aPlayer[nCntPlayer].pos = PLAYER_POSDEF;
		g_aPlayer[nCntPlayer].posOld = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aPlayer[nCntPlayer].rot = D3DXVECTOR3(0.0f, D3DX_PI, 0.0f);
		g_aPlayer[nCntPlayer].bDisp = true;
		g_aPlayer[nCntPlayer].bJump = false;
		g_aPlayer[nCntPlayer].nCounterMotion = 0;
		g_aPlayer[nCntPlayer].nKey = 0;
		g_aPlayer[nCntPlayer].state = PLAYERSTATE_NEUTRAL;
		g_aPlayer[nCntPlayer].motionType = MOTIONTYPE_NEUTRAL;
		g_aPlayer[nCntPlayer].motionTypeBlend = MOTIONTYPE_NEUTRAL;
		g_aPlayer[nCntPlayer].bFinishMotion = true;
		g_aPlayer[nCntPlayer].fMove = PLAYER_MOVE;
		g_aPlayer[nCntPlayer].bDash = false;
		g_aPlayer[nCntPlayer].bArea = false;

		if (nCntPlayer == PLAYERTYPE_GIRL)
		{
			g_aPlayer[nCntPlayer].playertype = PLAYERTYPE_GIRL;
		}
		else
		{
			g_aPlayer[nCntPlayer].playertype = PLAYERTYPE_MOUSE;
		}
	}

	// 少女のパーツ、モーションを読み込む
	LoadMotion("data\\Scripts\\geargirl.txt", &aIdxMotion[PLAYERTYPE_GIRL]);			// モーションスクリプトを読み込む
	LPPARTS_INFO pPartsInfoGirl = GetPartsInfo(aIdxMotion[PLAYERTYPE_GIRL]);	// パーツ情報のアドレスを取得
	LPMOTIONSCRIPT_INFO pMotionInfoGirl = GetMotionScriptInfo(aIdxMotion[PLAYERTYPE_GIRL]);	// モーション情報のアドレスを取得

	if (pPartsInfoGirl != NULL)
	{// NULLじゃなかったとき
		g_aPlayer[PLAYERTYPE_GIRL].PartsInfo = *pPartsInfoGirl;		// アドレスの中身のみをコピー

		for (int nCntParts = 0; nCntParts < g_aPlayer[PLAYERTYPE_GIRL].PartsInfo.nNumParts; nCntParts++)
		{
			PARTS* pParts = &g_aPlayer[PLAYERTYPE_GIRL].PartsInfo.aParts[nCntParts];
			pParts->posLocal = pParts->pos;
			pParts->rotLocal = pParts->rot;
		}
	}

	// ネズミのパーツ、モーションを読み込む
	LoadMotion("data\\Scripts\\mouse.txt", &aIdxMotion[PLAYERTYPE_MOUSE]);		// モーションスクリプトを読み込む
	LPPARTS_INFO pPartsInfoMouse = GetPartsInfo(aIdxMotion[PLAYERTYPE_MOUSE]);	// パーツ情報のアドレスを取得
	LPMOTIONSCRIPT_INFO pMotionInfoMouse = GetMotionScriptInfo(aIdxMotion[PLAYERTYPE_MOUSE]);	// モーション情報のアドレスを取得

	if (pPartsInfoMouse != NULL)
	{// NULLじゃなかったとき
		g_aPlayer[PLAYERTYPE_MOUSE].PartsInfo = *pPartsInfoMouse;	// アドレスの中身のみをコピー

		for (int nCntParts = 0; nCntParts < g_aPlayer[PLAYERTYPE_MOUSE].PartsInfo.nNumParts; nCntParts++)
		{
			PARTS* pParts = &g_aPlayer[PLAYERTYPE_MOUSE].PartsInfo.aParts[nCntParts];
			pParts->posLocal = pParts->pos;
			pParts->rotLocal = pParts->rot;
		}
	}
	// モーションのキー、フレーム情報を代入する
	for (int nCntMotion = 0; nCntMotion < MOTIONTYPE_MAX; nCntMotion++)
	{
		if (pMotionInfoGirl != NULL)
		{// NULLじゃなかったとき
			g_aPlayer[PLAYERTYPE_GIRL].aMotionInfo[nCntMotion] = pMotionInfoGirl->aMotionInfo[nCntMotion];
		}
		if (pMotionInfoMouse != NULL)
		{// NULLじゃなかったとき
			g_aPlayer[PLAYERTYPE_MOUSE].aMotionInfo[nCntMotion] = pMotionInfoMouse->aMotionInfo[nCntMotion];
		}
	}

	// === グローバル変数の初期化 === // 
	g_nNumPlayer = GetPlayerNum();		// プレイ人数
	g_ActivePlayer = 0;		// 現在の操作対象
	g_Functionkey = 0;		// デバッグ表示の切り替え用
	g_nUseArm = 0;			// 現在使用しているアームのインデックス
	g_armPlayer = ARMTYPE_NORMAL;	// 通常アーム
	g_aMovePlayer[0] = false;
	g_aMovePlayer[1] = false;
	g_bShotMouse = false;		// ネズミを発射するフラグ
	g_bMovable = true;			// 動ける状態かどうか

	if (GetFirstMode() == MODE_GAME)
	{
		g_nNumPlayer = 1;
	}

	// === メッシュオービットのインデックスを返り値でもらう === //
	g_nIdxOrbit = SetOrbit(D3DXVECTOR3(0.0f, -10.0f, 0.0f), D3DXVECTOR3(0.0f, 10.0f, 0.0f), &g_aPlayer[PLAYERTYPE_GIRL].PartsInfo.aParts[0].mtxWorld, 20);
	SetEnableOrbit(g_nIdxOrbit, false);

	g_nIdxOrbitSub = SetOrbit(D3DXVECTOR3(-10.0f, 0.0f, 0.0f), D3DXVECTOR3(10.0f, 0.0f, 0.0f), &g_aPlayer[PLAYERTYPE_GIRL].PartsInfo.aParts[0].mtxWorld, 20);
	SetEnableOrbit(g_nIdxOrbitSub, false);

	// デバイスの破棄
	EndDevice();
}

// =================================================
// 終了処理
// =================================================
void UninitPlayer(void)
{

}

// =================================================
// 更新処理
// =================================================
void UpdatePlayer(void)
{
	// プレイヤー構造体をポインタ化
	Player* pPlayer = &g_aPlayer[0];
	Player* pMouse = &g_aPlayer[1];

	// カメラの情報を取得
	Camera* pCamera = GetCamera();

	// 駅のギミック情報を取得
	Gimmick* pGimmick = GetGimmick() + 5;

	// 汎用フェードの情報の取得
	FADE Fade = GetCommonFade();

	pPlayer->bUseLandMotion = false;
	g_bMovable = true;	// 移動を可能に

	for (int nCntPlayer = 0; nCntPlayer < PLAYERTYPE_MAX; nCntPlayer++, pPlayer++)
	{
		g_aMovePlayer[nCntPlayer] = false;

		// === 現在位置の保存 === //
		pPlayer->posOld = pPlayer->pos;

		if (GAME_NOW && ITEMPROMPT_OFF && GetEnableUImenu() == false)
		{// ゲーム本編中
			// === ２人プレイもしくはアクティブなプレイヤーの処理 === //
			if (GetNumPlayer() == 2 || GetActivePlayer() == PlayerType(nCntPlayer))
			{
				ActionPlayer((PlayerType)nCntPlayer, pPlayer);
			}

			// === 少女操作時のみの処理 === //
			if (nCntPlayer == PLAYERTYPE_GIRL)
			{
				// 腕の切り替え
				UpdateArm();

				// カタパルトを起動した後の処理
				ShotMouse();
			}
		}

		// === 移動に関する処理 === //
		if (DIAROG_OFF & UIMENU_OFF)
		{// 何もダイアログがでいない場合に動ける
			// === ２人プレイもしくはアクティブなプレイヤーの処理 === //
			if (GetNumPlayer() == 2 || GetActivePlayer() == PlayerType(nCntPlayer))
			{
				if (g_aPlayer[PLAYERTYPE_GIRL].motionType != MOTIONTYPE_CUTTING
					&& g_aPlayer[PLAYERTYPE_GIRL].motionTypeBlend != MOTIONTYPE_CUTTING
					&& g_aPlayer[PLAYERTYPE_GIRL].motionType != MOTIONTYPE_ACTION
					&& g_aPlayer[PLAYERTYPE_GIRL].motionTypeBlend != MOTIONTYPE_ACTION
					&& g_aPlayer[PLAYERTYPE_GIRL].motionType != MOTIONTYPE_VALVE
					&& g_aPlayer[PLAYERTYPE_GIRL].motionTypeBlend != MOTIONTYPE_VALVE
					&& Fade == FADE_NONE)
				{
					MovePlayer((PlayerType)nCntPlayer);	// 移動に関する処理
				}

				// 左スティックを押し込むとダッシュ状態(速度)に
				if (pPlayer->motionType == MOTIONTYPE_MOVE && nCntPlayer == PLAYERTYPE_GIRL && GetJoypadTrigger(0, JOYKEY_LEFT_PUSH) == true)
				{
					pPlayer->bDash = pPlayer->bDash ^ true;
					pPlayer->fMove = PLAYER_MOVE * 1.5f;

					// ダッシュ時に軌跡を出す
					//SetEnableOrbit(g_nIdxOrbit, true);
					//SetOffSetOrbit(g_nIdxOrbit, D3DXVECTOR3(0.0f, -10.0f, 0.0f), D3DXVECTOR3(0.0f, 10.0f, 0.0f));
					//SetEnableOrbit(g_nIdxOrbitSub, true);
					//SetOffSetOrbit(g_nIdxOrbitSub, D3DXVECTOR3(-10.0f, 0.0f, 0.0f), D3DXVECTOR3(10.0f, 0.0f, 0.0f));

					if (pPlayer->bDash == true)
					{// ダッシュ時の風の音
						PlaySound(SOUND_LABEL_SE_G_DASH);
					}
				}
				else if (pPlayer->motionType == MOTIONTYPE_MOVE && nCntPlayer == PLAYERTYPE_GIRL && GetKeyboardTrigger(DIK_LSHIFT) == true)
				{// キーボードだと左shiftキー
					pPlayer->bDash = pPlayer->bDash ^ true;
					pPlayer->fMove = PLAYER_MOVE * 1.5f;

					// ダッシュ時に軌跡を出す
					//SetEnableOrbit(g_nIdxOrbit, true);
					//SetOffSetOrbit(g_nIdxOrbit, D3DXVECTOR3(0.0f, -10.0f, 0.0f), D3DXVECTOR3(0.0f, 10.0f, 0.0f));
					//SetEnableOrbit(g_nIdxOrbitSub, true);
					//SetOffSetOrbit(g_nIdxOrbitSub, D3DXVECTOR3(-10.0f, 0.0f, 0.0f), D3DXVECTOR3(10.0f, 0.0f, 0.0f));

					if (pPlayer->bDash == true)
					{// ダッシュ時の風の音
						PlaySound(SOUND_LABEL_SE_G_DASH);
					}
				}

				if (pPlayer->bDash == false)
				{// ダッシュ状態じゃなければ普通の速度
					pPlayer->fMove = PLAYER_MOVE;
					SetEnableOrbit(g_nIdxOrbit, false);
					SetEnableOrbit(g_nIdxOrbitSub, false);
				}

				if (pPlayer->state != PLAYERSTATE_THROWWAITING)
				{
					JumpPlayer((PlayerType)nCntPlayer);	// ジャンプに関する処理
				}
			}

			// === 少女操作時のねずみの処理 === //
			if (GetActivePlayer() == PLAYERTYPE_GIRL && nCntPlayer == PLAYERTYPE_MOUSE)
			{
				// 少女にネズミが追従する処理
				MouseKeepUp();
			}
		}

		if (TUTORIAL_NOW)
		{// チュートリアル中、特別な移動制限をかける
			if (pPlayer->pos.x > 1620.0f)
			{
				pPlayer->pos.x = 1620.0f;
			}
			if (pPlayer->pos.x < 1375.0f)
			{
				pPlayer->pos.x = 1375.0f;
			}
			if (pPlayer->pos.z > -440.0f)
			{
				pPlayer->pos.z = -440.0f;
			}
			if (pPlayer->pos.z < -1070.0f)
			{
				pPlayer->pos.z = -1070.0f;
			}
		}

		// === 投げモーション以外時は常時再生(投げ待機状態をキープするため) === //
		if (pPlayer->state != PLAYERSTATE_THROWWAITING)
		{
			// モーションの更新
			UpdateMotion((PlayerType)nCntPlayer);
		}

		// === ネズミ射出の予測軌跡の描画 === //
		if (pPlayer->state == PLAYERSTATE_THROWWAITING && nCntPlayer == PLAYERTYPE_GIRL)
		{
			D3DXVECTOR3 ParabolaVec = GetParabolaVec();
			D3DXVECTOR3 offset = RHAND_OFFSET;

			// 手にくっつけて、放物線の場所を確定させた位置から描画
			D3DXVec3TransformCoord(&g_aPlayer[PLAYERTYPE_MOUSE].pos, &offset, &pPlayer->PartsInfo.aParts[19].mtxWorld);

			// エフェクトの描画
			SetParabola(g_aPlayer[PLAYERTYPE_MOUSE].pos, ParabolaVec, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), 2.0f, 2.0f, 1.0f, true);
		}

		// === 何もしていない場合(何も入力されていない場合) === //
		if ((pPlayer->motionType != MOTIONTYPE_ACTION
			&& pPlayer->motionType != MOTIONTYPE_JUMP
			&& pPlayer->motionType != MOTIONTYPE_LANDING
			&& pPlayer->motionTypeBlend != MOTIONTYPE_ACTION
			&& pPlayer->motionTypeBlend != MOTIONTYPE_JUMP
			&& pPlayer->motionTypeBlend != MOTIONTYPE_LANDING
			&& pPlayer->motionType != MOTIONTYPE_NEUTRAL
			&& pPlayer->motionTypeBlend != MOTIONTYPE_NEUTRAL
			&& pPlayer->motionType != MOTIONTYPE_CUTTING
			&& pPlayer->motionTypeBlend != MOTIONTYPE_CUTTING
			&& pPlayer->motionType != MOTIONTYPE_VALVE
			&& pPlayer->motionTypeBlend != MOTIONTYPE_VALVE
			&& g_aMovePlayer[nCntPlayer] == false))
		{
			SetMotionType(MOTIONTYPE_NEUTRAL, true, 10, (PlayerType)nCntPlayer);
			pPlayer->bDash = false;
		}

		// === 重力をかけ続ける === //
		pPlayer->move.y += GRAVITY;

		// === マップの限界値まで行った時に、各移動量を0にする === //
		if (pPlayer->pos.z <= MAX_ZMOVE1)
		{// Z軸
			pPlayer->pos.z = MAX_ZMOVE1;
		}
		if (pPlayer->pos.z >= MAX_ZMOVE2)
		{
			pPlayer->pos.z = MAX_ZMOVE2;
		}
		if (pPlayer->pos.x <= MAX_XMOVE1)
		{// X軸
			pPlayer->pos.x = MAX_XMOVE1;
		}
		if (pPlayer->pos.x >= MAX_XMOVE2)
		{
			pPlayer->pos.x = MAX_XMOVE2;
		}

		// 駅とガラスの当たり判定のダブり解決用
		if (pPlayer->pos.x < 710.0f && pPlayer->pos.x >= 690.0f && pPlayer->pos.z <= -700.0f)
		{
			pPlayer->pos.x = 690.0f;
		}

		// === 駅のガラスの裏にいかないようにする === //
		if (pPlayer->pos.x <= 700.0f)
		{
			if (pPlayer->pos.z <= -1160.0f)
			{
				pPlayer->pos.z = -1160.0f;
			}
		}

		// === 移動量の更新 === //
		pPlayer->pos += pPlayer->move;

		// === 慣性を掛ける === //
		pPlayer->move.x += (0.0f - pPlayer->move.x) * (PLAYER_INI * 1.5f);
		pPlayer->move.z += (0.0f - pPlayer->move.z) * (PLAYER_INI * 1.5f);

		// === 地面に沈んだ時 === //
		if (pPlayer->pos.y < 100.0f)
		{
			pPlayer->pos.y = 100.0f;

			if (pPlayer->bJump == true
				&& pPlayer->motionType != MOTIONTYPE_LANDING
				&& pPlayer->motionTypeBlend != MOTIONTYPE_LANDING)
			{// 着地モーション
				SetMotionType(MOTIONTYPE_LANDING, true, 15, (PlayerType)nCntPlayer);
				pPlayer->bUseLandMotion = true;
			}

			if (nCntPlayer == PLAYERTYPE_MOUSE)
			{// 着地時に予想着地点を消す
				int nIdxfield = GetIdxEffectField();
				SetEnableField(nIdxfield, false);
			}

			pPlayer->bJump = false;
		}

		// === モデルとの当たり判定 === //
		if (nCntPlayer == PLAYERTYPE_GIRL || (nCntPlayer == PLAYERTYPE_MOUSE && GetActivePlayer() == PLAYERTYPE_MOUSE) || GetNumPlayer() == 2)
		{// 少女に対してと、ネズミは操作しているときのみ
			if (CollisionModel(&pPlayer->pos, &pPlayer->posOld, &pPlayer->move, 5, 50))
			{
				if (pPlayer->bJump == true
					&& pPlayer->motionType != MOTIONTYPE_LANDING
					&& pPlayer->motionTypeBlend != MOTIONTYPE_LANDING)
				{// 着地モーション
					SetMotionType(MOTIONTYPE_LANDING, true, 15, (PlayerType)nCntPlayer);
					pPlayer->bUseLandMotion = true;
				}

				if (nCntPlayer == PLAYERTYPE_MOUSE)
				{// 着地時に予想着地点を消す
					int nIdxfield = GetIdxEffectField();
					SetEnableField(nIdxfield, false);
				}
				pPlayer->bJump = false;
			}
		}



		// === ギミックとの当たり判定 === //
		if (nCntPlayer == PLAYERTYPE_GIRL || (nCntPlayer == PLAYERTYPE_MOUSE && GetActivePlayer() == PLAYERTYPE_MOUSE) || GetNumPlayer() == 2)
		{// 少女に対してと、ネズミは操作しているときのみ
			if (CollisionGimmick(&pPlayer->pos, &pPlayer->posOld, &pPlayer->move, &g_aPlayer[nCntPlayer], 5.0f, 20.0f - (nCntPlayer * 18.0f)))
			{
				if (pPlayer->bJump == true
					&& pPlayer->motionType != MOTIONTYPE_LANDING
					&& pPlayer->motionTypeBlend != MOTIONTYPE_LANDING)
				{// 着地モーション
					SetMotionType(MOTIONTYPE_LANDING, true, 15, (PlayerType)nCntPlayer);
					pPlayer->bUseLandMotion = true;
				}

				if (nCntPlayer == PLAYERTYPE_MOUSE)
				{// 着地時に予想着地点を消す
					int nIdxfield = GetIdxEffectField();
					SetEnableField(nIdxfield, false);
				}

				pPlayer->bJump = false;
			}
		}

		// === アイテムとの当たり判定 === //
		if (GetNumPlayer() == 2)
		{// 2人プレイ時
			if (GAME_NOW)
			{// ゲーム本編中
				CollisionItem(pPlayer->pos, PLAYER_RANGE, nCntPlayer);
			}
		}

		// === 壁との当たり判定 === //
		CollisionWall(&pPlayer->pos, &pPlayer->posOld, &pPlayer->move);

		// デバッグ表示
		if (g_Functionkey != 0)
		{
			PrintDebugProc("\nPlayer座標 : [%~3f]\n", pPlayer->pos.x, pPlayer->pos.y, pPlayer->pos.z);
		}
	}

	// === 操作している対象以外がアイテムをとってしまうのを防ぐ === //
	if (GetNumPlayer() == 1)
	{// 1人プレイ時
		if (GAME_NOW)
		{// ゲーム本編中
			// アイテムとの当たり判定
			if (GetActivePlayer() == PLAYERTYPE_GIRL)
			{
				CollisionItem(g_aPlayer[PLAYERTYPE_GIRL].pos, PLAYER_RANGE, PLAYERTYPE_GIRL);
			}
			else if (GetActivePlayer() == PLAYERTYPE_MOUSE)
			{
				CollisionItem(g_aPlayer[PLAYERTYPE_MOUSE].pos, PLAYER_RANGE, PLAYERTYPE_MOUSE);
			}
		}
	}

	// === プロンプトを描画(便利屋) === //
	DetectionPrompt(g_aPlayer[PLAYERTYPE_GIRL].pos, 30.0f);

	if (GAME_NOW)
	{// === チュートリアル外での更新 === // 
		if (ITEM_OFF & UIMENU_OFF)
		{// 提出する画面じゃないとき
			// 操作する対象を切り替える
			if (g_nNumPlayer == 1)
			{// シングルプレイ時
				if (GetJoypadTrigger(0, JOYKEY_LB) == true || GetKeyboardTrigger(DIK_Q) == true)
				{// 特定のモーション中は切り替え不可
					if (g_aPlayer[PLAYERTYPE_GIRL].motionType != MOTIONTYPE_ACTION && g_aPlayer[PLAYERTYPE_GIRL].motionTypeBlend != MOTIONTYPE_ACTION
						&& g_aPlayer[PLAYERTYPE_GIRL].motionType != MOTIONTYPE_CUTTING && g_aPlayer[PLAYERTYPE_GIRL].motionTypeBlend != MOTIONTYPE_CUTTING
						&& g_aPlayer[PLAYERTYPE_GIRL].motionType != MOTIONTYPE_VALVE && g_aPlayer[PLAYERTYPE_GIRL].motionTypeBlend != MOTIONTYPE_VALVE)
					{
						if (g_aPlayer[PLAYERTYPE_MOUSE].pos.z <= -774.0f && g_aPlayer[PLAYERTYPE_MOUSE].pos.x <= 700.0f || g_aPlayer[PLAYERTYPE_GIRL].bChangeable == false)
						{// 駅の範囲内なら切り替えを行わないようにする
							// 切り替え状態を不可にする
							g_aPlayer[PLAYERTYPE_GIRL].bArea = true;
							g_aPlayer[PLAYERTYPE_MOUSE].bArea = true;
						}
						else
						{
							// 切り替え可能状態にする
							g_aPlayer[PLAYERTYPE_GIRL].bArea = false;
							g_aPlayer[PLAYERTYPE_MOUSE].bArea = false;

							if (g_aPlayer[PLAYERTYPE_GIRL].state != PLAYERSTATE_THROWWAITING && g_bShotMouse == false)
							{
								PlaySound(SOUND_LABEL_SE_G_CHARACHANGE);
								if (g_ActivePlayer == 1)
								{// ネズミ→少女
									g_ActivePlayer = 0;
								}
								else
								{// 少女→ネズミ
									g_ActivePlayer = 1;
								}
							}
						}
					}
				}
			}
		}
	}
#ifdef _DEBUG
	// ***************************************************************************
	// デバッグ処理
		// デバッグ用のプレイ人数切り替え処理
	ChangeNumPlayer();

	// F1キーを押して、デバッグ表示のON/OFFを切り替える
	if (GetKeyboardTrigger(DIK_F1) == true)
	{
		if (g_Functionkey == 1)
		{
			g_Functionkey = 0;
		}
		else
		{
			g_Functionkey = 1;
		}
	}

	PrintDebugProc("\nPlayerデバッグ表示切り替え : [F1]\n");

	// デバッグ表示
	if (g_Functionkey != 0)
	{
		PrintDebugProc("\nPlayer座標 : [%~3f]\n", pPlayer->pos.x, pPlayer->pos.y, pPlayer->pos.z);

		PrintDebugProc("\nPlayer切り替え : [Q] : [%d]\n", g_ActivePlayer);
		PrintDebugProc("操作対象 : 0 → 少女\n                1 → ネズミ\n");
		PrintDebugProc("アームの切り替え [9] : ArmType [%d]\n", g_nUseArm);

		PrintDebugProc("\nPlayer0 : [SPACE] :  JUMP\n");
		PrintDebugProc("Player1 : [RSHIFT] :  JUMP\n");
	}
	// ***************************************************************************
#endif
}

// =================================================
// 描画処理
// =================================================
void DrawPlayer(void)
{
	// プレイヤー構造体をポインタ化
	Player* pPlayer = &g_aPlayer[0];

	AUTODEVICE9 Auto;		// デバイスの自動取得
	LPDIRECT3DDEVICE9 pDevice = Auto.pDevice;		// デバイスへのポインタ

	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス
	D3DMATERIAL9 matDef;			// 現在のマテリアル保存用

	for (int nCntPlayer = 0; nCntPlayer < PLAYERTYPE_MAX; nCntPlayer++, pPlayer++)
	{
		// プレイヤーのワールドマトリックスの初期化
		D3DXMatrixIdentity(&pPlayer->mtxWorld);

		D3DXVECTOR3 posTrans = pPlayer->pos;
		D3DXVECTOR3 rotTrans = pPlayer->rot;

		if (nCntPlayer == PLAYERTYPE_MOUSE && g_aPlayer[PLAYERTYPE_GIRL].state == PLAYERSTATE_THROWWAITING)
		{// オール0
			posTrans = VECNULL;
			rotTrans = VECNULL;
		}

		// プレイヤーの向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot,
			rotTrans.y, rotTrans.x, rotTrans.z);
		D3DXMatrixMultiply(&pPlayer->mtxWorld, &pPlayer->mtxWorld, &mtxRot);	// かけ合わせる

		// プレイヤーの位置を反映
		D3DXMatrixTranslation(&mtxTrans,
			posTrans.x, posTrans.y, posTrans.z);
		D3DXMatrixMultiply(&pPlayer->mtxWorld, &pPlayer->mtxWorld, &mtxTrans);	// かけ合わせる

		if (nCntPlayer == PLAYERTYPE_MOUSE && g_aPlayer[PLAYERTYPE_GIRL].state == PLAYERSTATE_THROWWAITING)
		{// 右手にくっつける
			D3DXMatrixMultiply(&pPlayer->mtxWorld, &pPlayer->mtxWorld, &g_aPlayer[PLAYERTYPE_GIRL].PartsInfo.aParts[19].mtxWorld);	// かけ合わせる
		}

		// 現在のマテリアルを取得(保存)
		pDevice->GetMaterial(&matDef);

		// 各パーツのマトリックスを計算
		CalcMatrix(pPlayer);

		// 全モデル(パーツ)の影の描画
		for (int nCntModel = 0; nCntModel < pPlayer->PartsInfo.nNumParts; nCntModel++)
		{
			if (nCntModel < START_ARMTYPE)
			{
				// 影の描画
				DrawShadowPlayer(pPlayer, nCntModel, pDevice);
			}
			else
			{
				if (nCntModel >= MAX_PARTS) break;
				int nCntModelArm = nCntModel + (3 * g_armPlayer);

				// 影の描画(腕切り替え版)
				DrawShadowPlayer(pPlayer, nCntModelArm, pDevice);
			}
		}

		// プレイヤーのワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &pPlayer->mtxWorld);

		// 全モデル(パーツ)の描画
		for (int nCntModel = 0; nCntModel < pPlayer->PartsInfo.nNumParts; nCntModel++)
		{
			if (nCntModel < START_ARMTYPE)
			{
				// 通常描画
				DrawNormalPlayer(pPlayer, nCntModel, pDevice);
			}
			else
			{
				if (nCntModel >= MAX_PARTS) break;
				int nCntModelArm = nCntModel + (3 * g_armPlayer);

				// 通常描画(腕切り替え版)
				DrawNormalPlayer(pPlayer, nCntModelArm, pDevice);
			}
		}

		// 保存していたマテリアルを戻す
		pDevice->SetMaterial(&matDef);
	}
}

// =================================================
// プレイヤーのマトリックスの計算処理
// =================================================
void CalcMatrix(Player* pPlayer)
{
	// 全モデル(パーツ)のマトリックスの計算
	for (int nCntModel = 0; nCntModel < pPlayer->PartsInfo.nNumParts; nCntModel++)
	{
		D3DXMATRIX mtxRotModel, mtxTransModel;	// 計算用マトリックス
		D3DXMATRIX mtxParent;	// 親のマトリックス

		// パーツのワールドマトリックス初期化
		D3DXMatrixIdentity(&pPlayer->PartsInfo.aParts[nCntModel].mtxWorld);

		// パーツの向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRotModel,
			pPlayer->PartsInfo.aParts[nCntModel].rot.y,
			pPlayer->PartsInfo.aParts[nCntModel].rot.x,
			pPlayer->PartsInfo.aParts[nCntModel].rot.z);

		// かけ合わせる
		D3DXMatrixMultiply(&pPlayer->PartsInfo.aParts[nCntModel].mtxWorld,
			&pPlayer->PartsInfo.aParts[nCntModel].mtxWorld,
			&mtxRotModel);

		// パーツの位置(オフセット)を反映
		D3DXMatrixTranslation(&mtxTransModel,
			pPlayer->PartsInfo.aParts[nCntModel].pos.x,
			pPlayer->PartsInfo.aParts[nCntModel].pos.y,
			pPlayer->PartsInfo.aParts[nCntModel].pos.z);

		// かけ合わせる
		D3DXMatrixMultiply(&pPlayer->PartsInfo.aParts[nCntModel].mtxWorld,
			&pPlayer->PartsInfo.aParts[nCntModel].mtxWorld,
			&mtxTransModel);

		// パーツの「親マトリックス」の設定
		if (pPlayer->PartsInfo.aParts[nCntModel].nIdxModelParent != -1)
		{// 親モデルが存在する
			mtxParent = pPlayer->PartsInfo.aParts[pPlayer->PartsInfo.aParts[nCntModel].nIdxModelParent].mtxWorld;
		}
		else
		{// 親モデルが存在しない
			mtxParent = pPlayer->mtxWorld;
		}

		// 算出した「パーツのワールドマトリックス」と「親のマトリックス」をかけ合わせる
		D3DXMatrixMultiply(&pPlayer->PartsInfo.aParts[nCntModel].mtxWorld,
			&pPlayer->PartsInfo.aParts[nCntModel].mtxWorld,
			&mtxParent);
	}
}

// =================================================
// プレイヤーの描画処理
// =================================================
void DrawNormalPlayer(Player* pPlayer, int nCntModel, LPDIRECT3DDEVICE9 pDevice)
{
	// 各NULLCHECK
	if (pPlayer == nullptr)
	{
		OutputDebugString(TEXT("pPlayerが設定されていませんよ！"));
		return;
	}

	if (pDevice == nullptr)
	{
		OutputDebugString(TEXT("pDeviceが設定されていませんよ！"));
		return;
	}

	D3DXMATERIAL* pMat;				// マテリアルデータへのポインタ

	// パーツのワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD,
		&pPlayer->PartsInfo.aParts[nCntModel].mtxWorld);

	LPMODELDATA pModelData = GetModelData(pPlayer->PartsInfo.aParts[nCntModel].nIdxModel);
	if (pModelData != NULL)
	{
		// パーツの描画	
			// マテリアルデータへのポインタを取得
		pMat = (D3DXMATERIAL*)pModelData->pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)pModelData->dwNumMat; nCntMat++)
		{
			// マテリアルの設定
			pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

			// テクスチャの設定
			pDevice->SetTexture(0, pModelData->apTexture[nCntMat]);

			// モデル(パーツ)の描画
			pModelData->pMesh->DrawSubset(nCntMat);
		}
	}
}

// =================================================
// シャドウマトリックスの作成処理
// =================================================
void CreateShadowMatrix(LPDIRECT3DDEVICE9 pDevice, D3DXMATRIX* pMtx, ShadowMatrix* pOut)
{
	// 各NULLCHECK
	if (pDevice == nullptr)
	{
		OutputDebugString(TEXT("pDeviceが設定されていませんよ！"));
		return;
	}

	if (pMtx == nullptr)
	{
		OutputDebugString(TEXT("pMtxが設定されていませんよ！"));
		return;
	}

	if (pOut == nullptr)
	{
		OutputDebugString(TEXT("pOutが設定されていませんよ！"));
		return;
	}

	D3DLIGHT9 light;

	// ライトの位置を設定
	pDevice->GetLight(0, &light);
	pOut->posLight = D3DXVECTOR4(-light.Direction.x, -light.Direction.y, -light.Direction.z, 0.0f);

	// 平面作成
	pOut->pos = D3DXVECTOR3(0, 100, 0);
	pOut->normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	D3DXPlaneFromPointNormal(&pOut->plane, &pOut->pos, &pOut->normal);

	// シャドウマトリックスの初期化
	D3DXMatrixIdentity(&pOut->mtxShadow);

	// シャドウマトリックスの作成
	D3DXMatrixShadow(&pOut->mtxShadow, &pOut->posLight, &pOut->plane);
	D3DXMatrixMultiply(&pOut->mtxShadow, pMtx, &pOut->mtxShadow);
}

// =================================================
// プレイヤーの影の描画処理
// =================================================
void DrawShadowPlayer(Player* pPlayer, int nCntModel, LPDIRECT3DDEVICE9 pDevice)
{
	// 各NULLCHECK
	if (pPlayer == nullptr)
	{
		OutputDebugString(TEXT("pPlayerが設定されていませんよ！"));
		return;
	}

	if (pDevice == nullptr)
	{
		OutputDebugString(TEXT("pDeviceが設定されていませんよ！"));
		return;
	}

	D3DXMATERIAL* pMat;				// マテリアルデータへのポインタ
	D3DMATERIAL9 shadowMat;			// マテリアル変更用
	ShadowMatrix shadow;			// 平面投影関連

	// シャドウマトリックスの作成
	CreateShadowMatrix(pDevice, &pPlayer->PartsInfo.aParts[nCntModel].mtxWorld, &shadow);

	// プレイヤーのワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &shadow.mtxShadow);

	LPMODELDATA pModelData = GetModelData(pPlayer->PartsInfo.aParts[nCntModel].nIdxModel);
	if (pModelData != NULL)
	{
		// パーツの描画	
			// マテリアルデータへのポインタを取得
		pMat = (D3DXMATERIAL*)pModelData->pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)pModelData->dwNumMat; nCntMat++)
		{
			shadowMat = pMat[nCntMat].MatD3D;
			shadowMat.Diffuse = { 0, 0, 0, 1 };		// 色を黒に設定

			// マテリアルの設定
			pDevice->SetMaterial(&shadowMat);

			// テクスチャの設定
			pDevice->SetTexture(0, pModelData->apTexture[nCntMat]);

			// モデル(パーツ)の描画
			pModelData->pMesh->DrawSubset(nCntMat);
		}
	}
}


// =================================================
// プレイヤーの情報を渡す
// =================================================
Player* GetPlayer(void)
{
	return &g_aPlayer[0];
}

// =================================================
// プレイヤーの行動関数
// =================================================
void ActionPlayer(PlayerType nPlayer, Player* pPlayer)
{
	Gimmick* pGimmick = GetGimmick() + 7;

	if (nPlayer == PLAYERTYPE_GIRL)
	{// 少女の処理
		if (GetKeyboardTrigger(DIK_RETURN) == true || GetJoypadTrigger(0, JOYKEY_B) == true)
		{
			switch (g_armPlayer)
			{
				// NORMAL
			case ARMTYPE_NORMAL:
				if (IsDetection(pGimmick->pos, pPlayer->pos, pGimmick->fRadius) == true)
				{
					if (pPlayer->pos.x >= 1115.0f)
					{// バルブの前でのみ可能
						g_bMovable = false;	// 移動を不可能に
						pPlayer->pos = D3DXVECTOR3(1120.0f, 100.0f, 109.0f);
						pPlayer->rot = D3DXVECTOR3(0.0f, D3DX_HALFPI, 0.0f);
						// 連打防止
						if (pPlayer->motionType != MOTIONTYPE_VALVE && pPlayer->motionTypeBlend != MOTIONTYPE_VALVE)
						{
							SetMotionType(MOTIONTYPE_VALVE, true, 10, nPlayer);
							CameraReset();
						}
						PlaySound(SOUND_LABEL_SE_G_VALVE);
					}
				}

				break;
				// CATAPULT
			case ARMTYPE_CATAPULT:
				if (IsClearGimmick(GIMMICKTYPE_BIGBUTTON) == true)
				{
					if (pPlayer->pos.x < 700.0f && pPlayer->pos.z < -510.0f)
					{// 駅のギミック周辺でしか使えないように設定
						if (pPlayer->state != PLAYERSTATE_THROWWAITING && GetNumPlayer() == 1)
						{// 1人プレイ時
							PlaySound(SOUND_LABEL_SE_G_THROW);
							SetMotionType(MOTIONTYPE_ACTION, true, 10, nPlayer);
							g_nMotionCounter = 10;
							g_bShotMouse = true;
						}
						else if (pPlayer->state != PLAYERSTATE_THROWWAITING && GetNumPlayer() == 2)
						{// 2人プレイ時
							if (g_aPlayer[PLAYERTYPE_MOUSE].pos.x - g_aPlayer[PLAYERTYPE_GIRL].pos.x <= 20.0f
								&& g_aPlayer[PLAYERTYPE_MOUSE].pos.x - g_aPlayer[PLAYERTYPE_GIRL].pos.x >= -20.0f
								&& g_aPlayer[PLAYERTYPE_MOUSE].pos.z - g_aPlayer[PLAYERTYPE_GIRL].pos.z <= 20.0f
								&& g_aPlayer[PLAYERTYPE_MOUSE].pos.z - g_aPlayer[PLAYERTYPE_GIRL].pos.z >= -20.0f)
							{
								SetMotionType(MOTIONTYPE_ACTION, true, 10, nPlayer);
								g_nMotionCounter = 10;
								g_bShotMouse = true;
							}
						}
					}
				}
				break;

				// CUT
			case ARMTYPE_CUT:

				if (g_aPlayer[PLAYERTYPE_GIRL].pos.z >= 200.0f && g_aPlayer[PLAYERTYPE_GIRL].pos.z <= 700.0f
					&& g_aPlayer[PLAYERTYPE_GIRL].pos.x >= 1670.0f && g_aPlayer[PLAYERTYPE_GIRL].pos.x <= 2160.0f)
				{// 教会の敷地内でのみ使用可能
					// 連打防止
					if (pPlayer->motionType != MOTIONTYPE_CUTTING && pPlayer->motionTypeBlend != MOTIONTYPE_CUTTING)
					{
						SetMotionType(MOTIONTYPE_CUTTING, true, 10, nPlayer);
					}
					PlaySound(SOUND_LABEL_SE_G_CHAINSAW);
					g_nMotionCounter = 8;
				}
				break;

			default:
				break;
			}
		}
	}
	else
	{

	}
}

// =================================================
// プレイヤーの移動関数
// =================================================
void MovePlayer(PlayerType nPlayer)
{
	// プレイヤー構造体をポインタ化
	Player* pPlayer = &g_aPlayer[nPlayer];

	if (ITEM_OFF)
	{
		// 歩行音再生
		if (nPlayer == PLAYERTYPE_GIRL)
		{ // 主人公
			if (!IsPlayingSound(SOUND_LABEL_SE_G_MOVE) && !IsPlayingSound(SOUND_LABEL_SE_G_MOVEDASH)
				&& pPlayer->motionType == MOTIONTYPE_MOVE)
			{
				if (pPlayer->bDash == false)
				{// 非ダッシュ時、ダッシュ音を止めて歩き効果音
					StopSound(SOUND_LABEL_SE_G_MOVEDASH);
					PlaySound(SOUND_LABEL_SE_G_MOVE);
				}
				else
				{// ダッシュ時、歩き効果音を止めてダッシュ音
					StopSound(SOUND_LABEL_SE_G_MOVE);
					PlaySound(SOUND_LABEL_SE_G_MOVEDASH);
				}
			}
			else if (pPlayer->motionType != MOTIONTYPE_MOVE)
			{// 移動しなくなったら、どちらも停止
				StopSound(SOUND_LABEL_SE_G_MOVE);
				StopSound(SOUND_LABEL_SE_G_MOVEDASH);
			}
		}
		else
		{ // ネズミ
			if (!IsPlayingSound(SOUND_LABEL_SE_G_MOUSEMOVE)
				&& pPlayer->motionType == MOTIONTYPE_MOVE)
			{
				PlaySound(SOUND_LABEL_SE_G_MOUSEMOVE);
			}
			else if (pPlayer->motionType != MOTIONTYPE_MOVE)
			{
				StopSound(SOUND_LABEL_SE_G_MOUSEMOVE);
			}
		}

		// カメラの向きを保存する
		vec3 Camerarot;

		// カメラの情報を取得
		GetCameraRot(nPlayer, &Camerarot);

		// 左スティックの情報
		vec3 stick = vec3_ZERO;

		if (nPlayer == 0)
		{// 少女の操作
			if (GetJoypadLeftStick(nPlayer, &stick))
			{// スティックの入力がある
				CheckMotionMove(nPlayer, pPlayer);

				// プレイヤーの状態を移動状態に
				pPlayer->state = PLAYERSTATE_MOVE;

				pPlayer->move.x += (sinf(-Camerarot.y) * stick.y + cosf(-Camerarot.y) * stick.x) * pPlayer->fMove;
				pPlayer->move.z += (cosf(Camerarot.y) * -stick.y + sinf(Camerarot.y) * -stick.x) * pPlayer->fMove;


				pPlayer->rotDest.y = atan2f(-pPlayer->move.x, -pPlayer->move.z);	// 目標の角度を設定
				pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

				// rotの補正
				RotRepair(nPlayer);
			}
			else if (GetKeyboardPress(DIK_A) == true)
			{//Aキーが押される	
				CheckMotionMove(nPlayer, pPlayer);

				// プレイヤーの状態を移動状態に
				pPlayer->state = PLAYERSTATE_MOVE;

				if (GetKeyboardPress(DIK_W) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_UP))
				{// WとA(左上)の入力
					pPlayer->move.x += sinf(Camerarot.y - D3DX_PI * 0.25f) * pPlayer->fMove;
					pPlayer->move.z += cosf(-Camerarot.y + D3DX_PI * 0.25f) * pPlayer->fMove;

					pPlayer->rotDest.y = D3DX_PI * 0.75f + Camerarot.y;	// 目標の角度を設定
					pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

					// rotの補正
					RotRepair(nPlayer);
				}
				else if (GetKeyboardPress(DIK_S) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_DOWN))
				{// SとA(左下)の入力
					pPlayer->move.x += sinf(Camerarot.y - D3DX_PI * 0.75f) * pPlayer->fMove;
					pPlayer->move.z += cosf(Camerarot.y - D3DX_PI * 0.75f) * pPlayer->fMove;

					pPlayer->rotDest.y = D3DX_PI * 0.25f + Camerarot.y;	// 目標の角度を設定
					pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

					// rotの補正
					RotRepair(nPlayer);
				}
				else
				{// A単体の入力
					pPlayer->move.x += sinf(Camerarot.y - D3DX_PI * 0.5f) * pPlayer->fMove;
					pPlayer->move.z += cosf(Camerarot.y - D3DX_PI * 0.5f) * pPlayer->fMove;

					pPlayer->rotDest.y = D3DX_PI * 0.5f + Camerarot.y;	// 目標の角度を設定
					pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

					// rotの補正
					RotRepair(nPlayer);
				}
			}
			else if (GetKeyboardPress(DIK_D) == true)
			{//Dキーが押される
				CheckMotionMove(nPlayer, pPlayer);

				// プレイヤーの状態を移動状態に
				pPlayer->state = PLAYERSTATE_MOVE;

				if (GetKeyboardPress(DIK_W) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_UP))
				{// WとD(右上)の入力
					pPlayer->move.x += sinf(Camerarot.y + D3DX_PI * 0.25f) * pPlayer->fMove;
					pPlayer->move.z += cosf(Camerarot.y + D3DX_PI * 0.25f) * pPlayer->fMove;

					pPlayer->rotDest.y = -D3DX_PI * 0.75f + Camerarot.y;	// 目標の角度を設定
					pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

					// rotの補正
					RotRepair(nPlayer);
				}
				else if (GetKeyboardPress(DIK_S) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_DOWN))
				{// SとD(右下)の入力
					pPlayer->move.x += sinf(Camerarot.y + D3DX_PI * 0.75f) * pPlayer->fMove;
					pPlayer->move.z += cosf(-Camerarot.y - D3DX_PI * 0.75f) * pPlayer->fMove;

					pPlayer->rotDest.y = -D3DX_PI * 0.25f + Camerarot.y;	// 目標の角度を設定
					pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

					// rotの補正
					RotRepair(nPlayer);
				}
				else
				{// Dだけの入力
					pPlayer->move.x += sinf(Camerarot.y + D3DX_PI * 0.5f) * pPlayer->fMove;
					pPlayer->move.z += cosf(Camerarot.y + D3DX_PI * 0.5f) * pPlayer->fMove;

					pPlayer->rotDest.y = -D3DX_PI * 0.5f + Camerarot.y;	// 目標の角度を設定
					pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

					// rotの補正
					RotRepair(nPlayer);
				}
			}
			else if (GetKeyboardPress(DIK_W) == true)
			{//Wキーが押される
				CheckMotionMove(nPlayer, pPlayer);

				// プレイヤーの状態を移動状態に
				pPlayer->state = PLAYERSTATE_MOVE;

				pPlayer->move.z += cosf(Camerarot.y) * pPlayer->fMove;
				pPlayer->move.x += sinf(Camerarot.y) * pPlayer->fMove;

				pPlayer->rotDest.y = D3DX_PI + Camerarot.y;	// 目標の角度を設定
				pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

				// rotの補正
				RotRepair(nPlayer);
			}
			else if (GetKeyboardPress(DIK_S) == true)
			{//Sキーが押される
				CheckMotionMove(nPlayer, pPlayer);

				// プレイヤーの状態を移動状態に
				pPlayer->state = PLAYERSTATE_MOVE;

				pPlayer->move.z += cosf(Camerarot.y - D3DX_PI) * pPlayer->fMove;
				pPlayer->move.x += sinf(Camerarot.y - D3DX_PI) * pPlayer->fMove;

				pPlayer->rotDest.y = Camerarot.y;	// 目標の角度を設定
				pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

				// rotの補正
				RotRepair(nPlayer);
			}
		}
		else
		{// ネズミの操作
			if (g_nNumPlayer == 2)
			{// 2人プレイ時
				if (GetJoypadLeftStick(nPlayer, &stick))
				{// スティックの入力がある
					CheckMotionMove(nPlayer, pPlayer);

					// プレイヤーの状態を移動状態に
					pPlayer->state = PLAYERSTATE_MOVE;

					pPlayer->move.x += (sinf(-Camerarot.y) * stick.y + cosf(-Camerarot.y) * stick.x) * PLAYER_MOVE;
					pPlayer->move.z += (cosf(Camerarot.y) * -stick.y + sinf(Camerarot.y) * -stick.x) * PLAYER_MOVE;

					pPlayer->rotDest.y = atan2f(-pPlayer->move.x, -pPlayer->move.z);	// 目標の角度を設定
					pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

					// rotの補正
					RotRepair(nPlayer);
				}
				else if (GetKeyboardPress(DIK_LEFT) == true)
				{// 左矢印が押される	
					CheckMotionMove(nPlayer, pPlayer);

					if (GetKeyboardPress(DIK_UP) == true || GetJoypadPress(nPlayer, JOYKEY_UP) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_UP))
					{// 左上の入力
						pPlayer->move.x += sinf(Camerarot.y - D3DX_PI * 0.25f) * PLAYER_MOVE;
						pPlayer->move.z += cosf(-Camerarot.y + D3DX_PI * 0.25f) * PLAYER_MOVE;

						pPlayer->rotDest.y = D3DX_PI * 0.75f + Camerarot.y;	// 目標の角度を設定
						pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

						// rotの補正
						RotRepair(nPlayer);
					}
					else if (GetKeyboardPress(DIK_DOWN) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_DOWN))
					{// 左下の入力
						pPlayer->move.x += sinf(Camerarot.y - D3DX_PI * 0.75f) * PLAYER_MOVE;
						pPlayer->move.z += cosf(Camerarot.y - D3DX_PI * 0.75f) * PLAYER_MOVE;

						pPlayer->rotDest.y = D3DX_PI * 0.25f + Camerarot.y;	// 目標の角度を設定
						pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

						// rotの補正
						RotRepair(nPlayer);
					}
					else
					{// 左矢印単体の入力
						pPlayer->move.x += sinf(Camerarot.y - D3DX_PI * 0.5f) * PLAYER_MOVE;
						pPlayer->move.z += cosf(Camerarot.y - D3DX_PI * 0.5f) * PLAYER_MOVE;

						pPlayer->rotDest.y = D3DX_PI * 0.5f + Camerarot.y;	// 目標の角度を設定
						pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

						// rotの補正
						RotRepair(nPlayer);
					}
				}
				else if (GetKeyboardPress(DIK_RIGHT) == true)
				{// 右矢印が押される
					CheckMotionMove(nPlayer, pPlayer);

					if (GetKeyboardPress(DIK_UP) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_UP))
					{// 右上の入力
						pPlayer->move.x += sinf(Camerarot.y + D3DX_PI * 0.25f) * PLAYER_MOVE;
						pPlayer->move.z += cosf(Camerarot.y + D3DX_PI * 0.25f) * PLAYER_MOVE;

						pPlayer->rotDest.y = -D3DX_PI * 0.75f + Camerarot.y;	// 目標の角度を設定
						pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

						// rotの補正
						RotRepair(nPlayer);
					}
					else if (GetKeyboardPress(DIK_DOWN) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_DOWN))
					{// 右下の入力
						pPlayer->move.x += sinf(Camerarot.y + D3DX_PI * 0.75f) * PLAYER_MOVE;
						pPlayer->move.z += cosf(-Camerarot.y - D3DX_PI * 0.75f) * PLAYER_MOVE;

						pPlayer->rotDest.y = -D3DX_PI * 0.25f + Camerarot.y;	// 目標の角度を設定
						pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

						// rotの補正
						RotRepair(nPlayer);
					}
					else
					{// 右矢印だけの入力
						pPlayer->move.x += sinf(Camerarot.y + D3DX_PI * 0.5f) * PLAYER_MOVE;
						pPlayer->move.z += cosf(Camerarot.y + D3DX_PI * 0.5f) * PLAYER_MOVE;

						pPlayer->rotDest.y = -D3DX_PI * 0.5f + Camerarot.y;	// 目標の角度を設定
						pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

						// rotの補正
						RotRepair(nPlayer);
					}
				}
				else if (GetKeyboardPress(DIK_UP) == true)
				{//上矢印が押される
					CheckMotionMove(nPlayer, pPlayer);

					pPlayer->move.z += cosf(Camerarot.y) * PLAYER_MOVE;
					pPlayer->move.x += sinf(Camerarot.y) * PLAYER_MOVE;

					pPlayer->rotDest.y = D3DX_PI + Camerarot.y;	// 目標の角度を設定
					pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

					// rotの補正
					RotRepair(nPlayer);
				}
				else if (GetKeyboardPress(DIK_DOWN) == true)
				{//下矢印が押される
					CheckMotionMove(nPlayer, pPlayer);

					pPlayer->move.z += cosf(Camerarot.y - D3DX_PI) * PLAYER_MOVE;
					pPlayer->move.x += sinf(Camerarot.y - D3DX_PI) * PLAYER_MOVE;

					pPlayer->rotDest.y = Camerarot.y;	// 目標の角度を設定
					pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

					// rotの補正
					RotRepair(nPlayer);
				}
			}
			else
			{// 1人プレイ時
				if (GetJoypadLeftStick(0, &stick))
				{// スティックの入力がある
					CheckMotionMove(nPlayer, pPlayer);

					// プレイヤーの状態を移動状態に
					pPlayer->state = PLAYERSTATE_MOVE;

					pPlayer->move.x += (sinf(-Camerarot.y) * stick.y + cosf(-Camerarot.y) * stick.x) * PLAYER_MOVE;
					pPlayer->move.z += (cosf(Camerarot.y) * -stick.y + sinf(Camerarot.y) * -stick.x) * PLAYER_MOVE;

					pPlayer->rotDest.y = atan2f(-pPlayer->move.x, -pPlayer->move.z);	// 目標の角度を設定
					pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

					// rotの補正
					RotRepair(nPlayer);
				}
				else if (GetKeyboardPress(DIK_A) == true)
				{// Aキーが押される	
					CheckMotionMove(nPlayer, pPlayer);

					if (GetKeyboardPress(DIK_W) == true || GetJoypadPress(nPlayer, JOYKEY_UP) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_UP))
					{// Wキーとの入力
						pPlayer->move.x += sinf(Camerarot.y - D3DX_PI * 0.25f) * PLAYER_MOVE;
						pPlayer->move.z += cosf(-Camerarot.y + D3DX_PI * 0.25f) * PLAYER_MOVE;

						pPlayer->rotDest.y = D3DX_PI * 0.75f + Camerarot.y;	// 目標の角度を設定
						pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

						// rotの補正
						RotRepair(nPlayer);
					}
					else if (GetKeyboardPress(DIK_S) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_DOWN))
					{// Sキーとの入力
						pPlayer->move.x += sinf(Camerarot.y - D3DX_PI * 0.75f) * PLAYER_MOVE;
						pPlayer->move.z += cosf(Camerarot.y - D3DX_PI * 0.75f) * PLAYER_MOVE;

						pPlayer->rotDest.y = D3DX_PI * 0.25f + Camerarot.y;	// 目標の角度を設定
						pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

						// rotの補正
						RotRepair(nPlayer);
					}
					else
					{// Aキー単体の入力
						pPlayer->move.x += sinf(Camerarot.y - D3DX_PI * 0.5f) * PLAYER_MOVE;
						pPlayer->move.z += cosf(Camerarot.y - D3DX_PI * 0.5f) * PLAYER_MOVE;

						pPlayer->rotDest.y = D3DX_PI * 0.5f + Camerarot.y;	// 目標の角度を設定
						pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

						// rotの補正
						RotRepair(nPlayer);
					}
				}
				else if (GetKeyboardPress(DIK_D) == true)
				{// 右矢印が押される
					CheckMotionMove(nPlayer, pPlayer);

					if (GetKeyboardPress(DIK_W) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_UP))
					{// Wキーとの入力
						pPlayer->move.x += sinf(Camerarot.y + D3DX_PI * 0.25f) * PLAYER_MOVE;
						pPlayer->move.z += cosf(Camerarot.y + D3DX_PI * 0.25f) * PLAYER_MOVE;

						pPlayer->rotDest.y = -D3DX_PI * 0.75f + Camerarot.y;	// 目標の角度を設定
						pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

						// rotの補正
						RotRepair(nPlayer);
					}
					else if (GetKeyboardPress(DIK_S) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_DOWN))
					{// Sキーとの入力
						pPlayer->move.x += sinf(Camerarot.y + D3DX_PI * 0.75f) * PLAYER_MOVE;
						pPlayer->move.z += cosf(-Camerarot.y - D3DX_PI * 0.75f) * PLAYER_MOVE;

						pPlayer->rotDest.y = -D3DX_PI * 0.25f + Camerarot.y;	// 目標の角度を設定
						pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

						// rotの補正
						RotRepair(nPlayer);
					}
					else
					{// Dキーだけの入力
						pPlayer->move.x += sinf(Camerarot.y + D3DX_PI * 0.5f) * PLAYER_MOVE;
						pPlayer->move.z += cosf(Camerarot.y + D3DX_PI * 0.5f) * PLAYER_MOVE;

						pPlayer->rotDest.y = -D3DX_PI * 0.5f + Camerarot.y;	// 目標の角度を設定
						pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

						// rotの補正
						RotRepair(nPlayer);
					}
				}
				else if (GetKeyboardPress(DIK_W) == true)
				{// Wキーが押される
					CheckMotionMove(nPlayer, pPlayer);

					pPlayer->move.z += cosf(Camerarot.y) * PLAYER_MOVE;
					pPlayer->move.x += sinf(Camerarot.y) * PLAYER_MOVE;

					pPlayer->rotDest.y = D3DX_PI + Camerarot.y;	// 目標の角度を設定
					pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

					// rotの補正
					RotRepair(nPlayer);
				}
				else if (GetKeyboardPress(DIK_S) == true)
				{// Sキーが押される
					CheckMotionMove(nPlayer, pPlayer);

					pPlayer->move.z += cosf(Camerarot.y - D3DX_PI) * PLAYER_MOVE;
					pPlayer->move.x += sinf(Camerarot.y - D3DX_PI) * PLAYER_MOVE;

					pPlayer->rotDest.y = Camerarot.y;	// 目標の角度を設定
					pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

					// rotの補正
					RotRepair(nPlayer);
				}
			}
		}
	}
}

// =================================================
// プレイヤーのジャンプ関数
// =================================================
void JumpPlayer(PlayerType nPlayer)
{
	// プレイヤー構造体をポインタ化
	Player* pPlayer = &g_aPlayer[nPlayer];

	if (ITEMPROMPT_OFF)
	{
		if (nPlayer == 0)
		{// 少女
			// ジャンプする
			if ((GetKeyboardTrigger(DIK_SPACE) == true || GetJoypadTrigger(0, JOYKEY_A)) && pPlayer->bJump == false)
			{
				PlaySound(SOUND_LABEL_SE_G_JUMP);
				pPlayer->state = PLAYERSTATE_JUMP;
				SetMotionType(MOTIONTYPE_JUMP, true, 20, nPlayer);

				pPlayer->move.y = JUMP_FORCE;
				pPlayer->bJump = true;
			}
		}
		else
		{// ネズミ
			// ジャンプする
			if (g_nNumPlayer == 2)
			{// 2人プレイ時
				if ((GetKeyboardTrigger(DIK_RSHIFT) == true || GetJoypadTrigger(nPlayer, JOYKEY_A)) && pPlayer->bJump == false)
				{
					PlaySound(SOUND_LABEL_SE_G_MOUSEJUMP);
					pPlayer->state = PLAYERSTATE_JUMP;
					SetMotionType(MOTIONTYPE_JUMP, true, 10, nPlayer);

					pPlayer->move.y = JUMP_FORCE;
					pPlayer->bJump = true;
				}
			}
			else
			{// 1人プレイ時
				if ((GetKeyboardTrigger(DIK_SPACE) == true || GetJoypadTrigger(0, JOYKEY_A)) && pPlayer->bJump == false)
				{
					PlaySound(SOUND_LABEL_SE_G_MOUSEJUMP);
					pPlayer->state = PLAYERSTATE_JUMP;
					SetMotionType(MOTIONTYPE_JUMP, true, 10, nPlayer);

					pPlayer->move.y = JUMP_FORCE;
					pPlayer->bJump = true;
				}
			}
		}
	}

}

//================================================================================================================
// --- モーションの変更！ ---
//================================================================================================================
void SetMotionType(MOTIONTYPE motionTypeNext, bool bBlend, int nFrameBlend, PlayerType PlayerType)
{
	Player* pPlayer = &g_aPlayer[PlayerType];	// プレイヤー情報へのアドレス	

	if (motionTypeNext < 0 || motionTypeNext >= MOTIONTYPE_MAX)
	{ // モーションインデックスの上下限確認
		return;
	}

	// 着地音再生
	if (motionTypeNext == MOTIONTYPE_LANDING)
	{
		if (PlayerType == PLAYERTYPE_GIRL)
		{
			PlaySound(SOUND_LABEL_SE_G_LANDING);
		}
		else
		{
			PlaySound(SOUND_LABEL_SE_G_MOUSELANDING);
		}
	}

	// ブレンドモーションをするかどうか
	pPlayer->bBlendMotion = bBlend;

	if (bBlend == false)
	{
		/*** 各変数を初期化し、次のモーションを設定 ***/
		pPlayer->nCounterMotion = pPlayer->nCounterMotionBlend;
		pPlayer->nKey = pPlayer->nKeyBlend;
		pPlayer->motionType = motionTypeNext;
		pPlayer->bLoop = pPlayer->aMotionInfo[motionTypeNext].bLoop;
		pPlayer->nNumKey = pPlayer->aMotionInfo[motionTypeNext].nNumKey;
		pPlayer->bFinishMotion = false;
		pPlayer->nCntAllround = 0;

		/*** 現在のモーションのキー情報へのポインタ ***/
		KEY_INFO* pInfo = &pPlayer->aMotionInfo[pPlayer->motionType].aKeyInfo[pPlayer->nKey];

		/*** 全パーツの初期設定！ ***/
		for (int nCntModel = 0; nCntModel < pPlayer->PartsInfo.nNumParts; nCntModel++)
		{
			int nNext = (pPlayer->nKey + 1) % pPlayer->aMotionInfo[pPlayer->motionType].nNumKey;

			// 次のキーの値			
			float fRateKey = (float)pPlayer->nCounterMotion / (float)pInfo->nFrame;			// モーションカウンター / 再生フレーム数	

			D3DXVECTOR3 diffPos = {};	// 位置の差分
			D3DXVECTOR3 UpdatePos = {};	// 更新する位置
			D3DXVECTOR3 diffRot = {};	// 角度の差分
			D3DXVECTOR3 UpdateRot = {};	// 更新する角度
			KEY* pKey = &pInfo->aKey[nCntModel];	// 現在のキー
			PARTS* pModel = &pPlayer->PartsInfo.aParts[nCntModel];
			KEY* pKeyNext = &pPlayer->aMotionInfo[pPlayer->motionType].aKeyInfo[nNext].aKey[nCntModel];		// 次のキー			

			/** キー情報から位置と向きを算出！ **/
			// === POS ===			
			// --- X ---		
			diffPos.x = pKeyNext->pos.x - pKey->pos.x;
			UpdatePos.x = pKey->pos.x + diffPos.x * fRateKey;

			// --- Y ---		
			diffPos.y = pKeyNext->pos.y - pKey->pos.y;
			UpdatePos.y = pKey->pos.y + diffPos.y * fRateKey;

			// --- Z ---			
			diffPos.z = pKeyNext->pos.z - pKey->pos.z;
			UpdatePos.z = pKey->pos.z + diffPos.z * fRateKey;

			// === ROT ===			
			// --- X ---			
			diffRot.x = pKeyNext->rot.x - pKey->rot.x;
			RepairRot(&diffRot.x, &diffRot.x);

			UpdateRot.x = pKey->rot.x + diffRot.x * fRateKey;
			RepairRot(&UpdateRot.x, &UpdateRot.x);

			// --- Y ---			
			diffRot.y = pKeyNext->rot.y - pKey->rot.y;
			RepairRot(&diffRot.y, &diffRot.y);

			UpdateRot.y = pKey->rot.y + diffRot.y * fRateKey;
			RepairRot(&UpdateRot.y, &UpdateRot.y);

			// --- Z ---			
			diffRot.z = pKeyNext->rot.z - pKey->rot.z;
			RepairRot(&diffRot.z, &diffRot.z);

			UpdateRot.z = pKey->rot.z + diffRot.z * fRateKey;
			RepairRot(&UpdateRot.z, &UpdateRot.z);

			/** 位置、向きを更新！ **/
			pModel->pos = pModel->posLocal + UpdatePos;
			pModel->rot = pModel->rotLocal + UpdateRot;
		}
	}
	else
	{
		/*** 各変数を初期化し、ブレンドモーションを設定 ***/
		pPlayer->bFinishMotion = false;
		pPlayer->motionTypeBlend = motionTypeNext;
		pPlayer->nCounterBlend = 0;
		pPlayer->nCounterMotionBlend = 0;
		pPlayer->nFrameBlend = nFrameBlend;
		pPlayer->nNumKeyBlend = pPlayer->aMotionInfo[motionTypeNext].nNumKey;
		pPlayer->nKeyBlend = 0;
		pPlayer->nCntAllround = 0;
	}
}

//================================================================================================================
// --- モーションアップデート！ ---
//================================================================================================================
void UpdateMotion(PlayerType Type)
{
	Player* pPlayer = &g_aPlayer[Type];	// プレイヤー情報へのアドレス	

	/*** 現在のモーションのキー情報へのポインタ ***/
	KEY_INFO* pInfo = &pPlayer->aMotionInfo[pPlayer->motionType].aKeyInfo[pPlayer->nKey];
	KEY_INFO* pInfoBlend = &pPlayer->aMotionInfo[pPlayer->motionTypeBlend].aKeyInfo[pPlayer->nKeyBlend];

	/*** 全パーツの更新！ ***/
	for (int nCntModel = 0; nCntModel < pPlayer->PartsInfo.nNumParts; nCntModel++)
	{
		int nNext = (pPlayer->nKey + 1) % pPlayer->aMotionInfo[pPlayer->motionType].nNumKey;

		// 次のキーの値		
		float fRateKey = (float)pPlayer->nCounterMotion / (float)pInfo->nFrame;		// モーションカウンター / 再生フレーム数
		D3DXVECTOR3 diffPos = {};	// 位置の差分
		D3DXVECTOR3 UpdatePos = {};	// 更新する位置	
		D3DXVECTOR3 diffRot = {};	// 角度の差分		
		D3DXVECTOR3 UpdateRot = {};	// 更新する角度		
		KEY* pKey = &pInfo->aKey[nCntModel];	// 現在のキー		
		KEY* pKeyNext = &pPlayer->aMotionInfo[pPlayer->motionType].aKeyInfo[nNext].aKey[nCntModel];		// 次のキー		
		PARTS* pModel = &pPlayer->PartsInfo.aParts[nCntModel];

		if (pPlayer->bBlendMotion == true)
		{ // モーションブレンドあり	
			D3DXVECTOR3 diffKeyPosCurrent = {};	// 現在のモーションの位置の差分			
			D3DXVECTOR3 diffKeyRotCurrent = {};	// 現在のモーションの角度の差分		
			D3DXVECTOR3 diffKeyPosBlend = {};	// ブレンドモーションの位置の差分			
			D3DXVECTOR3 diffKeyRotBlend = {};	// ブレンドモーションの角度の差分	
			D3DXVECTOR3 diffPosBlend = {};		// 最終差分		
			D3DXVECTOR3 diffRotBlend = {};		// 最終差分			
			int nNextBlend = (pPlayer->nKeyBlend + 1) % pPlayer->aMotionInfo[pPlayer->motionTypeBlend].nNumKey;			// 次のキーの値		
			KEY* pKeyBlend = &pInfoBlend->aKey[nCntModel];																// 現在のキー	
			KEY* pKeyNextBlend = &pPlayer->aMotionInfo[pPlayer->motionTypeBlend].aKeyInfo[nNextBlend].aKey[nCntModel];	// 次のキー
			float fRateKeyBlend = (float)pPlayer->nCounterMotionBlend / (float)pInfoBlend->nFrame;						// モーションカウンター / 再生フレーム数
			float fRateBlend = (float)pPlayer->nCounterBlend / (float)pPlayer->nFrameBlend;							// ブレンドの相対量		

			// --- X ---			
			/* 現在のモーション */
			diffPos.x = pKeyNext->pos.x - pKey->pos.x;
			diffKeyPosCurrent.x = pKey->pos.x + diffPos.x * fRateKey;

			/* ブレンドモーション */
			diffPos.x = pKeyNextBlend->pos.x - pKeyBlend->pos.x;
			diffKeyPosBlend.x = pKeyBlend->pos.x + diffPos.x * fRateKeyBlend;

			/* 求める差分 */
			diffPosBlend.x = diffKeyPosBlend.x - diffKeyPosCurrent.x;
			UpdatePos.x = diffKeyPosCurrent.x + (diffPosBlend.x * fRateBlend);

			// --- Y ---		
			/* 現在のモーション */
			diffPos.y = pKeyNext->pos.y - pKey->pos.y;
			diffKeyPosCurrent.y = pKey->pos.y + diffPos.y * fRateKey;

			/* ブレンドモーション */
			diffPos.y = pKeyNextBlend->pos.y - pKeyBlend->pos.y;
			diffKeyPosBlend.y = pKeyBlend->pos.y + diffPos.y * fRateKeyBlend;

			/* 求める差分 */
			diffPosBlend.y = diffKeyPosBlend.y - diffKeyPosCurrent.y;
			UpdatePos.y = diffKeyPosCurrent.y + (diffPosBlend.y * fRateBlend);

			// --- Z ---			
			/* 現在のモーション */
			diffPos.z = pKeyNext->pos.z - pKey->pos.z;
			diffKeyPosCurrent.z = pKey->pos.z + diffPos.z * fRateKey;

			/* ブレンドモーション */
			diffPos.z = pKeyNextBlend->pos.z - pKeyBlend->pos.z;
			diffKeyPosBlend.z = pKeyBlend->pos.z + diffPos.z * fRateKeyBlend;

			/* 求める差分 */
			diffPosBlend.z = diffKeyPosBlend.z - diffKeyPosCurrent.z;
			UpdatePos.z = diffKeyPosCurrent.z + (diffPosBlend.z * fRateBlend);

			// === ROT ===			
			// --- X ---			
			/* 現在のモーション */
			diffRot.x = pKeyNext->rot.x - pKey->rot.x;
			RepairRot(&diffRot.x, &diffRot.x);
			diffKeyRotCurrent.x = pKey->rot.x + diffRot.x * fRateKey;
			RepairRot(&diffKeyRotCurrent.x, &diffKeyRotCurrent.x);

			/* ブレンドモーション */
			diffRot.x = pKeyNextBlend->rot.x - pKeyBlend->rot.x;
			RepairRot(&diffRot.x, &diffRot.x);
			diffKeyRotBlend.x = pKeyBlend->rot.x + diffRot.x * fRateKeyBlend;
			RepairRot(&diffKeyRotBlend.x, &diffKeyRotBlend.x);

			/* 求める差分 */
			diffRotBlend.x = diffKeyRotBlend.x - diffKeyRotCurrent.x;
			UpdateRot.x = diffKeyRotCurrent.x + (diffRotBlend.x * fRateBlend);

			// --- Y ---			
			diffRot.y = pKeyNext->rot.y - pKey->rot.y;
			RepairRot(&diffRot.y, &diffRot.y);
			diffKeyRotCurrent.y = pKey->rot.y + diffRot.y * fRateKey;
			RepairRot(&diffKeyRotCurrent.y, &diffKeyRotCurrent.y);

			/* ブレンドモーション */
			diffRot.y = pKeyNextBlend->rot.y - pKeyBlend->rot.y;
			RepairRot(&diffRot.y, &diffRot.y);
			diffKeyRotBlend.y = pKeyBlend->rot.y + diffRot.y * fRateKeyBlend;
			RepairRot(&diffKeyRotBlend.y, &diffKeyRotBlend.y);

			/* 求める差分 */
			diffRotBlend.y = diffKeyRotBlend.y - diffKeyRotCurrent.y;
			UpdateRot.y = diffKeyRotCurrent.y + (diffRotBlend.y * fRateBlend);

			// --- Z ---			
			diffRot.z = pKeyNext->rot.z - pKey->rot.z;
			RepairRot(&diffRot.z, &diffRot.z);
			diffKeyRotCurrent.z = pKey->rot.z + diffRot.z * fRateKey;
			RepairRot(&diffKeyRotCurrent.z, &diffKeyRotCurrent.z);

			/* ブレンドモーション */
			diffRot.z = pKeyNextBlend->rot.z - pKeyBlend->rot.z;
			RepairRot(&diffRot.z, &diffRot.z);
			diffKeyRotBlend.z = pKeyBlend->rot.z + diffRot.z * fRateKeyBlend;
			RepairRot(&diffKeyRotBlend.z, &diffKeyRotBlend.z);

			/* 求める差分 */
			diffRotBlend.z = diffKeyRotBlend.z - diffKeyRotCurrent.z;
			UpdateRot.z = diffKeyRotCurrent.z + (diffRotBlend.z * fRateBlend);
		}
		else
		{ // ブレンド無し	
		  /** キー情報から位置と向きを算出！ **/
		  // === POS ===			
		  // --- X ---			
			diffPos.x = pKeyNext->pos.x - pKey->pos.x;
			UpdatePos.x = pKey->pos.x + diffPos.x * fRateKey;

			// --- Y ---		
			diffPos.y = pKeyNext->pos.y - pKey->pos.y;
			UpdatePos.y = pKey->pos.y + diffPos.y * fRateKey;

			// --- Z ---			
			diffPos.z = pKeyNext->pos.z - pKey->pos.z;
			UpdatePos.z = pKey->pos.z + diffPos.z * fRateKey;

			// === ROT ===			
			// --- X ---		
			diffRot.x = pKeyNext->rot.x - pKey->rot.x;
			RepairRot(&diffRot.x, &diffRot.x);
			UpdateRot.x = pKey->rot.x + diffRot.x * fRateKey;
			RepairRot(&UpdateRot.x, &UpdateRot.x);

			// --- Y ---		
			diffRot.y = pKeyNext->rot.y - pKey->rot.y;
			RepairRot(&diffRot.y, &diffRot.y);
			UpdateRot.y = pKey->rot.y + diffRot.y * fRateKey;
			RepairRot(&UpdateRot.y, &UpdateRot.y);

			// --- Z ---			
			diffRot.z = pKeyNext->rot.z - pKey->rot.z;
			RepairRot(&diffRot.z, &diffRot.z);
			UpdateRot.z = pKey->rot.z + diffRot.z * fRateKey;
			RepairRot(&UpdateRot.z, &UpdateRot.z);
		}

		/** 位置、向きを更新！ **/
		pModel->pos = pModel->posLocal + UpdatePos;
		pModel->rot = pModel->rotLocal + UpdateRot;
	}

	if (pPlayer->bBlendMotion == false)
	{// ブレンドなし
		pPlayer->nCounterMotion++;

		if (pPlayer->nCounterMotion >= pInfo->nFrame)
		{ // モーションカウンターが現在のキー情報のフレーム数を超えた場合
			if (pPlayer->motionType < 0 || pPlayer->motionType >= MOTIONTYPE_MAX)
			{ // モーションインデックスの上下限確認
				pPlayer->bFinishMotion = true;
				SetMotionType(MOTIONTYPE_NEUTRAL, true, 40, Type);
				pPlayer->state = PLAYERSTATE_NEUTRAL;
				return;
			}

			/** キーを一つ進める **/
			pPlayer->nKey = ((pPlayer->nKey + 1) % pPlayer->aMotionInfo[pPlayer->motionType].nNumKey);
			//PrintDebugProc("key %d\nFrame %d\nCounter %d\n", pPlayer->nKey, pInfo->nFrame, pPlayer->nCounterMotion);

			if (pPlayer->nKey == pPlayer->aMotionInfo[pPlayer->motionType].nNumKey - 1 && pPlayer->bLoop == false)
			{
				pPlayer->bFinishMotion = true;
				SetMotionType(MOTIONTYPE_NEUTRAL, true, 20, Type);
				pPlayer->state = PLAYERSTATE_NEUTRAL;
			}

			pPlayer->nCounterMotion = 0;
		}
	}
	else
	{// ブレンドあり
		pPlayer->nCounterMotionBlend++;

		if (pPlayer->nCounterMotionBlend >= pInfoBlend->nFrame)
		{ // モーションカウンターがブレンドモーションの現在のキーのフレーム数を超えた場合	
			/** ブレンドモーションのキーを一つ進める **/
			pPlayer->nKeyBlend = ((pPlayer->nKeyBlend + 1) % pPlayer->aMotionInfo[pPlayer->motionTypeBlend].nNumKey);
			pPlayer->nCounterMotionBlend = 0;
		}

		pPlayer->nCounterBlend++;

		if (pPlayer->nCounterBlend >= pPlayer->nFrameBlend)
		{ // ブレンドカウンターがブレンドフレーム数を超えた場合	
			pPlayer->bFinishMotion = true;
			SetMotionType(pPlayer->motionTypeBlend, false, 20, Type);
		}
	}
}

// =================================================
// プレイ人数情報を渡す
// =================================================
int GetNumPlayer(void)
{
	return g_nNumPlayer;
}

// =================================================
// 操作しているプレイヤー情報を渡す
// =================================================
int GetActivePlayer(void)
{
	return g_ActivePlayer;
}

// =================================================
// プレイヤーの位置情報を渡す
// =================================================
void SetPlayer(D3DXVECTOR3* pPosGirl, D3DXVECTOR3* pPosMouse)
{
	*pPosGirl = g_aPlayer[PLAYERTYPE_GIRL].pos;
	*pPosMouse = g_aPlayer[PLAYERTYPE_MOUSE].pos;
}

// =================================================
// プレイ人数を切り替える(デバッグ用)
// =================================================
void ChangeNumPlayer(void)
{
	// 1を押して、シングル・マルチプレイを切り替える
	if (GetKeyboardTrigger(DIK_1) == true)
	{
		if (g_nNumPlayer == 1)
		{// 1人→2人
			g_nNumPlayer = 2;
		}
		else
		{// 2人→1人
			g_nNumPlayer = 1;
		}
	}
}

// =================================================
// rotにおける逆回りを防ぐ補正
// =================================================
void RotRepair(PlayerType nPlayer)
{
	// プレイヤー構造体をポインタ化
	Player* pPlayer = &g_aPlayer[nPlayer];

	// カメラの向きを保存する
	vec3 Camerarot;

	// カメラの情報を取得
	GetCameraRot(nPlayer, &Camerarot);

	// もし、差分がπを超えたら
	if (pPlayer->rotDiff.y > D3DX_PI)
	{
		pPlayer->rotDiff.y -= D3DX_PI * 2;
	}
	else if (pPlayer->rotDiff.y < -D3DX_PI)
	{
		pPlayer->rotDiff.y += D3DX_PI * 2;
	}

	// 回転に補正を掛ける
	pPlayer->rot.y += pPlayer->rotDiff.y * PLAYER_INI;

	// もし、現在の角度がπを超えたら
	if (pPlayer->rot.y > D3DX_PI)
	{
		pPlayer->rot.y -= D3DX_PI * 2;
	}
	else if (pPlayer->rot.y < -D3DX_PI)
	{
		pPlayer->rot.y += D3DX_PI * 2;
	}
}

// =================================================
// モーションの確認処理
// =================================================
void CheckMotionMove(PlayerType nPlayer, Player* pPlayer)
{
	if (pPlayer->motionType != MOTIONTYPE_MOVE
		&& pPlayer->motionTypeBlend != MOTIONTYPE_MOVE
		&& pPlayer->motionTypeBlend != MOTIONTYPE_JUMP)
	{
		if (pPlayer->motionType == MOTIONTYPE_JUMP)
		{
			if (pPlayer->motionTypeBlend == MOTIONTYPE_LANDING)
			{
				SetMotionType(MOTIONTYPE_MOVE, true, 15, nPlayer);
				pPlayer->state = PLAYERSTATE_MOVE;
			}
		}
		else
		{
			SetMotionType(MOTIONTYPE_MOVE, true, 5, nPlayer);
			pPlayer->state = PLAYERSTATE_MOVE;
		}
	}

	g_aMovePlayer[nPlayer] = true;
}

// =================================================
// シングルプレイ時ネズミが少女についてくる処理
// =================================================
void MouseKeepUp(void)
{
	if (GetNumPlayer() == 1 && GetActivePlayer() == PLAYERTYPE_GIRL)
	{
		// 少女の位置情報
		Player* pGirl = GetPlayer();

		// ネズミの情報
		Player* pMouse = pGirl + 1;

		// ２人の距離
		float fPlayerDist = GetPTPLength3D(pGirl->pos, pMouse->pos);

		if (50.0f < fPlayerDist)
		{// 離れすぎていると切り替え不可能状態に
			pGirl->bChangeable = false;
			g_aPlayer[PLAYERTYPE_MOUSE].pos = pGirl->pos;
		}
		else if (50.0f >= fPlayerDist)
		{// 一定距離以内なら切り替え可能に
			pGirl->bChangeable = true;
		}

		if (5 < fPlayerDist)
		{
			// ネズミがいてほしい座標
			D3DXVECTOR3 mousePosDest = D3DXVECTOR3(pGirl->pos.x + (sinf(pGirl->rot.y) * 5), pGirl->pos.y, pGirl->pos.z + (cosf(pGirl->rot.y) * 5));

			// ネズミを移動
			MyMathUtil::HomingPosToPos(mousePosDest, &pMouse->pos, fPlayerDist * 0.08f);

			// 移動している方向に向く
			pMouse->rot.y = atan2f(pMouse->posOld.x - pMouse->pos.x, pMouse->posOld.z - pMouse->pos.z);

			CheckMotionMove(PLAYERTYPE_MOUSE, &g_aPlayer[PLAYERTYPE_MOUSE]);
		}

	}
}

// =================================================
// アームの更新
// =================================================
void UpdateArm(void)
{
	if (g_aPlayer[PLAYERTYPE_GIRL].state != PLAYERSTATE_THROWWAITING && GetActivePlayer() == PLAYERTYPE_GIRL)
	{
		if (GetKeyboardTrigger(DIK_0) == true || GetJoypadTrigger(0, JOYKEY_RB) == true)
		{
			PlaySound(SOUND_LABEL_SE_G_ARMSWITCH);
			int nArm = g_armPlayer;
			nArm++;
			if (FAILED(CheckIndex(ARMTYPE_MAX, nArm)))
			{
				nArm = 0;
			}

			g_armPlayer = (ArmType)nArm;
		}
	}
}

// =================================================
// アームの切り替え
// =================================================
void ChangeArmType(ArmType Type)
{
	g_armPlayer = Type;
}

// =================================================
// ネズミを発射 未完成
// =================================================
void ShotMouse(void)
{
	D3DXVECTOR3 offset = RHAND_OFFSET;

	if (g_bShotMouse)
	{
		// プレイヤー情報を取得
		Player* pMouse = GetPlayer() + 1;
		Player* pPlayer = GetPlayer();

		// カメラの情報を取得
		Camera* pCamera = GetCamera();

		// 放物線用のベクトルを取得
		D3DXVECTOR3 ShotParabolaVec = GetParabolaVec();

		if (pPlayer->state == PLAYERSTATE_THROWWAITING)
		{// プレイヤーの投げる向きをカメラとそろえる
			pPlayer->rot.y = pCamera->rot.y + D3DX_PI;
			pMouse->rot.y = pCamera->rot.y + D3DX_PI;

			// 手にくっつける
			D3DXVec3TransformCoord(&pMouse->pos, &offset, &pPlayer->PartsInfo.aParts[19].mtxWorld);
		}

		if (pPlayer->state != PLAYERSTATE_THROWWAITING)
		{
			g_nMotionCounter--;
		}

		if (g_nMotionCounter < 0)
		{
			if (g_nMotionCounter == -5)
			{
				// ネズミ投げ待機状態にする
				pPlayer->state = PLAYERSTATE_THROWWAITING;
				pCamera->rot.y = pPlayer->rot.y - D3DX_PI;

				// 角度補正
				if (pCamera->rot.y > D3DX_PI)
				{
					pCamera->rot.y -= D3DX_PI * 2.0f;
				}
				else if (pCamera->rot.y < -D3DX_PI)
				{
					pCamera->rot.y += D3DX_PI * 2.0f;
				}
			}

			if (GetKeyboardTrigger(DIK_RETURN) == true || GetJoypadTrigger(0, JOYKEY_B) == true)
			{// 投げる
				pPlayer->state = PLAYERSTATE_NEUTRAL;
				PlaySound(SOUND_LABEL_SE_G_THROW);

				// ネズミを操作対象に
				g_ActivePlayer = 1;
			}

			if (pPlayer->state == PLAYERSTATE_NEUTRAL)
			{
				// ネズミを飛ばす
				pMouse->move.x += ShotParabolaVec.x * 1.5f;
				pMouse->move.z += ShotParabolaVec.z * 1.5f;

				if (g_nMotionCounter == -5)
				{// Y軸移動は1Fのみ
					// 投げる手の位置に移動
					D3DXVec3TransformCoord(&pMouse->pos, &offset, &pPlayer->PartsInfo.aParts[19].mtxWorld);
					pMouse->move.y = ShotParabolaVec.y * 35.0f;
					// 下限
					if (pMouse->move.y < 5.5f)
					{
						pMouse->move.y = 5.5f;
					}
					// 上限
					else if (pMouse->move.y > 7.5f)
					{
						pMouse->move.y = 7.5f;
					}
				}
			}

			if (pMouse->pos.y < 110.0f && g_nMotionCounter < -10)
			{// 余韻を持たせて初期化
				g_bShotMouse = false;
				g_nMotionCounter = 0;
			}
		}
	}
}


void SetRotShop(D3DXVECTOR3 pos)
{
	Player* pPlayer = GetPlayer();

	// 角度を求める
	float fAngleplayer = atan2(pPlayer->pos.x - pos.x, pPlayer->pos.z - pos.z);

	pPlayer->rot.y = fAngleplayer;

	RotRepair(pPlayer->playertype);
}