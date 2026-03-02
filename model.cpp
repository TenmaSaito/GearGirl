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
#include "mesh.h"
#include "input.h"
#include "mathUtil.h"
#include "player.h"
#include "3Dmodel.h"
#include "texture.h"
#include "item.h"

using namespace MyMathUtil;

// =================================================
// マクロ定義
#define	MODEL_TXT	"data\\Scripts\\model.txt"	// モデルの外部ファイル
#define STR_SUCCESS(Realize)			(Realize != NULL)	// strstrでの成功判定
#define LOAD_ITEM	"ITEMSET"			// Item設置

// =================================================
// グローバル変数
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
	AUTODEVICE9 Auto;								// デバイスの自動取得
	LPDIRECT3DDEVICE9 pDevice = Auto.pDevice;		// デバイスへのポインタ

	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス
	D3DMATERIAL9 matDef;			// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;				// マテリアルデータへのポインタ

	/*** アルファテストを有効にする ***/
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// アルファテストを有効
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// 基準値よりも大きい場合にZバッファに書き込み
	pDevice->SetRenderState(D3DRS_ALPHAREF, 60);				// 基準値

	for (int nCntModel = 0; nCntModel < MAX_MODEL; nCntModel++)
	{
		if (g_ModelInfo[nCntModel].bUse == true)
		{
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

	/*** アルファテストを無効にする ***/
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);		// アルファテストを無効化
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);	// 無条件にZバッファに書き込み
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);					// 基準値
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

			// 変化しないマトリックスをここであらかじめ計算
			MyMathUtil::CalcWorldMatrix(&g_ModelInfo[nCntModel].mtxWorld,
				g_ModelInfo[nCntModel].pos,
				g_ModelInfo[nCntModel].rot);

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
	// === 全項目共通で使用可能な変数 === //
	MyMathUtil::INT_VECTOR3 pos;
	MyMathUtil::INT_VECTOR3 rot;

	// === スカイ用の変数を用意 === //
	int nIdxTexSky = 0;					// 空用のテクスチャのインデックス
	float fMoveSky = 0.0f;				// 空の移動量
	int nXdevideSky, nYdevideSky = 0;	// 空の分割数
	int nRadiusSky = 0;					// 空のサイズ

	// === 雲用の変数を用意 === // 
	int nIdxTexCloud = 0;	// 雲のテクスチャのインデックス
	int nRadiusCloud = 0;	// 雲の半径
	int nHeightCloud = 0;	// 雲のテクスチャの高さ
	int nXdevideCloud, nYdevideCloud = 0;	// 雲のテクスチャの分割数

	// === フィールドの情報を格納する変数 === //
	D3DXVECTOR3 posField;
	int nIdxTexField = 0;	// テクスチャのタイプ
	int aIdxTexture[100] = {};	// テクスチャのタイプ
	int nXdevideField, nYdevideField = 0;	// フィールドの分割数
	int nXsize, nYsize = 0;		// フィールドのサイズ
	int nCntField = 0;

	// === モデル用の変数を用意 === //
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

								// テクスチャの読み込み
								LoadTexture(&g_TextureName[nCntTexture][0], &aIdxTexture[nCntTexture]);

								// テクスチャのファイル名をカウント
								nCntTexture++;
								break;
							}

						}
					}
				}
			}
			if (strstr(&Realize[0], "SKYSET") != NULL)
			{// 空の情報を代入
				while (1)
				{
					fgets(&Realize[0], sizeof Realize, pFile);	// 最初に比較用文字の読み込み

					if (strstr(&Realize[0], "TEXTYPE") != NULL)
					{// テクスチャタイプを読み込む処理
						pEqual = strstr(Realize, "=");

						if (pEqual != NULL)
						{
							// アドレスを1こずらす
							pEqual++;

							// テクスチャのインデックスを読み込む
							(void)sscanf(pEqual, "%d", &nIdxTexSky);
						}
					}
					if (strstr(&Realize[0], "MOVE") != NULL)
					{// テクスチャの移動量を読み込む処理
						pEqual = strstr(Realize, "=");

						if (pEqual != NULL)
						{
							// アドレスを1こずらす
							pEqual++;

							// テクスチャの移動量を読み込む
							(void)sscanf(pEqual, "%f", &fMoveSky);
						}
					}
					if (strstr(&Realize[0], "SIZE") != NULL)
					{// 半径を読み込む処理
						pEqual = strstr(Realize, "=");

						if (pEqual != NULL)
						{
							// アドレスを1こずらす
							pEqual++;

							// 半径を読み込む
							(void)sscanf(pEqual, "%d", &nRadiusSky);
						}
					}
					if (strstr(&Realize[0], "BLOCK") != NULL)
					{// 分割数を読み込む処理
						pEqual = strstr(Realize, "=");

						if (pEqual != NULL)
						{
							// アドレスを1こずらす
							pEqual++;

							// 分割数を読み込む
							(void)sscanf(pEqual, "%d %d", &nXdevideSky, &nYdevideSky);
						}
					}
					if (strstr(&Realize[0], "END_SKYSET") != NULL)
					{
						// 代入した情報をメッシュリングドームに反映
						SetMeshDome(D3DXVECTOR3(0.0f, 0.0f, 0.0f), VECNULL,(float)nRadiusSky, nYdevideSky, nXdevideSky, D3DCULL_CCW, aIdxTexture[nIdxTexSky]);
						break;
					}
				}
			}
			if (strstr(&Realize[0], "CLOUDSET") != NULL)
			{// 雲の情報を代入
				while (1)
				{
					fgets(&Realize[0], sizeof Realize, pFile);	// 最初に比較用文字の読み込み

					if (strstr(&Realize[0], "TEXTYPE") != NULL)
					{// テクスチャタイプを読み込む処理
						pEqual = strstr(Realize, "=");

						if (pEqual != NULL)
						{
							// アドレスを1こずらす
							pEqual++;

							// テクスチャのインデックスを読み込む
							(void)sscanf(pEqual, "%d", &nIdxTexCloud);
						}
					}
					if (strstr(&Realize[0], "SIZE") != NULL)
					{// 半径を読み込む処理
						pEqual = strstr(Realize, "=");

						if (pEqual != NULL)
						{
							// アドレスを1こずらす
							pEqual++;

							// 半径を読み込む
							(void)sscanf(pEqual, "%d", &nRadiusCloud);
						}
					}
					if (strstr(&Realize[0], "HEIGHT") != NULL)
					{// テクスチャの移動量を読み込む処理
						pEqual = strstr(Realize, "=");

						if (pEqual != NULL)
						{
							// アドレスを1こずらす
							pEqual++;

							// テクスチャの移動量を読み込む
							(void)sscanf(pEqual, "%d", &nHeightCloud);
						}
					}
					if (strstr(&Realize[0], "BLOCK") != NULL)
					{// 分割数を読み込む処理
						pEqual = strstr(Realize, "=");

						if (pEqual != NULL)
						{
							// アドレスを1こずらす
							pEqual++;

							// 分割数を読み込む
							(void)sscanf(pEqual, "%d %d", &nXdevideCloud, &nYdevideCloud);
						}
					}
					if (strstr(&Realize[0], "END_CLOUDSET") != NULL)
					{
						// 代入した情報をメッシュリングドームに反映
						SetMeshCylinder(D3DXVECTOR3(0.0f, 0.0f, 0.0f), VECNULL, (float)nRadiusCloud, (float)nHeightCloud, nYdevideCloud, nXdevideCloud, D3DCULL_CW, aIdxTexture[nIdxTexCloud]);
						break;
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

							// テクスチャのインデックスを読み込む
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

							// 地面の位置を読み込む
							(void)sscanf(pEqual, "%f %f %f", &posField.x, &posField.y, &posField.z);
						}
					}
					if (strstr(&Realize[0], "ROT") != NULL)
					{
						pEqual = strstr(Realize, "=");

						if (pEqual != NULL)
						{
							// アドレスを1こずらす
							pEqual++;

							// 地面の向きを読み込む
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

							// 分割数を読み込む
							(void)sscanf(pEqual, "%d %d", &nXdevideField, &nYdevideField);
						}
					}
					if (strstr(&Realize[0], "SIZE") != NULL)
					{
						pEqual = strstr(Realize, "=");

						if (pEqual != NULL)
						{
							// アドレスを1こずらす
							pEqual++;

							// 縦横のサイズを読み込む
							(void)sscanf(pEqual, "%d %d", &nXsize, &nYsize);
						}
					}
					if (strstr(&Realize[0], "END_FIELDSET") != NULL)
					{
						D3DXVECTOR3 rotVec = DegreeToRadian(INTToFloat(rot));
						SetField(posField, VECNULL, rotVec, (float)nXsize, (float)nYsize, aIdxTexture[nIdxTexField], (float)nXdevideField, (float)nYdevideField, D3DCULL_CCW);
						nCntField++;
						break;
					}
				}

			}

			if (strstr(&Realize[0], LOAD_ITEM) != NULL)
			{// アイテムの情報を代入
				D3DXVECTOR3 position;
				INT_VECTOR3 rotation;
				ITEMTYPE Type;
				int nVisible = 0;
				bool bVisible[2] = {};

				while (1)
				{
					fgets(&Realize[0], sizeof Realize, pFile);	// 最初に比較用文字の読み込み

					if (strstr(&Realize[0], "ITEMTYPE") != NULL)
					{
						// 「=」の場所を見つける
						pEqual = strstr(Realize, "=");

						if (pEqual != NULL)
						{
							// アドレスを1こずらす
							pEqual++;

							// アイテムのインデックスを読み込む
							(void)sscanf(pEqual, "%d", &Type);
						}
					}
					if (strstr(&Realize[0], "POS") != NULL)
					{
						pEqual = strstr(Realize, "=");

						if (pEqual != NULL)
						{
							// アドレスを1こずらす
							pEqual++;

							// 地面の位置を読み込む
							(void)sscanf(pEqual, "%f %f %f", &position.x, &position.y, &position.z);
						}
					}
					if (strstr(&Realize[0], "ROT") != NULL)
					{
						pEqual = strstr(Realize, "=");

						if (pEqual != NULL)
						{
							// アドレスを1こずらす
							pEqual++;

							// 地面の向きを読み込む
							(void)sscanf(pEqual, "%d %d %d", &rotation.x, &rotation.y, &rotation.z);
						}
					}
					if (strstr(&Realize[0], "VISIBLE") != NULL)
					{
						pEqual = strstr(Realize, "=");

						if (pEqual != NULL)
						{
							// アドレスを1こずらす
							pEqual++;

							// 地面の向きを読み込む
							(void)sscanf(pEqual, "%d", &nVisible);
							if (nVisible == 0)
							{
								bVisible[0] = true;
								bVisible[1] = true;
							}
							else if (nVisible == 1)
							{
								bVisible[0] = true;
								bVisible[1] = false;
							}
							else
							{
								bVisible[0] = false;
								bVisible[1] = true;
							}
						}
					}
					if (strstr(&Realize[0], "END_ITEMSET") != NULL)
					{
						D3DXVECTOR3 rotationF = DegreeToRadian(INTToFloat(rotation));
						SetItem(position, rotationF, Type, bVisible[0], bVisible[1]);
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
								// テクスチャ読み込みの際に絶対パスがないかどうか
								CheckPath(pMat[nCntMat].pTextureFilename);

								 //テクスチャを読み込む
								HRESULT hr =  D3DXCreateTextureFromFile(pDevice,
									pMat[nCntMat].pTextureFilename,
									&g_Model[nCntModel].apTexture[nCntMat]);

								if (nCntModel == 31)
								{
									int n = 0;
								}
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
bool CollisionModel
(
	D3DXVECTOR3 *pPos,
	D3DXVECTOR3 *pPosOld,
	D3DXVECTOR3 *pMove,
	float fRadius,
	float fHeight
)
{
	// 当たっているかどうかをbool型で返す
	bool bLand = false;

	Player* pPlayer = GetPlayer();

	for (int nCntModel = 0; nCntModel < MAX_MODEL; nCntModel++)
	{
		if (g_ModelInfo[nCntModel].bUse == true)
		{
			Model* pObjInfo = &g_Model[g_ModelInfo[nCntModel].nType];
			ModelInfo* pObject = &g_ModelInfo[nCntModel];

			D3DXVECTOR3 posVtx[4] = {};		// オブジェクトの四辺の頂点
			D3DXVECTOR3 vecMove = D3DXVECTOR3_NULL;		// 移動ベクトル
			D3DXVECTOR3 vecLine = D3DXVECTOR3_NULL;		// 境界線ベクトル
			D3DXVECTOR3 vecLineA[4] = {};	// 境界線ベクトル
			D3DXVECTOR3 vecToPos = D3DXVECTOR3_NULL;	// 位置と境界線のはじめを結んだベクトル
			D3DXVECTOR3 vecToPosA[4] = {};	// 位置と境界線のはじめを結んだベクトル
			D3DXVECTOR3 vecToPosOld = D3DXVECTOR3_NULL;	// 過去位置と境界線のはじめを結んだベクトル 
			D3DXVECTOR3 vecLineNor = D3DXVECTOR3_NULL;	// 正規化した境界線ベクトル
			float fVecPos = 0.0f;
			float fVecPosA[4] = {};
			float fVecPosOld = 0.0f;
			float fPosToMove = 0.0f;					// vecToPosとの外積
			float fLineToMove = 0.0f;					// vecLineとの外積
			float fRate = 0.0f;							// 面積比率
			float fVecPosToNor = 0.0f;					// 逆法線との外積

			if (pPos->y >= pObject->pos.y + pObjInfo->vtxMin.y - 10.0f
				&& pPos->y <= pObject->pos.y + pObjInfo->vtxMax.y)
			{
				D3DXVECTOR3 Length;
				float fLength = 0.0f;
				float fAngle = 0.0f;
				bool bCollision[4] = {};

				posVtx[0].x = pObjInfo->vtxMin.x;
				posVtx[0].y = 0;
				posVtx[0].z = pObjInfo->vtxMin.z;

				D3DXVec3TransformCoord(&posVtx[0], &posVtx[0], &pObject->mtxWorld);

				posVtx[1].x = pObjInfo->vtxMax.x;
				posVtx[1].y = 0;
				posVtx[1].z = pObjInfo->vtxMin.z;

				D3DXVec3TransformCoord(&posVtx[1], &posVtx[1], &pObject->mtxWorld);

				posVtx[2].x = pObjInfo->vtxMax.x;
				posVtx[2].y = 0;
				posVtx[2].z = pObjInfo->vtxMax.z;

				D3DXVec3TransformCoord(&posVtx[2], &posVtx[2], &pObject->mtxWorld);

				posVtx[3].x = pObjInfo->vtxMin.x;
				posVtx[3].y = 0;
				posVtx[3].z = pObjInfo->vtxMax.z;

				D3DXVec3TransformCoord(&posVtx[3], &posVtx[3], &pObject->mtxWorld);

				/*** 移動ベクトルの取得 ***/
				vecMove = *pPos - *pPosOld;

				for (int nCntCollision = 0; nCntCollision < 4; nCntCollision++)
				{
					D3DXVECTOR3 vecNor = D3DXVECTOR3_NULL;		// 壁の逆法線ベクトル
					D3DXVECTOR3 vecCutLine = D3DXVECTOR3_NULL;	// 交点からの境界線ベクトル

					vecLine = posVtx[(nCntCollision + 1) % 4] - posVtx[nCntCollision];

					RepairFloat(&vecLine.x);
					RepairFloat(&vecLine.z);

					/*** 現在位置との関係を外積を使い求める ***/
					vecToPos = *pPos - posVtx[nCntCollision];

					fVecPos = (vecLine.z * vecToPos.x) - (vecLine.x * vecToPos.z);

					RepairFloat(&fVecPos);

					/*** 過去位置との関係を外積を使い求める ***/
					vecToPosOld = *pPosOld - posVtx[nCntCollision];

					fVecPosOld = (vecLine.z * vecToPosOld.x) - (vecLine.x * vecToPosOld.z);

					RepairFloat(&fVecPosOld);

					/*** 強制位置の判定 ***/

					/** 現在位置との外積 **/
					fPosToMove = (vecToPos.z * vecMove.x) - (vecToPos.x * vecMove.z);

					/** 最大値との外積 **/
					fLineToMove = (vecLine.z * vecMove.x) - (vecLine.x * vecMove.z);

					/** 面積比率の計算 **/
					fRate = fPosToMove / fLineToMove;

					vecNor.x = vecLine.z;
					vecNor.z = -vecLine.x;

					D3DXVec3Normalize(&vecNor, &vecNor);

					fVecPosToNor = (vecNor.z * vecMove.x) - (vecNor.x * vecMove.z);

					D3DXVECTOR3 vecF;
					vecCutLine = vecLine;

					if (fVecPosToNor == 0)
					{
						fVecPosToNor = fabsf(fVecPosToNor);
						D3DXVec3Normalize(&vecCutLine, &vecCutLine);

						vecF = vecCutLine * fVecPosToNor;
					}
					else if (fVecPosToNor > 0)
					{
						fVecPosToNor = fabsf(fVecPosToNor);
						D3DXVec3Normalize(&vecCutLine, &vecCutLine);

						vecF = -vecCutLine * fVecPosToNor;
					}
					else if (fVecPosToNor < 0)
					{
						fVecPosToNor = fabsf(fVecPosToNor);
						D3DXVec3Normalize(&vecCutLine, &vecCutLine);

						vecF = vecCutLine * fVecPosToNor;
					}

					/*** プレイヤーの壁のめり込み判定 ***/
					if (fVecPos <= 0 && fVecPosOld >= 0)
					{
						/*** もしも比率が範囲内であれば,衝突 ***/
						if (fRate >= 0.0f && fRate <= 1.0f)
						{
							pPos->x = posVtx[nCntCollision].x + (vecLine.x * fRate) + vecF.x;
							pPos->z = posVtx[nCntCollision].z + (vecLine.z * fRate) + vecF.z;
						}
					}

					if (fVecPos < 0 && fVecPosOld < 0)
					{
						bCollision[nCntCollision] = true;
					}
				}

				if (bCollision[0] == true
					&& bCollision[1] == true
					&& bCollision[2] == true
					&& bCollision[3] == true)
				{
					pPos->y = pObject->pos.y + pObjInfo->vtxMax.y;
					bLand = true;
					pMove->y = 0.0f;
				}
			}
		}
	}

	return bLand;
}