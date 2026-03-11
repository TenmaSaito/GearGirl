//================================================================================================================
//
// DirectXのフェード処理 [effect.cpp]
// Author : KOMATSU SHOTA
//
//================================================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include"main.h"
#include"camera.h"
#include"effect.h"
#include"field.h"
#include"gimmick.h"
#include"player.h"
#include"vector_defs.h"

//*************************************************************************************************
//*** マクロ定義 ***
//*************************************************************************************************
#define	MAX_EFFECT	(4096)	// エフェクトの最大数

//*************************************************************************************************
//*** エフェクト構造体の定義 ***
//*************************************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;	// 位置
	D3DXVECTOR3 posOld;	// 位置
	D3DXVECTOR3 vec;	// 速度
	D3DXCOLOR col;		// 色
	int nLife;			// 寿命
	bool bUse;			// 使用しているかどうか
	bool bGravity;		// 重力適用の有無
	float Width;		// 横
	float Height;		// 縦
	D3DXVECTOR3 posOri;	// 発射した位置
	D3DXVECTOR3 move;	// 移動量
	int nCounter;		// 処理を行いたい回数用のカウンター
	int nType;			// 普通のエフェクトか放物線か
	bool bVisible;		// ネズミにしか見えないやつかどうか

	D3DXMATRIX mtxWorld; // ワールドマトリックス
}Effect;

//*************************************************************************************************
//*** グローバル変数 ***
//*************************************************************************************************
LPDIRECT3DTEXTURE9 g_pTextureEffect = NULL;			// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffEffect = NULL;	// 頂点バッファへのポインタ
Effect g_aEffect[MAX_EFFECT];		// エフェクトの情報
int g_aIdxZTest[MAX_EFFECT] = {};	// Zテストを無効にするエフェクト全体のインデックスを保管する
int g_nIdxEffect = 0;				// 保管したZテスト向こうのエフェクトのインデックスを保管する
IDX_FIELD g_nIdxEffectField;		// フィールドのインデックス保存用

//================================================================================================================
// --- エフェクトの初期化処理 ---
//================================================================================================================
void InitEffect(void)
{
	int nCntEffect;

	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice, "DATA\\TEXTURE\\effect000.jpg", &g_pTextureEffect);

	// 弾の情報の初期化
	for (nCntEffect = 0; nCntEffect < MAX_EFFECT; nCntEffect++)
	{
		g_aEffect[nCntEffect].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aEffect[nCntEffect].col = D3DXCOLOR(1, 1, 1, 1);	// 白
		g_aEffect[nCntEffect].vec = D3DXVECTOR3(0, 0, 0);	// 黄色
		g_aEffect[nCntEffect].nLife = 100;
		g_aEffect[nCntEffect].bUse = false;	// 使用していない状態にする
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4 * MAX_EFFECT,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffEffect,
		NULL);

	VERTEX_3D* pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックして、頂点情報へのポインタを取得
	g_pVtxBuffEffect->Lock(0, 0, (void**)&pVtx, 0);

	for (nCntEffect = 0; nCntEffect < MAX_EFFECT; nCntEffect++)
	{
		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(-50.0f, 50.0f, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(50.0f, 50.0f, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-50.0f, -50.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(50.0f, -50.0f, 0.0f);

		// rhwの設定
		pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		// 頂点カラー
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += 4;	// 頂点データ
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffEffect->Unlock();

	// 初期化
	g_aIdxZTest[MAX_EFFECT] = {};


	g_nIdxEffectField = SetField(VECNULL, VECNULL, VECNULL, 10, 10, NULL, 1, 1, D3DCULL_NONE);
	SetEnableField(g_nIdxEffectField, false);

	EndDevice();
}

//================================================================================================================
// --- エフェクトの終了処理 ---
//================================================================================================================
void UninitEffect(void)
{
	// テクスチャの破棄
	if (g_pTextureEffect != NULL)
	{
		g_pTextureEffect->Release();
		g_pTextureEffect = NULL;
	}

	// バッファの破棄
	if (g_pVtxBuffEffect != NULL)
	{
		g_pVtxBuffEffect->Release();
		g_pVtxBuffEffect = NULL;
	}
}

//================================================================================================================
// --- エフェクトの更新処理 ---
//================================================================================================================
void UpdateEffect(void)
{
	// プレイヤー情報を取得
	Player* pPlayer = GetPlayer();
	Player* pMouse = GetPlayer() + 1;

	Camera* pCamera = GetCamera();;

	Gimmick* pGimmick = GetGimmick() + 5;

	int nCntEffect;
	VERTEX_3D* pVtx;

	// 頂点バッファをロックして、頂点情報へのポインタを取得
	g_pVtxBuffEffect->Lock(0, 0, (void**)&pVtx, 0);

	for (nCntEffect = 0; nCntEffect < MAX_EFFECT; nCntEffect++)
	{
		if (g_aEffect[nCntEffect].bUse == true)
		{// 弾が使用されている
			g_aEffect[nCntEffect].posOld = g_aEffect[nCntEffect].pos;

			if (g_aEffect[nCntEffect].nType == 1)
			{
				D3DXVECTOR3 VecParabola = pCamera->posRDest - pCamera->posV;

				D3DXVec3Normalize(&VecParabola, &VecParabola);

				g_aEffect[nCntEffect].move.x += VecParabola.x * 1.0f;
				g_aEffect[nCntEffect].move.z += VecParabola.z * 1.0f;

				// === マップの限界値まで行った時に、各移動量を0にする === //
				if (g_aEffect[nCntEffect].pos.z <= MAX_ZMOVE1)
				{// Z軸
					g_aEffect[nCntEffect].pos.z = MAX_ZMOVE1;
				}
				if (g_aEffect[nCntEffect].pos.z >= MAX_ZMOVE2)
				{
					g_aEffect[nCntEffect].pos.z = MAX_ZMOVE2;
				}
				if (g_aEffect[nCntEffect].pos.x <= MAX_XMOVE1)
				{// X軸
					g_aEffect[nCntEffect].pos.x = MAX_XMOVE1;
				}
				if (g_aEffect[nCntEffect].pos.x >= MAX_XMOVE2)
				{
					g_aEffect[nCntEffect].pos.x = MAX_XMOVE2;
				}

				if (pGimmick->myType == GIMMICKTYPE_CLOSEDDOOR)
				{
					CollisionGimmick(&g_aEffect[nCntEffect].pos, &g_aEffect[nCntEffect].posOld, &g_aEffect[nCntEffect].move, pMouse, 5.0f, 2.0f);
				}
			}

			if (g_aEffect[nCntEffect].bGravity == true)
			{
				g_aEffect[nCntEffect].move.y += GRAVITY *1.1f;
			}

			//弾の位置更新
			g_aEffect[nCntEffect].pos += g_aEffect[nCntEffect].move;

			// 慣性を掛ける
			g_aEffect[nCntEffect].move.x += (0.0f - g_aEffect[nCntEffect].move.x) * (PLAYER_INI * 1.5f);
			g_aEffect[nCntEffect].move.z += (0.0f - g_aEffect[nCntEffect].move.z) * (PLAYER_INI * 1.5f);

			// 頂点座標の設定
			pVtx[0].pos = D3DXVECTOR3(-g_aEffect[nCntEffect].Width * 0.5f, g_aEffect[nCntEffect].Height * 0.5f, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(g_aEffect[nCntEffect].Width * 0.5f, g_aEffect[nCntEffect].Height * 0.5f, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(-g_aEffect[nCntEffect].Width * 0.5f, -g_aEffect[nCntEffect].Height, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(g_aEffect[nCntEffect].Width * 0.5f, -g_aEffect[nCntEffect].Height * 0.5f, 0.0f);

			g_aEffect[nCntEffect].nLife -= 1;

			//寿命のカウントダウン
			if (g_aEffect[nCntEffect].nLife < 0)
			{
				//使用していない状態にする
				g_aEffect[nCntEffect].bUse = false;
			}

			if (g_aEffect[nCntEffect].nType == 0)
			{// 通常エフェクトの処理
				g_aEffect[nCntEffect].Width -= 0.01f;
				g_aEffect[nCntEffect].Height -= 0.01f;
			}
			else if (g_aEffect[nCntEffect].nType == 1)
			{// 放物線エフェクト専用の処理
				if (g_aEffect[nCntEffect].pos.y < 100.5f)
				{
					g_aEffect[nCntEffect].pos.y = 100.5f;

					// 着地点にフィールドを設置
					SetPositionField(g_nIdxEffectField, g_aEffect[nCntEffect].pos);
					SetColorField(g_nIdxEffectField, COL_RED);
					SetEnableField(g_nIdxEffectField, true);

					g_aEffect[nCntEffect].bUse = false;
				}
				else if (pPlayer->state != PLAYERSTATE_THROWWAITING)
				{
					g_aEffect[nCntEffect].bUse = false;
				}
			}
		}

		pVtx += 4;	// 頂点データ
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffEffect->Unlock();
}

//================================================================================================================
// --- エフェクトの描画処理 ---
//================================================================================================================
void DrawEffect(void)
{
	int nCntEffect;

	g_nIdxEffect = 0;

	D3DXMATRIX mtxTrans;		// 計算用マトリックス
	D3DXMATRIX mtxView;			// ビューマトリックスの取得用

	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffEffect, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, g_pTextureEffect);

	// aブレンディングを加算合計に設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// Zテストを設定
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);

	for (nCntEffect = 0; nCntEffect < MAX_EFFECT; nCntEffect++)
	{
		if (g_aEffect[nCntEffect].bUse == true)
		{// 弾が使用されている

			if (g_aEffect[nCntEffect].bVisible == true)
			{// Zテストを有効にしたいものは、インデックスを渡してスキップ
				g_aIdxZTest[g_nIdxEffect] = nCntEffect;
				g_nIdxEffect++;

				continue;
			}

			if (g_aEffect[nCntEffect].bVisible == false || (g_aEffect[nCntEffect].bVisible == true && GetReadyCamera() == CAMERATYPE_PLAYER_TWO))
			{// 全員が見える奴と、ネズミ限定の奴を描画
				/*** ワールドマトリックスの初期化 ***/
				D3DXMatrixIdentity(&g_aEffect[nCntEffect].mtxWorld);

				/*** カメラのビューマトリックスを取得 ***/
				pDevice->GetTransform(D3DTS_VIEW, &mtxView);

				/*** マトリックスの逆行列を求める (※ 位置を反映する前に必ず行うこと！) ***/
				D3DXMatrixInverse(&g_aEffect[nCntEffect].mtxWorld, NULL, &mtxView);

				/** 逆行列によって入ってしまった位置情報を初期化 **/
				g_aEffect[nCntEffect].mtxWorld._41 = 0.0f;
				g_aEffect[nCntEffect].mtxWorld._42 = 0.0f;
				g_aEffect[nCntEffect].mtxWorld._43 = 0.0f;

				/*** 位置を反映 (※ 向きを反映したのちに行うこと！) ***/
				D3DXMatrixTranslation(&mtxTrans,
					g_aEffect[nCntEffect].pos.x,
					g_aEffect[nCntEffect].pos.y,
					g_aEffect[nCntEffect].pos.z);

				D3DXMatrixMultiply(&g_aEffect[nCntEffect].mtxWorld, &g_aEffect[nCntEffect].mtxWorld, &mtxTrans);

				/*** ワールドマトリックスの設定 ***/
				pDevice->SetTransform(D3DTS_WORLD, &g_aEffect[nCntEffect].mtxWorld);

				// ポリゴン描写
				pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntEffect * 4, 2);
			}
		}
	}

	/*** Zテストを無効にする ***/
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	for (int nCntZTest = 0; nCntZTest < g_nIdxEffect; nCntZTest++)
	{
		if (g_aEffect[g_aIdxZTest[nCntZTest]].bVisible == false || (g_aEffect[g_aIdxZTest[nCntZTest]].bVisible == true && GetReadyCamera() == CAMERATYPE_PLAYER_TWO))
		{// 全員が見える奴と、ネズミ限定の奴を描画
			/*** ワールドマトリックスの初期化 ***/
			D3DXMatrixIdentity(&g_aEffect[g_aIdxZTest[nCntZTest]].mtxWorld);

			/*** カメラのビューマトリックスを取得 ***/
			pDevice->GetTransform(D3DTS_VIEW, &mtxView);

			/*** マトリックスの逆行列を求める (※ 位置を反映する前に必ず行うこと！) ***/
			D3DXMatrixInverse(&g_aEffect[g_aIdxZTest[nCntZTest]].mtxWorld, NULL, &mtxView);

			/** 逆行列によって入ってしまった位置情報を初期化 **/
			g_aEffect[g_aIdxZTest[nCntZTest]].mtxWorld._41 = 0.0f;
			g_aEffect[g_aIdxZTest[nCntZTest]].mtxWorld._42 = 0.0f;
			g_aEffect[g_aIdxZTest[nCntZTest]].mtxWorld._43 = 0.0f;

			/*** 位置を反映 (※ 向きを反映したのちに行うこと！) ***/
			D3DXMatrixTranslation(&mtxTrans,
				g_aEffect[g_aIdxZTest[nCntZTest]].pos.x,
				g_aEffect[g_aIdxZTest[nCntZTest]].pos.y,
				g_aEffect[g_aIdxZTest[nCntZTest]].pos.z);

			D3DXMatrixMultiply(&g_aEffect[g_aIdxZTest[nCntZTest]].mtxWorld, &g_aEffect[g_aIdxZTest[nCntZTest]].mtxWorld, &mtxTrans);

			/*** ワールドマトリックスの設定 ***/
			pDevice->SetTransform(D3DTS_WORLD, &g_aEffect[g_aIdxZTest[nCntZTest]].mtxWorld);

			// ポリゴン描写
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntZTest * 4, 2);
		}
	}

	// aブレンディングを元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// 
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	EndDevice();
}

//================================================================================================================
// --- エフェクトの設定処理 ---
//================================================================================================================
void SetEffect(D3DXVECTOR3 pos, D3DXCOLOR col, D3DXVECTOR3 vec, float Width, float Height, float speed, int nLife, bool bUseGravity, bool bVisible)
{
	int nCntEffect;
	VERTEX_3D* pVtx;

	// 頂点バッファをロックして、頂点情報へのポインタを取得
	g_pVtxBuffEffect->Lock(0, 0, (void**)&pVtx, 0);

	for (nCntEffect = 0; nCntEffect < MAX_EFFECT; nCntEffect++)
	{// 弾が使用されていない
		if (g_aEffect[nCntEffect].bUse == false)
		{
			D3DXVECTOR3 vecNor = vec;
			// 移動ベクトルの正規化
			D3DXVec3Normalize(&vecNor, &vecNor);

			// 頂点項目の設定
			g_aEffect[nCntEffect].col = col;
			g_aEffect[nCntEffect].nLife = nLife;
			g_aEffect[nCntEffect].Width = Width;
			g_aEffect[nCntEffect].Height = Height;
			g_aEffect[nCntEffect].move = vecNor * speed;
			g_aEffect[nCntEffect].bUse = true;	// 使用している状態にする
			g_aEffect[nCntEffect].bGravity = bUseGravity;
			g_aEffect[nCntEffect].nType = 0;
			g_aEffect[nCntEffect].bVisible = bVisible;	// 限定的に見えるやつか

			g_aEffect[nCntEffect].pos = pos;

			// 頂点座標の設定
			pVtx[0].pos = D3DXVECTOR3(-g_aEffect[nCntEffect].Width * 0.5f, g_aEffect[nCntEffect].Height * 0.5f, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(g_aEffect[nCntEffect].Width * 0.5f, g_aEffect[nCntEffect].Height * 0.5f, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(-g_aEffect[nCntEffect].Width * 0.5f, -g_aEffect[nCntEffect].Height, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(g_aEffect[nCntEffect].Width * 0.5f, -g_aEffect[nCntEffect].Height * 0.5f, 0.0f);

			// 色の適応
			for (int nCntcol = 0; nCntcol < 4; nCntcol++)
			{
				pVtx[nCntcol].col = col;
			}

			break;	// ここでfor文を抜ける
		}

		pVtx += 4;	// 頂点データ
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffEffect->Unlock();
}

// =================================================
// 放物線の設定処理
// =================================================
void SetParabola(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, float Width, float Height, float speed, bool bUseGravity)
{
	Player* pPlayer = GetPlayer();
	Camera* pCamera = GetCamera();

	VERTEX_3D* pVtx;

	// 頂点バッファをロックして、頂点情報へのポインタを取得
	g_pVtxBuffEffect->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntEffect = 0; nCntEffect < MAX_EFFECT; nCntEffect++)
	{
		if (g_aEffect[nCntEffect].bUse == false)
		{// 放物線未使用

			// === 引数を各変数に代入 === //
			//g_aEffect[nCntEffect].move = {};		// 移動量	
			g_aEffect[nCntEffect].pos = pos;	// 位置を代入
			g_aEffect[nCntEffect].posOri = pos;	// 発射位置を代入
			g_aEffect[nCntEffect].Width = Width;	// 幅
			g_aEffect[nCntEffect].Height = Height;	// 高さ
			g_aEffect[nCntEffect].nLife = 180;		// 寿命の設定
			g_aEffect[nCntEffect].bUse = true;		// 使用状態に
			g_aEffect[nCntEffect].bGravity = true;	// 重力をかけるかどうか
			g_aEffect[nCntEffect].nCounter = 0;
			g_aEffect[nCntEffect].nType = 1;
			g_aEffect[nCntEffect].bVisible = false;
			g_aEffect[nCntEffect].vec = move;

			for (int nCnt = 0; nCnt < 4; nCnt++)
			{// 色を設定
				pVtx[nCnt].col = col;
			}

			g_aEffect[nCntEffect].move.y = move.y * 35.0f;

			if (g_aEffect[nCntEffect].move.y < 5.5f)
			{
				g_aEffect[nCntEffect].move.y = 5.5f;
			}

			break;
		}
		pVtx += 4;
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffEffect->Unlock();
}

// =================================================
// 着地予想点のフィールドのインデックスを渡す
// =================================================
int GetIdxEffectField(void)
{
	return g_nIdxEffectField;
}