// =================================================
// 
// 放物線の処理[parabola.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include "parabola.h"
#include "player.h"
#include "camera.h"

// =================================================
// グローバル変数
LPDIRECT3DTEXTURE9	g_pTextureParabola = NULL;		// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9	g_pVtxBuffParabola = NULL;	// 頂点バッファへのポインタ
Parabola g_aParabola[MAX_PARABOLA];						// 放物線の情報

// =================================================
// 放物線の初期化処理
// =================================================
void InitParabola(void)
{
	LPDIRECT3DDEVICE9 pDevice;		// デバイスへのポインタ

	AUTODEVICE9 Auto;

	// デバイスの取得
	pDevice = Auto.pDevice;

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\effect000.jpg",
		&g_pTextureParabola);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4 * MAX_PARABOLA,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffParabola,
		NULL);

	// 構造体の中身を初期化
	for (int nCntParabola = 0; nCntParabola < MAX_PARABOLA; nCntParabola++)
	{
		g_aParabola[nCntParabola].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aParabola[nCntParabola].move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aParabola[nCntParabola].nLife = 0;
		g_aParabola[nCntParabola].bUse = false;
		g_aParabola[nCntParabola].Width = 0.0f;
		g_aParabola[nCntParabola].Height = 0.0f;
		g_aParabola[nCntParabola].bUseGravity = false;
	}

	VERTEX_3D* pVtx;		// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffParabola->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntParabola = 0; nCntParabola < MAX_PARABOLA; nCntParabola++)
	{
		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(g_aParabola[nCntParabola].pos.x - PARABOLA_XSIZE, g_aParabola[nCntParabola].pos.y + PARABOLA_YSIZE, g_aParabola[nCntParabola].pos.z);
		pVtx[1].pos = D3DXVECTOR3(g_aParabola[nCntParabola].pos.x + PARABOLA_XSIZE, g_aParabola[nCntParabola].pos.y + PARABOLA_YSIZE, g_aParabola[nCntParabola].pos.z);
		pVtx[2].pos = D3DXVECTOR3(g_aParabola[nCntParabola].pos.x - PARABOLA_XSIZE, g_aParabola[nCntParabola].pos.y - PARABOLA_YSIZE, g_aParabola[nCntParabola].pos.z);
		pVtx[3].pos = D3DXVECTOR3(g_aParabola[nCntParabola].pos.x + PARABOLA_XSIZE, g_aParabola[nCntParabola].pos.y - PARABOLA_YSIZE, g_aParabola[nCntParabola].pos.z);

		// 法線ベクトルの設定
		pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		// 頂点カラーの設定
		pVtx[0].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		pVtx[1].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		pVtx[3].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += 4;
	}

	// === グローバル変数を初期化 === //

	// 頂点バッファをアンロックする
	g_pVtxBuffParabola->Unlock();
}

// =================================================
// 放物線の終了処理
// =================================================
void UninitParabola(void)
{
	// テクスチャの破棄
	if (g_pTextureParabola != NULL)
	{
		g_pTextureParabola->Release();
		g_pTextureParabola = NULL;
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffParabola != NULL)
	{
		g_pVtxBuffParabola->Release();
		g_pVtxBuffParabola = NULL;
	}
}

// =================================================
// 放物線の更新処理
// =================================================
void UpdateParabola(void)
{
	// === 各種情報をポインタとして取得 === //
	Player* pPlayer = GetPlayer();
	Parabola* pParabola = &g_aParabola[0];
	Camera* pCamera = GetCamera();

	VERTEX_3D* pVtx;

	// 頂点バッファをロックして、頂点情報へのポインタを取得
	g_pVtxBuffParabola->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntParabola = 0; nCntParabola < MAX_PARABOLA; nCntParabola++, pParabola++)
	{
		if (pParabola->bUse == true)
		{
			pParabola->posOld = pParabola->pos;

			//// 注視点までのベクトルをだす
			//D3DXVECTOR3 vec = pCamera->posR - pCamera->posV;

			//// 出したベクトルを正規化
			//D3DXVec3Normalize(&vec, &vec);

			pParabola->move.x += pParabola->vec.x;
			pParabola->move.z += pParabola->vec.z;

			//if (pParabola->nCounter == 0)
			//{// 1Fだけ入る
			//	pParabola->move.y = 5.5f;
			//	pParabola->nCounter = 1;
			//}

			if (pParabola->bUseGravity == true)
			{
				// 重力をかけ続ける
				pParabola->move.y += GRAVITY;
			}

			// 位置を更新
			pParabola->pos += pParabola->move;

			// 慣性を掛ける
			pParabola->move.x += (0.0f - pParabola->move.x) * (PLAYER_INI * 1.5f);
			pParabola->move.z += (0.0f - pParabola->move.z) * (PLAYER_INI * 1.5f);

			// === 各種条件で未使用状態に === //
			if (pParabola->pos.y < 100.0f)
			{// 地面の下に埋まる
				pParabola->bUse = false;
			}
			else if (pPlayer->state != PLAYERSTATE_THROWWAITING)
			{// 投げ待機状態を解除
				pParabola->bUse = false;
			}
		}
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffParabola->Unlock();
}

// =================================================
// 放物線の描画処理
// =================================================
void DrawParabola(void)
{
	LPDIRECT3DDEVICE9 pDevice;		// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	D3DXMATRIX mtxTrans;	// 計算用マトリックス
	D3DXMATRIX mtxView;		// ビューマトリックスの取得用

	// ワールドマトリックスの初期化
	for (int nCntParabola = 0; nCntParabola < MAX_PARABOLA; nCntParabola++)
	{
		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&g_aParabola[nCntParabola].mtxWorld);

		// ビューマトリックスの取得
		pDevice->GetTransform(D3DTS_VIEW, &mtxView);

		// ポリゴンをカメラに対して正面に向ける
		D3DXMatrixInverse(&g_aParabola[nCntParabola].mtxWorld, NULL, &mtxView);	// 逆行列を求める
		g_aParabola[nCntParabola].mtxWorld._41 = 0.0f;		// 位置情報の初期化
		g_aParabola[nCntParabola].mtxWorld._42 = 0.0f;		// 位置情報の初期化
		g_aParabola[nCntParabola].mtxWorld._43 = 0.0f;		// 位置情報の初期化

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans,
			g_aParabola[nCntParabola].pos.x, g_aParabola[nCntParabola].pos.y, g_aParabola[nCntParabola].pos.z);
		D3DXMatrixMultiply(&g_aParabola[nCntParabola].mtxWorld, &g_aParabola[nCntParabola].mtxWorld, &mtxTrans);	// かけ合わせる

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &g_aParabola[nCntParabola].mtxWorld);

		// 頂点バッファをデータストリームに設定
		pDevice->SetStreamSource(0, g_pVtxBuffParabola, 0, sizeof(VERTEX_3D));

		// 頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_3D);

		// テクスチャの設定
		pDevice->SetTexture(0, g_pTextureParabola);

		// aブレンディングを加算合計に設定
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

		if (g_aParabola[nCntParabola].bUse == true)
		{
			// 放物線の描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0 + 4 * nCntParabola, 2);
		}

		// aブレンディングを元に戻す
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}

	EndDevice();
}

// =================================================
// 放物線の設定処理
// =================================================
//void SetParabola(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, float Width, float Height, float speed, bool bUseGravity)
//{
//	Player* pPlayer = GetPlayer();
//	Camera* pCamera = GetCamera();
//
//	VERTEX_3D* pVtx;
//
//	// 頂点バッファをロックして、頂点情報へのポインタを取得
//	g_pVtxBuffParabola->Lock(0, 0, (void**)&pVtx, 0);
//
//	for (int nCntParabola = 0; nCntParabola < MAX_PARABOLA; nCntParabola++)
//	{
//		if (g_aParabola[nCntParabola].bUse == false)
//		{// 放物線未使用
//
//			// === 引数を各変数に代入 === //
//			g_aParabola[nCntParabola].move = move;		// 移動量
//			g_aParabola[nCntParabola].Width = Width;	// 幅
//			g_aParabola[nCntParabola].Height = Height;	// 高さ
//			g_aParabola[nCntParabola].nLife = 120;		// 寿命の設定
//			g_aParabola[nCntParabola].bUse = true;		// 使用状態に
//			g_aParabola[nCntParabola].bUseGravity = bUseGravity;	// 重力をかけるかどうか
//			g_aParabola[nCntParabola].nCounter = 0;
//
//			for (int nCnt = 0; nCnt < 4; nCnt++)
//			{// 色を設定
//				pVtx[nCnt].col = col;
//			}
//
//			pVtx += 4;
//
//			D3DXVECTOR3 vec = move;
//
//			// 出したベクトルを正規化
//			D3DXVec3Normalize(&vec, &vec);
//
//			g_aParabola[nCntParabola].vec.x = vec.x * speed;
//			g_aParabola[nCntParabola].vec.z = vec.z * speed;
//
//			D3DXVec3Normalize(&g_aParabola[nCntParabola].vec, &g_aParabola[nCntParabola].vec);
//
//
//			//if (g_aParabola[nCntParabola].nCounter == 0)
//			//{// 1Fだけ入る
//			//	g_aParabola[nCntParabola].move.y = 5.5f;
//			//	g_aParabola[nCntParabola].nCounter = 1;
//			//}
//
//			g_aParabola[nCntParabola].pos = pos;	// 位置を代入
//			g_aParabola[nCntParabola].posOri = pos;	// 発射位置を代入
//
//			break;
//		}
//	}
//
//	// 頂点バッファをアンロックする
//	g_pVtxBuffParabola->Unlock();
//}

// =================================================
// 放物線の情報の譲渡
// =================================================
Parabola* GetParabola(void)
{
	return &g_aParabola[0];
}