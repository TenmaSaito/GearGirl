//==================================================================================================================================
// 
//			メッシュリングの処理 [meshring.cpp]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
//**************************************************************
// インクルード
#include "input.h"
#include "mesh.h"

//**************************************************************
// グローバル変数
LPDIRECT3DTEXTURE9			g_pTextureMeshRing = NULL;			// テクスチャへのポインタ
MeshRing				g_aMeshRing[MAX_MESHRING];		// メッシュリングの情報

//**************************************************************
// プロトタイプ宣言

//=========================================================================================
// メッシュリングの初期化処理
//=========================================================================================
void InitMeshRing(void)
{
	//**************************************************************
	// 変数宣言
	LPDIRECT3DDEVICE9	pDevice = GetDevice();		// デバイスへのポインタ

	//**************************************************************
	// テクスチャ読み込み
	D3DXCreateTextureFromFile(pDevice,"data\\TEXTURE\\object\\block002.jpg",&g_pTextureMeshRing);

	//**************************************************************
	// 位置・サイズの初期化
	for (int nCntMeshRing = 0; nCntMeshRing < MAX_MESHRING; nCntMeshRing++)
	{
		g_aMeshRing[nCntMeshRing].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aMeshRing[nCntMeshRing].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aMeshRing[nCntMeshRing].pVtxBuff = NULL;
		g_aMeshRing[nCntMeshRing].pIdxBuffer = NULL;
		g_aMeshRing[nCntMeshRing].fInnerRadius = 0.0f;
		g_aMeshRing[nCntMeshRing].fOuterRadius = 0.0f;
		g_aMeshRing[nCntMeshRing].nHeightDivision = 0;
		g_aMeshRing[nCntMeshRing].nCircleDivision = 0;
		g_aMeshRing[nCntMeshRing].fAngle = 0.0f;
		g_aMeshRing[nCntMeshRing].nVerti = 0;
		g_aMeshRing[nCntMeshRing].nPrim = 0;
		g_aMeshRing[nCntMeshRing].bInside = false;
		g_aMeshRing[nCntMeshRing].bOutside = false;
		g_aMeshRing[nCntMeshRing].bUse = false;
	}

	// メッシュリングの設置
	SetMeshRing(D3DXVECTOR3(-300.0f, 10.0f, -300.0f),  D3DXVECTOR3(0.0f, 0.0f, 0.0f), 50.0f,100.0f,3, 32,true,true);
	//SetMeshRing(D3DXVECTOR3(FIELD_SIZE, 0.0, 0.0f),  D3DXVECTOR3(0.0f, D3DX_PI * 0.5f, 0.0f), D3DXVECTOR3(1000.0f, 200.0f, 0.0f)	,2, 2);
	//SetMeshRing(D3DXVECTOR3(0.0f, 0.0, -FIELD_SIZE), D3DXVECTOR3(0.0f, -D3DX_PI, 0.0f), D3DXVECTOR3(1000.0f, 200.0f, 0.0f)		,3, 3);
	//SetMeshRing(D3DXVECTOR3(-FIELD_SIZE, 0.0, 0.0f), D3DXVECTOR3(0.0f, -D3DX_PI * 0.5f, 0.0f), D3DXVECTOR3(1000.0f, 200.0f, 0.0f)	,4, 4);
}

//=========================================================================================
// メッシュリングの終了処理
//=========================================================================================
void UninitMeshRing(void)
{
	//**************************************************************
	// テクスチャの破棄
	if (g_pTextureMeshRing != NULL)
	{
		g_pTextureMeshRing->Release();
		g_pTextureMeshRing = NULL;
	}

	for (int nCntMeshRing = 0; nCntMeshRing < MAX_MESHRING; nCntMeshRing++)
	{
		//**************************************************************
		// 頂点バッファの破棄
		if (g_aMeshRing[nCntMeshRing].pVtxBuff != NULL)
		{
			g_aMeshRing[nCntMeshRing].pVtxBuff->Release();
			g_aMeshRing[nCntMeshRing].pVtxBuff = NULL;
		}

		//**************************************************************
		// インデックスバッファの破棄
		if (g_aMeshRing[nCntMeshRing].pIdxBuffer != NULL)
		{
			g_aMeshRing[nCntMeshRing].pIdxBuffer->Release();
			g_aMeshRing[nCntMeshRing].pIdxBuffer = NULL;
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
	LPDIRECT3DDEVICE9 pDevice = GetDevice();		// デバイスへのポインタ
	D3DXMATRIX mtxRot, mtxTrans;					// マトリックス計算用

	for (int nCntMeshRing = 0; nCntMeshRing < MAX_MESHRING; nCntMeshRing++)
	{
		if (g_aMeshRing[nCntMeshRing].bUse)
		{
			//**************************************************************
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aMeshRing[nCntMeshRing].mtxWorld);

			//**************************************************************
			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aMeshRing[nCntMeshRing].rot.y, g_aMeshRing[nCntMeshRing].rot.x, g_aMeshRing[nCntMeshRing].rot.z);
			D3DXMatrixMultiply(&g_aMeshRing[nCntMeshRing].mtxWorld, &g_aMeshRing[nCntMeshRing].mtxWorld, &mtxRot);

			//**************************************************************
			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aMeshRing[nCntMeshRing].pos.x, g_aMeshRing[nCntMeshRing].pos.y, g_aMeshRing[nCntMeshRing].pos.z);
			D3DXMatrixMultiply(&g_aMeshRing[nCntMeshRing].mtxWorld, &g_aMeshRing[nCntMeshRing].mtxWorld, &mtxTrans);

			//**************************************************************
			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aMeshRing[nCntMeshRing].mtxWorld);

			//**************************************************************
			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0,
				g_aMeshRing[nCntMeshRing].pVtxBuff,
				0,
				sizeof(VERTEX_3D));

			// インデックスバッファを設定
			pDevice->SetIndices(g_aMeshRing[nCntMeshRing].pIdxBuffer);

			//**************************************************************
			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			//**************************************************************
			// テクスチャの設定
			pDevice->SetTexture(0, g_pTextureMeshRing);

			//**************************************************************
			// フィールドの描画
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, g_aMeshRing[nCntMeshRing].nVerti, 0, g_aMeshRing[nCntMeshRing].nPrim);
		}
	}
}

//=========================================================================================
// メッシュリングを設置
//=========================================================================================
void SetMeshRing(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fInner, float fOuter, int nHeightDivision, int nCircleDivision,bool bInside, bool bOutside)
{
	//**************************************************************
	// 変数宣言
	LPDIRECT3DDEVICE9	pDevice = GetDevice();				// デバイスへのポインタ
	VERTEX_3D*			pVtx;								// 頂点情報へのポインタ
	WORD*				pIdx;								// インデックス情報へのポインタ
	D3DXVECTOR3			vecDir;								// 法線ベクトル計算用
	float				fThick;								// 内径と外径の差
	int					nHeightVerti = nHeightDivision + 1,
						nCircleVerti = nCircleDivision + 1;	// 縦頂点数と横頂点数
	int					nBoth = 0;

	for (int nCntMeshRing = 0; nCntMeshRing < MAX_MESHRING; nCntMeshRing++)
	{
		if (g_aMeshRing[nCntMeshRing].bUse == false)
		{
			// 値の保存
			g_aMeshRing[nCntMeshRing].pos = pos;
			g_aMeshRing[nCntMeshRing].rot = rot;
			g_aMeshRing[nCntMeshRing].fInnerRadius = fInner;
			g_aMeshRing[nCntMeshRing].fOuterRadius = fOuter;
			g_aMeshRing[nCntMeshRing].nHeightDivision = nHeightDivision;
			g_aMeshRing[nCntMeshRing].nCircleDivision = nCircleDivision;
			g_aMeshRing[nCntMeshRing].fAngle = (float)(2 * D3DX_PI / nCircleDivision);
			g_aMeshRing[nCntMeshRing].nVerti = nHeightVerti * nCircleVerti;
			g_aMeshRing[nCntMeshRing].nPrim = (nHeightDivision * (nCircleDivision + 2) - 2) * 2;
			g_aMeshRing[nCntMeshRing].bInside = bInside;
			if (bInside)
				nBoth++;
			g_aMeshRing[nCntMeshRing].bOutside = bOutside;
			if (bOutside)
				nBoth++;

			//**************************************************************
			// 頂点バッファの読み込み
			pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * g_aMeshRing[nCntMeshRing].nVerti,
				D3DUSAGE_WRITEONLY,
				FVF_VERTEX_3D,
				D3DPOOL_MANAGED,
				&g_aMeshRing[nCntMeshRing].pVtxBuff,
				NULL);

			//**************************************************************
			// インデックスバッファの生成
			pDevice->CreateIndexBuffer(sizeof(WORD) * 2 * (nHeightDivision * (nCircleDivision + 2) - 1) * nBoth,
				D3DUSAGE_WRITEONLY,
				D3DFMT_INDEX16,
				D3DPOOL_MANAGED,
				&g_aMeshRing[nCntMeshRing].pIdxBuffer,
				NULL);

			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
			// 頂点バッファをロックし、頂点情報へのポインタを取得
			g_aMeshRing[nCntMeshRing].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			for (int nCntDepth = 0, nCntVer = 0; nCntDepth <= nHeightDivision; nCntDepth++)
			{
				fThick = fInner + (fOuter - fInner) / nHeightDivision * nCntDepth;

				for (int nCntCircle = 0; nCntCircle <= nCircleDivision; nCntCircle++, nCntVer++)
				{
					// 頂点座標を設定
					pVtx[nCntVer].pos = D3DXVECTOR3(fThick * -sinf(g_aMeshRing[nCntMeshRing].fAngle * nCntCircle),
						0.0f,
						fThick * cosf(g_aMeshRing[nCntMeshRing].fAngle * nCntCircle));

					// 法線ベクトルの設定
					pVtx[nCntVer].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

					// 頂点カラー設定
					pVtx[nCntVer].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

					// テクスチャ座標を設定
					pVtx[nCntVer].tex = D3DXVECTOR2((float)nCntCircle, (float)nCntDepth);
				}
			}

			// 頂点バッファのロック解除
			g_aMeshRing[nCntMeshRing].pVtxBuff->Unlock();
			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
			// インデックスバッファをロックし、頂点番号データへのポインタを取得
			g_aMeshRing[nCntMeshRing].pIdxBuffer->Lock(0, 0, (void**)&pIdx, 0);
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
			g_aMeshRing[nCntMeshRing].pIdxBuffer->Unlock();
			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

			g_aMeshRing[nCntMeshRing].bUse = true;
			break;
		}
	}
}

//=========================================================================================
// フィールド情報を取得
//=========================================================================================
P_MESH GetMeshRing(void)
{
	return &g_aMeshRing[0];
}