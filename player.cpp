// =================================================
// 
// プレイヤー処理[mpdel.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include<stdio.h>
#include<d3dx9math.h>
#include "main.h"
#include "camera.h"
#include "debugproc.h"
#include "game.h"
#include "input.h"
#include "item.h"
#include "model.h"
#include "modeldata.h"
#include "motion.h"
#include "player.h"
#include "MathUtil.h"

using namespace MyMathUtil;

// =================================================
// マクロ定義
#define PLAYER_RANGE	(50.0f)		// プレイヤーとアイテムとの当たり判定の距離

// =================================================
// プロトタイプ宣言
void UpdateMotion(PlayerType Type);	// モーションのアップデート
void SetMotionType(MOTIONTYPE motionTypeNext, bool bBlend, int nFrameBlend, PlayerType PlayerType);	// モーションの変更

// =================================================
// グローバル変数
Player g_aPlayer[PLAYERTYPE_MAX];	// プレイヤーの樹応報
int g_IdxShadowPlayer = -1;			// 使用する影の番号
float g_sinrot = 0;					// sinカーブを用いると起用の変数
int g_nNumPlayer = 1;				// プレイヤーのアクティブ人数
int g_ActivePlayer = 0;				// 操作しているプレイヤータイプ
int g_Functionkey = 0;

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
		g_aPlayer[nCntPlayer].posOri = D3DXVECTOR3(0.0f, 100.0f, 0.0f);
		g_aPlayer[nCntPlayer].pos = PLAYER_POSDEF;
		g_aPlayer[nCntPlayer].posOld = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aPlayer[nCntPlayer].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aPlayer[nCntPlayer].bDisp = true;
		g_aPlayer[nCntPlayer].bJump = false;
		g_aPlayer[nCntPlayer].nCounterMotion = 0;
		g_aPlayer[nCntPlayer].nKey = 0;
		g_aPlayer[nCntPlayer].state = PLAYERSTATE_NEUTRAL;
		g_aPlayer[nCntPlayer].motionType = MOTIONTYPE_NEUTRAL;
		g_aPlayer[nCntPlayer].bFinishMotion = true;
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

	// 影を設定
	//g_IdxShadowPlayer = SetShadow();
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

	for (int nCntPlayer = 0; nCntPlayer < PLAYERTYPE_MAX; nCntPlayer++, pPlayer++)
	{
		// 現在位置の保存
		pPlayer->posOld = pPlayer->pos;

		MovePlayer((PlayerType)nCntPlayer);	// 移動に関する処理

		JumpPlayer((PlayerType)nCntPlayer);	// ジャンプに関する処理

		UpdateMotion((PlayerType)nCntPlayer);

		//if (pPlayer->bFinishMotion == true && pPlayer->motionType != MOTIONTYPE_NEUTRAL)
		//{
		//	SetMotionType(MOTIONTYPE_NEUTRAL, true, 10, (PlayerType)nCntPlayer);
		//}

		// 重力をかけ続ける
		pPlayer->move.y += GRAVITY;

		// 移動量の更新
		pPlayer->pos += pPlayer->move;

		// 慣性を掛ける
		pPlayer->move.x += (0.0f - pPlayer->move.x) * (PLAYER_INI * 1.5f);
		pPlayer->move.z += (0.0f - pPlayer->move.z) * (PLAYER_INI * 1.5f);

		// 地面に沈んだ時
		if (pPlayer->pos.y < 100.0f)
		{
			pPlayer->pos.y = 100.0f;

			if (pPlayer->bJump == true && pPlayer->state == PLAYERSTATE_JUMP)
			{// 着地モーション
				SetMotionType(MOTIONTYPE_LANDING, false, 10, (PlayerType)nCntPlayer);
			}

			pPlayer->bJump = false;
			pPlayer->state = PLAYERSTATE_NEUTRAL;
		}

		// カメラを有効化させる
		//CameraEnable(pPlayer->nIdxCamera);

		// モデルとの当たり判定
		CollisionModel(&pPlayer->pos, &pPlayer->posOld, &pPlayer->move);

		// アイテムとの当たり判定
		CollisionItem(pPlayer->pos, PLAYER_RANGE);
	}


	// 操作する対象を切り替える
	if (g_nNumPlayer == 1)
	{// シングルプレイ時
		if (GetJoypadPress(0, JOYKEY_LB) == true || GetKeyboardTrigger(DIK_Q) == true)
		{
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

	// === デバッグ処理 === // 
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
	if (g_Functionkey == 0)
	{
		PrintDebugProc("\nPlayer切り替え : [Q] : [%d]\n", g_ActivePlayer);
		PrintDebugProc("操作対象 : 0 → 少女\n                1 → ネズミ\n");

		PrintDebugProc("\nPlayer0 : [SPACE] :  JUMP\n");
		PrintDebugProc("Player1 : [RSHIFT] :  JUMP\n");

		PrintDebugProc("Player1 : nCounterMotion [%d]\n", g_aPlayer[1].nCounterMotion);
		PrintDebugProc("Player1 : nCounterMotionBlend [%d]\n", g_aPlayer[1].nCounterMotionBlend);
		PrintDebugProc("Player1 : [%d]\n", g_aPlayer[0].bLoop);
		PrintDebugProc("Player1 : BlendMotion [%d]\n", g_aPlayer[1].bBlendMotion);
		PrintDebugProc("Player1 : Motiontype [%d]\n", g_aPlayer[1].motionType);
		PrintDebugProc("Player1 : MotiontypeBlend [%d]\n", g_aPlayer[1].motionTypeBlend);
		PrintDebugProc("Player0 : PlayerState [%d]\n", g_aPlayer[0].state);
	}
}

// =================================================
// 描画処理
// =================================================
void DrawPlayer(void)
{
	// プレイヤー構造体をポインタ化
	Player* pPlayer = &g_aPlayer[0];

	LPDIRECT3DDEVICE9 pDevice;		// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス
	D3DMATERIAL9 matDef;			// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;				// マテリアルデータへのポインタ

	for (int nCntPlayer = 0; nCntPlayer < PLAYERTYPE_MAX; nCntPlayer++, pPlayer++)
	{
		// プレイヤーのワールドマトリックスの初期化
		D3DXMatrixIdentity(&pPlayer->mtxWorld);

		// プレイヤーの向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot,
			pPlayer->rot.y, pPlayer->rot.x, pPlayer->rot.z);
		D3DXMatrixMultiply(&pPlayer->mtxWorld, &pPlayer->mtxWorld, &mtxRot);	// かけ合わせる

		// プレイヤーの位置を反映
		D3DXMatrixTranslation(&mtxTrans,
			pPlayer->pos.x, pPlayer->pos.y, pPlayer->pos.z);
		D3DXMatrixMultiply(&pPlayer->mtxWorld, &pPlayer->mtxWorld, &mtxTrans);	// かけ合わせる

		// プレイヤーのワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &pPlayer->mtxWorld);

		// 現在のマテリアルを取得(保存)
		pDevice->GetMaterial(&matDef);

		// 全モデル(パーツ)の描画
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

		// 保存していたマテリアルを戻す
		pDevice->SetMaterial(&matDef);
	}
	// デバイスの破棄
	EndDevice();
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
void MovePlayer(PlayerType nPlayer)
{
	// プレイヤー構造体をポインタ化
	Player* pPlayer = &g_aPlayer[nPlayer];

	// カメラの向きを保存する
	vec3 Camerarot;

	// カメラの情報を取得
	GetCameraRot(nPlayer, &Camerarot);

	if (nPlayer == 0)
	{// 少女の操作
		if (GetKeyboardPress(DIK_A) == true || GetJoypadPress(nPlayer, JOYKEY_LEFT) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_LEFT))
		{//Aキーが押される	

			if (pPlayer->bFinishMotion == true && pPlayer->state == PLAYERSTATE_NEUTRAL)
			{
				SetMotionType(MOTIONTYPE_MOVE, false, 10, nPlayer);
				pPlayer->state = PLAYERSTATE_MOVE;
			}

			if (GetKeyboardPress(DIK_W) == true || GetJoypadPress(nPlayer, JOYKEY_UP) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_UP))
			{// WとA(左上)の入力
				pPlayer->move.x += sinf(Camerarot.y - D3DX_PI * 0.25f) * PLAYER_MOVE;
				pPlayer->move.z += cosf(-Camerarot.y + D3DX_PI * 0.25f) * PLAYER_MOVE;

				pPlayer->rotDest.y = D3DX_PI * 0.75f + Camerarot.y;	// 目標の角度を設定
				pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

				// rotの補正
				RotRepair(nPlayer);
			}
			else if (GetKeyboardPress(DIK_S) == true || GetJoypadPress(nPlayer, JOYKEY_DOWN) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_DOWN))
			{// SとA(左下)の入力
				pPlayer->move.x += sinf(Camerarot.y - D3DX_PI * 0.75f) * PLAYER_MOVE;
				pPlayer->move.z += cosf(Camerarot.y - D3DX_PI * 0.75f) * PLAYER_MOVE;

				pPlayer->rotDest.y = D3DX_PI * 0.25f + Camerarot.y;	// 目標の角度を設定
				pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

				// rotの補正
				RotRepair(nPlayer);
			}
			else
			{// A単体の入力
				pPlayer->move.x += sinf(Camerarot.y - D3DX_PI * 0.5f) * PLAYER_MOVE;
				pPlayer->move.z += cosf(Camerarot.y - D3DX_PI * 0.5f) * PLAYER_MOVE;

				pPlayer->rotDest.y = D3DX_PI * 0.5f + Camerarot.y;	// 目標の角度を設定
				pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

				// rotの補正
				RotRepair(nPlayer);
			}
		}
		else if (GetKeyboardPress(DIK_D) == true || GetJoypadPress(nPlayer, JOYKEY_RIGHT) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_RIGHT))
		{//Dキーが押される

			if (GetKeyboardPress(DIK_W) == true || GetJoypadPress(nPlayer, JOYKEY_UP) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_UP))
			{// WとD(右上)の入力
				pPlayer->move.x += sinf(Camerarot.y + D3DX_PI * 0.25f) * PLAYER_MOVE;
				pPlayer->move.z += cosf(Camerarot.y + D3DX_PI * 0.25f) * PLAYER_MOVE;

				pPlayer->rotDest.y = -D3DX_PI * 0.75f + Camerarot.y;	// 目標の角度を設定
				pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

				// rotの補正
				RotRepair(nPlayer);
			}
			else if (GetKeyboardPress(DIK_S) == true || GetJoypadPress(nPlayer, JOYKEY_DOWN) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_DOWN))
			{// SとD(右下)の入力
				pPlayer->move.x += sinf(Camerarot.y + D3DX_PI * 0.75f) * PLAYER_MOVE;
				pPlayer->move.z += cosf(-Camerarot.y - D3DX_PI * 0.75f) * PLAYER_MOVE;

				pPlayer->rotDest.y = -D3DX_PI * 0.25f + Camerarot.y;	// 目標の角度を設定
				pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

				// rotの補正
				RotRepair(nPlayer);
			}
			else
			{// Dだけの入力
				pPlayer->move.x += sinf(Camerarot.y + D3DX_PI * 0.5f) * PLAYER_MOVE;
				pPlayer->move.z += cosf(Camerarot.y + D3DX_PI * 0.5f) * PLAYER_MOVE;

				pPlayer->rotDest.y = -D3DX_PI * 0.5f + Camerarot.y;	// 目標の角度を設定
				pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

				// rotの補正
				RotRepair(nPlayer);
			}
		}
		else if (GetKeyboardPress(DIK_W) == true || GetJoypadPress(nPlayer, JOYKEY_UP) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_UP))
		{//Wキーが押される
			pPlayer->move.z += cosf(Camerarot.y) * PLAYER_MOVE;
			pPlayer->move.x += sinf(Camerarot.y) * PLAYER_MOVE;

			pPlayer->rotDest.y = D3DX_PI + Camerarot.y;	// 目標の角度を設定
			pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

			// rotの補正
			RotRepair(nPlayer);
		}
		else if (GetKeyboardPress(DIK_S) == true || GetJoypadPress(nPlayer, JOYKEY_DOWN) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_DOWN))
		{//Sキーが押される
			pPlayer->move.z += cosf(Camerarot.y - D3DX_PI) * PLAYER_MOVE;
			pPlayer->move.x += sinf(Camerarot.y - D3DX_PI) * PLAYER_MOVE;

			pPlayer->rotDest.y = Camerarot.y;	// 目標の角度を設定
			pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

			// rotの補正
			RotRepair(nPlayer);
		}
	}
	else
	{// ネズミの操作
		if (GetKeyboardPress(DIK_LEFT) == true || GetJoypadPress(nPlayer, JOYKEY_LEFT) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_LEFT))
		{// 左矢印が押される	
			if (pPlayer->bFinishMotion == true && (pPlayer->motionType == MOTIONTYPE_NEUTRAL || pPlayer->motionTypeBlend == MOTIONTYPE_NEUTRAL)
				&& pPlayer->motionType != MOTIONTYPE_MOVE && pPlayer->motionTypeBlend != MOTIONTYPE_MOVE && pPlayer->state == PLAYERSTATE_NEUTRAL)
			{
				SetMotionType(MOTIONTYPE_MOVE, false, 10, nPlayer);
				pPlayer->state = PLAYERSTATE_MOVE;
			}

			if (GetKeyboardPress(DIK_UP) == true || GetJoypadPress(nPlayer, JOYKEY_UP) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_UP))
			{// 左上の入力
				pPlayer->move.x += sinf(Camerarot.y - D3DX_PI * 0.25f) * PLAYER_MOVE;
				pPlayer->move.z += cosf(-Camerarot.y + D3DX_PI * 0.25f) * PLAYER_MOVE;

				pPlayer->rotDest.y = D3DX_PI * 0.75f + Camerarot.y;	// 目標の角度を設定
				pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

				// rotの補正
				RotRepair(nPlayer);
			}
			else if (GetKeyboardPress(DIK_DOWN) == true || GetJoypadPress(nPlayer, JOYKEY_DOWN) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_DOWN))
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
		else if (GetKeyboardPress(DIK_RIGHT) == true || GetJoypadPress(nPlayer, JOYKEY_RIGHT) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_RIGHT))
		{// 右矢印が押される
			if (GetKeyboardPress(DIK_UP) == true || GetJoypadPress(nPlayer, JOYKEY_UP) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_UP))
			{// 右上の入力
				pPlayer->move.x += sinf(Camerarot.y + D3DX_PI * 0.25f) * PLAYER_MOVE;
				pPlayer->move.z += cosf(Camerarot.y + D3DX_PI * 0.25f) * PLAYER_MOVE;

				pPlayer->rotDest.y = -D3DX_PI * 0.75f + Camerarot.y;	// 目標の角度を設定
				pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

				// rotの補正
				RotRepair(nPlayer);
			}
			else if (GetKeyboardPress(DIK_DOWN) == true || GetJoypadPress(nPlayer, JOYKEY_DOWN) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_DOWN))
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
		else if (GetKeyboardPress(DIK_UP) == true || GetJoypadPress(nPlayer, JOYKEY_UP) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_UP))
		{//上矢印が押される
			pPlayer->move.z += cosf(Camerarot.y) * PLAYER_MOVE;
			pPlayer->move.x += sinf(Camerarot.y) * PLAYER_MOVE;

			pPlayer->rotDest.y = D3DX_PI + Camerarot.y;	// 目標の角度を設定
			pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

			// rotの補正
			RotRepair(nPlayer);
		}
		else if (GetKeyboardPress(DIK_DOWN) == true || GetJoypadPress(nPlayer, JOYKEY_DOWN) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_DOWN))
		{//下矢印が押される
			pPlayer->move.z += cosf(Camerarot.y - D3DX_PI) * PLAYER_MOVE;
			pPlayer->move.x += sinf(Camerarot.y - D3DX_PI) * PLAYER_MOVE;

			pPlayer->rotDest.y = Camerarot.y;	// 目標の角度を設定
			pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

			// rotの補正
			RotRepair(nPlayer);
		}
	}

	// スティックでの操作
	if (GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_RIGHT))
	{// 右
		pPlayer->move.x += sinf(Camerarot.y + D3DX_PI * 0.5f) * PLAYER_MOVE;
		pPlayer->move.z += cosf(Camerarot.y + D3DX_PI * 0.5f) * PLAYER_MOVE;

		pPlayer->rotDest.y = -D3DX_PI * 0.5f + Camerarot.y;	// 目標の角度を設定
		pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

		// rotの補正
		RotRepair(nPlayer);
	}
	if (GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_LEFT))
	{// 左
		pPlayer->move.x += sinf(Camerarot.y - D3DX_PI * 0.5f) * PLAYER_MOVE;
		pPlayer->move.z += cosf(Camerarot.y - D3DX_PI * 0.5f) * PLAYER_MOVE;

		pPlayer->rotDest.y = D3DX_PI * 0.5f + Camerarot.y;	// 目標の角度を設定
		pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

		// rotの補正
		RotRepair(nPlayer);
	}
	if (GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_UP))
	{// 上
		pPlayer->move.z += cosf(Camerarot.y) * PLAYER_MOVE;
		pPlayer->move.x += sinf(Camerarot.y) * PLAYER_MOVE;

		pPlayer->rotDest.y = D3DX_PI + Camerarot.y;	// 目標の角度を設定
		pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

		// rotの補正
		RotRepair(nPlayer);
	}
	if (GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_DOWN))
	{// 下
		pPlayer->move.z += cosf(Camerarot.y - D3DX_PI) * PLAYER_MOVE;
		pPlayer->move.x += sinf(Camerarot.y - D3DX_PI) * PLAYER_MOVE;

		pPlayer->rotDest.y = Camerarot.y;	// 目標の角度を設定
		pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

		// rotの補正
		RotRepair(nPlayer);
	}
}

// =================================================
// プレイヤーのジャンプ関数
// =================================================
void JumpPlayer(PlayerType nPlayer)
{
	// プレイヤー構造体をポインタ化
	Player* pPlayer = &g_aPlayer[nPlayer];

	if (nPlayer == 0)
	{// 少女
		// ジャンプする
		if ((GetKeyboardTrigger(DIK_SPACE) == true || GetJoypadTrigger(nPlayer, JOYKEY_A)) && pPlayer->bJump == false)
		{
			//PlaySound(SOUND_LABEL_JUMP);
			//pPlayer->state = PLAYERSTATE_JUMP;

			pPlayer->move.y = JUMP_FORCE;
			pPlayer->bJump = true;
		}
	}
	else
	{// ネズミ
		// ジャンプする
		if ((GetKeyboardTrigger(DIK_RSHIFT) == true || GetJoypadTrigger(nPlayer, JOYKEY_A)) && pPlayer->bJump == false)
		{
			//PlaySound(SOUND_LABEL_JUMP);
			pPlayer->state = PLAYERSTATE_JUMP;

			SetMotionType(MOTIONTYPE_JUMP, false, 5, nPlayer);

			pPlayer->move.y = JUMP_FORCE;
			pPlayer->bJump = true;
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
			KEY* pKey = &pInfo->aKey[nCntModel];		// 現在のキー	
			PARTS* pModel = &pPlayer->PartsInfo.aParts[nCntModel];
			KEY* pKeyNext = &pPlayer->aMotionInfo[pPlayer->motionType].aKeyInfo[nNext].aKey[nCntModel];
			// 次のキー			
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
		pPlayer->nCntAllround++;

		if (pPlayer->nCounterMotion >= pInfo->nFrame)
		{ // モーションカウンターが現在のキー情報のフレーム数を超えた場合
			if (pPlayer->motionType < 0 || pPlayer->motionType >= MOTIONTYPE_MAX)
			{ // モーションインデックスの上下限確認		
				return;
			}

			/** キーを一つ進める **/
			pPlayer->nKey = ((pPlayer->nKey + 1) % pPlayer->aMotionInfo[pPlayer->motionType].nNumKey);

			if (pPlayer->nKey == 0 && pPlayer->bLoop == false)
			{
				pPlayer->bFinishMotion = true;
				SetMotionType(MOTIONTYPE_NEUTRAL, false, 120, Type);
				pPlayer->state = PLAYERSTATE_NEUTRAL;
				pPlayer->nCntAllround = 0;
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
			SetMotionType(pPlayer->motionTypeBlend, false, 10, Type);
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
	if (pPlayer->rot.y > D3DX_PI + Camerarot.y)
	{
		pPlayer->rot.y -= D3DX_PI * 2;
	}
	else if (pPlayer->rot.y < -D3DX_PI + Camerarot.y)
	{
		pPlayer->rot.y += D3DX_PI * 2;
	}
}
