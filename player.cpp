// =================================================
// 
// プレイヤー処理[mpdel.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include<stdio.h>
#include "main.h"
#include "camera.h"
#include "debugproc.h"
#include "game.h"
#include "input.h"
#include "model.h"
#include "modeldata.h"
#include "motion.h"
#include "player.h"

// =================================================
// マクロ定義
#define	MAX_PLAYER		(2)				// プレイヤーの最大数
#define	MAX_TEX			(16)			// テクスチャの最大数
#define	PLAYER_MOVE		(0.5)				// プレイヤーの移動速度
#define	ATTACK_MOVE		(1.0f)			// 突進の移動速度
#define	PLAYER_ROTMOVE	(0.3f)			// プレイヤーの回転速度
#define	PLAYER_INI		(0.2f)			// プレイヤーの慣性
#define	PLAYER_LIM		(50)			// プレイヤーの移動制限
#define	GRAVITY			(-0.3f)			// 重力
#define	JUMP_FORCE		(4.1f)			// ジャンプ力
#define	STICK_DEADAREA	(10000)			// ジョイスティックのデッドエリア
#define	MODEL_PLAYER	"data\\model.txt"	// モデルを読み込むファイル名

// =================================================
// グローバル変数
Player g_Player[PLAYERTYPE_MAX];	// プレイヤーの樹応報
int g_IdxShadowPlayer = -1;			// 使用する影の番号
float g_sinrot = 0;					// sinカーブを用いると起用の変数
int g_nNumPlayer = 0;				// プレイヤーのアクティブ人数
int g_ActivePlayer = 0;				// 操作しているプレイヤータイプ

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
		g_Player[nCntPlayer].posOri = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Player[nCntPlayer].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Player[nCntPlayer].posOld = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Player[nCntPlayer].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Player[nCntPlayer].bDisp = true;
		g_Player[nCntPlayer].bJump = false;
		g_Player[nCntPlayer].nCounterMotion = 0;
		g_Player[nCntPlayer].nKey = 0;
		g_Player[nCntPlayer].state = PLAYERSTATE_NEUTRAL;
		g_Player[nCntPlayer].bFinishMotion = true;
		g_Player[nCntPlayer].nIdxCamera = GetCamera();
	}

	// 少女のパーツ、モーションを読み込む
	LoadMotion("data\\Scripts\\mouse.txt", &aIdxMotion[PLAYERTYPE_GIRL]);			// モーションスクリプトを読み込む
	LPPARTS_INFO pPartsInfoGirl = GetPartsInfo(aIdxMotion[PLAYERTYPE_GIRL]);	// パーツ情報のアドレスを取得
	LPMOTIONSCRIPT_INFO pMotionInfoGirl = GetMotionScriptInfo(aIdxMotion[PLAYERTYPE_GIRL]);	// モーション情報のアドレスを取得
	if (pPartsInfoGirl != NULL)
	{// NULLじゃなかったとき
		g_Player[PLAYERTYPE_GIRL].PartsInfo = *pPartsInfoGirl;		// アドレスの中身のみをコピー
	}

	// ネズミのパーツ、モーションを読み込む
	LoadMotion("data\\Scripts\\mouse.txt", &aIdxMotion[PLAYERTYPE_MOUSE]);		// モーションスクリプトを読み込む
	LPPARTS_INFO pPartsInfoMouse = GetPartsInfo(aIdxMotion[PLAYERTYPE_MOUSE]);	// パーツ情報のアドレスを取得
	LPMOTIONSCRIPT_INFO pMotionInfoMouse = GetMotionScriptInfo(aIdxMotion[PLAYERTYPE_MOUSE]);	// モーション情報のアドレスを取得
	if (pPartsInfoMouse != NULL)
	{// NULLじゃなかったとき
		g_Player[PLAYERTYPE_MOUSE].PartsInfo = *pPartsInfoMouse;	// アドレスの中身のみをコピー
	}
	// モーションのキー、フレーム情報を代入する
	for (int nCntMotion = 0; nCntMotion < MOTIONTYPE_MAX; nCntMotion++)
	{
		if (pMotionInfoGirl != NULL)
		{// NULLじゃなかったとき
			g_Player[PLAYERTYPE_GIRL].aMotionInfo[nCntMotion] = pMotionInfoGirl->aMotionInfo[nCntMotion];
		}
		if (pMotionInfoMouse != NULL)
		{// NULLじゃなかったとき
			g_Player[PLAYERTYPE_MOUSE].aMotionInfo[nCntMotion] = pMotionInfoMouse->aMotionInfo[nCntMotion];
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
	Player* pPlayer = &g_Player[0];

	for (int nCntPlayer = 0; nCntPlayer < PLAYERTYPE_MAX; nCntPlayer++, pPlayer++)
	{
		// 現在位置の保存
		pPlayer->posOld = pPlayer->pos;

		if (g_Player->bFinishMotion == true)
		{// 待機モーションを再生
		}

		MovePlayer(nCntPlayer);	// 移動に関する処理

		//JumpPlayer();	// ジャンプに関する処理

	//
	//// 突進と同時にposを移動する
	//	if (g_Player.motionType == MOTIONTYPEPLAYER_ATTACK && g_Player.nKey >= 3 && g_Player.dir == PLAYERDIRECTION_EAST)
	//	{
	//		g_Player.move.x += ATTACK_MOVE;
	//	}
	//	if (g_Player.motionType == MOTIONTYPEPLAYER_ATTACK && g_Player.nKey >= 3 && g_Player.dir == PLAYERDIRECTION_WEST)
	//	{
	//		g_Player.move.x -= ATTACK_MOVE;
	//	}
	//	if (g_Player.motionType == MOTIONTYPEPLAYER_ATTACK && g_Player.nKey >= 3 && g_Player.dir == PLAYERDIRECTION_NORTH)
	//	{
	//		g_Player.move.z += ATTACK_MOVE;
	//	}
	//	if (g_Player.motionType == MOTIONTYPEPLAYER_ATTACK && g_Player.nKey >= 3 && g_Player.dir == PLAYERDIRECTION_SOUTH)
	//	{
	//		g_Player.move.z -= ATTACK_MOVE;
	//	}
	//	if (g_Player.motionType == MOTIONTYPEPLAYER_ATTACK && g_Player.nKey >= 3 && g_Player.dir == PLAYERDIRECTION_NORTHEAST)
	//	{
	//		g_Player.move.x += ATTACK_MOVE;
	//		g_Player.move.z += ATTACK_MOVE;
	//	}
	//	if (g_Player.motionType == MOTIONTYPEPLAYER_ATTACK && g_Player.nKey >= 3 && g_Player.dir == PLAYERDIRECTION_NORTHWEST)
	//	{
	//		g_Player.move.x -= ATTACK_MOVE;
	//		g_Player.move.z += ATTACK_MOVE;
	//	}
	//	if (g_Player.motionType == MOTIONTYPEPLAYER_ATTACK && g_Player.nKey >= 3 && g_Player.dir == PLAYERDIRECTION_SOUTHEAST)
	//	{
	//		g_Player.move.x += ATTACK_MOVE;
	//		g_Player.move.z -= ATTACK_MOVE;
	//	}
	//	if (g_Player.motionType == MOTIONTYPEPLAYER_ATTACK && g_Player.nKey >= 3 && g_Player.dir == PLAYERDIRECTION_SOUTHWEST)
	//	{
	//		g_Player.move.x -= ATTACK_MOVE;
	//		g_Player.move.z -= ATTACK_MOVE;
	//	}く

		pPlayer->move.y += GRAVITY;	// 重力をかけ続ける

		// 移動量の更新
		pPlayer->pos += pPlayer->move;

		// 地面に沈んだ時
		if (pPlayer->pos.y < 0.0f)
		{
			pPlayer->pos.y = 0.0f;
		}

		// 慣性を掛ける
		pPlayer->move.x += (0.0f - pPlayer->move.x) * (PLAYER_INI * 1.5f);
		pPlayer->move.z += (0.0f - pPlayer->move.z) * (PLAYER_INI * 1.5f);

		// カメラに使用しているインデックスを渡して追従させる
		SetPotisionCamera(pPlayer->nIdxCamera, pPlayer->pos);

		// カメラを有効化させる
		CameraEnable(pPlayer->nIdxCamera);

		//// 地面に埋まった時の処理
		//if (CollisionMeshField(&pPlayer->pos, &pPlayer->posOld) == true)
		//{
		//	if ((pPlayer->state == PLAYERSTATE_WALK || pPlayer->state == PLAYERSTATE_JUMP) && pPlayer->bJump == true)
		//	{
		//		PlaySound(SOUND_LABEL_LAND);
		//	}

		//	pPlayer->bJump = false;
		//}
		//else
		//{
		//	pPlayer->bJump = true;
		//}

		//// モデルとの当たり判定
		//if (CollisionGimicck(&pPlayer->pos, &pPlayer->posOld, &pPlayer->move) == true)
		//{
		//	// 影の位置設定
		//	SetPositionShadow(g_IdxShadowPlayer, D3DXVECTOR3(pPlayer->pos.x, pGimicck->vtxMax.y + 10, pPlayer->pos.z), 30, 30);
		//}
		//else if (CollisionGimicck(&pPlayer->pos, &pPlayer->posOld, &pPlayer->move) == false)
		//{
		//	// 影の位置設定
		//	SetPositionShadow(g_IdxShadowPlayer, D3DXVECTOR3(pPlayer->pos.x, 5.0f, pPlayer->pos.z), 30, 30);
		//}

		//UpdateMotion();
	}


	//PrintDebugProc("[POS] : [%f]\n", g_Player.pos.x);
	//PrintDebugProc("        [%f]\n", g_Player.pos.y);
	//PrintDebugProc("        [%f]\n\n", g_Player.pos.z);
}

// =================================================
// 描画処理
// =================================================
void DrawPlayer(void)
{
	// プレイヤー構造体をポインタ化
	Player* pPlayer = &g_Player[0];

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
				pPlayer->PartsInfo.aParts[nCntModel].posLocal.x,
				pPlayer->PartsInfo.aParts[nCntModel].posLocal.y,
				pPlayer->PartsInfo.aParts[nCntModel].posLocal.z);
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
	return &g_Player[0];
}

// =================================================
// プレイヤーの行動関数
// =================================================
void MovePlayer(int nPlayer)
{
	// プレイヤー構造体をポインタ化
	Player* pPlayer = &g_Player[nPlayer];

	// カメラの向きを保存する
	vec3 Camerarot;

	// カメラの情報を取得
	GetCameraRot(nPlayer, &Camerarot);

	if (nPlayer == 0)
	{// 少女の操作
		if (GetKeyboardPress(DIK_A) == true || GetJoypadPress(nPlayer, JOYKEY_LEFT) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_LEFT))
		{//Aキーが押される	

			if (GetKeyboardPress(DIK_W) == true || GetJoypadPress(nPlayer, JOYKEY_UP) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_UP))
			{// WとA(左上)の入力
				pPlayer->move.x += sinf(Camerarot.y - D3DX_PI * 0.25f) * PLAYER_MOVE;
				pPlayer->move.z += cosf(-Camerarot.y + D3DX_PI * 0.25f) * PLAYER_MOVE;

				pPlayer->rotDest.y = D3DX_PI * 0.75f + Camerarot.y;	// 目標の角度を設定
				pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

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
			else if (GetKeyboardPress(DIK_S) == true || GetJoypadPress(nPlayer, JOYKEY_DOWN) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_DOWN))
			{// SとA(左下)の入力
				pPlayer->move.x += sinf(Camerarot.y - D3DX_PI * 0.75f) * PLAYER_MOVE;
				pPlayer->move.z += cosf(Camerarot.y - D3DX_PI * 0.75f) * PLAYER_MOVE;

				pPlayer->rotDest.y = D3DX_PI * 0.25f + Camerarot.y;	// 目標の角度を設定
				pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

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
			else
			{// A単体の入力
				pPlayer->move.x += sinf(Camerarot.y - D3DX_PI * 0.5f) * PLAYER_MOVE;
				pPlayer->move.z += cosf(Camerarot.y - D3DX_PI * 0.5f) * PLAYER_MOVE;

				pPlayer->rotDest.y = D3DX_PI * 0.5f + Camerarot.y;	// 目標の角度を設定
				pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

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
		}
		else if (GetKeyboardPress(DIK_D) == true || GetJoypadPress(nPlayer, JOYKEY_RIGHT) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_RIGHT))
		{//Dキーが押される

			if (GetKeyboardPress(DIK_W) == true || GetJoypadPress(nPlayer, JOYKEY_UP) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_UP))
			{// WとD(右上)の入力
				pPlayer->move.x += sinf(Camerarot.y + D3DX_PI * 0.25f) * PLAYER_MOVE;
				pPlayer->move.z += cosf(Camerarot.y + D3DX_PI * 0.25f) * PLAYER_MOVE;

				pPlayer->rotDest.y = -D3DX_PI * 0.75f + Camerarot.y;	// 目標の角度を設定
				pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

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
			else if (GetKeyboardPress(DIK_S) == true || GetJoypadPress(nPlayer, JOYKEY_DOWN) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_DOWN))
			{// SとD(右下)の入力
				pPlayer->move.x += sinf(Camerarot.y + D3DX_PI * 0.75f) * PLAYER_MOVE;
				pPlayer->move.z += cosf(-Camerarot.y - D3DX_PI * 0.75f) * PLAYER_MOVE;

				pPlayer->rotDest.y = -D3DX_PI * 0.25f + Camerarot.y;	// 目標の角度を設定
				pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

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
			else
			{// Dだけの入力
				pPlayer->move.x += sinf(Camerarot.y + D3DX_PI * 0.5f) * PLAYER_MOVE;
				pPlayer->move.z += cosf(Camerarot.y + D3DX_PI * 0.5f) * PLAYER_MOVE;

				pPlayer->rotDest.y = -D3DX_PI * 0.5f + Camerarot.y;	// 目標の角度を設定
				pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

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
		}
		else if (GetKeyboardPress(DIK_W) == true || GetJoypadPress(nPlayer, JOYKEY_UP) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_UP))
		{//Wキーが押される
			pPlayer->move.z += cosf(Camerarot.y) * PLAYER_MOVE;
			pPlayer->move.x += sinf(Camerarot.y) * PLAYER_MOVE;

			pPlayer->rotDest.y = D3DX_PI + Camerarot.y;	// 目標の角度を設定
			pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

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
		else if (GetKeyboardPress(DIK_S) == true || GetJoypadPress(nPlayer, JOYKEY_DOWN) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_DOWN))
		{//Sキーが押される
			pPlayer->move.z += cosf(Camerarot.y - D3DX_PI) * PLAYER_MOVE;
			pPlayer->move.x += sinf(Camerarot.y - D3DX_PI) * PLAYER_MOVE;

			pPlayer->rotDest.y = Camerarot.y;	// 目標の角度を設定
			pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

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
	}
	else
	{// ネズミの操作
		if (GetKeyboardPress(DIK_LEFT) == true || GetJoypadPress(nPlayer, JOYKEY_LEFT) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_LEFT))
		{// 左矢印が押される	

			if (GetKeyboardPress(DIK_UP) == true || GetJoypadPress(nPlayer, JOYKEY_UP) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_UP))
			{// 左上の入力
				pPlayer->move.x += sinf(Camerarot.y - D3DX_PI * 0.25f) * PLAYER_MOVE;
				pPlayer->move.z += cosf(-Camerarot.y + D3DX_PI * 0.25f) * PLAYER_MOVE;

				pPlayer->rotDest.y = D3DX_PI * 0.75f + Camerarot.y;	// 目標の角度を設定
				pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

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
			else if (GetKeyboardPress(DIK_DOWN) == true || GetJoypadPress(nPlayer, JOYKEY_DOWN) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_DOWN))
			{// 左下の入力
				pPlayer->move.x += sinf(Camerarot.y - D3DX_PI * 0.75f) * PLAYER_MOVE;
				pPlayer->move.z += cosf(Camerarot.y - D3DX_PI * 0.75f) * PLAYER_MOVE;

				pPlayer->rotDest.y = D3DX_PI * 0.25f + Camerarot.y;	// 目標の角度を設定
				pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

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
			else
			{// 左矢印単体の入力
				pPlayer->move.x += sinf(Camerarot.y - D3DX_PI * 0.5f) * PLAYER_MOVE;
				pPlayer->move.z += cosf(Camerarot.y - D3DX_PI * 0.5f) * PLAYER_MOVE;

				pPlayer->rotDest.y = D3DX_PI * 0.5f + Camerarot.y;	// 目標の角度を設定
				pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

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
		}
		else if (GetKeyboardPress(DIK_RIGHT) == true || GetJoypadPress(nPlayer, JOYKEY_RIGHT) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_RIGHT))
		{// 右矢印が押される
			if (GetKeyboardPress(DIK_UP) == true || GetJoypadPress(nPlayer, JOYKEY_UP) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_UP))
			{// 右上の入力
				pPlayer->move.x += sinf(Camerarot.y + D3DX_PI * 0.25f) * PLAYER_MOVE;
				pPlayer->move.z += cosf(Camerarot.y + D3DX_PI * 0.25f) * PLAYER_MOVE;

				pPlayer->rotDest.y = -D3DX_PI * 0.75f + Camerarot.y;	// 目標の角度を設定
				pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

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
			else if (GetKeyboardPress(DIK_DOWN) == true || GetJoypadPress(nPlayer, JOYKEY_DOWN) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_DOWN))
			{// 右下の入力
				pPlayer->move.x += sinf(Camerarot.y + D3DX_PI * 0.75f) * PLAYER_MOVE;
				pPlayer->move.z += cosf(-Camerarot.y - D3DX_PI * 0.75f) * PLAYER_MOVE;

				pPlayer->rotDest.y = -D3DX_PI * 0.25f + Camerarot.y;	// 目標の角度を設定
				pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

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
			else
			{// 右矢印だけの入力
				pPlayer->move.x += sinf(Camerarot.y + D3DX_PI * 0.5f) * PLAYER_MOVE;
				pPlayer->move.z += cosf(Camerarot.y + D3DX_PI * 0.5f) * PLAYER_MOVE;

				pPlayer->rotDest.y = -D3DX_PI * 0.5f + Camerarot.y;	// 目標の角度を設定
				pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

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
		}
		else if (GetKeyboardPress(DIK_UP) == true || GetJoypadPress(nPlayer, JOYKEY_UP) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_UP))
		{//上矢印が押される
			pPlayer->move.z += cosf(Camerarot.y) * PLAYER_MOVE;
			pPlayer->move.x += sinf(Camerarot.y) * PLAYER_MOVE;

			pPlayer->rotDest.y = D3DX_PI + Camerarot.y;	// 目標の角度を設定
			pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

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
		else if (GetKeyboardPress(DIK_DOWN) == true || GetJoypadPress(nPlayer, JOYKEY_DOWN) == true || GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_DOWN))
		{//下矢印が押される
			pPlayer->move.z += cosf(Camerarot.y - D3DX_PI) * PLAYER_MOVE;
			pPlayer->move.x += sinf(Camerarot.y - D3DX_PI) * PLAYER_MOVE;

			pPlayer->rotDest.y = Camerarot.y;	// 目標の角度を設定
			pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

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
	}

	// スティックでの操作
	if (GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_RIGHT))
	{// 右
		pPlayer->move.x += sinf(Camerarot.y + D3DX_PI * 0.5f) * PLAYER_MOVE;
		pPlayer->move.z += cosf(Camerarot.y + D3DX_PI * 0.5f) * PLAYER_MOVE;

		pPlayer->rotDest.y = -D3DX_PI * 0.5f + Camerarot.y;	// 目標の角度を設定
		pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

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
	if (GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_LEFT))
	{// 左
		pPlayer->move.x += sinf(Camerarot.y - D3DX_PI * 0.5f) * PLAYER_MOVE;
		pPlayer->move.z += cosf(Camerarot.y - D3DX_PI * 0.5f) * PLAYER_MOVE;

		pPlayer->rotDest.y = D3DX_PI * 0.5f + Camerarot.y;	// 目標の角度を設定
		pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

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
	if (GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_UP))
	{// 上
		pPlayer->move.z += cosf(Camerarot.y) * PLAYER_MOVE;
		pPlayer->move.x += sinf(Camerarot.y) * PLAYER_MOVE;

		pPlayer->rotDest.y = D3DX_PI + Camerarot.y;	// 目標の角度を設定
		pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

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
	if (GetJoypadStickLeft(nPlayer, JOYKEY_LEFT_STICK_DOWN))
	{// 下
		pPlayer->move.z += cosf(Camerarot.y - D3DX_PI) * PLAYER_MOVE;
		pPlayer->move.x += sinf(Camerarot.y - D3DX_PI) * PLAYER_MOVE;

		pPlayer->rotDest.y = Camerarot.y;	// 目標の角度を設定
		pPlayer->rotDiff.y = pPlayer->rotDest.y - pPlayer->rot.y;	// 現在と目標の角度の差分を算出

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
}
#if 0
// =================================================
// プレイヤーのジャンプ関数
// =================================================
void JumpPlayer(void)
{
	// プレイヤー構造体をポインタ化
	Player* pPlayer = &g_Player[0];

	for (int nCntPlayer = 0; nCntPlayer < PLAYERTYPE_MAX; nCntPlayer++, pPlayer++)
	{
		// ジャンプする
		if ((GetKeyboardTrigger(DIK_RETURN) == true || GetJoypadTrigger(JOYKEY_A)) && pPlayer->bJump == false)
		{
			PlaySound(SOUND_LABEL_JUMP);
			pPlayer->state = PLAYERSTATE_JUMP;

			pPlayer->move.y = JUMP_FORCE;
			pPlayer->bJump = true;
		}
	}
}

// =================================================
// モデル設置処理
// =================================================
void SetModel(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int NumIdx, int Parent, int nParts)
{
	g_Player[PLAYERTYPE_GIRL].nNumModel = nParts;	// パーツ数を設定
	g_Player[PLAYERTYPE_MOUSE].nNumModel = nParts;	// パーツ数を設定

	// === パーツの階層構造の設定 === //
	// 少女のパーツ設定
	g_Player[PLAYERTYPE_GIRL].aModel[NumIdx].nIdxModelParent = Parent;
	g_Player[PLAYERTYPE_GIRL].aModel[NumIdx].pos = pos;
	g_Player[PLAYERTYPE_GIRL].aModel[NumIdx].rot = rot;

	// ネズミのパーツ設定
	g_Player[PLAYERTYPE_MOUSE].aModel[NumIdx].nIdxModelParent = Parent;
	g_Player[PLAYERTYPE_MOUSE].aModel[NumIdx].pos = pos;
	g_Player[PLAYERTYPE_MOUSE].aModel[NumIdx].rot = rot;
}

// =================================================
// キー毎の位置、向きの設定
// =================================================
void SetKey(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	if (g_nCntMotion == 2)
	{
		int n = 0;
	}

	// === キー毎のpos、rotを代入 === //
	// 少女のpos、rotを代入
	g_Player[PLAYERTYPE_GIRL].aMotionInfo[g_nCntMotion].aKeyInfo[g_nCntKeyFrame].aKey[g_nCntKey].fPosX = pos.x;	// 各posを代入
	g_Player[PLAYERTYPE_GIRL].aMotionInfo[g_nCntMotion].aKeyInfo[g_nCntKeyFrame].aKey[g_nCntKey].fPosY = pos.y;	// 各posを代入
	g_Player[PLAYERTYPE_GIRL].aMotionInfo[g_nCntMotion].aKeyInfo[g_nCntKeyFrame].aKey[g_nCntKey].fPosZ = pos.z;	// 各posを代入	
	g_Player[PLAYERTYPE_GIRL].aMotionInfo[g_nCntMotion].aKeyInfo[g_nCntKeyFrame].aKey[g_nCntKey].fRotX = rot.x;	// 各rotを代入
	g_Player[PLAYERTYPE_GIRL].aMotionInfo[g_nCntMotion].aKeyInfo[g_nCntKeyFrame].aKey[g_nCntKey].fRotY = rot.y;	// 各rotを代入
	g_Player[PLAYERTYPE_GIRL].aMotionInfo[g_nCntMotion].aKeyInfo[g_nCntKeyFrame].aKey[g_nCntKey].fRotZ = rot.z;	// 各rotを代入

	// ネズミのpos、rotを代入
	g_Player[PLAYERTYPE_MOUSE].aMotionInfo[g_nCntMotion].aKeyInfo[g_nCntKeyFrame].aKey[g_nCntKey].fPosX = pos.x;	// 各posを代入
	g_Player[PLAYERTYPE_MOUSE].aMotionInfo[g_nCntMotion].aKeyInfo[g_nCntKeyFrame].aKey[g_nCntKey].fPosY = pos.y;	// 各posを代入
	g_Player[PLAYERTYPE_MOUSE].aMotionInfo[g_nCntMotion].aKeyInfo[g_nCntKeyFrame].aKey[g_nCntKey].fPosZ = pos.z;	// 各posを代入
	g_Player[PLAYERTYPE_MOUSE].aMotionInfo[g_nCntMotion].aKeyInfo[g_nCntKeyFrame].aKey[g_nCntKey].fRotX = rot.x;	// 各rotを代入
	g_Player[PLAYERTYPE_MOUSE].aMotionInfo[g_nCntMotion].aKeyInfo[g_nCntKeyFrame].aKey[g_nCntKey].fRotY = rot.y;	// 各rotを代入
	g_Player[PLAYERTYPE_MOUSE].aMotionInfo[g_nCntMotion].aKeyInfo[g_nCntKeyFrame].aKey[g_nCntKey].fRotZ = rot.z;	// 各rotを代入
}

// =================================================
// 一つ一つキーのフレーム設定
// =================================================
void SetKeyInfo(int nFrame)
{
	g_Player[PLAYERTYPE_GIRL].aMotionInfo[g_nCntMotion].aKeyInfo[g_nCntKeyFrame].nFrame = nFrame;	// 1キーごとのフレーム数
	g_Player[PLAYERTYPE_MOUSE].aMotionInfo[g_nCntMotion].aKeyInfo[g_nCntKeyFrame].nFrame = nFrame;	// 1キーごとのフレーム数
}

// =================================================
// モーション情報の設定
// =================================================
void SetMotionInfo(int nLoop, int NumKey)
{
	int Loop = nLoop;

	if (Loop == 0)
	{// ループしない
		g_Player[PLAYERTYPE_GIRL].aMotionInfo[g_nCntMotion].bLoop = false;
	}
	else if (Loop == 1)
	{// ループする
		g_Player[PLAYERTYPE_GIRL].aMotionInfo[g_nCntMotion].bLoop = true;
	}

	//g_Player.aMotionInfo[g_nCntMotion].nNumKey = NumKey;	// キーの総数
}

// =================================================
// モーションの更新処理
// =================================================
void UpdateMotion(void)
{
	float fDiffKey;
	float fPosX;
	float fPosY;
	float fPosZ;
	float fRotX;
	float fRotY;
	float fRotZ;

	KEY* pKey;		// 現在のモーションの現在のキーのポインタ
	KEY* pKeyNext;	// 現在のモーションの1つ次のキーのポインタ

	KEY* pKeyBlend;		// ブレンドモーションの現在のキーのポインタ
	KEY* pKeyNextBlend;	// ブレンドモーションの1つ次のキーのポインタ

	// プレイヤー構造体をポインタ化
	Player* pPlayer = &g_Player[0];

	for (int nCntPlayer = 0; nCntPlayer < PLAYERTYPE_MAX; nCntPlayer++, pPlayer++)
	{
		// 現在のモーションのフレーム数のポインタ
		KEY_INFO* pKeyFrame = &pPlayer->aMotionInfo[pPlayer->motionType].aKeyInfo[pPlayer->nKey];

		// ブレンドモーションのフレーム数のポインタ
		KEY_INFO* pKeyFrameBlend = &pPlayer->aMotionInfo[pPlayer->motionTypeBlend].aKeyInfo[pPlayer->nKeyBlend];

		// 全モデル(パーツ)の更新
		for (int nCntModel = 0; nCntModel < pPlayer->nNumModel; nCntModel++)
		{
			pPlayer->bFinishMotion = false;

			// 現在のモーションのフレームの分割数
			float fRateKey = (float)pPlayer->nCounterMotion / (float)pKeyFrame->nFrame;

			// 現在のモーションのポインタ
			pKey = &pPlayer->aMotionInfo[pPlayer->motionType].aKeyInfo[pPlayer->nKey].aKey[nCntModel];
			pKeyNext = &pPlayer->aMotionInfo[pPlayer->motionType].aKeyInfo[(pPlayer->nKey + 1) % pPlayer->nNumKey].aKey[nCntModel];

			if (pPlayer->bBlendMotion == true)
			{// ブレンドアリ
				// ブレンドモーションのポインタ
				pKeyBlend = &pPlayer->aMotionInfo[pPlayer->motionTypeBlend].aKeyInfo[pPlayer->nKeyBlend].aKey[nCntModel];
				pKeyNextBlend = &pPlayer->aMotionInfo[g_Player->motionTypeBlend].aKeyInfo[(pPlayer->nKeyBlend + 1) % pPlayer->nNumKeyBlend].aKey[nCntModel];

				// ブレンドモーションのフレームの分割数
				float fRateKeyBlend = (float)pPlayer->nCounterMotionBlend / (float)pKeyFrameBlend->nFrame;

				// ブレンド自体の相対値
				float fRateBlend = (float)pPlayer->nCounterBlend / (float)pPlayer->nFrameBlend;

				// キー情報から位置、向きを算出

				// Xの位置を算出
				// 現在のモーション
				fDiffKey = pKeyNext->fPosX - pKey->fPosX;
				float fPosXCurrent = pKey->fPosX + (fDiffKey * fRateKey);

				// ブレンドモーション
				float fDiffKeyBlend = pKeyNextBlend->fPosX - pKeyBlend->fPosX;
				float fPosXBlend = pKeyBlend->fPosX + (fDiffKeyBlend * fRateKeyBlend);

				float fDiffBlend = fPosXBlend - fPosXCurrent;	// 現在モーションとブレンドモーションの差分
				fPosX = fPosXCurrent + (fDiffBlend * fRateBlend);	// fPosXの向きを求める

				// Yの位置を算出
				// 現在のモーション
				fDiffKey = pKeyNext->fPosY - pKey->fPosY;	// 次のモーションとの差分を求める(現在のモーション)
				float fPosYCurrent = pKey->fPosY + (fDiffKey * fRateKey);	// 次のキーに徐々に近づける

				// ブレンドモーション
				fDiffKeyBlend = pKeyNextBlend->fPosY - pKeyBlend->fPosY;	// 次のモーションとの差分を求める(ブレンドモーション)
				float fPosYBlend = pKeyBlend->fPosY + (fDiffKeyBlend * fRateKeyBlend);	// 現在のキーから次のキーに徐々に近づける

				fDiffBlend = fPosYBlend - fPosYCurrent;		// 現在モーションとブレンドモーションの差分
				fPosY = fPosYCurrent + (fDiffBlend * fRateBlend);	// fPosYの位置を求める

				// Zの位置を算出
				// 現在のモーション
				fDiffKey = pKeyNext->fPosZ - pKey->fPosZ;	// 次のモーションとの差分を求める(現在のモーション)
				float fPosZCurrent = pKey->fPosZ + (fDiffKey * fRateKey);	// 次のキーに徐々に近づける

				// ブレンドモーション
				fDiffKeyBlend = pKeyNextBlend->fPosZ - pKeyBlend->fPosZ;	// 次のモーションとの差分を求める(ブレンドモーション)
				float fPosZBlend = pKeyBlend->fPosZ + (fDiffKeyBlend * fRateKeyBlend);	// 現在のキーから次のキーに徐々に近づける

				fDiffBlend = fPosZBlend - fPosZCurrent;		// 現在モーションとブレンドモーションの差分
				fPosZ = fPosZCurrent + (fDiffBlend * fRateBlend);	// fPosZの位置を求める

				// Xの向きを算出
				// 現在のモーション
				fDiffKey = pKeyNext->fRotX - pKey->fRotX;	// 次のモーションとの差分を求める(現在のモーション)

				// もし、差分がπ・-πを超えたら
				if (fDiffKey > D3DX_PI)
				{
					fDiffKey -= D3DX_PI * 2;
				}
				else if (fDiffKey < -D3DX_PI)
				{
					fDiffKey += D3DX_PI * 2;
				}

				float fRotXCurrent = pKey->fRotX + (fDiffKey * fRateKey);	// 次のキーに徐々に近づける

				// ブレンドモーション
				fDiffKeyBlend = pKeyNextBlend->fRotX - pKeyBlend->fRotX;	// 次のモーションとの差分を求める(ブレンドモーション)

				// もし、差分がπ・-πを超えたら
				if (fDiffKeyBlend > D3DX_PI)
				{
					fDiffKeyBlend -= D3DX_PI * 2;
				}
				else if (fDiffKeyBlend < -D3DX_PI)
				{
					fDiffKeyBlend += D3DX_PI * 2;
				}

				float fRotXBlend = pKeyBlend->fRotX + (fDiffKeyBlend * fRateKeyBlend);	// 現在のキーから次のキーに徐々に近づける

				fDiffBlend = fRotXBlend - fRotXCurrent;		// 現在モーションとブレンドモーションの差分
				fRotX = fRotXCurrent + (fDiffBlend * fRateBlend);	// RotXの向きを求める

				// Yの向きを算出
				// 現在のモーション
				fDiffKey = pKeyNext->fRotY - pKey->fRotY;	// 次のモーションとの差分を求める(現在のモーション)

				// もし、差分がπ・-πを超えたら
				if (fDiffKey > D3DX_PI)
				{
					fDiffKey -= D3DX_PI * 2;
				}
				else if (fDiffKey < -D3DX_PI)
				{
					fDiffKey += D3DX_PI * 2;
				}

				float fRotYCurrent = pKey->fRotY + (fDiffKey * fRateKey);	// 次のキーに徐々に近づける

				// ブレンドモーション
				fDiffKeyBlend = pKeyNextBlend->fRotY - pKeyBlend->fRotY;	// 次のモーションとの差分を求める(ブレンドモーション)

				// もし、差分がπ・-πを超えたら
				if (fDiffKeyBlend > D3DX_PI)
				{
					fDiffKeyBlend -= D3DX_PI * 2;
				}
				else if (fDiffKeyBlend < -D3DX_PI)
				{
					fDiffKeyBlend += D3DX_PI * 2;
				}

				float fRotYBlend = pKeyBlend->fRotY + (fDiffKeyBlend * fRateKeyBlend);	// 現在のキーから次のキーに徐々に近づける

				fDiffBlend = fRotXBlend - fRotYCurrent;		// 現在モーションとブレンドモーションの差分
				fRotY = fRotYCurrent + (fDiffBlend * fRateBlend);	// RotYの向きを求める

				// Zの向きを算出
				// 現在のモーション
				fDiffKey = pKeyNext->fRotZ - pKey->fRotZ;	// 次のモーションとの差分を求める(現在のモーション)

				// もし、差分がπ・-πを超えたら
				if (fDiffKey > D3DX_PI)
				{
					fDiffKey -= D3DX_PI * 2;
				}
				else if (fDiffKey < -D3DX_PI)
				{
					fDiffKey += D3DX_PI * 2;
				}

				float fRotZCurrent = pKey->fRotZ + (fDiffKey * fRateKey);	// 次のキーに徐々に近づける

				// ブレンドモーション
				fDiffKeyBlend = pKeyNextBlend->fRotZ - pKeyBlend->fRotZ;	// 次のモーションとの差分を求める(ブレンドモーション)

				// もし、差分がπ・-πを超えたら
				if (fDiffKeyBlend > D3DX_PI)
				{
					fDiffKeyBlend -= D3DX_PI * 2;
				}
				else if (fDiffKeyBlend < -D3DX_PI)
				{
					fDiffKeyBlend += D3DX_PI * 2;
				}

				float fRotZBlend = pKeyBlend->fRotZ + (fDiffKeyBlend * fRateKeyBlend);	// 現在のキーから次のキーに徐々に近づける

				fDiffBlend = fRotXBlend - fRotZCurrent;		// 現在モーションとブレンドモーションの差分
				fRotZ = fRotZCurrent + (fDiffBlend * fRateBlend);	// RotZの向きを求める
			}

			// キー情報から位置、向きを算出
			// Xの位置を算出
			fDiffKey = pKeyNext->fPosX - pKey->fPosX;
			fPosX = pKey->fPosX + (fDiffKey * fRateKey);

			// Yの位置を算出
			fDiffKey = pKeyNext->fPosY - pKey->fPosY;
			fPosY = pKey->fPosY + (fDiffKey * fRateKey);

			// Zの位置を算出
			fDiffKey = pKeyNext->fPosZ - pKey->fPosZ;
			fPosZ = pKey->fPosZ + (fDiffKey * fRateKey);

			// Xの向きを算出
			fDiffKey = pKeyNext->fRotX - pKey->fRotX;

			// もし、差分がπ・-πを超えたら
			if (fDiffKey > D3DX_PI)
			{
				fDiffKey -= D3DX_PI * 2;
			}
			else if (fDiffKey < -D3DX_PI)
			{
				fDiffKey += D3DX_PI * 2;
			}

			fRotX = pKey->fRotX + (fDiffKey * fRateKey);

			// Yの向きを算出
			fDiffKey = pKeyNext->fRotY - pKey->fRotY;

			// もし、差分がπ・-πを超えたら
			if (fDiffKey > D3DX_PI)
			{
				fDiffKey -= D3DX_PI * 2;
			}
			else if (fDiffKey < -D3DX_PI)
			{
				fDiffKey += D3DX_PI * 2;
			}

			fRotY = pKey->fRotY + (fDiffKey * fRateKey);

			// Zの向きを算出
			fDiffKey = pKeyNext->fRotZ - pKey->fRotZ;

			// もし、差分がπ・-πを超えたら
			if (fDiffKey > D3DX_PI)
			{
				fDiffKey -= D3DX_PI * 2;
			}
			else if (fDiffKey < -D3DX_PI)
			{
				fDiffKey += D3DX_PI * 2;
			}

			fRotZ = pKey->fRotZ + (fDiffKey * fRateKey);

			// パーツの位置・向きを設定
			pPlayer->aModel[nCntModel].pos = D3DXVECTOR3(pPlayer->Offset[nCntModel].x + fPosX, pPlayer->Offset[nCntModel].y + fPosY, pPlayer->Offset[nCntModel].z + fPosZ);
			pPlayer->aModel[nCntModel].rot = D3DXVECTOR3(fRotX, fRotY, fRotZ);
		}

		pPlayer->nCounterMotion++;

		// キーを1つ進める
		if (pPlayer->nCounterMotion >= pPlayer->aMotionInfo[pPlayer->motionType].aKeyInfo[pPlayer->nKey].nFrame && pPlayer->bFinishMotion == false)
		{// ループするモーション
			pPlayer->nKey = (pPlayer->nKey + 1) % pPlayer->nNumKey;
			pPlayer->nCounterMotion = 0;
			pPlayer->nCntAllround++;
		}
		else if (pPlayer->nCntAllround >= pPlayer->aMotionInfo[pPlayer->motionType].nNumKey)
		{// 1周したらtrueにする(連発防止)
			pPlayer->bFinishMotion = true;
		}
		else if (pPlayer->nCntAllround >= pPlayer->aMotionInfo[pPlayer->motionType].nNumKey && pPlayer->aMotionInfo[pPlayer->motionType].bLoop == false)
		{// ループしないモーションの全キーを回した場合
			pPlayer->bFinishMotion = true;
			pPlayer->state = PLAYERSTATE_NEUTRAL;
			pPlayer->nCounterMotion = 0;
		}
	}

}

// =================================================
// モーションの設定処理
// =================================================
void SetMotion(MOTIONTYPE motionType, bool bUseBrend, int nBlendFrame, PlayerType PlayerType)
{
	g_Player[PlayerType].bBlendMotion = bUseBrend;	// モーションブレンドするかどうか

	if (bUseBrend == false)
	{
		// モーションの設定
		g_Player[PlayerType].nCntAllround = 0;
		g_Player[PlayerType].motionType = motionType;	// 使用するモーション
		g_Player[PlayerType].nKey = g_Player[PlayerType].nKeyBlend;					// 現在のキー
		g_Player[PlayerType].nCounterMotion = g_Player[PlayerType].nCounterMotionBlend;		// 次のキーに行くためのカウンター
		g_Player[PlayerType].nNumKey = g_Player[PlayerType].aMotionInfo[motionType].nNumKey;	// モーションごとの総キー数	
		g_Player[PlayerType].bFinishMotion = false;		// モーションが終了したかどうか
		g_Player[PlayerType].bLoopMotion = g_Player[PlayerType].aMotionInfo[motionType].bLoop;		// モーションがループするかどうか

		// キー情報のポインタ
		KEY_INFO* pKeyInfo = &g_Player[PlayerType].aMotionInfo[motionType].aKeyInfo[g_Player[PlayerType].nKey];

		// 現在のモーションのフレーム数のポインタ
		KEY_INFO* pKeyFrame = &g_Player[PlayerType].aMotionInfo[g_Player[PlayerType].motionType].aKeyInfo[g_Player[PlayerType].nKey];

		KEY* pKey;		// 現在のモーションの現在のキーのポインタ
		KEY* pKeyNext;	// 現在のモーションの1つ次のキーのポインタ

		// パーツの初期設定
		for (int nCntModel = 0; nCntModel < g_Player[PlayerType].nNumModel; nCntModel++)
		{
			// 現在のモーションのポインタ
			pKey = &g_Player[PlayerType].aMotionInfo[g_Player[PlayerType].motionType].aKeyInfo[g_Player[PlayerType].nKey].aKey[nCntModel];
			pKeyNext = &g_Player[PlayerType].aMotionInfo[g_Player[PlayerType].motionType].aKeyInfo[(g_Player[PlayerType].nKey + 1) % g_Player[PlayerType].nNumKey].aKey[nCntModel];

			// キー情報から位置、向きを算出
			float fDiffKey;
			D3DXVECTOR3 Pos;
			D3DXVECTOR3 Rot;

			float fRateKey = (float)g_Player[PlayerType].nCounterMotion / (float)pKeyFrame->nFrame;

			// Xの位置を算出
			fDiffKey = pKeyNext->fPosX - pKey->fPosX;
			Pos.x = pKey->fPosX + (fDiffKey * fRateKey);

			// Yの位置を算出
			fDiffKey = pKeyNext->fPosY - pKey->fPosY;
			Pos.y = pKey->fPosY + (fDiffKey * fRateKey);

			// Zの位置を算出
			fDiffKey = pKeyNext->fPosZ - pKey->fPosZ;
			Pos.z = pKey->fPosZ + (fDiffKey * fRateKey);

			// Xの向きを算出
			fDiffKey = pKeyNext->fRotX - pKey->fRotX;

			// もし、差分がπ・-πを超えたら
			if (fDiffKey > D3DX_PI)
			{
				fDiffKey -= D3DX_PI * 2;
			}
			else if (fDiffKey < -D3DX_PI)
			{
				fDiffKey += D3DX_PI * 2;
			}

			Rot.x = pKey->fRotX + (fDiffKey * fRateKey);

			// Yの向きを算出
			fDiffKey = pKeyNext->fRotY - pKey->fRotY;

			// もし、差分がπ・-πを超えたら
			if (fDiffKey > D3DX_PI)
			{
				fDiffKey -= D3DX_PI * 2;
			}
			else if (fDiffKey < -D3DX_PI)
			{
				fDiffKey += D3DX_PI * 2;
			}

			Rot.y = pKey->fRotY + (fDiffKey * fRateKey);

			// Zの向きを算出
			fDiffKey = pKeyNext->fRotZ - pKey->fRotZ;

			// もし、差分がπ・-πを超えたら
			if (fDiffKey > D3DX_PI)
			{
				fDiffKey -= D3DX_PI * 2;
			}
			else if (fDiffKey < -D3DX_PI)
			{
				fDiffKey += D3DX_PI * 2;
			}

			Rot.z = pKey->fRotZ + (fDiffKey * fRateKey);

			// パーツの位置・向きを設定
			g_Player[PlayerType].aModel[nCntModel].pos = D3DXVECTOR3(g_Player[PlayerType].Offset[nCntModel].x + Pos.x, g_Player[PlayerType].Offset[nCntModel].y + Pos.y, g_Player[PlayerType].Offset[nCntModel].z + Pos.z);
			g_Player[PlayerType].aModel[nCntModel].rot = Rot;
		}

	}
	else
	{
		// ブレンドモーションの設定
		g_Player[PlayerType].nCntAllround = 0;
		g_Player[PlayerType].motionTypeBlend = motionType;
		g_Player[PlayerType].nFrameBlend = nBlendFrame;		// ブレンドにかけるフレーム数
		g_Player[PlayerType].nNumKeyBlend = g_Player[PlayerType].aMotionInfo[motionType].nNumKey;
		g_Player[PlayerType].nKeyBlend = 0;
		g_Player[PlayerType].nCounterBlend = 0;
		g_Player[PlayerType].nCounterMotionBlend = 0;
		g_Player[PlayerType].bFinishMotion = false;		// モーションが終了したかどうか
	}

}
#endif
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