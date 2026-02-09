//==================================================================================================================================
// 
//			メッシュリングの処理 [meshring.cpp]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
//**************************************************************
// インクルード
#include "mesh.h"
#include "Texture.h"

//**************************************************************
// マクロ定義
#define RINGRADIUS_INNER	size.x
#define RINGRADIUS_OUTER	size.z

//**************************************************************
// グローバル変数
MeshInfo				g_aMeshRing[MAX_MESHRING];		// メッシュリングの情報
int						g_nSetMeshRing;					// セット済みのリング数

//**************************************************************
// プロトタイプ宣言

//=========================================================================================
// メッシュリングの初期化処理
//=========================================================================================
void InitMeshRing(void)
{
	//**************************************************************
	// 変数宣言
	g_nSetMeshRing = 0;
	P_MESH pMesh = GetMeshRing();

	//**************************************************************
	// 位置・サイズの初期化
	for (int nCntMeshRing = 0; nCntMeshRing < MAX_MESHRING; nCntMeshRing++,pMesh++)
	{
		pMesh->pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pMesh->rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pMesh->pVtxBuff = NULL;
		pMesh->pIdxBuffer = NULL;
		pMesh->RINGRADIUS_INNER = 0.0f;
		pMesh->RINGRADIUS_OUTER = 0.0f;
		pMesh->nHeightDivision = 0;
		pMesh->nCircleDivision = 0;
		pMesh->nVerti = 0;
		pMesh->nPrim = 0;
		pMesh->bUse = false;
	}
}

//=========================================================================================
// メッシュリングの終了処理
//=========================================================================================
void UninitMeshRing(void)
{
	P_MESH pMesh = GetMeshRing();

	for (int nCntMeshRing = 0; nCntMeshRing < MAX_MESHRING; nCntMeshRing++,pMesh++)
	{
		//**************************************************************
		// 頂点バッファの破棄
		if (pMesh->pVtxBuff != NULL)
		{
			pMesh->pVtxBuff->Release();
			pMesh->pVtxBuff = NULL;
		}

		//**************************************************************
		// インデックスバッファの破棄
		if (pMesh->pIdxBuffer != NULL)
		{
			pMesh->pIdxBuffer->Release();
			pMesh->pIdxBuffer = NULL;
		}
	}
}

//=========================================================================================
// フィールド更新処理
//=========================================================================================
void UpdateMeshRing(void)
{

}

//=========================================================================================
// メッシュリングの描画処理
//=========================================================================================
void DrawMeshRing(void)
{
	//**************************************************************
	// 変数宣言
	LPDIRECT3DDEVICE9	pDevice = GetDevice();		// デバイスへのポインタ
	P_MESH				pMesh = GetMeshRing();		// メッシュ情報
	D3DXMATRIX			mtxRot, mtxTrans;			// マトリックス計算用

	for (int nCntMeshRing = 0; nCntMeshRing < MAX_MESHRING; nCntMeshRing++, pMesh++)
	{
		if (pMesh->bUse)
		{
			//**************************************************************
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&pMesh->mtxWorld);

			//**************************************************************
			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, pMesh->rot.y, pMesh->rot.x, pMesh->rot.z);
			D3DXMatrixMultiply(&pMesh->mtxWorld, &pMesh->mtxWorld, &mtxRot);

			//**************************************************************
			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, pMesh->pos.x, pMesh->pos.y, pMesh->pos.z);
			D3DXMatrixMultiply(&pMesh->mtxWorld, &pMesh->mtxWorld, &mtxTrans);

			//**************************************************************
			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &pMesh->mtxWorld);

			//**************************************************************
			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0,
				pMesh->pVtxBuff,
				0,
				sizeof(VERTEX_3D));

			// インデックスバッファを設定
			pDevice->SetIndices(pMesh->pIdxBuffer);

			//**************************************************************
			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			//**************************************************************
			// テクスチャの設定
			pDevice->SetTexture(0, GetTexture(pMesh->nIdxTexture));

			//**************************************************************
			// カリングモードの設定
			pDevice->SetRenderState(D3DRS_CULLMODE, pMesh->culling);

			//**************************************************************
			// フィールドの描画
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, pMesh->nVerti, 0, pMesh->nPrim);
		}
	}
	EndDevice();// デバイス取得終了
}

//=========================================================================================
// メッシュリングを設置
//=========================================================================================
void SetMeshRing(vec3 pos, vec3 rot, float fInner, float fOuter, int nHeightDivision, int nCircleDivision, D3DCULL cull, int nTex, bool bPat)
{
	//**************************************************************
	// 変数宣言
	LPDIRECT3DDEVICE9	pDevice = GetDevice();				// デバイスへのポインタ
	P_MESH				pMesh = GetMeshRing();				// メッシュリングポインタ
	VERTEX_3D*			pVtx;								// 頂点情報へのポインタ
	WORD*				pIdx;								// インデックス情報へのポインタ
	D3DXVECTOR3			vecDir;								// 法線ベクトル計算用
	float				fThick;								// 内径と外径の差
	int					nHeightVerti = nHeightDivision + 1,
						nCircleVerti = nCircleDivision + 1;	// 縦頂点数と横頂点数

	for (int nCntMeshRing = 0; nCntMeshRing < MAX_MESHRING; nCntMeshRing++, pMesh++)
	{
		if (pMesh->bUse == false)
		{
			// 値の保存
			pMesh->pos = pos;
			pMesh->rot = rot;
			pMesh->RINGRADIUS_INNER = fInner;
			pMesh->RINGRADIUS_OUTER = fOuter;
			pMesh->nHeightDivision = nHeightDivision;
			pMesh->nCircleDivision = nCircleDivision;
			float fAngle = (float)(2 * D3DX_PI / nCircleDivision);
			pMesh->nVerti = nHeightVerti * nCircleVerti;
			pMesh->nPrim = (nHeightDivision * (nCircleDivision + 2) - 2) * 2;
			pMesh->nIdxTexture = nTex;
			pMesh->bPattanrn = bPat;
			pMesh->culling = cull;

			//**************************************************************
			// 頂点バッファの読み込み
			pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * pMesh->nVerti,
				D3DUSAGE_WRITEONLY,
				FVF_VERTEX_3D,
				D3DPOOL_MANAGED,
				&pMesh->pVtxBuff,
				NULL);

			//**************************************************************
			// インデックスバッファの生成
			pDevice->CreateIndexBuffer(sizeof(WORD) * 2 * (nHeightDivision * (nCircleDivision + 2) - 1),
				D3DUSAGE_WRITEONLY,
				D3DFMT_INDEX16,
				D3DPOOL_MANAGED,
				&pMesh->pIdxBuffer,
				NULL);

			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
			// 頂点バッファをロックし、頂点情報へのポインタを取得
			pMesh->pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			for (int nCntDepth = 0, nCntVer = 0; nCntDepth <= nHeightDivision; nCntDepth++)
			{
				fThick = fInner + (fOuter - fInner) / nHeightDivision * nCntDepth;

				for (int nCntCircle = 0; nCntCircle <= nCircleDivision; nCntCircle++, nCntVer++)
				{
					// 頂点座標を設定
					pVtx[nCntVer].pos = D3DXVECTOR3(fThick * -sinf(fAngle * nCntCircle),
						0.0f,
						fThick * cosf(fAngle * nCntCircle));

					// 法線ベクトルの設定
					pVtx[nCntVer].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

					// 頂点カラー設定
					pVtx[nCntVer].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

					if (bPat)
					{// テクスチャを繰り返す場合
						// テクスチャ座標を設定
						pVtx[nCntVer].tex = D3DXVECTOR2((float)nCntCircle, (float)nCntDepth);

					}
					else
					{// 繰り返さない場合
						// テクスチャ座標を設定
						pVtx[nCntVer].tex = D3DXVECTOR2((float)nCntCircle / nCircleDivision, (float)nCntDepth / nHeightDivision);
					}
				}
			}

			// 頂点バッファのロック解除
			pMesh->pVtxBuff->Unlock();
			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
			// インデックスバッファをロックし、頂点番号データへのポインタを取得
			pMesh->pIdxBuffer->Lock(0, 0, (void**)&pIdx, 0);
			// 頂点番号データの設定
			for (int nCntDepth = 0; nCntDepth < nHeightDivision; nCntDepth++)
			{
				for (int nCntWidth = 0; nCntWidth <= nCircleDivision; nCntWidth++, pIdx += 2)
				{
					pIdx[0] = nCircleVerti * (nCntDepth + 1) + nCntWidth;
					pIdx[1] = nCircleVerti * nCntDepth + nCntWidth;

					if (nCntDepth + 1 < nHeightDivision && nCntWidth == nCircleDivision)
					{
						pIdx[2] = nCircleVerti * nCntDepth + nCntWidth;
						pIdx[3] = nCircleVerti * (nCntDepth + 2);
						pIdx += 2;
					}
				}
			}

			// インデックスバッファのロック解除
			pMesh->pIdxBuffer->Unlock();
			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

			pMesh->bUse = true;
			g_nSetMeshRing++;
			break;
		}
	}
	EndDevice();// デバイス取得終了
}

//=========================================================================================
// メッシュリング情報を取得
//=========================================================================================
P_MESH GetMeshRing(void)
{
	return &g_aMeshRing[0];
}

//=========================================================================================
// リング数を取得
//=========================================================================================
int GetNumMeshRing(void)
{
	return g_nSetMeshRing;
}