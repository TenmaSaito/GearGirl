//==================================================================================================================================
// 
//			フィールド処理 [meshdome.cpp]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
//**************************************************************
// インクルード
#include "mesh.h"
#include "Texture.h"

//**************************************************************
// グローバル変数
MeshInfo				g_aMeshDome[MAX_MESHDOME];		// メッシュ壁情報
int						g_nSetMeshDome;					// セット済みのメッシュドーム数

//**************************************************************
// プロトタイプ宣言

//=========================================================================================
// フィールド初期化処理
//=========================================================================================
void InitMeshDome(void)
{
	//**************************************************************
	// 変数宣言
	g_nSetMeshDome = 0;

	//**************************************************************
	// 位置・サイズの初期化
	for (int nCntMeshDome = 0; nCntMeshDome < MAX_MESHDOME; nCntMeshDome++)
	{
		g_aMeshDome[nCntMeshDome].pos = D3DXVECTOR3_NULL;
		g_aMeshDome[nCntMeshDome].rot = D3DXVECTOR3_NULL;
		//g_aMeshDome[nCntMeshDome].fSize = VECTOR3_ZERO;
		g_aMeshDome[nCntMeshDome].pVtxBuff = NULL;
		g_aMeshDome[nCntMeshDome].pIdxBuffer = NULL;
		g_aMeshDome[nCntMeshDome].nHeightDivision = 0;
		g_aMeshDome[nCntMeshDome].nCircleDivision = 0;
		g_aMeshDome[nCntMeshDome].nVerti = 0;
		g_aMeshDome[nCntMeshDome].nPrim = 0;
		g_aMeshDome[nCntMeshDome].bInner = false;
		g_aMeshDome[nCntMeshDome].bOuter = false;
		g_aMeshDome[nCntMeshDome].bUse = false;
	}
}

//=========================================================================================
// フィールド終了処理
//=========================================================================================
void UninitMeshDome(void)
{

	for (int nCntMeshDome = 0; nCntMeshDome < MAX_MESHDOME; nCntMeshDome++)
	{
		//**************************************************************
		// 頂点バッファの破棄
		if (g_aMeshDome[nCntMeshDome].pVtxBuff != NULL)
		{
			g_aMeshDome[nCntMeshDome].pVtxBuff->Release();
			g_aMeshDome[nCntMeshDome].pVtxBuff = NULL;
		}

		//**************************************************************
		// インデックスバッファの破棄
		if (g_aMeshDome[nCntMeshDome].pIdxBuffer != NULL)
		{
			g_aMeshDome[nCntMeshDome].pIdxBuffer->Release();
			g_aMeshDome[nCntMeshDome].pIdxBuffer = NULL;
		}
	}
}

//=========================================================================================
// フィールド更新処理
//=========================================================================================
void UpdateMeshDome(void)
{

}

//=========================================================================================
// フィールド描画処理
//=========================================================================================
void DrawMeshDome(void)
{
	//**************************************************************
	// 変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();		// デバイスへのポインタ
	D3DXMATRIX mtxRot, mtxTrans;					// マトリックス計算用

	for (int nCntMeshDome = 0; nCntMeshDome < MAX_MESHDOME; nCntMeshDome++)
	{
		if (g_aMeshDome[nCntMeshDome].bUse)
		{
			//**************************************************************
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aMeshDome[nCntMeshDome].mtxWorld);

			//**************************************************************
			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aMeshDome[nCntMeshDome].rot.y, g_aMeshDome[nCntMeshDome].rot.x, g_aMeshDome[nCntMeshDome].rot.z);
			D3DXMatrixMultiply(&g_aMeshDome[nCntMeshDome].mtxWorld, &g_aMeshDome[nCntMeshDome].mtxWorld, &mtxRot);

			//**************************************************************
			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aMeshDome[nCntMeshDome].pos.x, g_aMeshDome[nCntMeshDome].pos.y, g_aMeshDome[nCntMeshDome].pos.z);
			D3DXMatrixMultiply(&g_aMeshDome[nCntMeshDome].mtxWorld, &g_aMeshDome[nCntMeshDome].mtxWorld, &mtxTrans);

			//**************************************************************
			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aMeshDome[nCntMeshDome].mtxWorld);

			//**************************************************************
			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0,
				g_aMeshDome[nCntMeshDome].pVtxBuff,
				0,
				sizeof(VERTEX_3D));

			// インデックスバッファを設定
			pDevice->SetIndices(g_aMeshDome[nCntMeshDome].pIdxBuffer);

			//**************************************************************
			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			//**************************************************************
			// テクスチャの設定
			pDevice->SetTexture(0, GetTexture(g_aMeshDome[nCntMeshDome].nIdxTexture));

			//**************************************************************
			// フィールドの描画
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, g_aMeshDome[nCntMeshDome].nVerti, 0, g_aMeshDome[nCntMeshDome].nPrim);
		}
	}
	EndDevice();// デバイス取得終了
}

//=========================================================================================
// 壁を設置
//=========================================================================================
void SetMeshDome(vec3 pos, vec3 rot, float fRadius, int nHeightDivision, int nCircleDivision, bool bInner, bool bOuter, int nTex)
{
	//**************************************************************
	// 変数宣言
	LPDIRECT3DDEVICE9	pDevice = GetDevice();				// デバイスへのポインタ
	P_MESH				pMesh = GetMeshDome();				// メッシュドームポインタ
	VERTEX_3D*			pVtx;								// 頂点情報へのポインタ
	WORD*				pIdx;								// インデックス情報へのポインタ
	vec3				vecDir;								// 法線ベクトル（計算用
	vec3				angle;								// 計算用角度
	float				fRadiusCal;							// 計算用半径
	float				fHeightCal;							// 計算用高さ
	int					nHeightVerti = nHeightDivision + 1,
						nCircleVerti = nCircleDivision + 1;	// 縦頂点数と横頂点数

	for (int nCntMeshDome = 0; nCntMeshDome < MAX_MESHDOME; nCntMeshDome++,pMesh++)
	{
		if (pMesh->bUse == false)
		{
			// 値の保存
			pMesh->pos = pos;
			pMesh->rot = rot;
			pMesh->size = vec3(fRadius, fRadius * 0.5f, fRadius);
			pMesh->nHeightDivision = nHeightDivision;
			pMesh->nCircleDivision = nCircleDivision;
			pMesh->nVerti = nHeightVerti * nCircleVerti;
			pMesh->nPrim = (nHeightDivision * (nCircleDivision + 2) - 2) * 2;
			pMesh->bInner = bInner;
			pMesh->bOuter = bOuter;
			angle = vec3((float)D3DX_PI / nHeightDivision * 0.5f, (float)(2 * D3DX_PI / nCircleDivision), (float)D3DX_PI / nHeightDivision);

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

			for (int nCntHeight = 0, nCntVer = 0; nCntHeight <= nHeightDivision; nCntHeight++)
			{
				fRadiusCal = fRadius * sinf(angle.x * nCntHeight);
				fHeightCal = fRadius * cosf(angle.x * nCntHeight);

				for (int nCntCircle = 0; nCntCircle <= nCircleDivision; nCntCircle++, nCntVer++)
				{
					// 頂点座標を設定
					pVtx[nCntVer].pos = vec3(fRadiusCal * sinf(angle.y * nCntCircle),
						fHeightCal,
						fRadiusCal * cosf(angle.y * nCntCircle));

					// 法線ベクトルの設定(正規化)
					vecDir = D3DXVECTOR3(pVtx[nCntVer].pos.x, 0.0f, pVtx[nCntVer].pos.z); // 頂点座標<=真横なのでYだけ消す
					D3DXVec3Normalize(&vecDir, &vecDir);			//いい感じのベクトルに変換してくれる
					pVtx[nCntVer].nor = vecDir;

					// 頂点カラー設定
					pVtx[nCntVer].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

					// テクスチャ座標を設定
					pVtx[nCntVer].tex = D3DXVECTOR2((float)nCntCircle, (float)nCntHeight);
				}
			}

			// 頂点バッファのロック解除
			pMesh->pVtxBuff->Unlock();
			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
			// インデックスバッファをロックし、頂点番号データへのポインタを取得
			pMesh->pIdxBuffer->Lock(0, 0, (void**)&pIdx, 0);
			// 頂点番号データの設定
			for (int nCntHeight = 0; nCntHeight < nHeightDivision; nCntHeight++)
			{
				for (int nCntWidth = 0; nCntWidth <= nCircleDivision; nCntWidth++, pIdx += 2)
				{
					pIdx[0] = nCircleVerti * (nCntHeight + 1) + nCntWidth;
					pIdx[1] = nCircleVerti * nCntHeight + nCntWidth;

					if (nCntHeight + 1 < nHeightDivision && nCntWidth == nCircleDivision)
					{
						pIdx[2] = nCircleVerti * nCntHeight + nCntWidth;
						pIdx[3] = nCircleVerti * (nCntHeight + 2);
						pIdx += 2;
					}
				}
			}

			// インデックスバッファのロック解除
			pMesh->pIdxBuffer->Unlock();
			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

			pMesh->bUse = true;
			g_nSetMeshDome++;
			break;
		}
	}
	EndDevice();// デバイス取得終了
}

//=========================================================================================
// ドーム情報を取得
//=========================================================================================
P_MESH GetMeshDome(void)
{
	return &g_aMeshDome[0];
}

//=========================================================================================
// ドーム数を取得
//=========================================================================================
int GetNumMeshDome(void)
{
	return g_nSetMeshDome;
}