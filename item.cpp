//==================================================================================================================================
//
//			アイテム処理 [item.cpp]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
//**************************************************************
// インクルード
#include "item.h"

#include "camera.h"
#include "debugproc.h"
#include "input.h"
#include "mathUtil.h"
#include "Texture.h"

using  namespace MyMathUtil;


//**************************************************************
// グローバル変数宣言
int g_nSetItemNum;									// 設置済みのアイテム数
Item g_aItem[MAX_ITEM];								// アイテム情報
ItemInfo g_aItemInfo[ITEMTYPE_MAX] =
{
	{"data\\MODEL\\Item\\GearLarge.x",-1},	 // [0] 新品ののねじ
	{"data\\MODEL\\Item\\GearSmall.x",-1},	 // [1] 磨かれた小さな歯車
	{"data\\MODEL\\Item\\GearLarge.x",-1},	 // [2] 綺麗な大きい歯車
	{"data\\MODEL\\Item\\Shaft.x",-1},		 // [3] まっすぐな軸
	{"data\\MODEL\\Item\\GearLarge.x",-1},	 // [4] 丁寧に直されたぜんまい
	{"data\\MODEL\\Item\\GearLarge.x",-1},	 // [5] 古いねじ
	{"data\\MODEL\\Item\\GearLarge.x",-1},	 // [6] 錆びた小さい歯車
	{"data\\MODEL\\Item\\GearLargeDificit.x",-1},	 // [7] 欠けた大きい歯車
	{"data\\MODEL\\Item\\GearLarge.x",-1},	 // [8] 少しい曲がった軸
	{"data\\MODEL\\Item\\GearLarge.x",-1},	 // [9] ゆがんだぜんまい
};
//**************************************************************
// プロトタイプ宣言
void LoadItemSet(void);															// アイテムの配置情報を読み込み
void SetItem(vec3 pos, vec3 rot, ITEMTYPE type, bool bReflectGirl = true, bool bReflectMouse = true, bool bColi = false);			// アイテム設置
void SaveItemSet(void);															// アイテムの配置情報を書き出し
P_ITEM GetItem(void);															// 先頭アドレス取得

//=========================================================================================
// アイテム初期化
//=========================================================================================
void InitItem(void)
{
	//**************************************************************
	// 変数宣言
	P_ITEM		pItem = GetItem();
	P_ITEMINFO	pItemInfo = &g_aItemInfo[0];

	// アイテム情報読込
	for (int nCntItem = 0; nCntItem < ITEMTYPE_MAX; nCntItem++, pItemInfo++)
	{
		LoadModelData(pItemInfo->pModelFile, &pItemInfo->nNumGet);
	}

	// アイテム配置初期化
	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++, pItem++)
	{
		pItem->bGet = false;
		pItem->bUse = false;
	}

	// 設置
	g_nSetItemNum = 0;
	SetItem(vec3(0.0f, 140.0f, 100.0f), vec3_ZERO, ITEMTYPE_GEARL_TRUE,false,true);
	SetItem(vec3(0.0f, 140.0f, -100.0f), vec3_ZERO, ITEMTYPE_SHAFT_TRUE,true,false);
	SetItem(vec3(100.0f, 140.0f, 100.0f), vec3_ZERO, ITEMTYPE_GEARS_TRUE);
	SetItem(vec3(100.0f, 140.0f, -100.0f), vec3_ZERO, ITEMTYPE_GEARL_FALSE);
}

//=========================================================================================
// アイテム終了
//=========================================================================================
void UninitItem(void)
{

}

//=========================================================================================
// アイテム更新
//=========================================================================================
void UpdateItem(void)
{
	//**************************************************************
	// 変数宣言
	P_ITEM pItem = GetItem();

	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++, pItem++)
	{
		if (pItem->bUse)
		{
			switch (pItem->type)
			{
			default:
				pItem->rot.x += SPIN_ITEM;
				pItem->rot.y += SPIN_ITEM;

				MyMathUtil::RepairRot(pItem->rot.x);
				MyMathUtil::RepairRot(pItem->rot.y);

				break;
			}
		}
	}
}

//=========================================================================================
// アイテム描画
//=========================================================================================
void DrawItem(void)
{
	//**************************************************************
	// 変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();		// デバイスへのポインタ
	P_ITEM			pItem = GetItem();				// 配置情報へのポインタ
	D3DXMATRIX		mtxView, mtxRot, mtxTrans;		// マトリックス計算用
	PMODELDATA		pModel;							// モデルデータへのポインタ
	D3DMATERIAL9	matDef;							// 現在のマテリアル保存用
	D3DXMATERIAL*	pMat;							// マテリアルデータへのポインタ

	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++, pItem++)
	{
		if (pItem->bUse && (pItem->bGet == false || pItem->bCollision))
		{
			if ((pItem->bGirl && GetReadyCamera() == PLAYER_ONE) || (pItem->bMouse && GetReadyCamera() == PLAYER_TWO))
			{
				// インデックスからモデルデータを取得
				pModel = GetModelData(pItem->nIdxModel);

				//**************************************************************
				// ワールドマトリックスの初期化
				D3DXMatrixIdentity(&pItem->mtxWorld);

				// 向きを反映
				D3DXMatrixRotationYawPitchRoll(&mtxRot, pItem->rot.y, pItem->rot.x, pItem->rot.z);
				D3DXMatrixMultiply(&pItem->mtxWorld, &pItem->mtxWorld, &mtxRot);

				// 位置を反映
				D3DXMatrixTranslation(&mtxTrans, pItem->pos.x, pItem->pos.y, pItem->pos.z);
				D3DXMatrixMultiply(&pItem->mtxWorld, &pItem->mtxWorld, &mtxTrans);

				// ワールドマトリックスの設定
				pDevice->SetTransform(D3DTS_WORLD, &pItem->mtxWorld);

				//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
				// 現在のマテリアルを取得
				pDevice->GetMaterial(&matDef);

				// マテリアルデータへのポインタを取得
				pMat = (D3DXMATERIAL*)pModel->pBuffMat->GetBufferPointer();

				for (int nCntMat = 0; nCntMat < (int)pModel->dwNumMat; nCntMat++)
				{
					// マテリアルの設定
					pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

					// テクスチャの設定
					pDevice->SetTexture(0, pModel->apTexture[nCntMat]);

					// モデル(パーツ)の描画
					pModel->pMesh->DrawSubset(nCntMat);
				}

				// 保存していたマテリアルに戻す
				pDevice->SetMaterial(&matDef);
				//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
			}
		}
	}
	EndDevice();
}

//=========================================================================================
// アイテムの配置情報を読み込み
//=========================================================================================
void LoadItemSet(void)
{

}

//=========================================================================================
// 当たり判定
//=========================================================================================
void CollisionItem(vec3 pos, float fRange)
{
	//**************************************************************
	// 変数宣言
	P_ITEM pItem = GetItem();		// 先頭アドレス

	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++, pItem++)
	{
		if (pItem->bUse && pItem->bGet == false)
		{
			// 距離の計算
			float fDistX = SQUARE(pItem->pos.x - pos.x);
			float fDistY = SQUARE(pItem->pos.y - pos.y);
			float fDistZ = SQUARE(pItem->pos.z - pos.z);
			float fDist = sqrtf(__ABSOLUTE(SQUARE(pItem->pos.x - pos.x) + SQUARE(pItem->pos.y - pos.y) + SQUARE(pItem->pos.z - pos.z)));
			
			// PrintDebugProc("Player<=>Item %f\nPlayerRnge = %f\n ItemRange = %f\n", fDist, fRange, pItem->fRange);

			if (fDist <= pItem->fRange + fRange)
			{
				// PrintDebugProc("!!! 衝突中 !!!\n\n");
				pItem->bGet = true;
				break;
			}
		}
	}

}

//=========================================================================================
// 単体アイテム設置
void SetItem(vec3 pos, vec3 rot, ITEMTYPE type, bool bReflectGirl, bool bReflectMouse, bool bColi)
{
	//**************************************************************
	// 変数宣言
	P_ITEM pItem = GetItem();

	if (MIDCHECK(-1, type, ITEMTYPE_MAX))
	{
		for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++, pItem++)
		{
			if (pItem->bUse == false)
			{
				pItem->pos = pos;
				pItem->rot = rot;
				pItem->fRange = ITEM_RANGE;
				pItem->type = type;								// アイテムタイプ
				pItem->nIdxModel = g_aItemInfo[type].nNumGet;	// モデルデータインデックス
				pItem->bCollision = bColi;
				pItem->bGirl = bReflectGirl;
				pItem->bMouse = bReflectMouse;
				pItem->bGet = false;
				pItem->bUse = true;
				break;
			}
		}
	}
}

//=========================================================================================
// アイテム設置位置保存
void SaveItemSet(void)
{
	//**************************************************************
	// 変数宣言
	//FILE* pFile;

	// fopen(, pFile);
}

//=========================================================================================
// 先頭アドレス取得
P_ITEM GetItem(void)
{
	return &g_aItem[0];
}
