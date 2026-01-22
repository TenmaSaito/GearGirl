//==================================================================================================================================
// 
//			スフィア処理 [meshsphere.cpp]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
//**************************************************************
// インクルード
#include "input.h"
#include "meshsphere.h"

//**************************************************************
// グローバル変数
LPDIRECT3DTEXTURE9			g_pTextureMeshSphere = NULL;			// テクスチャへのポインタ
MeshSphere				g_aMeshSphere[MAX_MESHSPHERE];		// メッシュスフィア情報

//**************************************************************
// プロトタイプ宣言

//=========================================================================================
// スフィア初期化処理
//=========================================================================================
void InitMeshSphere(void)
{
	//**************************************************************
	// 変数宣言
	LPDIRECT3DDEVICE9	pDevice = GetDevice();		// デバイスへのポインタ

	//**************************************************************
	// テクスチャ読み込み
	D3DXCreateTextureFromFile(pDevice,"data\\TEXTURE\\object\\block002.jpg",&g_pTextureMeshSphere);

	//**************************************************************
	// 位置・サイズの初期化
	for (int nCntMeshSphere = 0; nCntMeshSphere < MAX_MESHSPHERE; nCntMeshSphere++)
	{
		g_aMeshSphere[nCntMeshSphere].pos = VECTOR3_ZERO;
		g_aMeshSphere[nCntMeshSphere].rot = VECTOR3_ZERO;
		//g_aMeshSphere[nCntMeshSphere].fSize = VECTOR3_ZERO;
		g_aMeshSphere[nCntMeshSphere].angle = VECTOR3_ZERO;
		g_aMeshSphere[nCntMeshSphere].pVtxBuff = NULL;
		g_aMeshSphere[nCntMeshSphere].pIdxBuffer = NULL;
		g_aMeshSphere[nCntMeshSphere].nHeightDivision = 0;
		g_aMeshSphere[nCntMeshSphere].nCircleDivision = 0;
		g_aMeshSphere[nCntMeshSphere].nVerti = 0;
		g_aMeshSphere[nCntMeshSphere].nPrim = 0;
		g_aMeshSphere[nCntMeshSphere].bInside = false;
		g_aMeshSphere[nCntMeshSphere].bOutside = false;
		g_aMeshSphere[nCntMeshSphere].bUse = false;
	}

	// 壁の設置
	SetMeshSphere(D3DXVECTOR3(-300.0f, 100.0f, 300.0f),  D3DXVECTOR3(0.0f, 0.0f, 0.0f), 100.0f,8, 32,true,true);
	//SetMeshSphere(D3DXVECTOR3(FIELD_SIZE, 0.0, 0.0f),  D3DXVECTOR3(0.0f, D3DX_PI * 0.5f, 0.0f), D3DXVECTOR3(1000.0f, 200.0f, 0.0f)	,2, 2);
	//SetMeshSphere(D3DXVECTOR3(0.0f, 0.0, -FIELD_SIZE), D3DXVECTOR3(0.0f, -D3DX_PI, 0.0f), D3DXVECTOR3(1000.0f, 200.0f, 0.0f)			,3, 3);
	//SetMeshSphere(D3DXVECTOR3(-FIELD_SIZE, 0.0, 0.0f), D3DXVECTOR3(0.0f, -D3DX_PI * 0.5f, 0.0f), D3DXVECTOR3(1000.0f, 200.0f, 0.0f)	,4, 4);
}

//=========================================================================================
// スフィア終了処理
//=========================================================================================
void UninitMeshSphere(void)
{
	//**************************************************************
	// テクスチャの破棄
	if (g_pTextureMeshSphere != NULL)
	{
		g_pTextureMeshSphere->Release();
		g_pTextureMeshSphere = NULL;
	}

	for (int nCntMeshSphere = 0; nCntMeshSphere < MAX_MESHSPHERE; nCntMeshSphere++)
	{
		//**************************************************************
		// 頂点バッファの破棄
		if (g_aMeshSphere[nCntMeshSphere].pVtxBuff != NULL)
		{
			g_aMeshSphere[nCntMeshSphere].pVtxBuff->Release();
			g_aMeshSphere[nCntMeshSphere].pVtxBuff = NULL;
		}

		//**************************************************************
		// インデックスバッファの破棄
		if (g_aMeshSphere[nCntMeshSphere].pIdxBuffer != NULL)
		{
			g_aMeshSphere[nCntMeshSphere].pIdxBuffer->Release();
			g_aMeshSphere[nCntMeshSphere].pIdxBuffer = NULL;
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
	LPDIRECT3DDEVICE9 pDevice = GetDevice();		// デバイスへのポインタ
	D3DXMATRIX mtxRot, mtxTrans;					// マトリックス計算用

	for (int nCntMeshSphere = 0; nCntMeshSphere < MAX_MESHSPHERE; nCntMeshSphere++)
	{
		if (g_aMeshSphere[nCntMeshSphere].bUse)
		{
			//**************************************************************
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aMeshSphere[nCntMeshSphere].mtxWorld);

			//**************************************************************
			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aMeshSphere[nCntMeshSphere].rot.y, g_aMeshSphere[nCntMeshSphere].rot.x, g_aMeshSphere[nCntMeshSphere].rot.z);
			D3DXMatrixMultiply(&g_aMeshSphere[nCntMeshSphere].mtxWorld, &g_aMeshSphere[nCntMeshSphere].mtxWorld, &mtxRot);

			//**************************************************************
			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aMeshSphere[nCntMeshSphere].pos.x, g_aMeshSphere[nCntMeshSphere].pos.y, g_aMeshSphere[nCntMeshSphere].pos.z);
			D3DXMatrixMultiply(&g_aMeshSphere[nCntMeshSphere].mtxWorld, &g_aMeshSphere[nCntMeshSphere].mtxWorld, &mtxTrans);

			//**************************************************************
			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aMeshSphere[nCntMeshSphere].mtxWorld);

			//**************************************************************
			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0,
				g_aMeshSphere[nCntMeshSphere].pVtxBuff,
				0,
				sizeof(VERTEX_3D));

			// インデックスバッファを設定
			pDevice->SetIndices(g_aMeshSphere[nCntMeshSphere].pIdxBuffer);

			//**************************************************************
			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			//**************************************************************
			// テクスチャの設定
			pDevice->SetTexture(0, g_pTextureMeshSphere);

			//**************************************************************
			// スフィアの描画
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, g_aMeshSphere[nCntMeshSphere].nVerti, 0, g_aMeshSphere[nCntMeshSphere].nPrim);
		}
	}
}

//=========================================================================================
// スフィアを設置
//=========================================================================================
void SetMeshSphere(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fSize, int nHeightDivision, int nCircleDivision,bool bInside, bool bOutside)
{
	//**************************************************************
	// 変数宣言
	LPDIRECT3DDEVICE9	pDevice = GetDevice();				// デバイスへのポインタ
	VERTEX_3D*			pVtx;								// 頂点情報へのポインタ
	WORD*				pIdx;								// インデックス情報へのポインタ
	D3DXVECTOR3			vecDir;								// 法線ベクトル（計算用
	float				fRadius;							// 計算用半径
	float				fHeight;							// 計算用高さ
	int					nHeightVerti = nHeightDivision + 1,
						nCircleVerti = nCircleDivision + 1;	// 縦頂点数と横頂点数
	int					nBoth = 0;

	for (int nCntMeshSphere = 0; nCntMeshSphere < MAX_MESHSPHERE; nCntMeshSphere++)
	{
		if (g_aMeshSphere[nCntMeshSphere].bUse == false)
		{
			// 値の保存
			g_aMeshSphere[nCntMeshSphere].pos = D3DXVECTOR3(pos.x, pos.y, pos.z);
			g_aMeshSphere[nCntMeshSphere].rot = rot;
			g_aMeshSphere[nCntMeshSphere].nHeightDivision = nHeightDivision;
			g_aMeshSphere[nCntMeshSphere].nCircleDivision = nCircleDivision;
			g_aMeshSphere[nCntMeshSphere].angle = D3DXVECTOR3((float)D3DX_PI / nHeightDivision,(float)(2 * D3DX_PI / nCircleDivision),(float)D3DX_PI / nHeightDivision);
			g_aMeshSphere[nCntMeshSphere].nVerti = nHeightVerti * nCircleVerti;
			g_aMeshSphere[nCntMeshSphere].nPrim = (nHeightDivision * (nCircleDivision + 2) - 2) * 2;
			g_aMeshSphere[nCntMeshSphere].bInside = bInside;
			if (bInside)
				nBoth++;
			g_aMeshSphere[nCntMeshSphere].bOutside = bOutside;
			if (bOutside)
				nBoth++;

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
				fRadius = fSize * sinf(g_aMeshSphere[nCntMeshSphere].angle.x * nCntHeight);
				fHeight = fSize * cosf(g_aMeshSphere[nCntMeshSphere].angle.x * nCntHeight);

				for (int nCntCircle = 0; nCntCircle <= nCircleDivision; nCntCircle++, nCntVer++)
				{
					// 頂点座標を設定
					pVtx[nCntVer].pos = D3DXVECTOR3(fRadius * -sinf(g_aMeshSphere[nCntMeshSphere].angle.y * nCntCircle),
						fHeight,
						fRadius * cosf(g_aMeshSphere[nCntMeshSphere].angle.y * nCntCircle));

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
			break;
		}
	}
}

//=========================================================================================
// スフィア情報を取得
//=========================================================================================
MeshSphere* GetMeshSphere(void)
{
	return &g_aMeshSphere[0];
}