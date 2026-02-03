//==================================================================================
//
// DirectXのガイドプロンプト表示処理 [prompt.cpp]
// Author : TENMA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "prompt.h"
#include "player.h"
#include "Texture.h"
#include "mathUtil.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#ifndef RELEASE
#define RELEASE(p)		do{ if(p != nullptr){(p)->Release(); (p) = nullptr;} }while(0)
#endif

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
Prompt g_aPrompt[MAX_PROMPT];
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffPrompt;	// 頂点バッファ

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitPrompt(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// 変数の初期化
	ZeroMemory(&g_aPrompt[0], sizeof(g_aPrompt));

	// 頂点バッファの作成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4 * MAX_PROMPT,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffPrompt,
		NULL);

	VERTEX_3D* pVtx = nullptr;				// 頂点バッファへのポインタ

	// 頂点バッファのロック
	g_pVtxBuffPrompt->Lock(0, 0, (void**)&pVtx, 0);

	if (pVtx == nullptr) return;

	for (int nCntPrompt = 0; nCntPrompt < MAX_PROMPT; nCntPrompt++)
	{
		// 頂点設定
		MyMathUtil::SetPolygonSize(pVtx, D3DXVECTOR2(0, 0), true);

		// 色指定
		MyMathUtil::SetDefaultColor<VERTEX_3D>(pVtx);

		// 法線設定
		MyMathUtil::SetPolygonNormal(pVtx, D3DXVECTOR3(0, 0, 1));

		// テクスチャ座標設定
		MyMathUtil::SetDefaultTexture<VERTEX_3D>(pVtx);

		pVtx += 4;
	}

	// 頂点バッファのアンロック
	g_pVtxBuffPrompt->Unlock();

	// デバイスの取得終了
	EndDevice();
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitPrompt(void)
{
	LPPROMPT pPrompt = &g_aPrompt[0];		// プロンプトポインタ

	// バッファの解放
	RELEASE(g_pVtxBuffPrompt)
}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdatePrompt(void)
{

}

//==================================================================================
// --- 描画 ---
//==================================================================================
void DrawPrompt(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	LPPROMPT pPrompt = &g_aPrompt[0];		// プロンプトポインタ
	D3DXMATRIX mtxTrans;					// 計算用マトリックス
	D3DXMATRIX mtxView;						// ビューマトリックスの取得用

	/*** Zテストを無効にする ***/
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	/*** ライティングをオフ ***/
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// NULLCHECK
	if (g_pVtxBuffPrompt != nullptr)
	{
		/*** 頂点バッファをデータストリームに設定 ***/
		pDevice->SetStreamSource(0, g_pVtxBuffPrompt, 0, sizeof(VERTEX_3D));

		/*** 頂点フォーマットの設定 ***/
		pDevice->SetFVF(FVF_VERTEX_3D);

		for (int nCntPrompt = 0; nCntPrompt < MAX_PROMPT; nCntPrompt++, pPrompt++)
		{
			if (pPrompt->bUse == true && pPrompt->bDisp == true)
			{
				/*** ワールドマトリックスの初期化 ***/
				D3DXMatrixIdentity(&pPrompt->mtxWorld);

				/*** カメラのビューマトリックスを取得 ***/
				pDevice->GetTransform(D3DTS_VIEW, &mtxView);

				/*** マトリックスの逆行列を求める (※ 位置を反映する前に必ず行うこと！) ***/
				D3DXMatrixInverse(&pPrompt->mtxWorld, NULL, &mtxView);
				/** 逆行列によって入ってしまった位置情報を初期化 **/
				pPrompt->mtxWorld._41 = 0.0f;
				pPrompt->mtxWorld._42 = 0.0f;
				pPrompt->mtxWorld._43 = 0.0f;

				/*** 位置を反映 (※ 向きを反映したのちに行うこと！) ***/
				D3DXMatrixTranslation(&mtxTrans,
					pPrompt->pos.x,
					pPrompt->pos.y,
					pPrompt->pos.z);

				D3DXMatrixMultiply(&pPrompt->mtxWorld, &pPrompt->mtxWorld, &mtxTrans);

				/*** ワールドマトリックスの設定 ***/
				pDevice->SetTransform(D3DTS_WORLD, &pPrompt->mtxWorld);

				/*** テクスチャの設定 ***/
				pDevice->SetTexture(0, GetTexture(pPrompt->nIdxTexture));

				/*** ポリゴンの描画 ***/
				pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,		// プリミティブの種類
					4 * nCntPrompt,								// 描画する最初の頂点インデックス
					2);											// 描画するプリミティブの数
			}
		}
	}

	/*** Zテストを無効にする ***/
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	/*** ライティングをオン ***/
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// デバイスの取得終了
	EndDevice();
}

//==================================================================================
// --- 設置 ---
//==================================================================================
int SetPrompt(D3DXVECTOR3 pos, D3DXVECTOR2 size, int nIdxTexture, bool bAuto)
{
	LPPROMPT pPrompt = &g_aPrompt[0];		// プロンプトポインタ
	VERTEX_3D* pVtx = nullptr;				// 頂点バッファへのポインタ
	int nReturnId = -1;

	// NULLCHECK
	if (g_pVtxBuffPrompt == nullptr) return nReturnId;

	// 頂点バッファのロック
	g_pVtxBuffPrompt->Lock(0, 0, (void**)&pVtx, 0);

	if (pVtx == nullptr) return nReturnId;

	for (int nCntPrompt = 0; nCntPrompt < MAX_PROMPT; nCntPrompt++, pPrompt++)
	{
		if (pPrompt->bUse != true)
		{ // 使われていなかった場合
			pPrompt->bUse = true;
			pPrompt->bDisp = false;
			pPrompt->bAuto = bAuto;
			pPrompt->pos = pos;
			pPrompt->size = size;
			pPrompt->nIdxTexture = nIdxTexture;

			// 頂点設定
			MyMathUtil::SetPolygonSize(pVtx, size, true);

			nReturnId = nCntPrompt;

			break;
		}

		pVtx += 4;
	}

	// 頂点バッファのアンロック
	g_pVtxBuffPrompt->Unlock();

	return nReturnId;
}

//==================================================================================
// --- 検知判定 ---
//==================================================================================
bool DetectionPrompt(D3DXVECTOR3 pos, float fLength)
{
	LPPROMPT pPrompt = &g_aPrompt[0];		// プロンプトポインタ

	for (int nCntPrompt = 0; nCntPrompt < MAX_PROMPT; nCntPrompt++, pPrompt++)
	{
		if (pPrompt->bUse = true && pPrompt->bAuto == true)
		{ // 使用されていて、検知判定オンなら
			// 二点間の差分を求める
			D3DXVECTOR3 length = pos - pPrompt->pos;

			// ベクトルの長さを取得
			float fLengthPrompt = D3DXVec3Length(&length);
			if (fLengthPrompt <= fLength)
			{ // 検知範囲内なら描画開始
				pPrompt->bDisp = true;
			}
			else
			{ // 検知範囲外なら描画停止
				pPrompt->bDisp = false;
			}
		}
	}
}

//==================================================================================
// --- 有効化 ---
//==================================================================================
void SetEnablePrompt(bool bDisp, int nIdxPrompt)
{
	if (nIdxPrompt < 0 || nIdxPrompt >= MAX_PROMPT) return;

	g_aPrompt[nIdxPrompt].bDisp = bDisp;
}

//==================================================================================
// --- 位置取得 ---
//==================================================================================
D3DXVECTOR3 GetPromptPos(int nIdxPrompt)
{
	if (nIdxPrompt < 0 || nIdxPrompt >= MAX_PROMPT) return VECNULL;

	return g_aPrompt[nIdxPrompt].pos;
}

//==================================================================================
// --- プロンプト取得 ---
//==================================================================================
LPPROMPT GetPrompt(void)
{
	return &g_aPrompt[0];
}