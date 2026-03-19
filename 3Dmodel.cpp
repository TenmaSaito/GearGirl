//================================================================================================================
//
// DirectXの3Dモデル配置のcppファイル [3DModel.cpp]
// Author : TENMA
//
//================================================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "3DModel.h"
#include "modeldata.h"
#include "mathUtil.h"
#include "Color_defs.h"

using namespace MyMathUtil;

//*************************************************************************************************
//*** マクロ定義 ***
//*************************************************************************************************
#define MAX_3DMODEL		(256)			// 設置できるモデルの最大数

//*************************************************************************************************
//*** グローバル変数 ***
//*************************************************************************************************
_3DMODEL g_aModel[MAX_3DMODEL];		// 3Dモデル情報
int g_nNum3DModel;					// 設置した3Dモデルの数

//=================================================================================================
// --- モデル初期化 ---
//=================================================================================================
void Init3DModel(void)
{
	// モデルを初期化
	ZeroMemory(&g_aModel[0], sizeof(_3DMODEL) * (MAX_3DMODEL));

	// 設置したモデルの数を初期化
	g_nNum3DModel = 0;
}

//=================================================================================================
// --- モデル終了 ---
//=================================================================================================
void Uninit3DModel(void)
{
	// 3Dmodel.cppにて動的にメモリを確保した場合(テクスチャ読み込み等)、ここで解放
}

//=================================================================================================
// --- モデル更新 ---
//=================================================================================================
void Update3DModel(void)
{
	
}

//=================================================================================================
// --- モデル描画 ---
//=================================================================================================
void Draw3DModel(void)
{
	/*** デバイスの取得 ***/
	AUTODEVICE9 Auto;							// デバイス自動解放システム
	LPDIRECT3DDEVICE9 pDevice = Auto.pDevice;	// 自動解放システムを介してデバイスを取得
	LP3DMODEL p3DModel = &g_aModel[0];	// 3Dモデルへのポインタ
	D3DXMATRIX mtxRot, mtxTrans;		// 計算用マトリックス
	D3DMATERIAL9 matDef;				// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;					// マテリアルデータへのポインタ

	// 3Dモデルの描画
	for (int nCnt3DModel = 0; nCnt3DModel < MAX_3DMODEL; nCnt3DModel++, p3DModel++)
	{
		if (p3DModel->bUse && p3DModel->bEnable)
		{ // もし使われていれば
			if (!p3DModel->bZFunc)
			{ // Zテスト無効
				SetEnableZFunction(pDevice, false);
			}

			/*** ワールドマトリックスの初期化 ***/
			D3DXMatrixIdentity(&p3DModel->mtxWorld);

			/*** 向きを反映 (※ 位置を反映する前に必ず行うこと！) ***/
			D3DXMatrixRotationYawPitchRoll(&mtxRot,
				p3DModel->rot.y,			// Y軸回転
				p3DModel->rot.x,			// X軸回転
				p3DModel->rot.z);			// Z軸回転

			D3DXMatrixMultiply(&p3DModel->mtxWorld, &p3DModel->mtxWorld, &mtxRot);

			/*** 位置を反映 (※ 向きを反映したのちに行うこと！) ***/
			D3DXMatrixTranslation(&mtxTrans,
				p3DModel->pos.x,
				p3DModel->pos.y,
				p3DModel->pos.z);

			D3DXMatrixMultiply(&p3DModel->mtxWorld, &p3DModel->mtxWorld, &mtxTrans);

			/*** ワールドマトリックスの設定 ***/
			pDevice->SetTransform(D3DTS_WORLD, &p3DModel->mtxWorld);

			// モデルデータを取得
			LPMODELDATA pModelData = GetModelData(p3DModel->nIdx3Dmodel);
			if (pModelData)
			{
				/*** 現在のマテリアルを保存 ***/
				pDevice->GetMaterial(&matDef);

				/*** マテリアルデータへのポインタを取得 ***/
				pMat = (D3DXMATERIAL*)pModelData->pBuffMat->GetBufferPointer();

				for (int nCntMat = 0; nCntMat < (int)pModelData->dwNumMat; nCntMat++)
				{
					if (p3DModel->col != COLOR_UNUSED)
					{
						D3DMATERIAL9 customMat = pMat[nCntMat].MatD3D;
						customMat.Diffuse = p3DModel->col;
						customMat.Emissive = p3DModel->col;
						if (p3DModel->bAlpha == false)
						{
							customMat.Diffuse.a = pMat[nCntMat].MatD3D.Diffuse.a;
						}

						/*** マテリアルの設定 ***/
						pDevice->SetMaterial(&customMat);
					}
					else
					{
						/*** マテリアルの設定 ***/
						pDevice->SetMaterial(&pMat[nCntMat].MatD3D);
					}

					/*** テクスチャの設定 ***/
					pDevice->SetTexture(0, pModelData->apTexture[nCntMat]);

					/*** モデル(パーツ)の描画 ***/
					pModelData->pMesh->DrawSubset(nCntMat);
				}

				/*** 保存していたマテリアルを戻す！ ***/
				pDevice->SetMaterial(&matDef);
			}

			if (!p3DModel->bZFunc)
			{ // Zテスト無効
				SetEnableZFunction(pDevice, true);
			}
		}
	}
}

//=================================================================================================
// --- モデル設置 ---
//=================================================================================================
IDX_3DMODEL Set3DModel(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nIdxModelData)
{
	LP3DMODEL p3DModel = &g_aModel[0];
	IDX_3DMODEL error = -1;

	// 3Dモデルの設置
	for (int nCnt3DModel = 0; nCnt3DModel < MAX_3DMODEL; nCnt3DModel++, p3DModel++)
	{
		if (p3DModel->bUse == false)
		{ // もし使われていなければ
			p3DModel->pos = pos;
			p3DModel->rot = rot;
			p3DModel->col = COLOR_UNUSED;
			p3DModel->nIdx3Dmodel = nIdxModelData;
			p3DModel->bUse = true;
			p3DModel->bEnable = true;
			p3DModel->bZFunc = true;
			p3DModel->bAlpha = false;
			g_nNum3DModel++;
			error = nCnt3DModel;

			break;
		}
	}

	return error;
}

//=================================================================================================
// --- 位置変更 ---
//=================================================================================================
void SetPosition3DModel(IDX_3DMODEL Idx, D3DXVECTOR3 pos)
{
	// もしインデックス外なら
	if (Idx < 0 || Idx >= MAX_3DMODEL) return;
	
	// 使用状態を取得
	LP3DMODEL p3DModel = &g_aModel[Idx];
	if (p3DModel->bUse == false) return;

	p3DModel->pos = pos;
}

//=================================================================================================
// --- 角度変更 ---
//=================================================================================================
void SetRotation3DModel(IDX_3DMODEL Idx, D3DXVECTOR3 rot)
{
	// もしインデックス外なら
	if (Idx < 0 || Idx >= MAX_3DMODEL) return;

	// 使用状態を取得
	LP3DMODEL p3DModel = &g_aModel[Idx];
	if (p3DModel->bUse == false) return;

	p3DModel->rot = rot;
}

//=================================================================================================
// --- 描画状態変更 ---
//=================================================================================================
void SetEnable3DModel(IDX_3DMODEL Idx, bool bEnable)
{
	// もしインデックス外なら
	if (Idx < 0 || Idx >= MAX_3DMODEL) return;

	// 使用状態を取得
	LP3DMODEL p3DModel = &g_aModel[Idx];
	if (p3DModel->bUse == false) return;

	p3DModel->bEnable = bEnable;
}

//=================================================================================================
// --- 色変更 ---
//=================================================================================================
void SetColor3DModel(IDX_3DMODEL Idx, D3DXCOLOR col, bool bAlpha)
{
	// もしインデックス外なら
	if (Idx < 0 || Idx >= MAX_3DMODEL) return;

	// 使用状態を取得
	LP3DMODEL p3DModel = &g_aModel[Idx];
	if (p3DModel->bUse == false) return;

	p3DModel->col = col;
	p3DModel->bAlpha = bAlpha;
}

//=================================================================================================
// --- Zテスト変更 ---
//=================================================================================================
void SetZFunc3DModel(IDX_3DMODEL Idx, bool bEnable)
{
	// もしインデックス外なら
	if (Idx < 0 || Idx >= MAX_3DMODEL) return;

	// 使用状態を取得
	LP3DMODEL p3DModel = &g_aModel[Idx];
	if (p3DModel->bUse == false) return;

	p3DModel->bZFunc = bEnable;
}

//=================================================================================================
// --- ポインタ取得 ---
//=================================================================================================
LP3DMODEL Get3DModel(IDX_3DMODEL nIdxModel)
{
	// もしインデックス外なら
	if (nIdxModel < 0 || nIdxModel >= MAX_3DMODEL) return NULL;

	// 使用状態を取得
	LP3DMODEL p3DModel = &g_aModel[nIdxModel];
	if (p3DModel->bUse == false) return NULL;

	return p3DModel;
}

//=================================================================================================
// --- 位置取得 ---
//=================================================================================================
bool GetPosition3DModel(IDX_3DMODEL nIdx, D3DXVECTOR3 *pOut)
{
	// もしインデックス外なら
	if (nIdx < 0 || nIdx >= MAX_3DMODEL) return false;
	if (pOut == NULL) return false;	// 書き出し先がNULL

	// 使用状態を取得
	LP3DMODEL p3DModel = &g_aModel[nIdx];
	if (p3DModel->bUse == false) return false;		// 未使用

	*pOut = p3DModel->pos;

	return true;
}

//=================================================================================================
// --- 角度取得 ---
//=================================================================================================
bool GetRotation3DModel(IDX_3DMODEL nIdx, D3DXVECTOR3 *pOut)
{
	// もしインデックス外なら
	if (nIdx < 0 || nIdx >= MAX_3DMODEL) return false;
	if (pOut == NULL) return false;	// 書き出し先がNULL

	// 使用状態を取得
	LP3DMODEL p3DModel = &g_aModel[nIdx];
	if (p3DModel->bUse == false) return false;

	*pOut = p3DModel->rot;

	return true;
}

//=================================================================================================
// --- 色取得 ---
//=================================================================================================
bool GetColor3DModel(IDX_3DMODEL nIdx, D3DXCOLOR *pOut)
{
	// もしインデックス外なら
	if (nIdx < 0 || nIdx >= MAX_3DMODEL) return false;
	if (pOut == NULL) return false;	// 書き出し先がNULL

	// 使用状態を取得
	LP3DMODEL p3DModel = &g_aModel[nIdx];
	if (p3DModel->bUse == false) return false;

	*pOut = p3DModel->col;

	return true;
}

//=================================================================================================
// --- 描画フラグ取得 ---
//=================================================================================================
bool GetEnable3DModel(IDX_3DMODEL nIdx)
{
	// もしインデックス外なら
	if (nIdx < 0 || nIdx >= MAX_3DMODEL) return false;

	// 使用状態を取得
	LP3DMODEL p3DModel = &g_aModel[nIdx];
	if (p3DModel->bUse == false) return false;

	return p3DModel->bEnable;
}

//=================================================================================================
// --- Zテストフラグ取得 ---
//=================================================================================================
bool GetZFunc3DModel(IDX_3DMODEL nIdx)
{
	// もしインデックス外なら
	if (nIdx < 0 || nIdx >= MAX_3DMODEL) return false;

	// 使用状態を取得
	LP3DMODEL p3DModel = &g_aModel[nIdx];
	if (p3DModel->bUse == false) return false;

	return p3DModel->bAlpha;
}

//=================================================================================================
// --- 使用状態取得 ---
//=================================================================================================
bool GetUse3DModel(IDX_3DMODEL nIdx)
{
	// もしインデックス外なら
	if (nIdx < 0 || nIdx >= MAX_3DMODEL) return false;

	// 使用状態を取得
	LP3DMODEL p3DModel = &g_aModel[nIdx];
	return p3DModel->bUse;
}