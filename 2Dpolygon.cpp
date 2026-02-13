//================================================================================================================
//
// DirectXの2Dポリゴン表示処理 [2Dpolygon.cpp]
// Author : TENMA
//
//================================================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "2Dpolygon.h"
#include "mathUtil.h"
#include "Texture.h"

//*************************************************************************************************
//*** マクロ定義 ***
//*************************************************************************************************
#define MAX_POLYGON			(256)		// 出現最大数

//*************************************************************************************************
//*** ポリゴン構造体の定義 ***
//*************************************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;		// 位置
	D3DXVECTOR3 rot;		// 向き
	float fWidth;			// 横幅
	float fSize;			// 縦もしくは横幅
	int nIdTexture;			// テクスチャ番号
	D3DXCOLOR col;			// 色
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff;	// 頂点バッファのポインタ
	bool bUse;
}_2DPOLYGON, *LP2DPOLYGON;

//*************************************************************************************************
//*** グローバル変数 ***
//*************************************************************************************************
_2DPOLYGON g_a2DPolygon[MAX_POLYGON];				// ポリゴンの情報

//================================================================================================================
// --- ポリゴンの初期化処理 ---
//================================================================================================================
void Init2DPolygon(void)
{
	ZeroMemory(&g_a2DPolygon[0], sizeof(_2DPOLYGON) * MAX_POLYGON);
}

//================================================================================================================
// --- ポリゴンの終了処理 ---
//================================================================================================================
void Uninit2DPolygon(void)
{
	LP2DPOLYGON p2DPoly = &g_a2DPolygon[0];

	for (int nCntRelease = 0; nCntRelease < MAX_POLYGON; nCntRelease++, p2DPoly++)
	{
		/*** 頂点バッファの破棄 ***/
		RELEASE(p2DPoly->pVtxBuff);
	}
}

//================================================================================================================
// --- ポリゴンの更新処理 ---
//================================================================================================================
void Update2DPolygon(void)
{
	
}

//================================================================================================================
// --- ポリゴンの描画処理 ---
//================================================================================================================
void Draw2DPolygon(void)
{
	/*** デバイスの取得 ***/
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	LP2DPOLYGON p2DPoly = &g_a2DPolygon[0];

	for (int nCntPoly = 0; nCntPoly < MAX_POLYGON; nCntPoly++, p2DPoly++)
	{
		if (p2DPoly->bUse == true)
		{
			/*** 頂点バッファをデータストリームに設定 ***/
			pDevice->SetStreamSource(0, p2DPoly->pVtxBuff, 0, sizeof(VERTEX_2D));

			/*** 頂点フォーマットの設定 ***/
			pDevice->SetFVF(FVF_VERTEX_2D);

			/*** テクスチャの設定 ***/
			pDevice->SetTexture(0, GetTexture(p2DPoly->nIdTexture));

			/*** ポリゴンの描画 ***/
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,		// プリミティブの種類
				0,											// 描画する最初の頂点インデックス
				2);											// 描画するプリミティブの数
		}
	}

	EndDevice();
}

//================================================================================================================
// --- ポリゴンの設置処理 ---
//================================================================================================================
int Set2DPolygon(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR2 size, int nIdTexture, D3DXCOLOR col)
{
	/*** デバイスの取得 ***/
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	LP2DPOLYGON p2DPoly = &g_a2DPolygon[0];
	VERTEX_2D *pVtx;					// 頂点情報へのポインタ

	for (int nCntPoly = 0; nCntPoly < MAX_POLYGON; nCntPoly++, p2DPoly++)
	{
		if (p2DPoly->bUse != true)
		{
			p2DPoly->pos = pos;
			p2DPoly->fWidth = size.x;
			p2DPoly->fSize = size.y;
			p2DPoly->nIdTexture = nIdTexture;
			p2DPoly->col = col;
			p2DPoly->bUse = true;

			if (p2DPoly->pVtxBuff == NULL)
			{
				/*** 頂点バッファの生成 ***/
				pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
					D3DUSAGE_WRITEONLY,
					FVF_VERTEX_2D,
					D3DPOOL_MANAGED,
					&p2DPoly->pVtxBuff,
					NULL);
			}

			/*** 頂点バッファの設定 ***/
			p2DPoly->pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			/*** 頂点座標の設定 ***/
			MyMathUtil::SetPolygonPos(pVtx, pos, size);

			/*** 座標変換用係数の設定 ***/
			MyMathUtil::SetPolygonRHW(pVtx);

			/*** 頂点カラー設定 ***/
			MyMathUtil::SetDefaultColor<VERTEX_2D>(pVtx, col);

			/*** テクスチャ設定 ***/
			MyMathUtil::SetDefaultTexture<VERTEX_2D>(pVtx);

			/*** 頂点バッファの設定を終了 ***/
			p2DPoly->pVtxBuff->Unlock();

			EndDevice();

			return nCntPoly;
		}
	}
	
	EndDevice();

	return -1;
}

//================================================================================================================
// --- ポリゴンの移動処理 ---
//================================================================================================================
void SetPosition2DPolygon(int nId2DPolygon, D3DXVECTOR3 pos)
{
	if (nId2DPolygon == -1 || nId2DPolygon >= (sizeof g_a2DPolygon / sizeof(_2DPOLYGON))) return;
	LP2DPOLYGON p2DPoly = &g_a2DPolygon[nId2DPolygon];
	VERTEX_2D *pVtx;					// 頂点情報へのポインタ

	p2DPoly->pos = pos;

	/*** 頂点バッファの設定 ***/
	p2DPoly->pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	/*** 頂点座標の設定 ***/
	pVtx[0].pos.x = pos.x - (p2DPoly->fWidth * 0.5f);
	pVtx[0].pos.y = pos.y - (p2DPoly->fSize * 0.5f);
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = pos.x + (p2DPoly->fWidth * 0.5f);
	pVtx[1].pos.y = pos.y - (p2DPoly->fSize * 0.5f);
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = pos.x - (p2DPoly->fWidth * 0.5f);
	pVtx[2].pos.y = pos.y + (p2DPoly->fSize * 0.5f);
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = pos.x + (p2DPoly->fWidth * 0.5f);
	pVtx[3].pos.y = pos.y + (p2DPoly->fSize * 0.5f);
	pVtx[3].pos.z = 0.0f;

	/*** 頂点バッファの設定を終了 ***/
	p2DPoly->pVtxBuff->Unlock();
}

//================================================================================================================
// --- ポリゴンの色を変更する処理 ---
//================================================================================================================
void SetColor2DPolygon(int nId2DPolygon, D3DXCOLOR col)
{
	if (nId2DPolygon == -1 || nId2DPolygon >= (sizeof g_a2DPolygon / sizeof(_2DPOLYGON))) return;
	LP2DPOLYGON p2DPoly = &g_a2DPolygon[nId2DPolygon];
	VERTEX_2D* pVtx;					// 頂点情報へのポインタ

	p2DPoly->col = col;

	/*** 頂点バッファの設定 ***/
	p2DPoly->pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	/*** 頂点座標の設定 ***/
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	/*** 頂点バッファの設定を終了 ***/
	p2DPoly->pVtxBuff->Unlock();
}

//================================================================================================================
// --- ポリゴンのサイズを変更する処理 ---
//================================================================================================================
void SetSize2DPolygon(int nId2DPolygon, D3DXVECTOR2 size)
{
	if (nId2DPolygon == -1 || nId2DPolygon >= (sizeof g_a2DPolygon / sizeof(_2DPOLYGON))) return;
	LP2DPOLYGON p2DPoly = &g_a2DPolygon[nId2DPolygon];
	VERTEX_2D* pVtx;					// 頂点情報へのポインタ

	p2DPoly->fWidth = size.x;
	p2DPoly->fSize = size.y;

	/*** 頂点バッファの設定 ***/
	p2DPoly->pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	/*** 頂点座標の設定 ***/
	pVtx[0].pos.x = p2DPoly->pos.x - (p2DPoly->fWidth * 0.5f);
	pVtx[0].pos.y = p2DPoly->pos.y - (p2DPoly->fSize * 0.5f);
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = p2DPoly->pos.x + (p2DPoly->fWidth * 0.5f);
	pVtx[1].pos.y = p2DPoly->pos.y - (p2DPoly->fSize * 0.5f);
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = p2DPoly->pos.x - (p2DPoly->fWidth * 0.5f);
	pVtx[2].pos.y = p2DPoly->pos.y + (p2DPoly->fSize * 0.5f);
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = p2DPoly->pos.x + (p2DPoly->fWidth * 0.5f);
	pVtx[3].pos.y = p2DPoly->pos.y + (p2DPoly->fSize * 0.5f);
	pVtx[3].pos.z = 0.0f;

	/*** 頂点バッファの設定を終了 ***/
	p2DPoly->pVtxBuff->Unlock();

}

//================================================================================================================
// --- ポリゴンの移動処理 ---
//================================================================================================================
void Destroy2DPolygon(int nIdx2DPolygon)
{
	if (nIdx2DPolygon == -1 || nIdx2DPolygon >= (sizeof g_a2DPolygon / sizeof(_2DPOLYGON))) return;
	LP2DPOLYGON p2DPoly = &g_a2DPolygon[nIdx2DPolygon];

	p2DPoly->bUse = false;
}