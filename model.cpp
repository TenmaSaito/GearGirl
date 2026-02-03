// =================================================
// 
// モデル処理[model.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "main.h"
#include "camera.h"
#include "debugproc.h"
#include "fade.h"
#include "field.h"
#include "game.h"
#include "model.h"
#include "input.h"
#include "mathUtil.h"
#include "3Dmodel.h"
#include "texture.h"

using  namespace MyMathUtil;

// =================================================
// マクロ定義
#define	MODEL_TXT	"data\\Scripts\\model.txt"	// モデルの外部ファイル
#define STR_SUCCESS(Realize)			(Realize != NULL)	// strstrでの成功判定

// =================================================
// グローバル変数
LPDIRECT3DTEXTURE9 g_pTextureModel[128] = {};	// 読み込むテクスチャ
Model g_Model[MAX_MODEL];			// モデルの情報
ModelInfo g_ModelInfo[MAX_MODEL];	// モデル位置の情報
int g_nNumTexture;					// テクスチャの数
char g_TextureName[MAX_MODEL][128];	// テクスチャのファイル名
int g_nNumModel;					// モデルの種類数
char g_ModelName[MAX_MODEL][128];	// モデルのファイル名
int g_nNumObj;						// オブジェクトの総数

// =================================================
// 初期化処理
// =================================================
void InitModel(void)
{
	// 各種変数を初期化(0固定)
	for (int nCntModel = 0; nCntModel < MAX_MODEL; nCntModel++)
	{
		g_ModelInfo[nCntModel].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 位置
		g_ModelInfo[nCntModel].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 向き
		g_ModelInfo[nCntModel].move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 移動量
		g_ModelInfo[nCntModel].nType = 0;		// 種類
		g_ModelInfo[nCntModel].nShadow = -1;		// 種類
		g_ModelInfo[nCntModel].bUse = false;		// 未使用
		g_ModelInfo[nCntModel].bDisp = false;	// 非描画
	}
}

// =================================================
// 終了処理
// =================================================
void UninitModel(void)
{
	for (int nCntModel = 0; nCntModel < MAX_MODEL; nCntModel++)
	{// モデルの数分破棄
		if (g_ModelInfo[nCntModel].bUse == true)
		{// 使用中なら
			// メッシュの破棄
			if (g_Model[nCntModel].pMesh != NULL)
			{
				g_Model[nCntModel].pMesh->Release();
				g_Model[nCntModel].pMesh = NULL;
			}

			// マテリアルの破棄
			if (g_Model[nCntModel].pBuffMat != NULL)
			{
				g_Model[nCntModel].pBuffMat->Release();
				g_Model[nCntModel].pBuffMat = NULL;
			}

			// テクスチャの破棄
			for (int nCntMat = 0; nCntMat < (int)g_Model[nCntModel].dwNumMat; nCntMat++)
			{
				if (g_Model[nCntModel].apTexture[nCntMat] != NULL)
				{
					g_Model[nCntModel].apTexture[nCntMat]->Release();
					g_Model[nCntModel].apTexture[nCntMat] = NULL;
				}
			}
		}
	}
}

// =================================================
// 更新処理
// =================================================
void UpdateModel(void)
{

}

// =================================================
// 描画処理
// =================================================
void DrawModel(void)
{
	LPDIRECT3DDEVICE9 pDevice;		// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス
	D3DMATERIAL9 matDef;			// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;				// マテリアルデータへのポインタ

	for (int nCntModel = 0; nCntModel < MAX_MODEL; nCntModel++)
	{
		if (g_ModelInfo[nCntModel].bUse == true)
		{
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_ModelInfo[nCntModel].mtxWorld);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot,
				g_ModelInfo[nCntModel].rot.y, g_ModelInfo[nCntModel].rot.x, g_ModelInfo[nCntModel].rot.z);
			D3DXMatrixMultiply(&g_ModelInfo[nCntModel].mtxWorld, &g_ModelInfo[nCntModel].mtxWorld, &mtxRot);	// かけ合わせる

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans,
				g_ModelInfo[nCntModel].pos.x, g_ModelInfo[nCntModel].pos.y, g_ModelInfo[nCntModel].pos.z);
			D3DXMatrixMultiply(&g_ModelInfo[nCntModel].mtxWorld, &g_ModelInfo[nCntModel].mtxWorld, &mtxTrans);	// かけ合わせる

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_ModelInfo[nCntModel].mtxWorld);

			// 現在のマテリアルを取得(保存)
			pDevice->GetMaterial(&matDef);

			// マテリアルデータへのポインタを取得
			pMat = (D3DXMATERIAL*)g_Model[g_ModelInfo[nCntModel].nType].pBuffMat->GetBufferPointer();

			for (int nCntMat = 0; nCntMat < (int)g_Model[g_ModelInfo[nCntModel].nType].dwNumMat; nCntMat++)
			{
				// マテリアルの設定
				pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

				// テクスチャの設定
				pDevice->SetTexture(0, g_Model[g_ModelInfo[nCntModel].nType].apTexture[nCntMat]);

				// モデルの描画
				g_Model[g_ModelInfo[nCntModel].nType].pMesh->DrawSubset(nCntMat);
			}

			// 保存していたマテリアルを戻す
			pDevice->SetMaterial(&matDef);
		}
	}

	// デバイスの破棄
	EndDevice();
}

// =================================================
// モデルの設定処理
// =================================================
void SetModel(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nNumIdx, int nUseShadow)
{
	for (int nCntModel = 0; nCntModel < MAX_MODEL; nCntModel++)
	{
		if (g_ModelInfo[nCntModel].bUse == false)
		{
			// 各引数を構造体の変数に代入
			g_ModelInfo[nCntModel].pos = pos;
			g_ModelInfo[nCntModel].rot = rot;
			g_ModelInfo[nCntModel].nType = nNumIdx;
			g_ModelInfo[nCntModel].bUse = true;
			g_ModelInfo[nCntModel].bDisp = true;

			if (nUseShadow == 0)
			{
				g_ModelInfo[nCntModel].bUseShadow = true;
			}
			else
			{
				g_ModelInfo[nCntModel].bUseShadow = false;
			}

			// 影を設定
			//g_ModelInfo[nCntModel].nShadow = SetShadow();

			break;
		}
	}
}

// =================================================
// モデル情報の譲渡
// =================================================
Model* GetModel(void)
{
	return &g_Model[0];
}

// =================================================
// モデル情報の譲渡
// =================================================
ModelInfo* GetModelInfo(void)
{
	return &g_ModelInfo[0];
}

// =================================================
// モデルの読み込む処理
// =================================================
bool LoadModel(void)
{	
	// === フィールドの情報を格納する変数 === //
	int nIdxTexField = 0;	// テクスチャのタイプ
	int aIdxTexture[100] = {};	// テクスチャのタイプ
	int Xdevide, Ydevide = 0;	// フィールドの分割数
	int Xsize, Ysize = 0;		// フィールドのサイズ
	int nCntField = 0;

	// === モデル用の変数を用意 === //
	MyMathUtil::INT_VECTOR3 pos;
	MyMathUtil::INT_VECTOR3 rot;

	int nNumIdx = 0;			// モデルファイル名のインデックス
	int nNumModel = 0;			// モデル数
	char Realize[128];			// 判別用の変数
	char* pEqual = NULL;		// =を消去するためのポインタ
	int nCntModel = 0;			// 読み込むモデルファイル名をカウントする
	int nCntTexture = 0;		// 読み込むテクスチャのファイル名をカウントする
	int nUseShadow = 0;			// 影を使用するかどうか用の変数

	int nNumVtx[MAX_MODEL];		// 頂点数
	DWORD dwSizeFVF[MAX_MODEL];	// 頂点フォーマットのサイズ
	BYTE* pVtxBuff;					// 頂点バッファへのポインタ
	LPDIRECT3DDEVICE9 pDevice;		// デバイスへのポインタ
	D3DXMATERIAL* pMat;				// マテリアルへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// ステージ情報を読み込み
	FILE* pFile;
	pFile = NULL;

	pFile = fopen(MODEL_TXT, "r");

	if (pFile != NULL)
	{// ファイルを開いた
		while (1)
		{
			fgets(&Realize[0], sizeof Realize, pFile);	// 最初に比較用文字の読み込み

			if (JudgeComent(&Realize[0]) == true)
			{// 何かの情報を読み込む合図を取得したとき
				if (strcmp(&Realize[0], "SCRIPT") == 0)
				{

				}
				if (strstr(&Realize[0], "NUM_TEXTURE") != NULL)
				{// テクスチャの数を代入
					// 「=」の場所を見つける
					pEqual = strstr(Realize, "=");

					if (pEqual != NULL)
					{
						// アドレスを1こずらす
						pEqual++;

						// テクスチャの数を読み込む
						(void)sscanf(pEqual, "%d", &g_nNumTexture);
					}

				}
				if (strstr(&Realize[0], "TEXTURE_FILENAME") != NULL)
				{// テクスチャのファイル名を代入
					while (1)
					{
						if (nCntTexture < g_nNumTexture)
						{// 上で代入したテクスチャ数よりも、読み込んだファイル数が少ないとき
							// 「=」の場所を見つける
							pEqual = strstr(Realize, "=");

							if (pEqual != NULL)
							{
								// アドレスを1こずらす
								pEqual++;

								// テクスチャの数を読み込む
								(void)sscanf(pEqual, "%s", &g_TextureName[nCntTexture][0]);

								LoadTexture(&g_TextureName[nCntTexture][0], &aIdxTexture[nCntTexture]);

								// テクスチャの読み込み
								D3DXCreateTextureFromFile(pDevice,
									&g_TextureName[nCntTexture][0],
									&g_pTextureModel[nCntTexture]);

								// テクスチャのファイル名をカウント
								nCntTexture++;
								break;
							}

						}
					}
				}
			}
			if (strstr(&Realize[0], "FIELDSET") != NULL)
			{// フィールドの情報を代入
				while (1)
				{
					fgets(&Realize[0], sizeof Realize, pFile);	// 最初に比較用文字の読み込み

					if (strstr(&Realize[0], "TEXTYPE") != NULL)
					{
						// 「=」の場所を見つける
						pEqual = strstr(Realize, "=");

						if (pEqual != NULL)
						{
							// アドレスを1こずらす
							pEqual++;

							// テクスチャの数を読み込む
							(void)sscanf(pEqual, "%d", &nIdxTexField);
						}
					}
					if (strstr(&Realize[0], "POS") != NULL)
					{
						pEqual = strstr(Realize, "=");

						if (pEqual != NULL)
						{
							// アドレスを1こずらす
							pEqual++;

							// モデルの数を読み込む
							(void)sscanf(pEqual, "%d %d %d", &pos.x, &pos.y, &pos.z);
						}
					}
					if (strstr(&Realize[0], "ROT") != NULL)
					{
						pEqual = strstr(Realize, "=");

						if (pEqual != NULL)
						{
							// アドレスを1こずらす
							pEqual++;

							// モデルの数を読み込む
							(void)sscanf(pEqual, "%d %d %d", &rot.x, &rot.y, &rot.z);
						}
					}
					if (strstr(&Realize[0], "BLOCK") != NULL)
					{
						pEqual = strstr(Realize, "=");

						if (pEqual != NULL)
						{
							// アドレスを1こずらす
							pEqual++;

							// モデルの数を読み込む
							(void)sscanf(pEqual, "%d %d", &Xdevide, &Ydevide);
						}
					}
					if (strstr(&Realize[0], "SIZE") != NULL)
					{
						pEqual = strstr(Realize, "=");

						if (pEqual != NULL)
						{
							// アドレスを1こずらす
							pEqual++;

							// モデルの数を読み込む
							(void)sscanf(pEqual, "%d %d", &Xsize, &Ysize);
						}
					}
					if (strstr(&Realize[0], "END_FIELDSET") != NULL)
					{
						SetField(INTToFloat(pos), VECNULL, INTToFloat(rot), (float)Xsize, (float)Ysize, aIdxTexture[nIdxTexField], (float)Xdevide, (float)Ydevide,D3DCULL_CCW);
						nCntField++;
						break;
					}
				}

			}
			if (strstr(&Realize[0], "NUM_MODEL") != NULL)
			{// モデルの数を代入
				// 「=」の場所を見つける
				pEqual = strstr(Realize, "=");

				if (pEqual != NULL)
				{
					// アドレスを1こずらす
					pEqual++;

					// モデルの数を読み込む
					(void)sscanf(pEqual, "%d", &g_nNumModel);
				}

			}
			if (strstr(&Realize[0], "MODEL_FILENAME") != NULL)
			{// モデルのファイル名を代入
				// 「=」の場所を見つける
				pEqual = strstr(Realize, "=");

				if (pEqual != NULL)
				{
					HRESULT hr;		// エラー文用

					// アドレスを1こずらす
					pEqual++;

					// モデルの数を読み込む
					sscanf(pEqual, "%s", &g_ModelName[nCntModel][0]);

					g_Model[nCntModel].dwNumMat = 0;

					//Xファイルの読み込み
					hr = D3DXLoadMeshFromX(&g_ModelName[nCntModel][0],
						D3DXMESH_SYSTEMMEM,
						pDevice,
						NULL,
						&g_Model[nCntModel].pBuffMat,
						NULL,
						&g_Model[nCntModel].dwNumMat,
						&g_Model[nCntModel].pMesh);

					if (FAILED(hr))
					{ // 読み込み失敗時
						MyMathUtil::GenerateMessageBox(MB_ICONERROR,
							"ERROR!",
							"Xファイルの読み込みに失敗しました!\n対象パス : %s",
							&g_ModelName[nCntModel][0]);
					}
					else
					{
						//マテリアルデータへのポインタを取得
						pMat = (D3DXMATERIAL*)g_Model[nCntModel].pBuffMat->GetBufferPointer();

						for (int nCntMat = 0; nCntMat < (int)g_Model[nCntModel].dwNumMat; nCntMat++)
						{
							if (pMat[nCntMat].pTextureFilename != NULL)
							{// テクスチャファイルが存在する
								 //テクスチャを読み込む
								D3DXCreateTextureFromFile(pDevice,
									pMat[nCntMat].pTextureFilename,
									&g_Model[nCntModel].apTexture[nCntMat]);
							}
						}

						//モデル構造体の中身の初期化
						g_Model[nCntModel].vtxMin = D3DXVECTOR3(10000, 10000, 10000);
						g_Model[nCntModel].vtxMax = D3DXVECTOR3(-10000, -10000, -10000);

						//頂点数を取得
						nNumVtx[nCntModel] = g_Model[nCntModel].pMesh->GetNumVertices();

						// 頂点フォーマットのサイズを取得
						dwSizeFVF[nCntModel] = D3DXGetFVFVertexSize(g_Model[nCntModel].pMesh->GetFVF());

						//頂点バッファをロック
						g_Model[nCntModel].pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

						for (int nCntVtx = 0; nCntVtx < nNumVtx[nCntModel]; nCntVtx++)
						{
							D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;

							// すべての頂点を比較してモデルの最小値、最大値を抜き出す
							// 最小値
							if (vtx.x <= g_Model[nCntModel].vtxMin.x)
							{
								g_Model[nCntModel].vtxMin.x = vtx.x;
							}
							if (vtx.y <= g_Model[nCntModel].vtxMin.y)
							{
								g_Model[nCntModel].vtxMin.y = vtx.y;
							}
							if (vtx.z <= g_Model[nCntModel].vtxMin.z)
							{
								g_Model[nCntModel].vtxMin.z = vtx.z;
							}
							// 最大値
							if (vtx.x >= g_Model[nCntModel].vtxMax.x)
							{
								g_Model[nCntModel].vtxMax.x = vtx.x;
							}
							if (vtx.y >= g_Model[nCntModel].vtxMax.y)
							{
								g_Model[nCntModel].vtxMax.y = vtx.y;
							}
							if (vtx.z >= g_Model[nCntModel].vtxMax.z)
							{
								g_Model[nCntModel].vtxMax.z = vtx.z;
							}

							// 頂点フォーマットのサイズ分進める
							pVtxBuff += dwSizeFVF[nCntModel];
						}

						//頂点バッファをアンロック
						g_Model[nCntModel].pMesh->UnlockVertexBuffer();
					}

					nCntModel++;
				}
			}
			if (STR_SUCCESS(strstr(&Realize[0], "MODELSET")))
			{
				while (1)
				{
					fgets(&Realize[0], sizeof Realize, pFile);	// 最初に比較用文字の読み込み

					if (JudgeComent(&Realize[0]) == true)
					{
						if (strstr(&Realize[0], "TYPE") != NULL)
						{
							pEqual = strstr(Realize, "=");

							if (pEqual != NULL)
							{
								// アドレスを1こずらす
								pEqual++;

								// モデルの数を読み込む
								(void)sscanf(pEqual, "%d", &nNumIdx);
							}
						}
						if (strstr(&Realize[0], "POS") != NULL)
						{
							pEqual = strstr(Realize, "=");

							if (pEqual != NULL)
							{
								// アドレスを1こずらす
								pEqual++;

								// モデルの数を読み込む
								(void)sscanf(pEqual, "%d %d %d", &pos.x, &pos.y, &pos.z);
							}
						}
						if (strstr(&Realize[0], "ROT") != NULL)
						{
							pEqual = strstr(Realize, "=");

							if (pEqual != NULL)
							{
								// アドレスを1こずらす
								pEqual++;

								// モデルの数を読み込む
								(void)sscanf(pEqual, "%d %d %d", &rot.x, &rot.y, &rot.z);
							}
						}
						if (strstr(&Realize[0], "SHADOW") != NULL)
						{
							pEqual = strstr(Realize, "=");

							if (pEqual != NULL)
							{
								// アドレスを1こずらす
								pEqual++;

								// モデルの数を読み込む
								(void)sscanf(pEqual, "%d", &nUseShadow);
							}
						}
						if (strstr(&Realize[0], "END_MODELSET") != NULL)
						{
							//Set3DModel
							SetModel(MyMathUtil::INTToFloat(pos), MyMathUtil::DegreeToRadian(MyMathUtil::INTToFloat(rot)), nNumIdx, nUseShadow);
							break;
						}
					}
				}
			}
			else if (strcmp(&Realize[0], "END_SCRIPT") == 0)
			{// スクリプト読み込みを終了する
				break;
			}
		}
		fclose(pFile);	// ファイルを閉じる
	}
	else
	{// ファイルオープン失敗時
		// デバイスの破棄
		EndDevice();
		return false;
	}

	// デバイスの破棄
	EndDevice();

	return true;
}

// =================================================
// スクリプト読み込みでのコメントアウトの処理
// =================================================
bool JudgeComent(char* pStr)
{
	// アドレス取得用のchar型のポインタを用意
	char* pTrash = {};

	if (strncmp(pStr, "#", 1) != 0 && strncmp(pStr, "\n", 1) != 0)
	{// 文頭が、コメントアウト、改行でなかった場合
		// #があるかを判別
		pTrash = strstr(pStr, "#");

		if (pTrash != NULL)
		{// #が存在する
			// それ以降の文字列を消去
			strcpy(pTrash, "");
		}

		while (1)
		{// tabが存在する場合回し続ける
			// tabのアドレスを取得
			pTrash = strstr(pStr, "\t");

			if (pTrash != NULL)
			{// tabが存在する

				if (pTrash + 1 == NULL)
				{// 文末にあった時
					// 後ろを空白にする
					strcpy(pTrash + 1, "");
				}
				else
				{// 文頭にあった時
					// tab以降の文字列をtabがあったところに代入
					strcpy(pTrash, pTrash + 1);
				}
			}
			else if (pTrash == NULL)
			{// tabが存在しなくなったとき
				break;
			}
		}

		return true;
	}
	else
	{// コメントアウト、改行が文頭だった場合
		return false;
	}
}

// =================================================
// モデルの当たり判定
// =================================================
bool CollisionModel(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove)
{
	// 当たっているかどうかをbool 型で返す
	bool bLand = false;

	for (int nCntModel = 0; nCntModel < g_nNumModel; nCntModel++)
	{
		if (g_ModelInfo[nCntModel].bUse == true)
		{
			/*** オブジェクトにめり込んでるか判定 ***/
			if (pPos->x >= g_ModelInfo[nCntModel].pos.x + g_Model[g_ModelInfo[nCntModel].nType].vtxMin.x && pPos->x <= g_ModelInfo[nCntModel].pos.x + g_Model[g_ModelInfo[nCntModel].nType].vtxMax.x
				&& pPos->y <= g_ModelInfo[nCntModel].pos.y + g_Model[g_ModelInfo[nCntModel].nType].vtxMax.y && pPos->y >= g_ModelInfo[nCntModel].pos.y + g_Model[g_ModelInfo[nCntModel].nType].vtxMin.y
				&& pPos->z >= g_ModelInfo[nCntModel].pos.z + g_Model[g_ModelInfo[nCntModel].nType].vtxMin.z && pPos->z <= g_ModelInfo[nCntModel].pos.z + g_Model[g_ModelInfo[nCntModel].nType].vtxMax.z)
			{
				if (pPosOld->x <= g_ModelInfo[nCntModel].pos.x + g_Model[g_ModelInfo[nCntModel].nType].vtxMin.x)
				{/** 過去の位置が、モデルのXの最小値よりも小さい位置にいた場合 **/
					pPos->x = g_ModelInfo[nCntModel].pos.x + g_Model[g_ModelInfo[nCntModel].nType].vtxMin.x;
				}
				else if (pPosOld->x >= g_ModelInfo[nCntModel].pos.x + g_Model[g_ModelInfo[nCntModel].nType].vtxMax.x)
				{/** 過去の位置が、モデルのXの最大値よりも大さい位置にいた場合 **/
					pPos->x = g_ModelInfo[nCntModel].pos.x + g_Model[g_ModelInfo[nCntModel].nType].vtxMax.x;
				}

				if (pPosOld->y < g_ModelInfo[nCntModel].pos.y + g_Model[g_ModelInfo[nCntModel].nType].vtxMin.y)
				{/** 過去の位置が、モデルのYの最小値よりも小さい位置にいた場合 **/
					pPos->y = g_ModelInfo[nCntModel].pos.y + g_Model[g_ModelInfo[nCntModel].nType].vtxMin.y;
				}
				else if (pPosOld->y >= g_ModelInfo[nCntModel].pos.y + g_Model[g_ModelInfo[nCntModel].nType].vtxMax.y)
				{/** 過去の位置が、モデルのYの最大値よりも大さい位置にいた場合 **/
					pPos->y = g_ModelInfo[nCntModel].pos.y + g_Model[g_ModelInfo[nCntModel].nType].vtxMax.y;
					bLand = true;
					pMove->y = 0.0f;
				}

				if (pPosOld->z <= g_ModelInfo[nCntModel].pos.z + g_Model[g_ModelInfo[nCntModel].nType].vtxMin.z)
				{/** 過去の位置が、モデルのZの最小値よりも小さい位置にいた場合 **/
					pPos->z = g_ModelInfo[nCntModel].pos.z + g_Model[g_ModelInfo[nCntModel].nType].vtxMin.z;
				}
				else if (pPosOld->z >= g_ModelInfo[nCntModel].pos.z + g_Model[g_ModelInfo[nCntModel].nType].vtxMax.z)
				{/** 過去の位置が、モデルのZの最大値よりも大さい位置にいた場合 **/
					pPos->z = g_ModelInfo[nCntModel].pos.z + g_Model[g_ModelInfo[nCntModel].nType].vtxMax.z;
				}
			}
		}
	}

	return bLand;
}