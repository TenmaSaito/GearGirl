//==================================================================================
//
// DirectXのダイアログのソースファイル [dialog.cpp]
// Author : TENMA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "meshOrbit.h"

#include "mathUtil.h"
#include "MyInline.inl"
USE_UTIL;

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define MAX_ORBIT		(32)		// 同時設置可能なオービットの最大数

//**********************************************************************************
//*** メッシュオービット構造体 ***
//**********************************************************************************
typedef struct
{
	D3DXVECTOR3 aPos[MAX_ORB_ARRAY];	// 位置
	D3DXVECTOR3 aPosOffset[2];			// オフセット座標
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff;	// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 pIdxBuff;	// インデックスバッファ
	D3DXMATRIX *pMtxParent;				// 親マトリックスへのポインタ
	IDX_TEXTURE tex;					// テクスチャインデックス
	int nNumVertices;					// オービットのメッシュ数
	bool bUse;							// 使用状況
	bool bEnable;						// 描画状態
} MeshOrbit;

typedef MeshOrbit *LPMESHORBIT;

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************
LPMESHORBIT GetOrbit(IDX_MESHORBIT Idx = 0);
void AddOrbit(LPMESHORBIT pOrbit);
void SlideOrbit(D3DXVECTOR3 *pOrbitPos, size_t orbitSize);

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
MeshOrbit g_aOrbit[64];		// オービットの情報

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitMeshOrbit(void)
{
	// 初期化
	AutoZeroMemory(g_aOrbit);
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitMeshOrbit(void)
{
	LPMESHORBIT pOrbit = GetOrbit();

	for (int nCntOrbit = 0; nCntOrbit < MAX_ORBIT; nCntOrbit++, pOrbit++)
	{
		RELEASE(pOrbit->pVtxBuff);
		RELEASE(pOrbit->pIdxBuff);
	}
}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateMeshOrbit(void)
{

}

//==================================================================================
// --- 描画 ---
//==================================================================================
void DrawMeshOrbit(void)
{
	// デバイス取得
	AUTODEVICE9 AD9;
	LPDIRECT3DDEVICE9 pDevice = AD9.pDevice;
	VERTEX_3D* pVtx;
	LPMESHORBIT pOrbit = GetOrbit();

	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// アルファブレンディングを加算合成に設定！
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	for (int nCntOrbit = 0; nCntOrbit < MAX_ORBIT; nCntOrbit++, pOrbit++)
	{
		// 描画チェック
		if (pOrbit->bEnable NAND pOrbit->bUse) continue;
		
		// 埋める
		SlideOrbit(&pOrbit->aPos[0], sizeof(D3DXVECTOR3) * pOrbit->nNumVertices);

		D3DXVec3TransformCoord(&pOrbit->aPos[0], &pOrbit->aPosOffset[0], pOrbit->pMtxParent);
		D3DXVec3TransformCoord(&pOrbit->aPos[1], &pOrbit->aPosOffset[1], pOrbit->pMtxParent);

		pOrbit->pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntVtx = 0; nCntVtx < pOrbit->nNumVertices; nCntVtx++)
		{
			pVtx->pos = pOrbit->aPos[nCntVtx];
			pVtx++;
		}

		pOrbit->pVtxBuff->Unlock();

		D3DXMATRIX mtxWorld;

		D3DXMatrixIdentity(&mtxWorld);
		pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);

		/*** 頂点バッファをデータストリームに設定 ***/
		pDevice->SetStreamSource(0, pOrbit->pVtxBuff, 0, sizeof(VERTEX_3D));

		/*** 頂点フォーマットの設定 ***/
		pDevice->SetFVF(FVF_VERTEX_3D);

		/*** テクスチャの設定 ***/
		HRESULT hr = pDevice->SetTexture(0,GetTexture(pOrbit->tex));
		hr = pDevice->SetIndices(pOrbit->pIdxBuff);

		pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,
			0,
			0,
			pOrbit->nNumVertices,
			0,
			pOrbit->nNumVertices - 2);
	}

	// アルファブレンディングを元に戻す！！(重要！)
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	pDevice->SetRenderState(D3DRS_LIGHTING, true);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//==================================================================================
// --- オービット作成 ---
//==================================================================================
void AddOrbit(LPMESHORBIT pOrbit)
{
	// デバイス取得
	AUTODEVICE9 AD9;
	LPDIRECT3DDEVICE9 pDevice = AD9.pDevice;
	VERTEX_3D* pVtx;

	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * pOrbit->nNumVertices,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&pOrbit->pVtxBuff,
		NULL);

	pOrbit->pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntVtx = 0; nCntVtx < pOrbit->nNumVertices; nCntVtx++, pVtx++)
	{
		pVtx->nor = D3DXVECTOR3(0, 0, -1);
		pVtx->col = D3DXCOLOR_NULL;
		pVtx->tex = D3DXVECTOR2(1.0f * (nCntVtx / 2), 1.0f * (nCntVtx % 2));
	}

	pOrbit->pVtxBuff->Unlock();

	pDevice->CreateIndexBuffer(sizeof(WORD) * pOrbit->nNumVertices,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&pOrbit->pIdxBuff,
		NULL);

	WORD* pIdx;							// インデックス情報へのポインタ

	/*** インデックスバッファをロックし、頂点番号データを取得 ***/
	pOrbit->pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	/*** 頂点番号データの設定 ***/
	for (int nIdx = 0; nIdx < pOrbit->nNumVertices; nIdx++, pIdx++)
	{
		*pIdx = nIdx;
	}

	/*** インデックスバッファをアンロック ***/
	pOrbit->pIdxBuff->Unlock();
}

//==================================================================================
// --- 位置更新 ---
//==================================================================================
void SlideOrbit(D3DXVECTOR3 *pOrbit, size_t orbitSize)
{
	size_t copySize = orbitSize - (sizeof(D3DXVECTOR3) * 2);

	memcpy(&pOrbit[2], &pOrbit[0], copySize);
}

//==================================================================================
// --- 設置 ---
//==================================================================================
IDX_MESHORBIT SetOrbit(D3DXVECTOR3 offset0, D3DXVECTOR3 offset1, D3DXMATRIX* pMtx, UINT nNumVertices)
{
	LPMESHORBIT pOrbit = GetOrbit();
	int nError = -1;

	for (int nCntOrbit = 0; nCntOrbit < MAX_ORBIT; nCntOrbit++, pOrbit++)
	{
		if (pOrbit->bUse) continue;

		pOrbit->nNumVertices = nNumVertices;
		pOrbit->aPosOffset[0] = offset0;
		pOrbit->aPosOffset[1] = offset1;

		// 位置をオフセットに適用
		for (int nCntOrbit = 0; nCntOrbit < nNumVertices; nCntOrbit++)
		{
			pOrbit->aPos[nCntOrbit] = offset0;

			nCntOrbit++;

			pOrbit->aPos[nCntOrbit] = offset1;
		}

		// 親マトリックスを保存
		pOrbit->pMtxParent = pMtx;
		pOrbit->bUse = true;

		nError = nCntOrbit;		// 設定に成功したインデックスを保存
		break;
	}

	return nError;
}

//==================================================================================
// --- 色設定 ---
//==================================================================================
void SetColorOrbit(IDX_MESHORBIT IdxMeshOrbit, D3DXCOLOR col)
{
	LPMESHORBIT pOrbit = GetOrbit(IdxMeshOrbit);
	if (pOrbit == NULL) return;
	if (!pOrbit->bUse) return;

	VERTEX_3D *pVtx;

	// 色を設定
	pOrbit->pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntVtx = 0; nCntVtx < pOrbit->nNumVertices; nCntVtx++, pVtx++)
	{
		pVtx->col = col;
	}

	pOrbit->pVtxBuff->Unlock();
}

//==================================================================================
// --- テクスチャ設定 ---
//==================================================================================
void SetTextureOrbit(IDX_MESHORBIT IdxMeshOrbit, IDX_TEXTURE tex)
{
	LPMESHORBIT pOrbit = GetOrbit(IdxMeshOrbit);
	if (pOrbit == NULL) return;
	if (!pOrbit->bUse) return;

	// テクスチャインデックスを保存
	pOrbit->tex = tex;
}

//==================================================================================
// --- 有効化設定 ---
//==================================================================================
void SetEnableOrbit(IDX_MESHORBIT IdxMeshOrbit, bool bEnable)
{
	LPMESHORBIT pOrbit = GetOrbit(IdxMeshOrbit);
	if (pOrbit == NULL) return;
	if (!pOrbit->bUse) return;

	// 設定を保存
	pOrbit->bEnable = bEnable;
}

//==================================================================================
// --- オフセット更新 ---
//==================================================================================
void SetOffSetOrbit(IDX_MESHORBIT IdxMeshOrbit, D3DXVECTOR3 offset0, D3DXVECTOR3 offset1)
{
	LPMESHORBIT pOrbit = GetOrbit(IdxMeshOrbit);
	if (pOrbit == NULL) return;
	if (!pOrbit->bUse) return;

	// オフセットを保存
	pOrbit->aPosOffset[0] = offset0;
	pOrbit->aPosOffset[1] = offset1;
}

//==================================================================================
// --- 親子関係更新 ---
//==================================================================================
void SetParentOrbit(IDX_MESHORBIT IdxMeshOrbit, D3DXMATRIX* pMtx)
{
	LPMESHORBIT pOrbit = GetOrbit(IdxMeshOrbit);
	if (pOrbit == NULL) return;
	if (!pOrbit->bUse) return;

	// 親マトリックスへのポインタを保存
	pOrbit->pMtxParent = pMtx;
}

//==================================================================================
// --- 破棄 ---
//==================================================================================
void DestroyOrbit(IDX_MESHORBIT IdxMeshOrbit)
{
	LPMESHORBIT pOrbit = GetOrbit(IdxMeshOrbit);
	if (pOrbit == NULL) return;
	if (!pOrbit->bUse) return;

	// バッファを解放
	RELEASE(pOrbit->pVtxBuff);
	RELEASE(pOrbit->pIdxBuff);

	// 未使用状態に設定
	pOrbit->bUse = false;
	pOrbit->bEnable = false;
}

//==================================================================================
// --- 取得 ---
//==================================================================================
LPMESHORBIT GetOrbit(IDX_MESHORBIT Idx)
{
	if (FAILED(CheckIndex(MAX_ORBIT, Idx))) return NULL;	// インデックス外

	return &g_aOrbit[Idx];
}