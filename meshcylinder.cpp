//==================================================================================================================================
// 
//			フィールド処理 [meshcylinder.cpp]
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

//**************************************************************
// プロトタイプ宣言

//=========================================================================================
// フィールド初期化処理
//=========================================================================================
void InitMeshCylinder(void)
{
	//**************************************************************
	// 変数宣言
	LPDIRECT3DDEVICE9	pDevice = GetDevice();		// デバイスへのポインタ

	//**************************************************************
	// 位置・サイズの初期化
	for (int nCntMeshCylinder = 0; nCntMeshCylinder < MAX_MESHCYLINDER; nCntMeshCylinder++)
	{
		g_aMeshCylinder[nCntMeshCylinder].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aMeshCylinder[nCntMeshCylinder].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aMeshCylinder[nCntMeshCylinder].size = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aMeshCylinder[nCntMeshCylinder].pVtxBuff = NULL;
		g_aMeshCylinder[nCntMeshCylinder].pIdxBuffer = NULL;
		g_aMeshCylinder[nCntMeshCylinder].nHeightDivision = 0;
		g_aMeshCylinder[nCntMeshCylinder].nCircleDivision = 0;
		g_aMeshCylinder[nCntMeshCylinder].nVerti = 0;
		g_aMeshCylinder[nCntMeshCylinder].nPrim = 0;
		g_aMeshCylinder[nCntMeshCylinder].bInner = false;
		g_aMeshCylinder[nCntMeshCylinder].bOuter = false;
		g_aMeshCylinder[nCntMeshCylinder].bUse = false;
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
	LPDIRECT3DDEVICE9 pDevice = GetDevice();		// デバイスへのポインタ
	D3DXMATRIX mtxRot, mtxTrans;					// マトリックス計算用

	for (int nCntMeshCylinder = 0; nCntMeshCylinder < MAX_MESHCYLINDER; nCntMeshCylinder++)
	{
		if (g_aMeshCylinder[nCntMeshCylinder].bUse)
		{
			//**************************************************************
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aMeshCylinder[nCntMeshCylinder].mtxWorld);

			//**************************************************************
			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aMeshCylinder[nCntMeshCylinder].rot.y, g_aMeshCylinder[nCntMeshCylinder].rot.x, g_aMeshCylinder[nCntMeshCylinder].rot.z);
			D3DXMatrixMultiply(&g_aMeshCylinder[nCntMeshCylinder].mtxWorld, &g_aMeshCylinder[nCntMeshCylinder].mtxWorld, &mtxRot);

			//**************************************************************
			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aMeshCylinder[nCntMeshCylinder].pos.x, g_aMeshCylinder[nCntMeshCylinder].pos.y, g_aMeshCylinder[nCntMeshCylinder].pos.z);
			D3DXMatrixMultiply(&g_aMeshCylinder[nCntMeshCylinder].mtxWorld, &g_aMeshCylinder[nCntMeshCylinder].mtxWorld, &mtxTrans);

			//**************************************************************
			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aMeshCylinder[nCntMeshCylinder].mtxWorld);

			//**************************************************************
			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0,
				g_aMeshCylinder[nCntMeshCylinder].pVtxBuff,
				0,
				sizeof(VERTEX_3D));

			// インデックスバッファを設定
			pDevice->SetIndices(g_aMeshCylinder[nCntMeshCylinder].pIdxBuffer);

			//**************************************************************
			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			//**************************************************************
			// テクスチャの設定
			pDevice->SetTexture(0, GetTexture(g_aMeshCylinder[nCntMeshCylinder].nIdxTexture));

			//**************************************************************
			// フィールドの描画
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, g_aMeshCylinder[nCntMeshCylinder].nVerti, 0, g_aMeshCylinder[nCntMeshCylinder].nPrim);
		}
	}
}

//=========================================================================================
// 壁を設置
//=========================================================================================
//void SetMeshCylinder(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 size, int nHeightDivision, int nCircleDivision,bool bInside, bool bOutside)
void SetMeshCylinder(vec3 pos, vec3 rot, float fRadius, float fHeight, int nHeightDivision, int nCircleDivision, bool bInner, bool bOuter, int nTex)

{
	//**************************************************************
	// 変数宣言
	LPDIRECT3DDEVICE9	pDevice = GetDevice();				// デバイスへのポインタ
	VERTEX_3D*			pVtx;								// 頂点情報へのポインタ
	WORD*				pIdx;								// インデックス情報へのポインタ
	D3DXVECTOR3			vecDir;								// 法線ベクトル計算用
	int					nHeightVerti = nHeightDivision + 1,
						nCircleVerti = nCircleDivision + 1;	// 縦頂点数と横頂点数
	int					nBoth = 0;
	float				fAngle = 0.0f;

	for (int nCntMeshCylinder = 0; nCntMeshCylinder < MAX_MESHCYLINDER; nCntMeshCylinder++)
	{
		if (g_aMeshCylinder[nCntMeshCylinder].bUse == false)
		{
			// 値の保存
			g_aMeshCylinder[nCntMeshCylinder].pos = pos;
			g_aMeshCylinder[nCntMeshCylinder].rot = rot;

			g_aMeshCylinder[nCntMeshCylinder].size = vec3(fRadius, fHeight, fRadius);
			g_aMeshCylinder[nCntMeshCylinder].nHeightDivision = nHeightDivision;
			g_aMeshCylinder[nCntMeshCylinder].nCircleDivision = nCircleDivision;
			fAngle = (float)(2 * D3DX_PI / nCircleDivision);
			g_aMeshCylinder[nCntMeshCylinder].nVerti = nHeightVerti * nCircleVerti;
			g_aMeshCylinder[nCntMeshCylinder].nPrim = (nHeightDivision * (nCircleDivision + 2) - 2) * 2;
			g_aMeshCylinder[nCntMeshCylinder].bInner = bInner;
			if (bInner)
				nBoth++;
			g_aMeshCylinder[nCntMeshCylinder].bOuter = bOuter;
			if (bOuter)
				nBoth++;
			g_aMeshCylinder[nCntMeshCylinder].nIdxTexture = nTex;

			//**************************************************************
			// 頂点バッファの読み込み
			pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * g_aMeshCylinder[nCntMeshCylinder].nVerti,
				D3DUSAGE_WRITEONLY,
				FVF_VERTEX_3D,
				D3DPOOL_MANAGED,
				&g_aMeshCylinder[nCntMeshCylinder].pVtxBuff,
				NULL);

			//**************************************************************
			// インデックスバッファの生成
			pDevice->CreateIndexBuffer(sizeof(WORD) * 2 * (nHeightDivision * (nCircleDivision + 2) - 1) * nBoth,
				D3DUSAGE_WRITEONLY,
				D3DFMT_INDEX16,
				D3DPOOL_MANAGED,
				&g_aMeshCylinder[nCntMeshCylinder].pIdxBuffer,
				NULL);

			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
			// 頂点バッファをロックし、頂点情報へのポインタを取得
			g_aMeshCylinder[nCntMeshCylinder].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			for (int nCntDepth = 0, nCntVer = 0; nCntDepth <= nHeightDivision; nCntDepth++)
			{
				for (int nCntCircle = 0; nCntCircle <= nCircleDivision; nCntCircle++, nCntVer++)
				{
					// 頂点座標を設定
					pVtx[nCntVer].pos = D3DXVECTOR3(g_aMeshCylinder[nCntMeshCylinder].size.x * sinf(fAngle * nCntCircle),
						(g_aMeshCylinder[nCntMeshCylinder].size.y / nHeightDivision) * nCntDepth,
						g_aMeshCylinder[nCntMeshCylinder].size.z * cosf(fAngle * nCntCircle));

					// 法線ベクトルの設定(正規化)
					vecDir = D3DXVECTOR3(pVtx[nCntVer].pos.x, 0.0f, pVtx[nCntVer].pos.z); // 頂点座標<=真横なのでYだけ消す
					D3DXVec3Normalize(&vecDir, &vecDir);			//いい感じのベクトルに変換してくれる
					pVtx[nCntVer].nor = vecDir;

					// 頂点カラー設定
					pVtx[nCntVer].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

					// テクスチャ座標を設定
					pVtx[nCntVer].tex = D3DXVECTOR2((float)nCntCircle, (float)nCntDepth);
				}
			}

			// 頂点バッファのロック解除
			g_aMeshCylinder[nCntMeshCylinder].pVtxBuff->Unlock();
			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
			// インデックスバッファをロックし、頂点番号データへのポインタを取得
			g_aMeshCylinder[nCntMeshCylinder].pIdxBuffer->Lock(0, 0, (void**)&pIdx, 0);
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
			g_aMeshCylinder[nCntMeshCylinder].pIdxBuffer->Unlock();
			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

			g_aMeshCylinder[nCntMeshCylinder].bUse = true;
			break;
		}
	}
}

//=========================================================================================
// フィールド情報を取得
//=========================================================================================
P_MESH GetMeshCylinder(void)
{
	return &g_aMeshCylinder[0];
}