//==================================================================================================================================
// 
//			メッシュの処理 [mesh.cpp]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
//**************************************************************
// インクルード
#include "input.h"
#include "debugproc.h"
#include "mesh.h"

//**************************************************************
// グローバル変数
LPDIRECT3DTEXTURE9			g_pTextureSky[MAX_TEX_SKY] = {};			// テクスチャへのポインタ
Sky							g_aSky[MAX_SKY];				// 空の情報

//**************************************************************
// プロトタイプ宣言

//=========================================================================================
// 空の初期化処理
//=========================================================================================
void InitSky(void)
{
	//**************************************************************
	// 変数宣言
	LPDIRECT3DDEVICE9	pDevice = GetDevice();		// デバイスへのポインタ

	//**************************************************************
	// テクスチャ読み込み
	if (FAILED(D3DXCreateTextureFromFile(pDevice, "data\\TEXTURE\\BG\\sky001.jpg", &g_pTextureSky[0])))
		PrintDebugProc("sky001.jpgを読み込めませんでした");
	if (FAILED(D3DXCreateTextureFromFile(pDevice, "data\\TEXTURE\\BG\\sky002.jpg", &g_pTextureSky[1])))
		PrintDebugProc("sky000.jpgを読み込めませんでした");

	//**************************************************************
	// 位置・サイズの初期化
	for (int nCntSky = 0; nCntSky < MAX_SKY; nCntSky++)
	{
		g_aSky[nCntSky].pos = VECNULL;
		g_aSky[nCntSky].rot = VECNULL;
		g_aSky[nCntSky].angle = VECNULL;
		g_aSky[nCntSky].pVtxBuff = NULL;
		g_aSky[nCntSky].pIdxBuffer = NULL;
		g_aSky[nCntSky].nHeightDivision = 0;
		g_aSky[nCntSky].nCircleDivision = 0;
		g_aSky[nCntSky].nVerti = 0;
		g_aSky[nCntSky].nPrim = 0;
		g_aSky[nCntSky].nTex = 0;
		g_aSky[nCntSky].bInside = false;
		g_aSky[nCntSky].bOutside = false;
		g_aSky[nCntSky].bUse = false;
	}

	// 空を設置
	SetSky(D3DXVECTOR3(0.0f, -100.0f, 0.0f),  D3DXVECTOR3(0.0f, 0.0f, 0.0f), 5000.0f,16, 32,0,true,true);
}

//=========================================================================================
// 空の終了処理
//=========================================================================================
void UninitSky(void)
{
	//**************************************************************
	// テクスチャの破棄
	for (int nCntTex = 0; nCntTex < MAX_TEX_SKY; nCntTex++)
	{
		if (g_pTextureSky[nCntTex] != NULL)
		{
			g_pTextureSky[nCntTex]->Release();
			g_pTextureSky[nCntTex] = NULL;
		}
	}

	for (int nCntSky = 0; nCntSky < MAX_SKY; nCntSky++)
	{
		//**************************************************************
		// 頂点バッファの破棄
		if (g_aSky[nCntSky].pVtxBuff != NULL)
		{
			g_aSky[nCntSky].pVtxBuff->Release();
			g_aSky[nCntSky].pVtxBuff = NULL;
		}

		//**************************************************************
		// インデックスバッファの破棄
		if (g_aSky[nCntSky].pIdxBuffer != NULL)
		{
			g_aSky[nCntSky].pIdxBuffer->Release();
			g_aSky[nCntSky].pIdxBuffer = NULL;
		}
	}
}

//=========================================================================================
// 空の更新処理
//=========================================================================================
void UpdateSky(void)
{

}

//=========================================================================================
// 空の描画処理
//=========================================================================================
void DrawSky(void)
{
	//**************************************************************
	// 変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();		// デバイスへのポインタ
	D3DXMATRIX mtxRot, mtxTrans;					// マトリックス計算用

	for (int nCntSky = 0; nCntSky < MAX_SKY; nCntSky++)
	{
		if (g_aSky[nCntSky].bUse)
		{
			//**************************************************************
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aSky[nCntSky].mtxWorld);

			//**************************************************************
			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aSky[nCntSky].rot.y, g_aSky[nCntSky].rot.x, g_aSky[nCntSky].rot.z);
			D3DXMatrixMultiply(&g_aSky[nCntSky].mtxWorld, &g_aSky[nCntSky].mtxWorld, &mtxRot);

			//**************************************************************
			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aSky[nCntSky].pos.x, g_aSky[nCntSky].pos.y, g_aSky[nCntSky].pos.z);
			D3DXMatrixMultiply(&g_aSky[nCntSky].mtxWorld, &g_aSky[nCntSky].mtxWorld, &mtxTrans);

			//**************************************************************
			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aSky[nCntSky].mtxWorld);

			//**************************************************************
			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0,
				g_aSky[nCntSky].pVtxBuff,
				0,
				sizeof(VERTEX_3D));

			// インデックスバッファを設定
			pDevice->SetIndices(g_aSky[nCntSky].pIdxBuffer);

			//**************************************************************
			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			//**************************************************************
			// テクスチャの設定
			pDevice->SetTexture(0, g_pTextureSky[g_aSky[nCntSky].nTex]);

			//**************************************************************
			// 空の描画
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, g_aSky[nCntSky].nVerti, 0, g_aSky[nCntSky].nPrim);
		}
	}
}

//=========================================================================================
// 空を設置
//=========================================================================================
void SetSky(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fSize, int nHeightDivision, int nCircleDivision,int nTex,bool bInside, bool bOutside)
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
	int					nBoth = 0;							// 裏表

	for (int nCntSky = 0; nCntSky < MAX_SKY; nCntSky++)
	{
		if (g_aSky[nCntSky].bUse == false)
		{
			// 値の保存
			g_aSky[nCntSky].pos = D3DXVECTOR3(pos.x, pos.y, pos.z);
			g_aSky[nCntSky].rot = rot;
			g_aSky[nCntSky].nHeightDivision = nHeightDivision;
			g_aSky[nCntSky].nCircleDivision = nCircleDivision;
			g_aSky[nCntSky].angle = D3DXVECTOR3((float)D3DX_PI / nHeightDivision,(float)(2 * D3DX_PI / nCircleDivision),(float)D3DX_PI / nHeightDivision);
			g_aSky[nCntSky].nVerti = nHeightVerti * nCircleVerti;
			g_aSky[nCntSky].nPrim = (nHeightDivision * (nCircleDivision + 2) - 2) * 2;
			g_aSky[nCntSky].nTex = nTex;
			g_aSky[nCntSky].bInside = bInside;
			if (bInside)
				nBoth++;
			g_aSky[nCntSky].bOutside = bOutside;
			if (bOutside)
				nBoth++;

			//**************************************************************
			// 頂点バッファの読み込み
			pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * g_aSky[nCntSky].nVerti,
				D3DUSAGE_WRITEONLY,
				FVF_VERTEX_3D,
				D3DPOOL_MANAGED,
				&g_aSky[nCntSky].pVtxBuff,
				NULL);

			//**************************************************************
			// インデックスバッファの生成
			pDevice->CreateIndexBuffer(sizeof(WORD) * 2 * (nHeightDivision * (nCircleDivision + 2) - 1),
				D3DUSAGE_WRITEONLY,
				D3DFMT_INDEX16,
				D3DPOOL_MANAGED,
				&g_aSky[nCntSky].pIdxBuffer,
				NULL);

			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
			// 頂点バッファをロックし、頂点情報へのポインタを取得
			g_aSky[nCntSky].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			for (int nCntHeight = 0, nCntVer = 0; nCntHeight <= nHeightDivision; nCntHeight++)
			{
				fRadius = fSize * sinf(g_aSky[nCntSky].angle.x * nCntHeight);
				fHeight = fSize * cosf(g_aSky[nCntSky].angle.x * nCntHeight);

				for (int nCntCircle = 0; nCntCircle <= nCircleDivision; nCntCircle++, nCntVer++)
				{
					// 頂点座標を設定
					pVtx[nCntVer].pos = D3DXVECTOR3(fRadius * sinf(g_aSky[nCntSky].angle.y * nCntCircle),
						fHeight,
						fRadius * cosf(g_aSky[nCntSky].angle.y * nCntCircle));

					//// 法線ベクトルの設定(正規化)
					//vecDir = D3DXVECTOR3(pVtx[nCntVer].pos.x, 0.0f, pVtx[nCntVer].pos.z);	// 頂点座標<=真横なのでYだけ消す
					//D3DXVec3Normalize(&vecDir, &vecDir);									//いい感じのベクトルに変換してくれる
					//pVtx[nCntVer].nor = vecDir;

					// 法線ベクトルの設定
					pVtx[nCntVer].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);


					// 頂点カラー設定
					pVtx[nCntVer].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

					// テクスチャ座標を設定
					pVtx[nCntVer].tex = D3DXVECTOR2((float)1 / nCircleDivision * nCntCircle, (float)1 / nHeightDivision * nCntHeight);
				}
			}

			// 頂点バッファのロック解除
			g_aSky[nCntSky].pVtxBuff->Unlock();
			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
			// インデックスバッファをロックし、頂点番号データへのポインタを取得
			g_aSky[nCntSky].pIdxBuffer->Lock(0, 0, (void**)&pIdx, 0);
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
			g_aSky[nCntSky].pIdxBuffer->Unlock();
			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

			g_aSky[nCntSky].bUse = true;
			break;
		}
	}
}

//=========================================================================================
// 空の情報を取得
//=========================================================================================
Sky* GetSky(void)
{
	return &g_aSky[0];
}