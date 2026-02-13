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
#include "2Dpolygon.h"
#include "UImenu.h"
#include "judgeEnd.h"

using  namespace MyMathUtil;

//==============================================================
// アイテム配置構造体定義
//==============================================================
typedef struct ItemQuota
{
	vec3		pos;
	colX		col;
	int			nType;			// 表示するアイテムの種類
	int			nIdxBox;		// この構造体の番号
	bool		bUse;			// 構造体を使用しているかどうか
}ItemQuota;
POINTER(ItemQuota, P_ITEMQUOTA);

//**************************************************************
// グローバル変数宣言
int				g_nSetItemNum = 0;								// 設置済みのアイテム数
int				g_nSelectPut = -1;								// 提出時のカーソル
bool			g_bPutOut = false;								// アイテムを提出するときtrue
Item			g_aItem[MAX_ITEM];								// アイテム情報
ItemQuota		g_aItemQuota[ITEMTYPE_MAX];						// 所持アイテムを表示する枠のインデックス
ItemQuota		g_aPutQuota[NUM_PUTOUTITEM];					// 提出アイテムを表示する枠のインデックス
ITEMTYPE		g_aPutOut[NUM_PUTOUTITEM];						// 提出したアイテム
ItemInfo		g_aItemInfo[ITEMTYPE_MAX] =
{
	{"data\\MODEL\\Item\\GearLarge.x",-1},		 // [0] 新品ののねじ
	{"data\\MODEL\\Item\\GearSmall.x",-1},		 // [1] 磨かれた小さな歯車
	{"data\\MODEL\\Item\\GearLarge.x",-1},		 // [2] 綺麗な大きい歯車
	{"data\\MODEL\\Item\\Shaft.x",-1},			 // [3] まっすぐな軸
	{"data\\MODEL\\Item\\GearLarge.x",-1},		 // [4] 丁寧に直されたぜんまい

	{"data\\MODEL\\Item\\GearLarge.x",-1},		 // [5] 古いねじ
	{"data\\MODEL\\Item\\GearLarge.x",-1},		 // [6] 錆びた小さい歯車
	{"data\\MODEL\\Item\\GearLargeDificit.x",-1},// [7] 欠けた大きい歯車
	{"data\\MODEL\\Item\\GearLarge.x",-1},		 // [8] 少しい曲がった軸
	{"data\\MODEL\\Item\\GearLarge.x",-1},		 // [9] ゆがんだぜんまい
};
//**************************************************************
// プロトタイプ宣言
void UpdateMapItem(void);			// マップ上のアイテムを更新
void UpdatePouchItem(void);			// 取得済みのアイテムを更新
void PutOut(void);					// 提出できるアイテムを表示
void SelectItem(void);				// 提出アイテムの選択
P_ITEM GetItem(void);				// 先頭アドレス取得

//=========================================================================================
// アイテム初期化
//=========================================================================================
void InitItem(void)
{
	//**************************************************************
	// 変数宣言
	P_ITEM		pItem = GetItem();
	P_ITEMINFO	pItemInfo = &g_aItemInfo[0];
	P_ITEMQUOTA pItemQuota = &g_aItemQuota[0];
	P_ITEMQUOTA pPutQuota = &g_aPutQuota[0];

	g_bPutOut = false;		// アイテムを提出状態ではない
	g_nSelectPut = -1;		// 提出時のカーソル

	// アイテム情報読込
	for (int nCntItem = 0; nCntItem < ITEMTYPE_MAX; nCntItem++, pItemInfo++)
		LoadModelData(pItemInfo->pModelFile, &pItemInfo->nNumGet);

	// アイテム配置初期化
	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++, pItem++)
	{
		pItem->bGet = false;
		pItem->bUse = false;
		pItem->nIdxQuota = -1;
	}

	// 表示用の枠を取得
	for (int nCntQuota = 0; nCntQuota < ITEMTYPE_MAX; nCntQuota++, pItemQuota++)
	{
		pItemQuota->nIdxBox = Set2DPolygon(vec3(SCREEN_WIDTH / ITEMTYPE_MAX * (nCntQuota + 0.5f), SCREEN_HEIGHT * 0.9f, 0.0f), vec3_ZERO, vec2(SCREEN_WIDTH * 0.05f, SCREEN_WIDTH * 0.05f), -1, colX_ZERO);
		pItemQuota->nType = -1;
		pItemQuota->bUse = false;
	}

	// 提出時用の枠を取得
	for (int nCntQuota = 0; nCntQuota < NUM_PUTOUTITEM; nCntQuota++, pPutQuota++)
	{
		pPutQuota->nIdxBox = Set2DPolygon(vec3_ZERO, vec3_ZERO, vec2(SCREEN_WIDTH * 0.05f, SCREEN_WIDTH * 0.05f), -1, colX_ZERO);
		pPutQuota->nType = -1;
		pPutQuota->bUse = false;
		g_aPutOut[nCntQuota] = ITEMTYPE_SPRING_FALSE;
	}

	// 設置
	g_nSetItemNum = 0;	// 配置数を初期化
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
	// マップアイテム更新
	UpdateMapItem();

	// ポーチアイテム更新
	UpdatePouchItem();

	if (GetKeyboardTrigger(DIK_F3))
		EnableItemPut();
}

//=========================================================================================
// マップアイテム更新
void UpdateMapItem(void)
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
// ポーチアイテム更新
void UpdatePouchItem(void)
{
	P_ITEMQUOTA pItemQuota = &g_aItemQuota[0];
	P_ITEMQUOTA pPutQuota = &g_aPutQuota[0];

	// 提出フラグが立っていたら
	if (g_bPutOut)
	{
		PutOut();		// 取得済みアイテム表示

		SelectItem();	// 選んだアイテム表示
	}
	else
	{
		// 所持アイテム枠の位置を変更
		for (int nCntQuota = 0; nCntQuota < ITEMTYPE_MAX; nCntQuota++, pItemQuota++)
			SetPosition2DPolygon(pItemQuota->nIdxBox,vec3(SCREEN_WIDTH / ITEMTYPE_MAX * (nCntQuota + 0.5f), SCREEN_HEIGHT * 0.9f, 0.0f));

		// 提出枠を消す
		for (int nCntQuota = 0; nCntQuota < NUM_PUTOUTITEM; nCntQuota++, pPutQuota++)
		{
			SetColor2DPolygon(pPutQuota->nIdxBox, colX(0.8f, 0.8f, 0.8f, 0.0f));	// 消す
			pPutQuota->nType = -1;
			pPutQuota->bUse = false;
		}

		P_ITEMQUOTA pItemQuota = &g_aItemQuota[0];		// ポインタを先頭に戻す
		if (GetEnableUImenu())
		{
			// UI表示中、色をつける
			for (int nCntQuota = 0; nCntQuota < ITEMTYPE_MAX; nCntQuota++, pItemQuota++)
				SetColor2DPolygon(pItemQuota->nIdxBox, colX_WHITE);
		}
		else
		{
			// UI非表示中、透明にする
			for (int nCntQuota = 0; nCntQuota < ITEMTYPE_MAX; nCntQuota++, pItemQuota++)
				SetColor2DPolygon(pItemQuota->nIdxBox, colX_ZERO);
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
			if ((pItem->bGirl && GetReadyCamera() == CAMERATYPE_PLAYER_ONE) || (pItem->bMouse && GetReadyCamera() == CAMERATYPE_PLAYER_TWO))
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
// 当たり判定
//=========================================================================================
void CollisionItem(vec3 pos, float fRange)
{
	//**************************************************************
	// 変数宣言
	P_ITEM pItem = GetItem();					// 先頭アドレス
	P_ITEMQUOTA	pItemQuota = &g_aItemQuota[0];

	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++, pItem++)
	{
		if (pItem->bUse && pItem->bGet == false)
		{
			// 距離の計算
			float fDistX = SQUARE(pItem->pos.x - pos.x);
			float fDistY = SQUARE(pItem->pos.y - pos.y);
			float fDistZ = SQUARE(pItem->pos.z - pos.z);
			float fDist = sqrtf(__ABSOLUTE(SQUARE(pItem->pos.x - pos.x) + SQUARE(pItem->pos.y - pos.y) + SQUARE(pItem->pos.z - pos.z)));

			if (fDist <= pItem->fRange + fRange)
			{
				for (int nCntQuota = 0; nCntQuota < ITEMTYPE_MAX; nCntQuota++,pItemQuota++)
				{
					if (pItemQuota->bUse == false)
					{
						pItemQuota->bUse = true;
						pItemQuota->nType = (int)pItem->type;
						pItem->nIdxQuota = nCntQuota;
						break;
					}
				}
				pItem->bGet = true;
				break;
			}
		}
	}
}

//=========================================================================================
// 提出処理
void PutOut(void)
{
	P_ITEMQUOTA pItemQuota = &g_aItemQuota[0];

	for (int nCntQuota = 0; nCntQuota < ITEMTYPE_MAX; nCntQuota++, pItemQuota++)
	{
		// 位置を更新
		SetPosition2DPolygon(pItemQuota->nIdxBox, vec3(SCREEN_WIDTH / ITEMTYPE_MAX * (nCntQuota + 0.5f), SCREEN_HEIGHT * 0.5f, 0.0f));

		if (pItemQuota->bUse)
		{// アイテムが登録されている
			if (pItemQuota->nIdxBox == g_nSelectPut)
			{// 選択中なら
				// 枠内のアイテムを動かす
				SetColor2DPolygon(pItemQuota->nIdxBox, colX(1.0f, 0.8f, 0.8f, 0.5f));// 枠を明るく				
			}
			else
			{
				SetColor2DPolygon(pItemQuota->nIdxBox, colX(0.8f, 0.8f, 0.8f, 0.5f));// 枠を明るく				
			}
		}
		else
		{// されてない
			SetColor2DPolygon(pItemQuota->nIdxBox, colX(0.3f, 0.3f, 0.3f, 0.3f));// 枠を薄暗く
		}
	}
	// アイテムを選ぶ
	if (GetKeyboardTrigger(DIK_D) || GetKeyboardTrigger(DIK_RIGHT))
	{
		g_nSelectPut++;
		if (ITEMTYPE_MAX < g_nSelectPut)
		{
			g_nSelectPut = -1;
		}
	}

	if (GetKeyboardTrigger(DIK_A) || GetKeyboardTrigger(DIK_LEFT))
	{
		g_nSelectPut--;
		if (g_nSelectPut < -1)
		{
			g_nSelectPut = ITEMTYPE_MAX;
		}
	}	

	PrintDebugProc("\nSelect : %d\n", g_nSelectPut);
}

//=========================================================================================
// アイテム選択
void SelectItem(void)
{
	P_ITEMQUOTA pItemQuota = &g_aItemQuota[0];

	P_ITEMQUOTA pPutQuota = &g_aPutQuota[0];

	// 選択中のアイテムを表示
	for (int nCntQuota = 0; nCntQuota < NUM_PUTOUTITEM; nCntQuota++, pPutQuota++)
	{
		// 位置を更新
		SetPosition2DPolygon(pPutQuota->nIdxBox, vec3(SCREEN_WIDTH / NUM_PUTOUTITEM * (nCntQuota + 0.5f), SCREEN_HEIGHT * 0.4f, 0.0f));

		if (pPutQuota->bUse)
		{// アイテムが選ばれた枠
			// 未確定の枠
			SetColor2DPolygon(pPutQuota->nIdxBox, colX(0.8f, 0.8f, 0.8f, 0.8f));	// 枠を明るく
		}
		else
		{
			// 未確定の枠
			SetColor2DPolygon(pPutQuota->nIdxBox, colX(0.6f, 0.6f, 0.6f, 0.8f));	// 枠を暗く
		}
	}

	// 選択処理
	if (GetKeyboardTrigger(DIK_RETURN))
	{
		// アイテムが入っていたら
		if (g_aItemQuota[g_nSelectPut].bUse == true)
		{
			for (int nCnt = 0; nCnt < NUM_PUTOUTITEM; nCnt++)
			{
				// 入れていない枠に
				if (g_aPutQuota[nCnt].bUse == false)
				{
					g_aPutQuota[nCnt].nType = g_aItemQuota[g_nSelectPut].nType;
					g_aPutOut[nCnt] = (ITEMTYPE)g_aPutQuota[nCnt].nType;
					g_aPutQuota[nCnt].bUse = true;
					break;
				}
			}
		}
		else
		{// アイテム以外（決定ボタン）を選択していたら
			JudgmentEnding(&g_aPutOut[0],5);
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
// 先頭アドレス取得
P_ITEM GetItem(void)
{
	return &g_aItem[0];
}

//=========================================================================================
// アイテム提出フラグを切り替え
//=========================================================================================
void EnableItemPut(void)
{
	g_bPutOut = g_bPutOut ^ 1;
}