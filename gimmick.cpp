//==================================================================================
//
// DirectXのギミック処理 [gimmick.cpp]
// Author : TENMA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "gimmick.h"
#include "motion.h"
#include "modeldata.h"
#include "mathUtil.h"

using namespace MyMathUtil;

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************

//**********************************************************************************
//*** ギミック構造体 ***
//**********************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;						// 中心位置
	D3DXVECTOR3 rot;						// ギミック全体の向き
	D3DXMATRIX mtxWorld;					// ワールドマトリックス
	PARTS_INFO parts;						// ギミックパーツ
	int nCounter;							// 汎用カウンター
	bool bUse;								// 使用状況

	MOTION_INFO aMotion[MOTIONTYPE_MAX];	// クリア時モーション
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
}Gimmick, *LPGIMMICK;

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
Gimmick g_aGimmick[GIMMICKTYPE_MAX];		// ギミック情報
const char *g_apMotionPath[GIMMICKTYPE_MAX] =
{
	"data/",
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

		// モーションを読み込み
		if (LoadMotion(g_apMotionPath[nCntMotion], &nIdxMotion) == true)
		{
			LPMOTIONSCRIPT_INFO pScrInfo = nullptr;
			pScrInfo = GetMotionScriptInfo(nIdxMotion);
			if (pScrInfo != nullptr)
			{ // NULLCHECK
				for (int nCntRead = 0; nCntRead < pScrInfo->nNumMotion; nCntRead++)
				{ // モーション数分保存
					g_aGimmick[nCntMotion].aMotion[nCntRead] = pScrInfo->aMotionInfo[nCntRead];
				}

				g_aGimmick[nCntMotion].bUse = true;
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
	D3DXMATERIAL *pMat;				// マテリアルデータへのポインタ

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
	if (FAILED(CheckIndex(type, GIMMICKTYPE_MAX)))
	{ // インデックス確認
		OutputDebugString(TEXT("ちょっと。ギミックの種類にそんなものないわよ。"));
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
	if (FAILED(CheckIndex(type, GIMMICKTYPE_MAX)))
	{ // インデックス確認
		OutputDebugString(TEXT("ちょっと。ギミックの種類にそんなものないってば。"));
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
	if (FAILED(CheckIndex(type, GIMMICKTYPE_MAX)))
	{ // インデックス確認
		OutputDebugString(TEXT("ちょっと。ギミックの種類にそんなものないってば。"));
		return false;
	}

	// クリア状態を取得
	return g_aGimmick[type].bClear;
}

//==================================================================================
// --- 当たり判定 ---
//==================================================================================
bool CollisionGimmick(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, float fHeight)
{
	//LPGIMMICK pGimmick = &g_aGimmick[0];
	//bool bLand = false;

	//for (int nCntGimmick = 0; nCntGimmick < GIMMICKTYPE_MAX; nCntGimmick++, pGimmick++)
	//{
	//	if (pGimmick->bUse == false) continue;

	//	for (int nCntModel = 0; nCntModel < pGimmick->parts.nNumParts; nCntModel++)
	//	{
	//		LPMODELDATA lpModelData = nullptr;
	//		lpModelData = GetModelData(pGimmick->parts.aParts[nCntModel].nIdxModel);
	//		if (lpModelData == nullptr) continue;

	//		D3DXVECTOR3 posColl = D3DXVECTOR3(pGimmick->parts.aParts[nCntModel].pos._41,
	//			pGimmick->parts.aParts[nCntModel].mtxWorld._42,
	//			pGimmick->parts.aParts[nCntModel].mtxWorld._43);

	//		if (pPos->y + fHeight >= posColl.y + lpModelData->mtxMin.y
	//			&& pPos->y <= posColl.y + lpModelData->mtxMax.y)
	//		{
	//			D3DXVECTOR3 posVtx[4] = {};		// オブジェクトの四辺の頂点
	//			D3DXVECTOR3 vecMove = D3DXVECTOR3_NULL;		// 移動ベクトル
	//			D3DXVECTOR3 vecLine = D3DXVECTOR3_NULL;		// 境界線ベクトル
	//			D3DXVECTOR3 vecLineA[4] = {};	// 境界線ベクトル
	//			D3DXVECTOR3 vecToPos = D3DXVECTOR3_NULL;	// 位置と境界線のはじめを結んだベクトル
	//			D3DXVECTOR3 vecToPosA[4] = {};	// 位置と境界線のはじめを結んだベクトル
	//			D3DXVECTOR3 vecToPosOld = D3DXVECTOR3_NULL;	// 過去位置と境界線のはじめを結んだベクトル 
	//			D3DXVECTOR3 vecLineNor = D3DXVECTOR3_NULL;	// 正規化した境界線ベクトル
	//			float fVecPos = 0.0f;
	//			float fVecPosA[4] = {};
	//			float fVecPosOld = 0.0f;
	//			float fPosToMove = 0.0f;					// vecToPosとの外積
	//			float fLineToMove = 0.0f;					// vecLineとの外積
	//			float fRate = 0.0f;							// 面積比率
	//			float fVecPosToNor = 0.0f;					// 逆法線との外積
	//			D3DXVECTOR3 Length;
	//			float fLength = 0.0f;
	//			float fAngle = 0.0f;
	//			bool bCollision[4] = {};

	//			posVtx[0].x = pObject->pos.x + pObjInfo->mtxMin.x;
	//			posVtx[0].y = pObject->pos.y;
	//			posVtx[0].z = pObject->pos.z + pObjInfo->mtxMin.z;

	//			Length = posVtx[0] - pObject->pos;

	//			fLength = D3DXVec3Length(&Length);
	//			fAngle = GetPosToPos(posVtx[0], pObject->pos);

	//			posVtx[0].x = pObject->pos.x + (sinf(fAngle + pObject->rot.y) * fLength);
	//			posVtx[0].y = pObject->pos.y + pObjInfo->mtxMax.y;
	//			posVtx[0].z = pObject->pos.z + (cosf(fAngle + pObject->rot.y) * fLength);

	//			posVtx[1].x = pObject->pos.x + pObjInfo->mtxMax.x;
	//			posVtx[1].y = pObject->pos.y;
	//			posVtx[1].z = pObject->pos.z + pObjInfo->mtxMin.z;

	//			Length = posVtx[1] - pObject->pos;

	//			fLength = D3DXVec3Length(&Length);
	//			fAngle = GetPosToPos(posVtx[1], pObject->pos);

	//			posVtx[1].x = pObject->pos.x + (sinf(fAngle + pObject->rot.y) * fLength);
	//			posVtx[1].y = pObject->pos.y;
	//			posVtx[1].z = pObject->pos.z + (cosf(fAngle + pObject->rot.y) * fLength);

	//			posVtx[2].x = pObject->pos.x + pObjInfo->mtxMax.x;
	//			posVtx[2].y = pObject->pos.y;
	//			posVtx[2].z = pObject->pos.z + pObjInfo->mtxMax.z;

	//			Length = posVtx[2] - pObject->pos;

	//			fLength = D3DXVec3Length(&Length);
	//			fAngle = GetPosToPos(posVtx[2], pObject->pos);

	//			posVtx[2].x = pObject->pos.x + (sinf(fAngle + pObject->rot.y) * fLength);
	//			posVtx[2].y = pObject->pos.y;
	//			posVtx[2].z = pObject->pos.z + (cosf(fAngle + pObject->rot.y) * fLength);

	//			posVtx[3].x = pObject->pos.x + pObjInfo->mtxMin.x;
	//			posVtx[3].y = pPos->y;
	//			posVtx[3].z = pObject->pos.z + pObjInfo->mtxMax.z;

	//			Length = posVtx[3] - pObject->pos;

	//			fLength = D3DXVec3Length(&Length);
	//			fAngle = GetPosToPos(posVtx[3], pObject->pos);

	//			posVtx[3].x = pObject->pos.x + (sinf(fAngle + pObject->rot.y) * fLength);
	//			posVtx[3].y = pObject->pos.y;
	//			posVtx[3].z = pObject->pos.z + (cosf(fAngle + pObject->rot.y) * fLength);

	//			/*** 移動ベクトルの取得 ***/
	//			vecMove = *pPos - *pPosOld;

	//			for (int nCntCollision = 0; nCntCollision < 4; nCntCollision++)
	//			{
	//				D3DXVECTOR3 vecNor = D3DXVECTOR3_NULL;		// 壁の逆法線ベクトル
	//				D3DXVECTOR3 vecCutLine = D3DXVECTOR3_NULL;	// 交点からの境界線ベクトル

	//				vecLine = posVtx[(nCntCollision + 1) % 4] - posVtx[nCntCollision];

	//				RepairFloat(&vecLine.x);
	//				RepairFloat(&vecLine.z);

	//				/*** 現在位置との関係を外積を使い求める ***/
	//				vecToPos = *pPos - posVtx[nCntCollision];

	//				fVecPos = (vecLine.z * vecToPos.x) - (vecLine.x * vecToPos.z);

	//				RepairFloat(&fVecPos);

	//				/*** 過去位置との関係を外積を使い求める ***/
	//				vecToPosOld = *pPosOld - posVtx[nCntCollision];

	//				fVecPosOld = (vecLine.z * vecToPosOld.x) - (vecLine.x * vecToPosOld.z);

	//				RepairFloat(&fVecPosOld);

	//				/*** 強制位置の判定 ***/

	//				/** 現在位置との外積 **/
	//				fPosToMove = (vecToPos.z * vecMove.x) - (vecToPos.x * vecMove.z);

	//				/** 最大値との外積 **/
	//				fLineToMove = (vecLine.z * vecMove.x) - (vecLine.x * vecMove.z);

	//				/** 面積比率の計算 **/
	//				fRate = fPosToMove / fLineToMove;

	//				vecNor.x = vecLine.z;
	//				vecNor.z = -vecLine.x;

	//				D3DXVec3Normalize(&vecNor, &vecNor);

	//				fVecPosToNor = (vecNor.z * vecMove.x) - (vecNor.x * vecMove.z);

	//				D3DXVECTOR3 vecF;
	//				vecCutLine = vecLine;

	//				if (fVecPosToNor == 0)
	//				{
	//					fVecPosToNor = fabsf(fVecPosToNor);
	//					D3DXVec3Normalize(&vecCutLine, &vecCutLine);

	//					vecF = vecCutLine * fVecPosToNor;
	//				}
	//				else if (fVecPosToNor > 0)
	//				{
	//					fVecPosToNor = fabsf(fVecPosToNor);
	//					D3DXVec3Normalize(&vecCutLine, &vecCutLine);

	//					vecF = -vecCutLine * fVecPosToNor;
	//				}
	//				else if (fVecPosToNor < 0)
	//				{
	//					fVecPosToNor = fabsf(fVecPosToNor);
	//					D3DXVec3Normalize(&vecCutLine, &vecCutLine);

	//					vecF = vecCutLine * fVecPosToNor;
	//				}

	//				/*** プレイヤーの壁のめり込み判定 ***/
	//				if (fVecPos <= 0 && fVecPosOld >= 0)
	//				{
	//					/*** もしも比率が範囲内であれば,衝突 ***/
	//					if (fRate >= 0.0f && fRate <= 1.0f)
	//					{
	//						pPos->x = posVtx[nCntCollision].x + (vecLine.x * fRate) + vecF.x;
	//						pPos->z = posVtx[nCntCollision].z + (vecLine.z * fRate) + vecF.z;

	//						//pMove->x = pMove->x * 0.95f;
	//						//pMove->z = pMove->z * 0.95f;
	//					}
	//				}

	//				if (fVecPos < 0 && fVecPosOld < 0)
	//				{
	//					bCollision[nCntCollision] = true;
	//				}
	//			}

	//			if (bCollision[0] == true
	//				&& bCollision[1] == true
	//				&& bCollision[2] == true
	//				&& bCollision[3] == true)
	//			{
	//				pPos->y = pObject->pos.y + pObjInfo->mtxMax.y;
	//				bLand = true;
	//				pMove->y = 0.0f;

	//				if (pColl != NULL)
	//				{
	//					*pColl = true;
	//				}
	//			}
	//		}
	//	}
	//}

	return false;
}