// =================================================
// 
// タイトル用プレイヤー処理[tTPlayer.cpp]
// Author : Tenma Saito
// 
// =================================================
#include "tplayer.h"
#include "modeldata.h"
#include "motion.h"
#include "mathUtil.h"

USE_UTIL;

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
void UpdateMotion(void);	// モーションのアップデート
void SetMotionType(MOTIONTYPE motionTypeNext, bool bBlend, int nFrameBlend);	// モーションの変更
void DrawNormalTPlayer(TPlayer* pTPlayer, int nCntModel, LPDIRECT3DDEVICE9 pDevice);	// プレイヤーの通常描画
void DrawShadowTPlayer(TPlayer* pTPlayer, int nCntModel, LPDIRECT3DDEVICE9 pDevice);	// プレイヤーの影の描画
void CreateShadowMatrixT(LPDIRECT3DDEVICE9 pDevice, D3DXMATRIX* pMtxTPlayer, ShadowMatrix* pOut);		// シャドウマトリックスの作成
void CalcMatrix(TPlayer* pTPlayer);

// =================================================
// 定数変数
const char *pTPlayerMotionPath = "data/Scripts/titlegeargirl.txt";

// =================================================
// グローバル変数
TPlayer g_TPlayer;	// プレイヤーの樹応報
int g_IdxShadowTPlayer = -1;		// 使用する影の番号

// =================================================
// 初期化処理
// =================================================
void InitTPlayer(void)
{
	LPDIRECT3DDEVICE9 pDevice;		// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// モーション情報のインデックスを格納
	int nIdxMotion = {};

	// 各種変数を初期化(0固定)
	for (int nCntTPlayer = 0; nCntTPlayer < 1; nCntTPlayer++)
	{
		g_TPlayer.pos = PLAYER_POSDEF;
		g_TPlayer.posOld = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_TPlayer.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_TPlayer.bDisp = true;
		g_TPlayer.bJump = false;
		g_TPlayer.nCounterMotion = 0;
		g_TPlayer.nKey = 0;
		g_TPlayer.motionType = MOTIONTYPE_NEUTRAL;
		g_TPlayer.motionTypeBlend = MOTIONTYPE_NEUTRAL;
		g_TPlayer.nNumMotion = 1;
		g_TPlayer.nNumModel = 0;
		g_TPlayer.bFinishMotion = true;
		g_TPlayer.bDash = false;
	}

	// 少女のパーツ、モーションを読み込む
	LoadMotion(pTPlayerMotionPath, &nIdxMotion);			// モーションスクリプトを読み込む
	LPPARTS_INFO pPartsInfoGirl = GetPartsInfo(nIdxMotion);	// パーツ情報のアドレスを取得
	LPMOTIONSCRIPT_INFO pMotionInfoGirl = GetMotionScriptInfo(nIdxMotion);	// モーション情報のアドレスを取得

	if (pPartsInfoGirl != NULL)
	{// NULLじゃなかったとき
		g_TPlayer.PartsInfo = *pPartsInfoGirl;		// アドレスの中身のみをコピー

		for (int nCntParts = 0; nCntParts < g_TPlayer.PartsInfo.nNumParts; nCntParts++)
		{
			PARTS* pParts = &g_TPlayer.PartsInfo.aParts[nCntParts];
			pParts->posLocal = pParts->pos;
			pParts->rotLocal = pParts->rot;
		}
	}

	// モーションのキー、フレーム情報を代入する
	for (int nCntMotion = 0; nCntMotion < MOTIONTYPE_MAX; nCntMotion++)
	{
		if (pMotionInfoGirl != NULL)
		{// NULLじゃなかったとき
			g_TPlayer.aMotionInfo[nCntMotion] = pMotionInfoGirl->aMotionInfo[nCntMotion];
		}
	}

	// デバイスの破棄
	EndDevice();

	// 待機モーションに設定
	SetMotionType(MOTIONTYPE_NEUTRAL, false, 0);
}

// =================================================
// 終了処理
// =================================================
void UninitTPlayer(void)
{

}

// =================================================
// 更新処理
// =================================================
void UpdateTPlayer(void)
{
	if (g_TPlayer.nNumMotion == 0) return;

	// モーションの更新
	UpdateMotion();
}

// =================================================
// 描画処理
// =================================================
void DrawTPlayer(void)
{
	// プレイヤー構造体をポインタ化
	TPlayer* pTPlayer = &g_TPlayer;

	AUTODEVICE9 Auto;		// デバイスの自動取得
	LPDIRECT3DDEVICE9 pDevice = Auto.pDevice;		// デバイスへのポインタ

	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス
	D3DMATERIAL9 matDef;			// 現在のマテリアル保存用

		// プレイヤーのワールドマトリックスの初期化
	D3DXMatrixIdentity(&pTPlayer->mtxWorld);

	D3DXVECTOR3 posTrans = pTPlayer->pos;
	D3DXVECTOR3 rotTrans = pTPlayer->rot;

	// プレイヤーの向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
		rotTrans.y, rotTrans.x, rotTrans.z);

	D3DXMatrixMultiply(&pTPlayer->mtxWorld, &pTPlayer->mtxWorld, &mtxRot);	// かけ合わせる

	// プレイヤーの位置を反映
	D3DXMatrixTranslation(&mtxTrans,
		posTrans.x, posTrans.y, posTrans.z);
	D3DXMatrixMultiply(&pTPlayer->mtxWorld, &pTPlayer->mtxWorld, &mtxTrans);	// かけ合わせる

	// 現在のマテリアルを取得(保存)
	pDevice->GetMaterial(&matDef);

	// 各パーツのマトリックスを計算
	CalcMatrix(pTPlayer);

	// 全モデル(パーツ)の影の描画
	for (int nCntModel = 0; nCntModel < pTPlayer->PartsInfo.nNumParts; nCntModel++)
	{
		// 影の描画
		DrawShadowTPlayer(pTPlayer, nCntModel, pDevice);
	}

	// プレイヤーのワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &pTPlayer->mtxWorld);

	// 全モデル(パーツ)の描画
	for (int nCntModel = 0; nCntModel < pTPlayer->PartsInfo.nNumParts; nCntModel++)
	{
		if (nCntModel < START_ARMTYPE)
		{
			// 通常描画
			DrawNormalTPlayer(pTPlayer, nCntModel, pDevice);
		}
		else
		{
			if (nCntModel >= MAX_PARTS) break;
			int nCntModelArm = nCntModel;

			// 通常描画(腕切り替え版)
			DrawNormalTPlayer(pTPlayer, nCntModelArm, pDevice);
		}
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}

// =================================================
// プレイヤーのマトリックスの計算処理
// =================================================
void CalcMatrix(TPlayer* pTPlayer)
{
	// 全モデル(パーツ)のマトリックスの計算
	for (int nCntModel = 0; nCntModel < pTPlayer->PartsInfo.nNumParts; nCntModel++)
	{
		D3DXMATRIX mtxRotModel, mtxTransModel;	// 計算用マトリックス
		D3DXMATRIX mtxParent;	// 親のマトリックス

		// パーツのワールドマトリックス初期化
		D3DXMatrixIdentity(&pTPlayer->PartsInfo.aParts[nCntModel].mtxWorld);

		// パーツの向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRotModel,
			pTPlayer->PartsInfo.aParts[nCntModel].rot.y,
			pTPlayer->PartsInfo.aParts[nCntModel].rot.x,
			pTPlayer->PartsInfo.aParts[nCntModel].rot.z);

		// かけ合わせる
		D3DXMatrixMultiply(&pTPlayer->PartsInfo.aParts[nCntModel].mtxWorld,
			&pTPlayer->PartsInfo.aParts[nCntModel].mtxWorld,
			&mtxRotModel);

		// パーツの位置(オフセット)を反映
		D3DXMatrixTranslation(&mtxTransModel,
			pTPlayer->PartsInfo.aParts[nCntModel].pos.x,
			pTPlayer->PartsInfo.aParts[nCntModel].pos.y,
			pTPlayer->PartsInfo.aParts[nCntModel].pos.z);

		// かけ合わせる
		D3DXMatrixMultiply(&pTPlayer->PartsInfo.aParts[nCntModel].mtxWorld,
			&pTPlayer->PartsInfo.aParts[nCntModel].mtxWorld,
			&mtxTransModel);

		// パーツの「親マトリックス」の設定
		if (pTPlayer->PartsInfo.aParts[nCntModel].nIdxModelParent != -1)
		{// 親モデルが存在する
			mtxParent = pTPlayer->PartsInfo.aParts[pTPlayer->PartsInfo.aParts[nCntModel].nIdxModelParent].mtxWorld;
		}
		else
		{// 親モデルが存在しない
			mtxParent = pTPlayer->mtxWorld;
		}

		// 算出した「パーツのワールドマトリックス」と「親のマトリックス」をかけ合わせる
		D3DXMatrixMultiply(&pTPlayer->PartsInfo.aParts[nCntModel].mtxWorld,
			&pTPlayer->PartsInfo.aParts[nCntModel].mtxWorld,
			&mtxParent);
	}
}

// =================================================
// プレイヤーの描画処理
// =================================================
void DrawNormalTPlayer(TPlayer* pTPlayer, int nCntModel, LPDIRECT3DDEVICE9 pDevice)
{
	// 各NULLCHECK
	if (pTPlayer == nullptr)
	{
		OutputDebugString(TEXT("pTPlayerが設定されていませんよ！"));
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
		&pTPlayer->PartsInfo.aParts[nCntModel].mtxWorld);

	LPMODELDATA pModelData = GetModelData(pTPlayer->PartsInfo.aParts[nCntModel].nIdxModel);
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
void CreateShadowMatrixT(LPDIRECT3DDEVICE9 pDevice, D3DXMATRIX* pMtx, ShadowMatrix* pOut)
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
void DrawShadowTPlayer(TPlayer* pTPlayer, int nCntModel, LPDIRECT3DDEVICE9 pDevice)
{
	// 各NULLCHECK
	if (pTPlayer == nullptr)
	{
		OutputDebugString(TEXT("pTPlayerが設定されていませんよ！"));
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
	CreateShadowMatrixT(pDevice, &pTPlayer->PartsInfo.aParts[nCntModel].mtxWorld, &shadow);

	// プレイヤーのワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &shadow.mtxShadow);

	LPMODELDATA pModelData = GetModelData(pTPlayer->PartsInfo.aParts[nCntModel].nIdxModel);
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
TPlayer* GetTPlayer(void)
{
	return &g_TPlayer;
}

//================================================================================================================
// --- モーションの変更！ ---
//================================================================================================================
void SetMotionType(MOTIONTYPE motionTypeNext, bool bBlend, int nFrameBlend)
{
	TPlayer* pTPlayer = &g_TPlayer;		// プレイヤー情報へのアドレス	

	if (motionTypeNext < 0 || motionTypeNext >= MOTIONTYPE_MAX)
	{ // モーションインデックスの上下限確認
		return;
	}

	// ブレンドモーションをするかどうか
	pTPlayer->bBlendMotion = bBlend;

	if (bBlend == false)
	{
		/*** 各変数を初期化し、次のモーションを設定 ***/
		pTPlayer->nCounterMotion = pTPlayer->nCounterMotionBlend;
		pTPlayer->nKey = pTPlayer->nKeyBlend;
		pTPlayer->motionType = motionTypeNext;
		pTPlayer->bLoop = pTPlayer->aMotionInfo[motionTypeNext].bLoop;
		pTPlayer->nNumKey = pTPlayer->aMotionInfo[motionTypeNext].nNumKey;
		pTPlayer->bFinishMotion = false;
		pTPlayer->nCntAllround = 0;

		/*** 現在のモーションのキー情報へのポインタ ***/
		KEY_INFO* pInfo = &pTPlayer->aMotionInfo[pTPlayer->motionType].aKeyInfo[pTPlayer->nKey];

		/*** 全パーツの初期設定！ ***/
		for (int nCntModel = 0; nCntModel < pTPlayer->PartsInfo.nNumParts; nCntModel++)
		{
			int nNext = (pTPlayer->nKey + 1) % pTPlayer->aMotionInfo[pTPlayer->motionType].nNumKey;

			// 次のキーの値			
			float fRateKey = (float)pTPlayer->nCounterMotion / (float)pInfo->nFrame;			// モーションカウンター / 再生フレーム数	

			D3DXVECTOR3 diffPos = {};	// 位置の差分
			D3DXVECTOR3 UpdatePos = {};	// 更新する位置
			D3DXVECTOR3 diffRot = {};	// 角度の差分
			D3DXVECTOR3 UpdateRot = {};	// 更新する角度
			KEY* pKey = &pInfo->aKey[nCntModel];	// 現在のキー
			PARTS* pModel = &pTPlayer->PartsInfo.aParts[nCntModel];
			KEY* pKeyNext = &pTPlayer->aMotionInfo[pTPlayer->motionType].aKeyInfo[nNext].aKey[nCntModel];		// 次のキー			

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
		pTPlayer->bFinishMotion = false;
		pTPlayer->motionTypeBlend = motionTypeNext;
		pTPlayer->nCounterBlend = 0;
		pTPlayer->nCounterMotionBlend = 0;
		pTPlayer->nFrameBlend = nFrameBlend;
		pTPlayer->nNumKeyBlend = pTPlayer->aMotionInfo[motionTypeNext].nNumKey;
		pTPlayer->nKeyBlend = 0;
		pTPlayer->nCntAllround = 0;
	}
}

//================================================================================================================
// --- モーションアップデート！ ---
//================================================================================================================
void UpdateMotion(void)
{
	TPlayer* pTPlayer = &g_TPlayer;	// プレイヤー情報へのアドレス	

	/*** 現在のモーションのキー情報へのポインタ ***/
	KEY_INFO* pInfo = &pTPlayer->aMotionInfo[pTPlayer->motionType].aKeyInfo[pTPlayer->nKey];
	KEY_INFO* pInfoBlend = &pTPlayer->aMotionInfo[pTPlayer->motionTypeBlend].aKeyInfo[pTPlayer->nKeyBlend];

	/*** 全パーツの更新！ ***/
	for (int nCntModel = 0; nCntModel < pTPlayer->PartsInfo.nNumParts; nCntModel++)
	{
		int nNext = (pTPlayer->nKey + 1) % pTPlayer->aMotionInfo[pTPlayer->motionType].nNumKey;

		// 次のキーの値		
		float fRateKey = (float)pTPlayer->nCounterMotion / (float)pInfo->nFrame;		// モーションカウンター / 再生フレーム数
		D3DXVECTOR3 diffPos = {};	// 位置の差分
		D3DXVECTOR3 UpdatePos = {};	// 更新する位置	
		D3DXVECTOR3 diffRot = {};	// 角度の差分		
		D3DXVECTOR3 UpdateRot = {};	// 更新する角度		
		KEY* pKey = &pInfo->aKey[nCntModel];	// 現在のキー		
		KEY* pKeyNext = &pTPlayer->aMotionInfo[pTPlayer->motionType].aKeyInfo[nNext].aKey[nCntModel];		// 次のキー		
		PARTS* pModel = &pTPlayer->PartsInfo.aParts[nCntModel];

		if (pTPlayer->bBlendMotion == true)
		{ // モーションブレンドあり	
			D3DXVECTOR3 diffKeyPosCurrent = {};	// 現在のモーションの位置の差分			
			D3DXVECTOR3 diffKeyRotCurrent = {};	// 現在のモーションの角度の差分		
			D3DXVECTOR3 diffKeyPosBlend = {};	// ブレンドモーションの位置の差分			
			D3DXVECTOR3 diffKeyRotBlend = {};	// ブレンドモーションの角度の差分	
			D3DXVECTOR3 diffPosBlend = {};		// 最終差分		
			D3DXVECTOR3 diffRotBlend = {};		// 最終差分			
			int nNextBlend = (pTPlayer->nKeyBlend + 1) % pTPlayer->aMotionInfo[pTPlayer->motionTypeBlend].nNumKey;			// 次のキーの値		
			KEY* pKeyBlend = &pInfoBlend->aKey[nCntModel];																// 現在のキー	
			KEY* pKeyNextBlend = &pTPlayer->aMotionInfo[pTPlayer->motionTypeBlend].aKeyInfo[nNextBlend].aKey[nCntModel];	// 次のキー
			float fRateKeyBlend = (float)pTPlayer->nCounterMotionBlend / (float)pInfoBlend->nFrame;						// モーションカウンター / 再生フレーム数
			float fRateBlend = (float)pTPlayer->nCounterBlend / (float)pTPlayer->nFrameBlend;							// ブレンドの相対量		

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

	if (pTPlayer->bBlendMotion == false)
	{// ブレンドなし
		pTPlayer->nCounterMotion++;

		if (pTPlayer->nCounterMotion >= pInfo->nFrame)
		{ // モーションカウンターが現在のキー情報のフレーム数を超えた場合
			if (pTPlayer->motionType < 0 || pTPlayer->motionType >= MOTIONTYPE_MAX)
			{ // モーションインデックスの上下限確認
				pTPlayer->bFinishMotion = true;
				SetMotionType(MOTIONTYPE_NEUTRAL, true, 40);
				return;
			}

			/** キーを一つ進める **/
			pTPlayer->nKey = ((pTPlayer->nKey + 1) % pTPlayer->aMotionInfo[pTPlayer->motionType].nNumKey);
			//PrintDebugProc("key %d\nFrame %d\nCounter %d\n", pTPlayer->nKey, pInfo->nFrame, pTPlayer->nCounterMotion);

			if (pTPlayer->nKey == pTPlayer->aMotionInfo[pTPlayer->motionType].nNumKey - 1 && pTPlayer->bLoop == false)
			{
				pTPlayer->bFinishMotion = true;
				SetMotionType(MOTIONTYPE_NEUTRAL, true, 20);
			}

			pTPlayer->nCounterMotion = 0;
		}
	}
	else
	{// ブレンドあり
		pTPlayer->nCounterMotionBlend++;

		if (pTPlayer->nCounterMotionBlend >= pInfoBlend->nFrame)
		{ // モーションカウンターがブレンドモーションの現在のキーのフレーム数を超えた場合	
			/** ブレンドモーションのキーを一つ進める **/
			pTPlayer->nKeyBlend = ((pTPlayer->nKeyBlend + 1) % pTPlayer->aMotionInfo[pTPlayer->motionTypeBlend].nNumKey);
			pTPlayer->nCounterMotionBlend = 0;
		}

		pTPlayer->nCounterBlend++;

		if (pTPlayer->nCounterBlend >= pTPlayer->nFrameBlend)
		{ // ブレンドカウンターがブレンドフレーム数を超えた場合	
			pTPlayer->bFinishMotion = true;
			SetMotionType(pTPlayer->motionTypeBlend, false, 20);
		}
	}
}

// =================================================
// 位置と角度を適用
// =================================================
void SetTPlayer(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	g_TPlayer.pos = pos;
	g_TPlayer.rot = rot;
}