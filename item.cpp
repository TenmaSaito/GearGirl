//==================================================================================================================================
//
//			アイテム処理 [item.cpp]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
//**************************************************************
// インクルード
#include "item.h"

#include "2Dpolygon.h"
#include "camera.h"
#include "debugproc.h"
#include "fade.h"
#include "game.h"
#include "input.h"
#include "judgeEnd.h"
#include "mathUtil.h"
#include "mesh.h"
#include "particle.h"
#include "player.h"
#include "prompt.h"
#include "Texture.h"
#include "timer.h"
#include "UImenu.h"
#include "itemEffector.h"

using  namespace MyMathUtil;

//**************************************************************
// マクロ定義

//==============================================================
// アイテム配置構造体定義
//==============================================================
typedef struct ItemQuota
{
	vec3		pos;
	vec3		rot;
	colX		col;
	vec2		size;
	int			nType;			// 表示するアイテムの種類 もしくはテクスチャ番号
	int			nIdxBox;		// この構造体の番号
	int			nSave;			// 入力したアイテムの番号（一つのアイテムを複数回入力できないようにするため
	bool		bUse;			// 構造体を使用しているかどうか
}ItemQuota;
POINTER(ItemQuota, P_ITEMQUOTA);

//**************************************************************
// グローバル変数宣言
bool			g_bOnDebugItem = false;				// デバッグ中
int				g_nSetItemNum = 0;					// 設置済みのアイテム数
int				g_nSelectPut = -1;					// 提出時のカーソル
int				g_nChoisePut = -1;					// 選択中のカーソル
int				g_nItemQuotaFlameTex = -1;			// アイテム欄のフレームのテクスチャ番号
int				g_nCoutItemFram = 0;				// フレーム数カウント
bool			g_bPutOut = false;					// アイテムを提出するときtrue
Item			g_aItem[MAX_ITEM];					// アイテム情報
ItemQuota		g_aItemQuota[ITEMTYPE_MAX];			// 所持アイテムを表示する枠のインデックス
ItemQuota		g_aPutQuota[NUM_PUTOUTITEM];		// 提出アイテムを表示する枠のインデックス

ItemQuota		g_aPutOutUI[PUTOUTUI_MAX];			// 提出時　背景の暗転、決定やとりけしのUI等
ITEMTYPE		g_aPutOut[NUM_PUTOUTITEM];			// 提出したアイテム
ItemInfo		g_aItemInfo[ITEMTYPE_MAX] =
{
	{"data\\MODEL\\Item\\Screw.x",-1},				// [0] 新品ののねじ
	{"data\\MODEL\\Item\\GearSmall.x",-1},			// [1] 磨かれた小さな歯車
	{"data\\MODEL\\Item\\GearLarge.x",-1},			// [2] 綺麗な大きい歯車
	{"data\\MODEL\\Item\\Shaft.x",-1},				// [3] まっすぐな軸
	{"data\\MODEL\\Item\\GearLarge.x",-1},			// [4] 丁寧に直されたぜんまい

	{"data\\MODEL\\Item\\old_screw.x",-1},			// [5] 古いねじ
	{"data\\MODEL\\Item\\GearSmallDificit.x",-1},	// [6] 錆びた小さい歯車
	{"data\\MODEL\\Item\\GearLargeDificit.x",-1},	// [7] 欠けた大きい歯車
	{"data\\MODEL\\Item\\ShaftCurv.x",-1},			// [8] 少しい曲がった軸
	{"data\\MODEL\\Item\\GearLarge.x",-1},			// [9] ゆがんだぜんまい
};
ItemInfo		g_aItemUIInfo[PUTOUTUI_MAX] =
{
	{NULL,-1},
	{"data\\TEXTURE\\putout.png",-1},
	{"data\\TEXTURE\\cancel.png",-1},
	{"data\\TEXTURE\\close.png",-1},
};

//**************************************************************
// プロトタイプ宣言
void UpdateMapItem(void);			// マップ上のアイテムを更新
void UpdatePouchItem(void);			// 取得済みのアイテムを更新
void OnUIitemEnable(P_ITEMQUOTA pQuota, int nMAX);	// 描画の簡略化のための関数わけ
void PutOut(void);					// 提出できるアイテムを表示
void SelectItem(void);				// 提出アイテムの選択

//=========================================================================================
// アイテム初期化
//=========================================================================================
void InitItem(void)
{
	//**************************************************************
	// 変数宣言
	P_ITEM		pItem = GetItem();
	P_ITEMQUOTA pItemQuota = &g_aItemQuota[0];
	P_ITEMQUOTA pPutQuota = &g_aPutQuota[0];
	P_ITEMQUOTA pUI = &g_aPutOutUI[0];

	g_bPutOut = false;		// アイテムを提出状態ではない
	g_nSelectPut = -1;		// 提出時のカーソル
	g_nChoisePut = -1;
	g_nCoutItemFram = 0;
	LoadTexture("data\\TEXTURE\\flame.png", &g_nItemQuotaFlameTex);

	// アイテム情報読込
	P_ITEMINFO	pItemInfo = &g_aItemInfo[0];
	for (int nCntItem = 0; nCntItem < ITEMTYPE_MAX; nCntItem++, pItemInfo++)
		LoadModelData(pItemInfo->pFile, &pItemInfo->nNumGet);

	// UIテクスチャ読込
	P_ITEMINFO pItemUI = &g_aItemUIInfo[0];
	for (int nCntUI = 0; nCntUI < PUTOUTUI_MAX; nCntUI++, pItemUI++)
	{
		if (pItemUI->pFile)
			LoadTexture(pItemUI->pFile, &pItemUI->nNumGet);
	}

	// アイテム配置初期化
	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++, pItem++)
	{
		pItem->bGet = false;
		pItem->bUse = false;
		pItem->nIdxQuota = -1;
	}

	// その他のUIを取得
	for (int nCntUI = 0; nCntUI < PUTOUTUI_MAX; nCntUI++, pUI++)
	{
		if (nCntUI == 0)
		{// 背景
			pUI->pos = vec3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.4f, 0.0f);
			pUI->size = vec2(SCREEN_WIDTH, SCREEN_HEIGHT * 0.8f);
			pUI->col = colX(0.5f, 0.5f, 0.5f, 0.3f);
		}
		else if (nCntUI == 1)
		{// 決定
			pUI->pos = vec3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.7f, 0.0f);
			pUI->size = vec2(SCREEN_WIDTH * 0.15f, SCREEN_WIDTH * 0.06f);
			pUI->col = colX(1.0f, 1.0f, 1.0f, 0.8f);
		}
		else if (nCntUI == 2)
		{// 解除
			pUI->pos = vec3(SCREEN_WIDTH * 0.3f, SCREEN_HEIGHT * 0.7f, 0.0f);
			pUI->size = vec2(SCREEN_WIDTH * 0.1f, SCREEN_WIDTH * 0.04f);
			pUI->col = colX(1.0f, 1.0f, 1.0f, 0.8f);
		}
		else
		{// やめる
			pUI->pos = vec3(SCREEN_WIDTH * 0.15f, SCREEN_HEIGHT * 0.7f, 0.0f);
			pUI->size = vec2(SCREEN_WIDTH * 0.1f, SCREEN_WIDTH * 0.04f);
			pUI->col = colX(1.0f, 1.0f, 1.0f, 0.8f);
		}
		pUI->nType = g_aItemUIInfo[nCntUI].nNumGet;
		pUI->nIdxBox = Set2DPolygon(pUI->pos, vec3_ZERO, pUI->size, pUI->nType, pUI->col);
		SetEnable2DPolygon(pUI->nIdxBox, false);
		pUI->bUse = false;
	}

	// 所持アイテムの枠を取得
	for (int nCntQuota = 0; nCntQuota < ITEMTYPE_MAX; nCntQuota++, pItemQuota++)
	{
		pItemQuota->pos = vec3(nCntQuota * SCREEN_WIDTH * 0.2f, SCREEN_HEIGHT * 0.75f, 0.0f);
		pItemQuota->col = colX_ZERO;
		pItemQuota->size = vec2(SCREEN_WIDTH * 0.08f, SCREEN_WIDTH * 0.08f);
		pItemQuota->nIdxBox = Set2DPolygon(pItemQuota->pos, vec3_ZERO, pItemQuota->size, g_nItemQuotaFlameTex, pItemQuota->col);
		SetEnable2DPolygon(pItemQuota->nIdxBox, false);
		pItemQuota->nType = -1;
		pItemQuota->bUse = false;
	}

	// 提出用の枠を取得
	for (int nCntQuota = 0; nCntQuota < NUM_PUTOUTITEM; nCntQuota++, pPutQuota++)
	{
		pPutQuota->pos = vec3_ZERO;
		pPutQuota->col = colX_ZERO;
		pPutQuota->size = vec2(SCREEN_WIDTH * 0.05f, SCREEN_WIDTH * 0.05f);
		pPutQuota->nIdxBox = Set2DPolygon(pPutQuota->pos, vec3_ZERO, pPutQuota->size, -1, pPutQuota->col);
		SetEnable2DPolygon(pPutQuota->nIdxBox, false);
		pPutQuota->nType = -1;
		pPutQuota->bUse = false;
		g_aPutOut[nCntQuota] = ITEMTYPE_SPRING_FALSE;
	}

	// 設置
	g_nSetItemNum = 0;	// 配置数を初期化
	g_bOnDebugItem = false;

#ifdef _DEBUG
	// デバッグ用　見た目だけ
	vec3 pos = vec3(450.0f, 120.0f, -600.0f);

	for (int nCnt = 0; nCnt < ITEMTYPE_MAX; nCnt++)
	{
		if (nCnt == (int)ITEMTYPE_SCREW_FALSE)
		{
			pos.x = 350.0f;
			pos.z = -600.0f;
		}

		// SetItem(pos, vec3_ZERO, (ITEMTYPE)nCnt, true, true, true);
		pos.z += 50.0f;
	}
#endif
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
	// 便利屋判定
	if (IsDispPrompt(GetIdxShopPrompt()) && g_bPutOut == false)
	{
		if (GetActivePlayer() == PLAYERTYPE_GIRL || 1 < GetNumPlayer())
		{
			if (GetKeyboardTrigger(DIK_RETURN) || GetKeyboardTrigger(DIK_F) || GetJoypadTrigger(PLAYERTYPE_GIRL, JOYKEY_A))
			{
				g_bPutOut = true;
				g_nCoutItemFram = 10;
			}
		}

		if (GetKeyboardTrigger(DIK_BACK) || GetJoypadTrigger(0, JOYKEY_B))
		{
			g_bPutOut = false;
		}
		if (g_bPutOut == false)
		{// 提出がオフになったら初期化
			g_nSelectPut = 0;
			g_nChoisePut = -1;
		}
	}

	// マップアイテム更新
	UpdateMapItem();

	// ポーチアイテム更新
	UpdatePouchItem();

	if(0 < g_nCoutItemFram)
	g_nCoutItemFram--;
#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_F3))
	{
		g_bOnDebugItem = g_bOnDebugItem ^ 1;
		g_bPutOut = g_bOnDebugItem;
	}
#endif
}

//=========================================================================================
// マップアイテム更新
void UpdateMapItem(void)
{
	//**************************************************************
	// 変数宣言
	P_ITEM pItem = GetItem();

	for (int nCntItem = 0; nCntItem < ITEMTYPE_MAX; nCntItem++, pItem++)
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

				// アイテムを囲むようにシリンダーを描画
				SetMeshCylinder(pItem->pos, VECNULL, D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.4f), 3.0f, 1000.0f, 1, 8, D3DCULL_CCW, -1, false);

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
	if (g_bPutOut && g_nCoutItemFram <= 0)
	{
		PutOut();		// 取得済みアイテム表示

		SelectItem();	// 選んだアイテム表示

		AddTimer(1);	// タイマー停止
	}
	else
	{// 提出フラグが消えたら

		// アイテムを選ぶ
		if (GetKeyboardTrigger(DIK_D) || GetKeyboardTrigger(DIK_RIGHT)
			|| GetJoypadRepeat(0, JOYKEY_RIGHT) || GetJoypadRepeat(0, JOYKEY_LEFT_STICK_RIGHT))
		{
			g_nSelectPut++;
			if (ITEMTYPE_MAX - 1 < g_nSelectPut)
			{
				g_nSelectPut = 0;
			}
		}
		if (GetKeyboardTrigger(DIK_A) || GetKeyboardTrigger(DIK_LEFT)
			|| GetJoypadRepeat(0, JOYKEY_LEFT) || GetJoypadRepeat(0, JOYKEY_LEFT_STICK_LEFT))
		{
			g_nSelectPut--;
			if (g_nSelectPut < 0)
			{
				g_nSelectPut = ITEMTYPE_MAX - 1;
			}
		}

		// 所持アイテム枠の位置を変更
		for (int nCntQuota = 0; nCntQuota < ITEMTYPE_MAX; nCntQuota++, pItemQuota++)
		{
			pItemQuota->pos = vec3(SCREEN_WIDTH * (0.5f - 0.12f * g_nSelectPut) + SCREEN_WIDTH * 0.12f * nCntQuota, SCREEN_HEIGHT * 0.75f, 0.0f);
			SetPosition2DPolygon(pItemQuota->nIdxBox, pItemQuota->pos);
		}

		// 選択した分を初期化・透明化
		for (int nCntQuota = 0; nCntQuota < NUM_PUTOUTITEM; nCntQuota++, pPutQuota++)
		{
			SetEnable2DPolygon(pPutQuota->nIdxBox, false);	// 消す
			pPutQuota->nType = -1;
			pPutQuota->nSave = -1;
			pPutQuota->bUse = false;
		}

		// UIの透明化
		P_ITEMQUOTA pUI = &g_aPutOutUI[0];
		for (int nCntUI = 0; nCntUI < PUTOUTUI_MAX; nCntUI++, pUI++)
			SetEnable2DPolygon(pUI->nIdxBox, false);

		// アイテムポーチを元に戻す
		P_ITEMQUOTA pItemQuota = &g_aItemQuota[0];		// ポインタを先頭に戻す
		if (GetEnableUImenu())
		{
			// メニュー表示中、色をつける
			// アイテム欄
			for (int nCntQuota = 0; nCntQuota < ITEMTYPE_MAX; nCntQuota++, pItemQuota++)
			{
				SetColor2DPolygon(pItemQuota->nIdxBox, colX(1.0f, 1.0f, 1.0f, 0.3f));
				SetEnable2DPolygon(pItemQuota->nIdxBox, true);
			}
		}
		else
		{
			// UI非表示中、透明にする
			for (int nCntQuota = 0; nCntQuota < ITEMTYPE_MAX; nCntQuota++, pItemQuota++)
				SetEnable2DPolygon(pItemQuota->nIdxBox, false);
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
	D3DXMATERIAL* pMat;							// マテリアルデータへのポインタ

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
// UI表示用のアイテム描画
//=========================================================================================
void DrawUIItem(void)
{
	// 提出処理
	if (g_bPutOut)
	{
		OnUIitemEnable(&g_aPutQuota[0], NUM_PUTOUTITEM);
		OnUIitemEnable(&g_aItemQuota[0], ITEMTYPE_MAX);
	}
	// 所持アイテムの確認
	else if (GetEnableUImenu())
	{
		OnUIitemEnable(&g_aItemQuota[0], ITEMTYPE_MAX);
	}
}

//=========================================================================================
// UI表示用 描画の簡略化のための関数わけ
//=========================================================================================
void OnUIitemEnable(P_ITEMQUOTA pQuota, int nMAX)
{
	D3DXMATRIX			mtxView, mtxRot, mtxTrans, mtxWorld;	// マトリックス計算用
	PMODELDATA			pModel;						// モデルデータへのポインタ
	LPDIRECT3DDEVICE9	pDevice = GetDevice();		// デバイスへのポインタ
	D3DMATERIAL9		matDef;						// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;						// マテリアルデータへのポインタ
	vec3				pos = vec3_ZERO, rot = vec3_ZERO;

	SetEnableZFunction(pDevice, true);

	for (int nCntQuota = 0; nCntQuota < nMAX; nCntQuota++, pQuota++)
	{
		if (pQuota->bUse)
		{
			// カメラ設置
			SetUiCameraCenter(pQuota->pos, vec2(100.0f, 100.0f));

			//------------------------------
			// モデル設置	
			// インデックスからモデルデータを取得
			pModel = GetModelData(g_aItemInfo[pQuota->nType].nNumGet);

			//**************************************************************
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&mtxWorld);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, pQuota->rot.y, pQuota->rot.x, pQuota->rot.z);
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);

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
	EndDevice();
}

//=========================================================================================
// 当たり判定
//=========================================================================================
void CollisionItem(vec3 pos, float fRange, int type)
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

			// 距離計算
			if (fDist <= pItem->fRange + fRange)
			{
				// 見えていないプレイヤーがあたっても取得できないように
				if (pItem->bGirl == false && (PlayerType)type == PLAYERTYPE_GIRL)
					continue;
				if (pItem->bMouse == false && (PlayerType)type == PLAYERTYPE_MOUSE)
					continue;

				// アイテム欄の空き状況を取得
				for (int nCntQuota = 0; nCntQuota < ITEMTYPE_MAX; nCntQuota++, pItemQuota++)
				{
					if (pItemQuota->bUse == false)
					{
						// アイテム欄に保存
						pItemQuota->bUse = true;
						pItemQuota->nType = (int)pItem->type;
						pItem->nIdxQuota = nCntQuota;
						break;
					}
				}

				// 取得処理
				pItem->bGet = true;
				SetParticle(pItem->pos, colX(0.5f, 0.1f, 0.1f, 0.3f), vec3(-0.5f, -0.5f, -0.5f), vec3(1.0f, 1.0f, 1.0f), 1, 5.0f, 20, 10, false);
				SetGetEffect(pItem->nIdxEffector);
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

	//*********************************************************
	// 提出アイテム欄
	for (int nCntQuota = 0; nCntQuota < ITEMTYPE_MAX; nCntQuota++, pItemQuota++)
	{
		SetEnable2DPolygon(pItemQuota->nIdxBox, true); // 描画
		// 位置を更新
		pItemQuota->pos = vec3(SCREEN_WIDTH * (0.5f - 0.2f * g_nSelectPut) + SCREEN_WIDTH * 0.2f * nCntQuota, SCREEN_HEIGHT * 0.6f, 0.0f);
		SetPosition2DPolygon(pItemQuota->nIdxBox, pItemQuota->pos);

		// アイテム欄
		if (nCntQuota == g_nSelectPut && g_nChoisePut < 0)
		{// 選択中なら
			pItemQuota->size = vec2(SCREEN_WIDTH * 0.1f, SCREEN_WIDTH * 0.1f);
			SetSize2DPolygon(pItemQuota->nIdxBox, pItemQuota->size);					// 少し拡大

			if (pItemQuota->bUse)
			{// アイテムが登録されている
				SetColor2DPolygon(pItemQuota->nIdxBox, colX(1.0f, 0.8f, 0.8f, 0.5f));	// 枠を赤	
			}
		}
		else
		{// 選択されていない
			pItemQuota->size = vec2(SCREEN_WIDTH * 0.08f, SCREEN_WIDTH * 0.08f);
			SetSize2DPolygon(pItemQuota->nIdxBox, pItemQuota->size);					// 元の大きさ

			if (pItemQuota->bUse)
			{//	アイテムあり
				SetColor2DPolygon(pItemQuota->nIdxBox, colX(0.8f, 0.8f, 0.8f, 0.5f));	// 枠を明るく			
			}
			else
			{
				SetColor2DPolygon(pItemQuota->nIdxBox, colX(0.3f, 0.3f, 0.3f, 0.3f));	// 枠を薄暗く
			}
		}
	}

	// カーソル左右移動
	if (GetKeyboardRepeat(DIK_D) || GetKeyboardRepeat(DIK_RIGHT)
		|| GetJoypadRepeat(0, JOYKEY_RIGHT) || GetJoypadRepeat(0, JOYKEY_LEFT_STICK_RIGHT))
	{
		// カーソルが上段にいたら
		if (-1 < g_nChoisePut && g_nChoisePut < NUM_PUTOUTITEM)
		{
			g_nChoisePut++;
			if (NUM_PUTOUTITEM <= g_nChoisePut)
			{
				g_nChoisePut = NUM_PUTOUTITEM - 1;
			}
		}
		else if (g_nChoisePut < 0)
		{// カーソルが中段なら
			g_nSelectPut++;
			if (ITEMTYPE_MAX - 1 < g_nSelectPut)
			{
				g_nSelectPut = 0;
			}
		}
		else
		{// カーソルが下段なら
			g_nChoisePut--;
			if (g_nChoisePut < NUM_PUTOUTITEM)
				g_nChoisePut = NUM_PUTOUTITEM;
		}
	}
	if (GetKeyboardRepeat(DIK_A) || GetKeyboardRepeat(DIK_LEFT)
		|| GetJoypadRepeat(0, JOYKEY_LEFT) || GetJoypadRepeat(0, JOYKEY_LEFT_STICK_LEFT))
	{
		// カーソルが上段にいたら
		if (-1 < g_nChoisePut && g_nChoisePut < NUM_PUTOUTITEM)
		{
			g_nChoisePut--;
			if (g_nChoisePut < 0)
			{
				g_nChoisePut = 0;
			}
		}
		else if (g_nChoisePut < 0)
		{// カーソルが中段なら
			g_nSelectPut--;
			if (g_nSelectPut < 0)
			{
				g_nSelectPut = ITEMTYPE_MAX - 1;
			}
		}
		else
		{// カーソルが下段なら
			g_nChoisePut++;
			if (NUM_PUTOUTITEM + 2 < g_nChoisePut)
				g_nChoisePut = NUM_PUTOUTITEM + 2;
		}
	}

	// カーソル上下移動
	if (GetKeyboardRepeat(DIK_W) || GetKeyboardRepeat(DIK_UP)
		|| GetJoypadRepeat(0, JOYKEY_UP) || GetJoypadRepeat(0, JOYKEY_LEFT_STICK_UP))
	{
		if (NUM_PUTOUTITEM <= g_nChoisePut)
			g_nChoisePut = -1;
		else if (g_nChoisePut == -1)
			g_nChoisePut = 2;
	}
	if (GetKeyboardRepeat(DIK_S) || GetKeyboardRepeat(DIK_DOWN)
		|| GetJoypadRepeat(0, JOYKEY_DOWN) || GetJoypadRepeat(0, JOYKEY_LEFT_STICK_DOWN))
	{
		if (g_nChoisePut < 0)
			g_nChoisePut = NUM_PUTOUTITEM;
		else if (g_nChoisePut < NUM_PUTOUTITEM)
			g_nChoisePut = -1;
	}

	PrintDebugProc("\nSelect : %d", g_nSelectPut);
	PrintDebugProc("\nChoisePut : %d\n", g_nChoisePut);
}

//=========================================================================================
// アイテム選択
void SelectItem(void)
{
	P_ITEMQUOTA pPutQuota = &g_aPutQuota[0];
	P_ITEMQUOTA pUI = &g_aPutOutUI[0];

	//*********************************************************
	// 所持アイテム欄
	for (int nCntQuota = 0; nCntQuota < NUM_PUTOUTITEM; nCntQuota++, pPutQuota++)
	{
		SetEnable2DPolygon(pPutQuota->nIdxBox, true);	// 描画

		// 位置を更新
		pPutQuota->pos = vec3(SCREEN_WIDTH / NUM_PUTOUTITEM * (nCntQuota + 0.5f), SCREEN_HEIGHT * 0.4f, 0.0f);
		SetPosition2DPolygon(pPutQuota->nIdxBox, pPutQuota->pos);

		// 選択中なら
		if (nCntQuota == g_nChoisePut)
			pPutQuota->size = vec2(SCREEN_WIDTH * 0.08f, SCREEN_WIDTH * 0.08f);
		else
			pPutQuota->size = vec2(SCREEN_WIDTH * 0.05f, SCREEN_WIDTH * 0.05f);
		SetSize2DPolygon(pPutQuota->nIdxBox, pPutQuota->size);

		if (pPutQuota->bUse)
		{// 選ばれたアイテムの枠
			pPutQuota->col = colX(0.8f, 0.8f, 0.8f, 0.8f);// 枠を明るく
		}
		else
		{// 未確定の枠
			pPutQuota->col = colX(0.6f, 0.6f, 0.6f, 0.8f);	// 枠を暗く
		}
		SetColor2DPolygon(pPutQuota->nIdxBox, pPutQuota->col);
	}

	//*********************************************************
	// 各UI（下段
	for (int nCntUI = 0; nCntUI < PUTOUTUI_MAX; nCntUI++, pUI++)
	{
		if (g_nChoisePut - PUTOUTUI_MAX == nCntUI && nCntUI != 0)
		{// 選択されている場合
			SetSize2DPolygon(pUI->nIdxBox, vec2(pUI->size.x * 1.2f, pUI->size.y * 1.2f));
			SetColor2DPolygon(pUI->nIdxBox, colX(1.0f, 1.0f, 1.0f, 1.0f));
		}
		else
		{// それ以外
			SetSize2DPolygon(pUI->nIdxBox, pUI->size);
			SetColor2DPolygon(pUI->nIdxBox, pUI->col);
		}

		SetEnable2DPolygon(pUI->nIdxBox, true);
	}

	//*********************************************************
	// 決定処理
	if (GetKeyboardTrigger(DIK_RETURN) || GetJoypadTrigger(0, JOYKEY_A))
	{
		// 上下のメニューが選択されていたら
		if (g_nChoisePut != -1)
		{
			if (PUTOUTUI_MAX < g_nChoisePut)
			{// 下段
				switch (g_nChoisePut - PUTOUTUI_MAX)
				{
				case 1:// 提出
					if (GetFade() != FADE_NONE) return; // フェード中なら無視

					JudgmentEnding(&g_aPutOut[0], 5);
					SetFade(MODE_RESULT);

					break;

				case 2:// 取り消し
					pPutQuota = &g_aPutQuota[0];

					for (int nCntQuota = 0; nCntQuota < NUM_PUTOUTITEM; nCntQuota++, pPutQuota++)
					{
						pPutQuota->nType = -1;
						pPutQuota->nSave = -1;
						pPutQuota->bUse = false;
					}

					break;

				case 3:// やめる
					g_bPutOut = false;
					break;

				default:
					break;
				}
			}
			else
			{// 上段

			}
		}
		else if (-1 < g_nSelectPut && g_nSelectPut < ITEMTYPE_MAX)
		{// アイテム枠なら
			// アイテムが入っていたら
			if (g_aItemQuota[g_nSelectPut].bUse == true)
			{
				for (int nCnt = 0; nCnt < NUM_PUTOUTITEM; nCnt++)
				{
					if (g_aPutQuota[nCnt].nSave == g_nSelectPut)
					{// すでに決定しているアイテムなら拒否
						// 選択中ですよ！
						break;
					}
					// 入れていない枠に
					else if (g_aPutQuota[nCnt].bUse == false)
					{
						g_aPutQuota[nCnt].nSave = g_nSelectPut;
						g_aPutQuota[nCnt].nType = g_aItemQuota[g_nSelectPut].nType;
						g_aPutOut[nCnt] = (ITEMTYPE)g_aPutQuota[nCnt].nType;
						g_aPutQuota[nCnt].bUse = true;
						break;
					}
				}
			}
		}
		else if (g_nSelectPut < 0)
		{// 選択取り消し
		}
		else
		{// それら以外を選択していたら決定と同じ処理
			if (GetFade() != FADE_NONE) return; // フェード中なら無視

			JudgmentEnding(&g_aPutOut[0], 5);

			if (GetTimer() <= 10)
				SetFade(MODE_RESULT);
		}
	}

	//*********************************************************
	// 取り消し
	if (GetKeyboardTrigger(DIK_BACK))
	{
		pPutQuota = &g_aPutQuota[0];

		for (int nCntQuota = 0; nCntQuota < NUM_PUTOUTITEM; nCntQuota++, pPutQuota++)
		{
			pPutQuota->nType = -1;
			pPutQuota->nSave = -1;
			pPutQuota->bUse = false;
		}
	}
}

//=========================================================================================
// 単体アイテム設置
IDX_ITEM SetItem(vec3 pos, vec3 rot, ITEMTYPE type, bool bReflectGirl, bool bReflectMouse, bool bColi)
{
	//**************************************************************
	// 変数宣言
	P_ITEM pItem = GetItem();
	IDX_ITEM error = -1;

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
				error = nCntItem;

				pItem->nIdxEffector = SetItemEffector(pos, D3DXCOLOR(0, 1, 0, 1));
				break;
			}
		}
	}

	return error;
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

//=========================================================================================
// アイテム提出フラグを取得
//=========================================================================================
bool IsEnableItemPut(void)
{
	return g_bPutOut;
}