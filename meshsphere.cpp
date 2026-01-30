//==================================================================================================================================
// 
//			スフィア処理 [meshsphere.cpp]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
//**************************************************************
// インクルード
#include "input.h"
#include "mesh.h"
#include "Texture.h"

//**************************************************************
// グローバル変数
MeshInfo				g_aMeshSphere[MAX_MESHSPHERE];		// メッシュスフィア情報
int						g_nSetMeshSphere;					// セット済みのメッシュスフィア数

//**************************************************************
// プロトタイプ宣言

//=========================================================================================
// スフィア初期化処理
//=========================================================================================
void InitMeshSphere(void)
{
	//**************************************************************
	// 変数宣言
	P_MESH pMesh = GetMeshSphere();
	g_nSetMeshSphere = 0;

	//**************************************************************
	// 構造体初期化
	for (int nCntMesh = 0; nCntMesh < MAX_MESHSPHERE; nCntMesh++, pMesh++)
	{
		memset(pMesh, 0, sizeof(MeshInfo));
		pMesh->bUse = false;
	}
}

//=========================================================================================
// スフィア終了処理
//=========================================================================================
void UninitMeshSphere(void)
{
	//**************************************************************
	// 変数宣言
	P_MESH pMesh = GetMeshSphere();

	for (int nCntMeshSphere = 0; nCntMeshSphere < MAX_MESHSPHERE; nCntMeshSphere++, pMesh++)
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
// スフィア更新処理
//=========================================================================================
void UpdateMeshSphere(void)
{

}

//=========================================================================================
// スフィア描画処理
//=========================================================================================
void DrawMeshSphere(void)
{
	//**************************************************************
	// 変数宣言
	LPDIRECT3DDEVICE9	pDevice = GetDevice();		// デバイスへのポインタ
	P_MESH				pMesh = GetMeshSphere();
	D3DXMATRIX mtxRot, mtxTrans;					// マトリックス計算用

	for (int nCntMeshSphere = 0; nCntMeshSphere < MAX_MESHSPHERE; nCntMeshSphere++, pMesh++)
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
			// スフィアの描画
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, pMesh->nVerti, 0, pMesh->nPrim);
		}
	}

	EndDevice();	// デバイス取得終了
}

//=========================================================================================
// スフィアを設置
//=========================================================================================
void SetMeshSphere(vec3 pos, vec3 rot, float fRadius, int nHeightDivision, int nCircleDivision, bool bInner, bool bOuter, int nTex)
{
	//**************************************************************
	// 変数宣言
	LPDIRECT3DDEVICE9	pDevice = GetDevice();				//--------- デバイス取得 ---------//
	P_MESH				pMesh = GetMeshSphere();			// メッシュスフィア先頭アドレス
	VERTEX_3D*			pVtx;								// 頂点情報へのポインタ
	WORD*				pIdx;								// インデックス情報へのポインタ
	D3DXVECTOR3			vecDir;								// 法線ベクトル（計算用
	float				fRadiusCal;							// 計算用半径
	float				fHeightCal;							// 計算用高さ
	int					nHeightVerti = nHeightDivision + 1,
						nCircleVerti = nCircleDivision + 1;	// 縦頂点数と横頂点数
	vec3 angle = D3DXVECTOR3_NULL;

	for (int nCntMeshSphere = 0; nCntMeshSphere < MAX_MESHSPHERE; nCntMeshSphere++, pMesh++)
	{
		if (pMesh->bUse == false)
		{
			// 値の保存
			pMesh->pos = pos;
			pMesh->rot = rot;
			pMesh->size = vec3(fRadius, fRadius, fRadius);
			pMesh->nHeightDivision = nHeightDivision;
			pMesh->nCircleDivision = nCircleDivision;
			angle = vec3((float)D3DX_PI / nHeightDivision,(float)(2 * D3DX_PI / nCircleDivision),(float)D3DX_PI / nHeightDivision);
			pMesh->nVerti = nHeightVerti * nCircleVerti;
			pMesh->nPrim = (nHeightDivision * (nCircleDivision + 2) - 2) * 2;
			pMesh->bInner = bInner;
			pMesh->bOuter = bOuter;
			pMesh->nIdxTexture = nTex;

			//**************************************************************
			// 頂点バッファの読み込み
			pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * g_aMeshSphere[nCntMeshSphere].nVerti,
				D3DUSAGE_WRITEONLY,
				FVF_VERTEX_3D,
				D3DPOOL_MANAGED,
				&g_aMeshSphere[nCntMeshSphere].pVtxBuff,
				NULL);

			//**************************************************************
			// インデックスバッファの生成
			pDevice->CreateIndexBuffer(sizeof(WORD) * 2 * (nHeightDivision * (nCircleDivision + 2) - 1),
				D3DUSAGE_WRITEONLY,
				D3DFMT_INDEX16,
				D3DPOOL_MANAGED,
				&g_aMeshSphere[nCntMeshSphere].pIdxBuffer,
				NULL);

			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
			// 頂点バッファをロックし、頂点情報へのポインタを取得
			g_aMeshSphere[nCntMeshSphere].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			for (int nCntHeight = 0, nCntVer = 0; nCntHeight <= nHeightDivision; nCntHeight++)
			{
				fRadiusCal = fRadius * sinf(angle.x * nCntHeight);
				fHeightCal = fRadius * cosf(angle.x * nCntHeight);

				for (int nCntCircle = 0; nCntCircle <= nCircleDivision; nCntCircle++, nCntVer++)
				{
					// 頂点座標を設定
					pVtx[nCntVer].pos = D3DXVECTOR3(fRadiusCal * sinf(angle.y * nCntCircle),
						fHeightCal,
						fRadiusCal * cosf(angle.y * nCntCircle));

					// 法線ベクトルの設定(正規化)
					vecDir = D3DXVECTOR3(pVtx[nCntVer].pos.x, 0.0f, pVtx[nCntVer].pos.z); // 頂点座標<=真横なのでYだけ消す
					D3DXVec3Normalize(&vecDir, &vecDir);			//いい感じのベクトルに変換してくれる
					pVtx[nCntVer].nor = vecDir;

					// 頂点カラー設定
					pVtx[nCntVer].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

					// テクスチャ座標を設定
					pVtx[nCntVer].tex = D3DXVECTOR2((float)nCntCircle / nCircleDivision, (float)nCntHeight / nHeightDivision);
				}
			}

			// 頂点バッファのロック解除
			g_aMeshSphere[nCntMeshSphere].pVtxBuff->Unlock();
			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
			// インデックスバッファをロックし、頂点番号データへのポインタを取得
			g_aMeshSphere[nCntMeshSphere].pIdxBuffer->Lock(0, 0, (void**)&pIdx, 0);

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
			g_aMeshSphere[nCntMeshSphere].pIdxBuffer->Unlock();
			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

			g_aMeshSphere[nCntMeshSphere].bUse = true;
			g_nSetMeshSphere++;
			break;
		}
	}

	EndDevice();										// デバイス取得終了
}

//=========================================================================================
// スフィア情報を取得
//=========================================================================================
P_MESH GetMeshSphere(void)
{
	return &g_aMeshSphere[0];
}

//=========================================================================================
// スフィア数を取得
//=========================================================================================
int GetNumMeshSphere(void)
{
	return g_nSetMeshSphere;
}