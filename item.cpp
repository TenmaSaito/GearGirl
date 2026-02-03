//==================================================================================================================================
//
//			アイテム処理 [item.cpp]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
//**************************************************************
// インクルード
#include "item.h"
#include "Texture.h"
#include "debugproc.h"
#include "input.h"

//**************************************************************
// グローバル変数宣言
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffItem = NULL;		// 頂点バッファへのポインタ
int g_nSetItemNum;									// 設置済みのアイテム数
Item g_aItem[MAX_ITEM];								// アイテム情報

//**************************************************************
// プロトタイプ宣言
void LoadItemSet(void);										// アイテムの配置情報を読み込み
void SetItem(vec3 pos, vec3 rot, ITEMTYPE type, int nTex);	// アイテム設置
void SaveItemSet(void);										// アイテムの配置情報を書き出し
P_ITEM GetItem(void);										// 先頭アドレス取得

//=========================================================================================
// アイテム初期化
//=========================================================================================
void InitItem(void)
{
	//**************************************************************
	// 変数宣言
	P_ITEM pItem = GetItem();
	LPDIRECT3DDEVICE9 pDevice = GetDevice();		// デバイスへのポインタ
	VERTEX_3D* pVtx;								// 頂点情報へのポインタ

	g_nSetItemNum = 0;

	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++, pItem++)
	{
		pItem->bGet = false;
		pItem->bUse = false;
	}

	// 頂点バッファの読み込み
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4 * MAX_ITEM,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffItem,
		NULL);

	//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffItem->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++, pVtx += 4)
	{
		// 頂点座標を設定
		pVtx[0].pos = vec3(-ITEM_RANGE,  ITEM_RANGE, 0.0f);
		pVtx[1].pos = vec3( ITEM_RANGE,  ITEM_RANGE, 0.0f);
		pVtx[2].pos = vec3(-ITEM_RANGE, -ITEM_RANGE, 0.0f);
		pVtx[3].pos = vec3( ITEM_RANGE, -ITEM_RANGE, 0.0f);

		// 法線ベクトルの設定
		pVtx[0].nor = vec3(0.0f, 0.0f, -1.0f);
		pVtx[1].nor = vec3(0.0f, 0.0f, -1.0f);
		pVtx[2].nor = vec3(0.0f, 0.0f, -1.0f);
		pVtx[3].nor = vec3(0.0f, 0.0f, -1.0f);

		// 頂点カラー設定
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャの座標設定
		pVtx[0].tex = vec2(0.0f, 0.0f);
		pVtx[1].tex = vec2(1.0f, 0.0f);
		pVtx[2].tex = vec2(0.0f, 1.0f);
		pVtx[3].tex = vec2(1.0f, 1.0f);
	}
	// 頂点バッファのロック解除
	g_pVtxBuffItem->Unlock();
	//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

	EndDevice();

	SetItem(vec3(0.0f, 140.0f, 100.0f), vec3_ZERO, ITEMTYPE_GEARL_FALSE,3);

}

//=========================================================================================
// アイテム終了
//=========================================================================================
void UninitItem(void)
{
	//**************************************************************
	// 頂点バッファの破棄
	if (g_pVtxBuffItem != NULL)
	{
		g_pVtxBuffItem->Release();
		g_pVtxBuffItem = NULL;
	}
}

//=========================================================================================
// アイテム更新
//=========================================================================================
void UpdateItem(void)
{
	//**************************************************************
	// 変数宣言
	P_ITEM pItem = GetItem();



}

//=========================================================================================
// アイテム描画
//=========================================================================================
void DrawItem(void)
{
	//**************************************************************
	// 変数宣言
	P_ITEM pItem = GetItem();
	LPDIRECT3DDEVICE9 pDevice = GetDevice();		// デバイスへのポインタ
	D3DXMATRIX mtxView, mtxTrans;					// マトリックス計算用

	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++, pItem++)
	{
		if (pItem->bUse)
		{
			//**************************************************************
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&pItem->mtxWorld);

			// ビューマトリックスを取得
			pDevice->GetTransform(D3DTS_VIEW, &mtxView);

			//**************************************************************
			// 向きを反映
			// ポリゴンをカメラに対して正面に向ける

			// D3DXMatrixInverse(&g_aBullet[nCntBullet].mtxWorld, NULL, &mtxView);　// 全部反対に向ける
			// 下の９行はInVerseの内容
			// Y軸
			pItem->mtxWorld._11 = mtxView._11;
			pItem->mtxWorld._12 = mtxView._21;
			pItem->mtxWorld._13 = mtxView._31;

			//// X軸　->　そのまま
			//pItem->mtxWorld._21 = mtxView._12;
			//pItem->mtxWorld._22 = mtxView._22;
			//pItem->mtxWorld._23 = mtxView._32;

			// Z軸
			pItem->mtxWorld._31 = mtxView._13;
			pItem->mtxWorld._32 = mtxView._23;
			pItem->mtxWorld._33 = mtxView._33;

			//// pos追従
			//pItem->mtxWorld._41 = 0.0f;
			//pItem->mtxWorld._42 = 0.0f;
			//pItem->mtxWorld._43 = 0.0f;

			//**************************************************************
			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, pItem->pos.x, pItem->pos.y, pItem->pos.z);
			D3DXMatrixMultiply(&pItem->mtxWorld, &pItem->mtxWorld, &mtxTrans);

			//**************************************************************
			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &pItem->mtxWorld);

			//**************************************************************
			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0,
				g_pVtxBuffItem,
				0,
				sizeof(VERTEX_3D));

			////$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
			//// Zテストを無効にする
			//pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
			//pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

			// Alphaテストを無効

			//**************************************************************
			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			//**************************************************************
			// テクスチャの設定
			pDevice->SetTexture(0, GetTexture(pItem->nIdxTex));

			//**************************************************************
			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

			// Aplphaテストを有効

			//// Zテストを有効にする
			//pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
			//pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			////$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
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
	P_ITEM pItem = GetItem();		// 先頭アドレス


	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++, pItem++)
	{
		if (pItem->bUse && pItem->bGet == false)
		{
			// 距離の計算
			float fDist = sqrtf(SQUARE(pItem->pos.x - pos.x) + SQUARE(pItem->pos.y - pos.y) + SQUARE(pItem->pos.z - pos.z));
			
			if (fDist <= pItem->fRange + fRange)
			{
				pItem->bGet = false;
				break;
			}
		}
	}

}

//=========================================================================================
// 単体アイテム設置
void SetItem(vec3 pos, vec3 rot, ITEMTYPE type, int nTex)
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
				pItem->type = type;
				pItem->nIdxTex = nTex;
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

}

//=========================================================================================
// 先頭アドレス取得
P_ITEM GetItem(void)
{
	return &g_aItem[0];
}
