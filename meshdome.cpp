//==================================================================================================================================
// 
//			フィールド処理 [meshdome.cpp]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
//**************************************************************
// インクルード
#include "input.h"
#include "meshdome.h"

//**************************************************************
// グローバル変数
LPDIRECT3DTEXTURE9			g_pTextureMeshDome = NULL;			// テクスチャへのポインタ
MeshDome				g_aMeshDome[MAX_MESHDOME];		// メッシュ壁情報

//**************************************************************
// プロトタイプ宣言

//=========================================================================================
// フィールド初期化処理
//=========================================================================================
void InitMeshDome(void)
{
	//**************************************************************
	// 変数宣言
	LPDIRECT3DDEVICE9	pDevice = GetDevice();		// デバイスへのポインタ

	//**************************************************************
	// テクスチャ読み込み
	D3DXCreateTextureFromFile(pDevice,"data\\TEXTURE\\object\\block002.jpg",&g_pTextureMeshDome);

	//**************************************************************
	// 位置・サイズの初期化
	for (int nCntMeshDome = 0; nCntMeshDome < MAX_MESHDOME; nCntMeshDome++)
	{
		g_aMeshDome[nCntMeshDome].pos = VECTOR3_ZERO;
		g_aMeshDome[nCntMeshDome].rot = VECTOR3_ZERO;
		//g_aMeshDome[nCntMeshDome].fSize = VECTOR3_ZERO;
		g_aMeshDome[nCntMeshDome].angle = VECTOR3_ZERO;
		g_aMeshDome[nCntMeshDome].pVtxBuff = NULL;
		g_aMeshDome[nCntMeshDome].pIdxBuffer = NULL;
		g_aMeshDome[nCntMeshDome].nHeightDivision = 0;
		g_aMeshDome[nCntMeshDome].nCircleDivision = 0;
		g_aMeshDome[nCntMeshDome].nVerti = 0;
		g_aMeshDome[nCntMeshDome].nPrim = 0;
		g_aMeshDome[nCntMeshDome].bInside = false;
		g_aMeshDome[nCntMeshDome].bOutside = false;
		g_aMeshDome[nCntMeshDome].bUse = false;
	}

	// 壁の設置

	SetMeshDome(D3DXVECTOR3(300.0f, 0.0f, 300.0f),  D3DXVECTOR3(0.0f, 0.0f, 0.0f), 100.0f,8, 32,true,true);
	//SetMeshDome(D3DXVECTOR3(FIELD_SIZE, 0.0, 0.0f),  D3DXVECTOR3(0.0f, D3DX_PI * 0.5f, 0.0f), D3DXVECTOR3(1000.0f, 200.0f, 0.0f)	,2, 2);
	//SetMeshDome(D3DXVECTOR3(0.0f, 0.0, -FIELD_SIZE), D3DXVECTOR3(0.0f, -D3DX_PI, 0.0f), D3DXVECTOR3(1000.0f, 200.0f, 0.0f)			,3, 3);
	//SetMeshDome(D3DXVECTOR3(-FIELD_SIZE, 0.0, 0.0f), D3DXVECTOR3(0.0f, -D3DX_PI * 0.5f, 0.0f), D3DXVECTOR3(1000.0f, 200.0f, 0.0f)	,4, 4);
}

//=========================================================================================
// フィールド終了処理
//=========================================================================================
void UninitMeshDome(void)
{
	//**************************************************************
	// テクスチャの破棄
	if (g_pTextureMeshDome != NULL)
	{
		g_pTextureMeshDome->Release();
		g_pTextureMeshDome = NULL;
	}

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
			pDevice->SetTexture(0, g_pTextureMeshDome);

			//**************************************************************
			// フィールドの描画
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, g_aMeshDome[nCntMeshDome].nVerti, 0, g_aMeshDome[nCntMeshDome].nPrim);
		}
	}
}

//=========================================================================================
// 壁を設置
//=========================================================================================
void SetMeshDome(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fSize, int nHeightDivision, int nCircleDivision,bool bInside, bool bOutside)
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

	for (int nCntMeshDome = 0; nCntMeshDome < MAX_MESHDOME; nCntMeshDome++)
	{
		if (g_aMeshDome[nCntMeshDome].bUse == false)
		{
			// 値の保存
			g_aMeshDome[nCntMeshDome].pos = D3DXVECTOR3(pos.x, pos.y, pos.z);
			g_aMeshDome[nCntMeshDome].rot = rot;
			//g_aMeshDome[nCntMeshDome].fSize = fSize;
			g_aMeshDome[nCntMeshDome].nHeightDivision = nHeightDivision;
			g_aMeshDome[nCntMeshDome].nCircleDivision = nCircleDivision;
			g_aMeshDome[nCntMeshDome].angle = D3DXVECTOR3((float)D3DX_PI / nHeightDivision * 0.5f,(float)(2 * D3DX_PI / nCircleDivision),(float)D3DX_PI / nHeightDivision);
			g_aMeshDome[nCntMeshDome].nVerti = nHeightVerti * nCircleVerti;
			g_aMeshDome[nCntMeshDome].nPrim = (nHeightDivision * (nCircleDivision + 2) - 2) * 2;
			g_aMeshDome[nCntMeshDome].bInside = bInside;
			if (bInside)
				nBoth++;
			g_aMeshDome[nCntMeshDome].bOutside = bOutside;
			if (bOutside)
				nBoth++;

			//**************************************************************
			// 頂点バッファの読み込み
			pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * g_aMeshDome[nCntMeshDome].nVerti,
				D3DUSAGE_WRITEONLY,
				FVF_VERTEX_3D,
				D3DPOOL_MANAGED,
				&g_aMeshDome[nCntMeshDome].pVtxBuff,
				NULL);

			//**************************************************************
			// インデックスバッファの生成
			pDevice->CreateIndexBuffer(sizeof(WORD) * 2 * (nHeightDivision * (nCircleDivision + 2) - 1),
				D3DUSAGE_WRITEONLY,
				D3DFMT_INDEX16,
				D3DPOOL_MANAGED,
				&g_aMeshDome[nCntMeshDome].pIdxBuffer,
				NULL);

			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
			// 頂点バッファをロックし、頂点情報へのポインタを取得
			g_aMeshDome[nCntMeshDome].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			for (int nCntHeight = 0, nCntVer = 0; nCntHeight <= nHeightDivision; nCntHeight++)
			{
				fRadius = fSize * sinf(g_aMeshDome[nCntMeshDome].angle.x * nCntHeight);
				fHeight = fSize * cosf(g_aMeshDome[nCntMeshDome].angle.x * nCntHeight);

				for (int nCntCircle = 0; nCntCircle <= nCircleDivision; nCntCircle++, nCntVer++)
				{
					// 頂点座標を設定
					pVtx[nCntVer].pos = D3DXVECTOR3(fRadius * -sinf(g_aMeshDome[nCntMeshDome].angle.y * nCntCircle),
						fHeight,
						fRadius * cosf(g_aMeshDome[nCntMeshDome].angle.y * nCntCircle));

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
			g_aMeshDome[nCntMeshDome].pVtxBuff->Unlock();
			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
			// インデックスバッファをロックし、頂点番号データへのポインタを取得
			g_aMeshDome[nCntMeshDome].pIdxBuffer->Lock(0, 0, (void**)&pIdx, 0);
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
			g_aMeshDome[nCntMeshDome].pIdxBuffer->Unlock();
			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

			g_aMeshDome[nCntMeshDome].bUse = true;
			break;
		}
	}
}

//=========================================================================================
// フィールド情報を取得
//=========================================================================================
MeshDome* GetMeshDome(void)
{
	return &g_aMeshDome[0];
}