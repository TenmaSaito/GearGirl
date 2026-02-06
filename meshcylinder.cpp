//==================================================================================================================================
// 
//			メッシュシリンダー処理 [meshcylinder.cpp]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
//**************************************************************
// インクルード
#include "mesh.h"
#include "Texture.h"
#include "debugproc.h"

//**************************************************************
// グローバル変数
MeshInfo				g_aMeshCylinder[MAX_MESHCYLINDER];		// メッシュ壁情報
int						g_nSetMeshCylinder;						// セット済みのメッシュシリンダー数

//**************************************************************
// プロトタイプ宣言

//=========================================================================================
// フィールド初期化処理
//=========================================================================================
void InitMeshCylinder(void)
{
	//**************************************************************
	// 変数宣言
	g_nSetMeshCylinder = 0;
	P_MESH pMesh = GetMeshCylinder();

	//**************************************************************
	// 位置・サイズの初期化
	for (int nCntMeshCylinder = 0; nCntMeshCylinder < MAX_MESHCYLINDER; nCntMeshCylinder++,pMesh++)
	{
		pMesh->pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pMesh->rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pMesh->size = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pMesh->pVtxBuff = NULL;
		pMesh->pIdxBuffer = NULL;
		pMesh->nHeightDivision = 0;
		pMesh->nCircleDivision = 0;
		pMesh->nVerti = 0;
		pMesh->nPrim = 0;
		pMesh->bUse = false;
	}
}

//=========================================================================================
// フィールド終了処理
//=========================================================================================
void UninitMeshCylinder(void)
{
	for (int nCntMeshCylinder = 0; nCntMeshCylinder < MAX_MESHCYLINDER; nCntMeshCylinder++)
	{
		//**************************************************************
		// 頂点バッファの破棄
		if (g_aMeshCylinder[nCntMeshCylinder].pVtxBuff != NULL)
		{
			g_aMeshCylinder[nCntMeshCylinder].pVtxBuff->Release();
			g_aMeshCylinder[nCntMeshCylinder].pVtxBuff = NULL;
		}

		//**************************************************************
		// インデックスバッファの破棄
		if (g_aMeshCylinder[nCntMeshCylinder].pIdxBuffer != NULL)
		{
			g_aMeshCylinder[nCntMeshCylinder].pIdxBuffer->Release();
			g_aMeshCylinder[nCntMeshCylinder].pIdxBuffer = NULL;
		}
	}
}

//=========================================================================================
// フィールド更新処理
//=========================================================================================
void UpdateMeshCylinder(void)
{
	P_MESH pMesh = &g_aMeshCylinder[0];

	for (int nCntMeshCylinder = 0; nCntMeshCylinder < MAX_MESHCYLINDER; nCntMeshCylinder++,pMesh++)
	{
		if (pMesh->bUse)
		{
			PrintDebugProc("MESH CYLINDER %d\n pos : %~3f\n", nCntMeshCylinder,pMesh->pos.x, pMesh->pos.y, pMesh->pos.z);
			PrintDebugProc(" radius %f\n height %f",pMesh->size.x, pMesh->size.y);

		}
	}
}

//=========================================================================================
// フィールド描画処理
//=========================================================================================
void DrawMeshCylinder(void)
{
	//**************************************************************
	// 変数宣言
	LPDIRECT3DDEVICE9	pDevice = GetDevice();		// デバイスへのポインタ
	P_MESH				pMesh = GetMeshCylinder();	// メッシュ情報
	D3DXMATRIX			mtxRot, mtxTrans;					// マトリックス計算用

	for (int nCntMeshCylinder = 0; nCntMeshCylinder < MAX_MESHCYLINDER; nCntMeshCylinder++, pMesh++)
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
// 壁を設置
//=========================================================================================
void SetMeshCylinder(vec3 pos, vec3 rot, float fRadius, float fHeight, int nHeightDivision, int nCircleDivision, D3DCULL cull, int nTex, bool bPat)

{
	//**************************************************************
	// 変数宣言
	LPDIRECT3DDEVICE9	pDevice = GetDevice();				// デバイスへのポインタ
	P_MESH				pMesh = GetMeshCylinder();			// メッシュ情報
	VERTEX_3D*			pVtx;								// 頂点情報へのポインタ
	WORD*				pIdx;								// インデックス情報へのポインタ
	D3DXVECTOR3			vecDir;								// 法線ベクトル計算用
	int					nHeightVerti = nHeightDivision + 1,
						nCircleVerti = nCircleDivision + 1;	// 縦頂点数と横頂点数
	int					nBoth = 0;
	float				fAngle = 0.0f;

	for (int nCntMeshCylinder = 0; nCntMeshCylinder < MAX_MESHCYLINDER; nCntMeshCylinder++, pMesh++)
	{
		if (pMesh->bUse == false)
		{
			// 値の保存
			pMesh->pos = pos;
			pMesh->rot = rot;

			pMesh->size = vec3(fRadius, fHeight, fRadius);
			pMesh->nHeightDivision = nHeightDivision;
			pMesh->nCircleDivision = nCircleDivision;
			fAngle = (float)(2 * D3DX_PI / nCircleDivision);
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
			pDevice->CreateIndexBuffer(sizeof(WORD) * 2 * (nHeightDivision * (nCircleDivision + 2) - 1) * nBoth,
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
				for (int nCntCircle = 0; nCntCircle <= nCircleDivision; nCntCircle++, nCntVer++)
				{
					// 頂点座標を設定
					pVtx[nCntVer].pos = D3DXVECTOR3(pMesh->size.x * sinf(fAngle * nCntCircle),
						(pMesh->size.y / nHeightDivision) * nCntDepth,
						 pMesh->size.z * cosf(fAngle * nCntCircle));

					// 法線ベクトルの設定(正規化)
					vecDir = D3DXVECTOR3(pVtx[nCntVer].pos.x, 0.0f, pVtx[nCntVer].pos.z); // 頂点座標<=真横なのでYだけ消す
					D3DXVec3Normalize(&vecDir, &vecDir);			//いい感じのベクトルに変換してくれる
					pVtx[nCntVer].nor = vecDir;

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
			g_nSetMeshCylinder++;
			break;
		}
	}

	EndDevice();// デバイス取得終了
}

//=========================================================================================
// フィールド情報を取得
//=========================================================================================
P_MESH GetMeshCylinder(void)
{
	return &g_aMeshCylinder[0];
}

//=========================================================================================
// フィールド数を取得
//=========================================================================================
int GetNumMeshCylinder(void)
{
	return g_nSetMeshCylinder;
}