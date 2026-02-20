//==================================================================================
//
// DirectXのギミック処理 [gimmick.cpp]
// Author : TENMA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "effect.h"
#include "gimmick.h"
#include "motion.h"
#include "modeldata.h"
#include "mathUtil.h"
#include "prompt.h"
#include "Texture.h"

using namespace MyMathUtil;

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************

//**********************************************************************************
//*** ギミッククリア可能なプレイヤータイプ ***
//**********************************************************************************
typedef enum
{
	COULD_PLAYER_GIRL = 0,
	COULD_PLAYER_MOUSE,
	COULD_PLAYER_ALL,
	COULD_PLAYER_MAX
} COULD_PLAYER;

//**********************************************************************************
//*** ギミック構造体 ***
//**********************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;						// 中心位置
	D3DXVECTOR3 rot;						// ギミック全体の向き
	D3DXMATRIX mtxWorld;					// ワールドマトリックス
	PARTS_INFO parts;						// ギミックパーツ
	COULD_PLAYER could;						// クリア可能なプレイヤータイプ
	GIMMICKTYPE myType;						// 自身のギミックの種類
	float fRadius;							// 検知半径
	int nCounter;							// 汎用カウンター
	int nIdxPrompt;							// プロンプトのインデックス
	bool bUse;								// 使用状況

	MOTION_INFO aMotionInfo[MOTIONTYPE_MAX];// クリア時モーション
	int nNumMotion;							// 現在のモーションの総数
	bool bLoop;								// ループするかどうか
	int nNumKey;							// 現在のモーションのキーの総数
	int nKey;								// 現在のモーションの現在のキーNo.
	int nCounterMotion;						// 現在のモーションのカウンター
	bool bFinishMotion;						// 現在のモーションが終了したかどうか

	// モーションブレンドの要素
	bool bBlendMotion;						// ブレンドモーションするかどうか
	MOTIONTYPE motionTypeBlend;				// ブレンドモーションの種類
	bool bLoopMotionBlend;					// ブレンドモーションがループするか
	int nNumKeyBlend;						// ブレンドモーションの総キー数
	int nKeyBlend;							// ブレンドモーションの現在のキーNo.
	int nCounterMotionBlend;				// ブレンドモーションのカウンター
	int nFrameBlend;						// ブレンドフレーム数
	int nCounterBlend;						// ブレンドカウンター

	MOTIONTYPE motionType;					// モーションタイプ
	bool bClear;							// クリアしたか
}Gimmick, * LPGIMMICK;

//**********************************************************************************
//*** ギミッククリア構造体 ***
//**********************************************************************************
STRUCT()
{
	const char* pFileName;		// ファイル名
	COULD_PLAYER could;			// クリア可能なプレイヤータイプ
	D3DXVECTOR3 posDefault;		// デフォルト位置
	D3DXVECTOR3 rotDefault;		// デフォルト角度
	float fRadius;				// 当たり判定の半径
}GIMMICK_DATA;

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************
void CaseMulti(LPGIMMICK pGimmick);
void CaseSolo(LPGIMMICK pGimmick);
void SetMotionType(MOTIONTYPE, bool bBlend, int nFrameBlend, GIMMICKTYPE);
void UpdateMotion(GIMMICKTYPE);

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
Gimmick g_aGimmick[GIMMICKTYPE_MAX];		// ギミック情報
GIMMICK_DATA g_aGimmickData[GIMMICKTYPE_MAX] =
{
	{"data/Scripts/bigbutton_g.txt", COULD_PLAYER_GIRL, D3DXVECTOR3(535, 100, -630), D3DXVECTOR3(0, 0, 0), 0.0f},
	{"data/Scripts/smallbutton_g.txt", COULD_PLAYER_MOUSE, D3DXVECTOR3(565, 100, -630), D3DXVECTOR3(0, 0, 0), 0.0f},
	{"data/Scripts/station_g.txt", COULD_PLAYER_ALL, D3DXVECTOR3(573, 100, -900), D3DXVECTOR3(0, D3DX_PI + D3DX_HALFPI, 0), 200.0f },
	{},
};

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitGimmick(void)
{
	// 構造体を初期化
	memset(&g_aGimmick[0], NULL, sizeof(Gimmick) * GIMMICKTYPE_MAX);

	// ギミック数分だけ
	for (int nCntMotion = 0; nCntMotion < GIMMICKTYPE_MAX; nCntMotion++)
	{
		int nIdxMotion = -1;

		// ギミックの種類を保存
		g_aGimmick[nCntMotion].myType = (GIMMICKTYPE)nCntMotion;

		// モーションを読み込み
		if (LoadMotion(g_aGimmickData[nCntMotion].pFileName, &nIdxMotion) == true)
		{
			LPPARTS_INFO pPartsInfo = GetPartsInfo(nIdxMotion);	// パーツ情報のアドレスを取得
			if (pPartsInfo != nullptr)
			{ // NULLCHECK
				g_aGimmick[nCntMotion].parts = *pPartsInfo;		// アドレスの中身のみをコピー

				for (int nCntParts = 0; nCntParts < g_aGimmick[nCntMotion].parts.nNumParts; nCntParts++)
				{
					PARTS* pParts = &g_aGimmick[nCntMotion].parts.aParts[nCntParts];
					pParts->posLocal = pParts->pos;
					pParts->rotLocal = pParts->rot;
				}
			}

			LPMOTIONSCRIPT_INFO pScrInfo = nullptr;
			pScrInfo = GetMotionScriptInfo(nIdxMotion);
			if (pScrInfo != nullptr)
			{ // NULLCHECK
				for (int nCntRead = 0; nCntRead < pScrInfo->nNumMotion; nCntRead++)
				{ // モーション数分保存
					g_aGimmick[nCntMotion].aMotionInfo[nCntRead] = pScrInfo->aMotionInfo[nCntRead];
				}

				g_aGimmick[nCntMotion].bUse = true;

				int Tex;
				LoadTexture("data/TEXTURE/TestPrompt.png", &Tex);
				g_aGimmick[nCntMotion].nIdxPrompt = SetPrompt(g_aGimmick[nCntMotion].pos, D3DXVECTOR2(15.0f, 8.0f), Tex, false);
				SetEnablePrompt(true, g_aGimmick[nCntMotion].nIdxPrompt);

				g_aGimmick[nCntMotion].could = g_aGimmickData[nCntMotion].could;
				g_aGimmick[nCntMotion].pos = g_aGimmickData[nCntMotion].posDefault;
				g_aGimmick[nCntMotion].rot = g_aGimmickData[nCntMotion].rotDefault;
				g_aGimmick[nCntMotion].fRadius = g_aGimmickData[nCntMotion].fRadius;
			}
		}
	}
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitGimmick(void)
{

}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateGimmick(void)
{
	LPGIMMICK pGimmick = &g_aGimmick[0];

	// もし更新すべき何かがあるならここで更新
	for (int nCntGimmick = 0; nCntGimmick < GIMMICKTYPE_MAX; nCntGimmick++, pGimmick++)
	{
		if (pGimmick->bUse == false) continue;

		if (GetNumPlayer() == 1)
		{ // ソロプレイ時
			CaseSolo(pGimmick);
		}
		else
		{ // マルチプレイ時
			CaseMulti(pGimmick);
		}

		UpdateMotion(pGimmick->myType);

		pGimmick->nCounter++;		// カウンター増加
	}
}

//==================================================================================
// --- 描画 ---
//==================================================================================
void DrawGimmick(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	LPGIMMICK pGimmick = &g_aGimmick[0];
	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス
	D3DMATERIAL9 matDef;			// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;				// マテリアルデータへのポインタ

	for (int nCntGimmick = 0; nCntGimmick < GIMMICKTYPE_MAX; nCntGimmick++, pGimmick++)
	{
		if (pGimmick->bUse == false) continue;

		// プレイヤーのワールドマトリックスの初期化
		D3DXMatrixIdentity(&pGimmick->mtxWorld);

		// プレイヤーの向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot,
			pGimmick->rot.y,
			pGimmick->rot.x,
			pGimmick->rot.z);

		D3DXMatrixMultiply(&pGimmick->mtxWorld, &pGimmick->mtxWorld, &mtxRot);	// かけ合わせる

		// プレイヤーの位置を反映
		D3DXMatrixTranslation(&mtxTrans,
			pGimmick->pos.x,
			pGimmick->pos.y,
			pGimmick->pos.z);

		D3DXMatrixMultiply(&pGimmick->mtxWorld, &pGimmick->mtxWorld, &mtxTrans);	// かけ合わせる

		// プレイヤーのワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &pGimmick->mtxWorld);

		// 現在のマテリアルを取得(保存)
		pDevice->GetMaterial(&matDef);

		// 全モデル(パーツ)の描画
		for (int nCntModel = 0; nCntModel < pGimmick->parts.nNumParts; nCntModel++)
		{
			D3DXMATRIX mtxRotModel, mtxTransModel;	// 計算用マトリックス
			D3DXMATRIX mtxParent;	// 親のマトリックス

			// パーツのワールドマトリックス初期化
			D3DXMatrixIdentity(&pGimmick->parts.aParts[nCntModel].mtxWorld);

			// パーツの向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRotModel,
				pGimmick->parts.aParts[nCntModel].rot.y,
				pGimmick->parts.aParts[nCntModel].rot.x,
				pGimmick->parts.aParts[nCntModel].rot.z);

			// かけ合わせる
			D3DXMatrixMultiply(&pGimmick->parts.aParts[nCntModel].mtxWorld,
				&pGimmick->parts.aParts[nCntModel].mtxWorld,
				&mtxRotModel);

			// パーツの位置(オフセット)を反映
			D3DXMatrixTranslation(&mtxTransModel,
				pGimmick->parts.aParts[nCntModel].pos.x,
				pGimmick->parts.aParts[nCntModel].pos.y,
				pGimmick->parts.aParts[nCntModel].pos.z);

			// かけ合わせる
			D3DXMatrixMultiply(&pGimmick->parts.aParts[nCntModel].mtxWorld,
				&pGimmick->parts.aParts[nCntModel].mtxWorld,
				&mtxTransModel);

			// パーツの「親マトリックス」の設定
			if (pGimmick->parts.aParts[nCntModel].nIdxModelParent != -1)
			{// 親モデルが存在する
				mtxParent = pGimmick->parts.aParts[pGimmick->parts.aParts[nCntModel].nIdxModelParent].mtxWorld;
			}
			else
			{// 親モデルが存在しない
				mtxParent = pGimmick->mtxWorld;
			}

			// 算出した「パーツのワールドマトリックス」と「親のマトリックス」をかけ合わせる
			D3DXMatrixMultiply(&pGimmick->parts.aParts[nCntModel].mtxWorld,
				&pGimmick->parts.aParts[nCntModel].mtxWorld,
				&mtxParent);

			// パーツのワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD,
				&pGimmick->parts.aParts[nCntModel].mtxWorld);

			LPMODELDATA pModelData = GetModelData(pGimmick->parts.aParts[nCntModel].nIdxModel);
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

	EndDevice();
}

//==================================================================================
// --- 設置 ---
//==================================================================================
void SetGimmick(D3DXVECTOR3 pos, D3DXVECTOR3 rot, GIMMICKTYPE type)
{
	if (FAILED(CheckIndex(GIMMICKTYPE_MAX, type)))
	{ // インデックス確認
		OutputDebugString(TEXT("ちょっと。ギミックの種類にそんなものないわよ。\n"));
		return;
	}

	// 値を代入
	g_aGimmick[type].pos = pos;
	g_aGimmick[type].rot = rot;
}

//==================================================================================
// --- クリア ---
//==================================================================================
void ClearGimmick(GIMMICKTYPE type)
{
	if (FAILED(CheckIndex(GIMMICKTYPE_MAX, type)))
	{ // インデックス確認
		OutputDebugString(TEXT("ちょっと。ギミックの種類にそんなものないってば。\n"));
		return;
	}

	// クリア状態に設定
	g_aGimmick[type].bClear = true;
}

//==================================================================================
// --- クリア状況取得 ---
//==================================================================================
bool IsClearGimmick(GIMMICKTYPE type)
{
	if (FAILED(CheckIndex(GIMMICKTYPE_MAX, type)))
	{ // インデックス確認
		OutputDebugString(TEXT("ちょっと。ギミックの種類にそんなものないってば。\n"));
		return false;
	}

	// クリア状態を取得
	return g_aGimmick[type].bClear;
}

//==================================================================================
// --- マルチプレイ時判定 ---
//==================================================================================
void CaseMulti(LPGIMMICK pGimmick)
{
	Player* pPlayer = GetPlayer();
	bool bDetection = false;

	if (IsDetection(pGimmick->pos, pGimmick[PLAYERTYPE_GIRL].pos, pGimmick->fRadius)
		&& (pGimmick->could == COULD_PLAYER_GIRL || pGimmick->could == COULD_PLAYER_ALL))
	{ // 少女の判定
		bDetection = true;
	}

	if (IsDetection(pGimmick->pos, pGimmick[PLAYERTYPE_MOUSE].pos, pGimmick->fRadius)
		&& (pGimmick->could == COULD_PLAYER_MOUSE || pGimmick->could == COULD_PLAYER_ALL))
	{ // ネズミの判定
		bDetection = true;
	}

	if (bDetection == true)
	{ // どちらかが半径に入った場合
		SetEnablePrompt(true, pGimmick->nIdxPrompt);
	}
	else
	{
		SetEnablePrompt(false, pGimmick->nIdxPrompt);
	}
}

//==================================================================================
// --- ソロプレイ時判定 ---
//==================================================================================
void CaseSolo(LPGIMMICK pGimmick)
{
	Player* pPlayer = GetPlayer();
	PlayerType type = (PlayerType)GetActivePlayer();
	bool bDetection = false;

	if (IsDetection(pGimmick->pos, pGimmick[type].pos, pGimmick->fRadius)
		&& (pGimmick->could == type || pGimmick->could == COULD_PLAYER_ALL))
	{ // 操作中プレイヤーの判定
		if (pGimmick->motionType == MOTIONTYPE_ACTION) return;
		SetMotionType(MOTIONTYPE_ACTION, false, 0, pGimmick->myType);

		bDetection = true;
	}

	if (bDetection == true)
	{ // 半径に入った場合
		SetEnablePrompt(true, pGimmick->nIdxPrompt);
	}
	else
	{
		SetEnablePrompt(false, pGimmick->nIdxPrompt);
	}
}

//==================================================================================
// --- 当たり判定 ---
//==================================================================================
bool CollisionGimmick(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, Player *pPlayer)
{
	// 当たっているかどうかをbool型で返す
	bool bLand = false;

	for (int nCntModel = 0; nCntModel < GIMMICKTYPE_MAX; nCntModel++)
	{
		if (g_aGimmick[nCntModel].bUse == true)
		{
			Gimmick* pObject = &g_aGimmick[nCntModel];

			for (int nCntParts = 0; nCntParts < pObject->parts.nNumParts; nCntParts++)
			{
				LPMODELDATA pObjInfo = GetModelData(pObject->parts.aParts[nCntParts].nIdxModel);
#if 1
				D3DXVECTOR3 posVtx[4] = {};		// オブジェクトの四辺の頂点
				D3DXVECTOR3 vecMove = D3DXVECTOR3_NULL;		// 移動ベクトル
				D3DXVECTOR3 vecLine = D3DXVECTOR3_NULL;		// 境界線ベクトル
				D3DXVECTOR3 vecLineA[4] = {};	// 境界線ベクトル
				D3DXVECTOR3 vecToPos = D3DXVECTOR3_NULL;	// 位置と境界線のはじめを結んだベクトル
				D3DXVECTOR3 vecToPosA[4] = {};	// 位置と境界線のはじめを結んだベクトル
				D3DXVECTOR3 vecToPosOld = D3DXVECTOR3_NULL;	// 過去位置と境界線のはじめを結んだベクトル 
				D3DXVECTOR3 vecLineNor = D3DXVECTOR3_NULL;	// 正規化した境界線ベクトル
				float fVecPos = 0.0f;
				float fVecPosA[4] = {};
				float fVecPosOld = 0.0f;
				float fPosToMove = 0.0f;					// vecToPosとの外積
				float fLineToMove = 0.0f;					// vecLineとの外積
				float fRate = 0.0f;							// 面積比率
				float fVecPosToNor = 0.0f;					// 逆法線との外積

				if (pPos->y >= pObject->pos.y + pObjInfo->mtxMin.y - 10.0f
					&& pPos->y <= pObject->pos.y + pObjInfo->mtxMax.y)
				{
					D3DXVECTOR3 Length;
					float fLength = 0.0f;
					float fAngle = 0.0f;
					bool bCollision[4] = {};

					posVtx[0].x = pObjInfo->mtxMin.x;
					posVtx[0].y = 0;
					posVtx[0].z = pObjInfo->mtxMin.z;

					D3DXVec3TransformCoord(&posVtx[0], &posVtx[0], &pObject->parts.aParts[nCntParts].mtxWorld);

					if (nCntParts == 1)
						SetEffect(posVtx[0], D3DXCOLOR_NULL, VECNULL, 5, 5, 0, 3);

					posVtx[1].x = pObjInfo->mtxMax.x;
					posVtx[1].y = 0;
					posVtx[1].z = pObjInfo->mtxMin.z;

					D3DXVec3TransformCoord(&posVtx[1], &posVtx[1], &pObject->parts.aParts[nCntParts].mtxWorld);

					if (nCntParts == 1)
						SetEffect(posVtx[1], D3DXCOLOR_NULL, VECNULL, 5, 5, 0, 3);

					posVtx[2].x = pObjInfo->mtxMax.x;
					posVtx[2].y = 0;
					posVtx[2].z = pObjInfo->mtxMax.z;

					D3DXVec3TransformCoord(&posVtx[2], &posVtx[2], &pObject->parts.aParts[nCntParts].mtxWorld);

					if (nCntParts == 1)
						SetEffect(posVtx[2], D3DXCOLOR_NULL, VECNULL, 5, 5, 0, 3);

					posVtx[3].x = pObjInfo->mtxMin.x;
					posVtx[3].y = 0;
					posVtx[3].z = pObjInfo->mtxMax.z;

					D3DXVec3TransformCoord(&posVtx[3], &posVtx[3], &pObject->parts.aParts[nCntParts].mtxWorld);

					if (nCntParts == 1)
						SetEffect(posVtx[3], D3DXCOLOR_NULL, VECNULL, 5, 5, 0, 3);

					/*** 移動ベクトルの取得 ***/
					vecMove = *pPos - *pPosOld;

					for (int nCntCollision = 0; nCntCollision < 4; nCntCollision++)
					{
						D3DXVECTOR3 vecNor = D3DXVECTOR3_NULL;		// 壁の逆法線ベクトル
						D3DXVECTOR3 vecCutLine = D3DXVECTOR3_NULL;	// 交点からの境界線ベクトル

						vecLine = posVtx[(nCntCollision + 1) % 4] - posVtx[nCntCollision];

						RepairFloat(&vecLine.x);
						RepairFloat(&vecLine.z);

						/*** 現在位置との関係を外積を使い求める ***/
						vecToPos = *pPos - posVtx[nCntCollision];

						fVecPos = (vecLine.z * vecToPos.x) - (vecLine.x * vecToPos.z);

						RepairFloat(&fVecPos);

						/*** 過去位置との関係を外積を使い求める ***/
						vecToPosOld = *pPosOld - posVtx[nCntCollision];

						fVecPosOld = (vecLine.z * vecToPosOld.x) - (vecLine.x * vecToPosOld.z);

						RepairFloat(&fVecPosOld);

						/*** 強制位置の判定 ***/

						/** 現在位置との外積 **/
						fPosToMove = (vecToPos.z * vecMove.x) - (vecToPos.x * vecMove.z);

						/** 最大値との外積 **/
						fLineToMove = (vecLine.z * vecMove.x) - (vecLine.x * vecMove.z);

						/** 面積比率の計算 **/
						fRate = fPosToMove / fLineToMove;

						vecNor.x = vecLine.z;
						vecNor.z = -vecLine.x;

						D3DXVec3Normalize(&vecNor, &vecNor);

						fVecPosToNor = (vecNor.z * vecMove.x) - (vecNor.x * vecMove.z);

						D3DXVECTOR3 vecF;
						vecCutLine = vecLine;

						if (fVecPosToNor == 0)
						{
							fVecPosToNor = fabsf(fVecPosToNor);
							D3DXVec3Normalize(&vecCutLine, &vecCutLine);

							vecF = vecCutLine * fVecPosToNor;
						}
						else if (fVecPosToNor > 0)
						{
							fVecPosToNor = fabsf(fVecPosToNor);
							D3DXVec3Normalize(&vecCutLine, &vecCutLine);

							vecF = -vecCutLine * fVecPosToNor;
						}
						else if (fVecPosToNor < 0)
						{
							fVecPosToNor = fabsf(fVecPosToNor);
							D3DXVec3Normalize(&vecCutLine, &vecCutLine);

							vecF = vecCutLine * fVecPosToNor;
						}

						/*** プレイヤーの壁のめり込み判定 ***/
						if (fVecPos <= 0 && fVecPosOld >= 0)
						{
							/*** もしも比率が範囲内であれば,衝突 ***/
							if (fRate >= 0.0f && fRate <= 1.0f)
							{
								if (g_aGimmick[nCntModel].myType != GIMMICKTYPE_BIGBUTTON && g_aGimmick[nCntModel].myType != GIMMICKTYPE_SMALLBUTTON)
								{// ボタン以外には側面の当たり判定をつける
									pPos->x = posVtx[nCntCollision].x + (vecLine.x * fRate) + vecF.x;
									pPos->z = posVtx[nCntCollision].z + (vecLine.z * fRate) + vecF.z;
								}
							}
						}

						if (fVecPos < 0 && fVecPosOld < 0)
						{
							bCollision[nCntCollision] = true;
						}
					}

					if (bCollision[0] == true
						&& bCollision[1] == true
						&& bCollision[2] == true
						&& bCollision[3] == true)
					{// 上の当たり判定
						pPos->y = pObject->pos.y + pObjInfo->mtxMax.y;
						bLand = true;
						pMove->y = 0.0f;
						
						if (g_aGimmick[nCntModel].bClear == false && g_aGimmick[nCntModel].myType == GIMMICKTYPE_BIGBUTTON && GetActivePlayer() == PLAYERTYPE_GIRL)
						{// でかボタンを押す
							SetMotionType(MOTIONTYPE_ACTION, false, 0, GIMMICKTYPE_BIGBUTTON);
							ClearGimmick(GIMMICKTYPE_BIGBUTTON);
						}
						else if (g_aGimmick[nCntModel].bClear == false && g_aGimmick[nCntModel].myType == GIMMICKTYPE_SMALLBUTTON && GetActivePlayer() == PLAYERTYPE_MOUSE)
						{// ちびボタンを押す
							SetMotionType(MOTIONTYPE_ACTION, false, 0, GIMMICKTYPE_SMALLBUTTON);
							ClearGimmick(GIMMICKTYPE_SMALLBUTTON);
						}
					}
				}
			}
#endif
		}
	}

	return bLand;
}

//================================================================================================================
// --- モーションの変更！ ---
//================================================================================================================
void SetMotionType(MOTIONTYPE motionTypeNext, bool bBlend, int nFrameBlend, GIMMICKTYPE Type)
{
	LPGIMMICK pGimmick = &g_aGimmick[Type];	// プレイヤー情報へのアドレス	

	if (motionTypeNext < 0 || motionTypeNext >= MOTIONTYPE_MAX)
	{ // モーションインデックスの上下限確認
		return;
	}

	// ブレンドモーションをするかどうか
	pGimmick->bBlendMotion = bBlend;

	if (bBlend == false)
	{
		/*** 各変数を初期化し、次のモーションを設定 ***/
		pGimmick->nCounterMotion = pGimmick->nCounterMotionBlend;
		pGimmick->nKey = pGimmick->nKeyBlend;
		pGimmick->motionType = motionTypeNext;
		pGimmick->bLoop = pGimmick->aMotionInfo[motionTypeNext].bLoop;
		pGimmick->nNumKey = pGimmick->aMotionInfo[motionTypeNext].nNumKey;
		pGimmick->bFinishMotion = false;

		/*** 現在のモーションのキー情報へのポインタ ***/
		KEY_INFO* pInfo = &pGimmick->aMotionInfo[pGimmick->motionType].aKeyInfo[pGimmick->nKey];

		/*** 全パーツの初期設定！ ***/
		for (int nCntModel = 0; nCntModel < pGimmick->parts.nNumParts; nCntModel++)
		{
			int nNext = (pGimmick->nKey + 1) % pGimmick->aMotionInfo[pGimmick->motionType].nNumKey;

			// 次のキーの値			
			float fRateKey = (float)pGimmick->nCounterMotion / (float)pInfo->nFrame;			// モーションカウンター / 再生フレーム数	

			D3DXVECTOR3 diffPos = {};	// 位置の差分
			D3DXVECTOR3 UpdatePos = {};	// 更新する位置
			D3DXVECTOR3 diffRot = {};	// 角度の差分
			D3DXVECTOR3 UpdateRot = {};	// 更新する角度
			KEY* pKey = &pInfo->aKey[nCntModel];	// 現在のキー
			PARTS* pModel = &pGimmick->parts.aParts[nCntModel];
			KEY* pKeyNext = &pGimmick->aMotionInfo[pGimmick->motionType].aKeyInfo[nNext].aKey[nCntModel];		// 次のキー			

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
		pGimmick->bFinishMotion = false;
		pGimmick->motionTypeBlend = motionTypeNext;
		pGimmick->nCounterBlend = 0;
		pGimmick->nCounterMotionBlend = 0;
		pGimmick->nFrameBlend = nFrameBlend;
		pGimmick->nNumKeyBlend = pGimmick->aMotionInfo[motionTypeNext].nNumKey;
		pGimmick->nKeyBlend = 0;
	}
}

//================================================================================================================
// --- モーションアップデート！ ---
//================================================================================================================
void UpdateMotion(GIMMICKTYPE type)
{
	Gimmick* pGimmick = &g_aGimmick[type];	// プレイヤー情報へのアドレス	

	/*** 現在のモーションのキー情報へのポインタ ***/
	KEY_INFO* pInfo = &pGimmick->aMotionInfo[pGimmick->motionType].aKeyInfo[pGimmick->nKey];
	KEY_INFO* pInfoBlend = &pGimmick->aMotionInfo[pGimmick->motionTypeBlend].aKeyInfo[pGimmick->nKeyBlend];

	/*** 全パーツの更新！ ***/
	for (int nCntModel = 0; nCntModel < pGimmick->parts.nNumParts; nCntModel++)
	{
		if ((pGimmick->nKey + 1) == pGimmick->aMotionInfo[pGimmick->motionType].nNumKey
			&& pGimmick->aMotionInfo[pGimmick->motionType].bLoop == false) continue;
		int nNext = (pGimmick->nKey + 1) % pGimmick->aMotionInfo[pGimmick->motionType].nNumKey;

		// 次のキーの値		
		float fRateKey = (float)pGimmick->nCounterMotion / (float)pInfo->nFrame;		// モーションカウンター / 再生フレーム数
		D3DXVECTOR3 diffPos = {};	// 位置の差分
		D3DXVECTOR3 UpdatePos = {};	// 更新する位置	
		D3DXVECTOR3 diffRot = {};	// 角度の差分		
		D3DXVECTOR3 UpdateRot = {};	// 更新する角度		
		KEY* pKey = &pInfo->aKey[nCntModel];	// 現在のキー		
		KEY* pKeyNext = &pGimmick->aMotionInfo[pGimmick->motionType].aKeyInfo[nNext].aKey[nCntModel];		// 次のキー		
		PARTS* pModel = &pGimmick->parts.aParts[nCntModel];

		if (pGimmick->bBlendMotion == true)
		{ // モーションブレンドあり	
			D3DXVECTOR3 diffKeyPosCurrent = {};	// 現在のモーションの位置の差分			
			D3DXVECTOR3 diffKeyRotCurrent = {};	// 現在のモーションの角度の差分		
			D3DXVECTOR3 diffKeyPosBlend = {};	// ブレンドモーションの位置の差分			
			D3DXVECTOR3 diffKeyRotBlend = {};	// ブレンドモーションの角度の差分	
			D3DXVECTOR3 diffPosBlend = {};		// 最終差分		
			D3DXVECTOR3 diffRotBlend = {};		// 最終差分			
			int nNextBlend = (pGimmick->nKeyBlend + 1) % pGimmick->aMotionInfo[pGimmick->motionTypeBlend].nNumKey;			// 次のキーの値		
			KEY* pKeyBlend = &pInfoBlend->aKey[nCntModel];																// 現在のキー	
			KEY* pKeyNextBlend = &pGimmick->aMotionInfo[pGimmick->motionTypeBlend].aKeyInfo[nNextBlend].aKey[nCntModel];	// 次のキー
			float fRateKeyBlend = (float)pGimmick->nCounterMotionBlend / (float)pInfoBlend->nFrame;						// モーションカウンター / 再生フレーム数
			float fRateBlend = (float)pGimmick->nCounterBlend / (float)pGimmick->nFrameBlend;							// ブレンドの相対量		

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

	if (pGimmick->bBlendMotion == false)
	{// ブレンドなし
		pGimmick->nCounterMotion++;

		if (pGimmick->nCounterMotion >= pInfo->nFrame && pGimmick->bFinishMotion == false)
		{ // モーションカウンターが現在のキー情報のフレーム数を超えた場合
			if (pGimmick->motionType < 0 || pGimmick->motionType >= MOTIONTYPE_MAX)
			{ // モーションインデックスの上下限確認		
				return;
			}

			/** キーを一つ進める **/
			pGimmick->nKey = ((pGimmick->nKey + 1) % pGimmick->aMotionInfo[pGimmick->motionType].nNumKey);

			if (pGimmick->nKey == pGimmick->aMotionInfo[pGimmick->motionType].nNumKey - 1 && pGimmick->bLoop == false)
			{
				pGimmick->bFinishMotion = true;
			}

			pGimmick->nCounterMotion = 0;
		}
	}
	else
	{// ブレンドあり
		pGimmick->nCounterMotionBlend++;

		if (pGimmick->nCounterMotionBlend >= pInfoBlend->nFrame)
		{ // モーションカウンターがブレンドモーションの現在のキーのフレーム数を超えた場合	
			/** ブレンドモーションのキーを一つ進める **/
			pGimmick->nKeyBlend = ((pGimmick->nKeyBlend + 1) % pGimmick->aMotionInfo[pGimmick->motionTypeBlend].nNumKey);
			pGimmick->nCounterMotionBlend = 0;
		}

		pGimmick->nCounterBlend++;

		if (pGimmick->nCounterBlend >= pGimmick->nFrameBlend)
		{ // ブレンドカウンターがブレンドフレーム数を超えた場合	
			pGimmick->bFinishMotion = true;
			SetMotionType(pGimmick->motionTypeBlend, false, 20, type);
		}
	}
}