#include"main.h"
#include"player.h"
#include"timer.h"
#include"fade.h"

#define NUM_PLACE	(3)			// タイマーの桁数
#define Timer_POSX	(550.0f)	// タイマー表記　横
#define Timer_POSY	(25.0f)		// タイマー表記　縦
#define Timer_SIZEX	(60.0f)		// タイマー伸ばし　横
#define Timer_SIZEY	(60.0f)		// タイマー伸ばし　縦

// グローバル変数
LPDIRECT3DTEXTURE9 g_pTextureTimer = NULL;		// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffTimer = NULL;	// 頂点バッファへのポインタ
int g_nTimer;		// スコアの値
bool g_bTimeover;	// タイムオーバーしてるかどうか
bool g_bRanking;

//==============================
// タイマーの初期化処理
//==============================
void InitTimer(bool bRanking)
{
	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice, "DATA\\TEXTURE\\number000.png", &g_pTextureTimer);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer({ sizeof(VERTEX_2D) * 4 * NUM_PLACE },
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffTimer,
		NULL);

	g_bTimeover = false;
	g_bRanking = bRanking;

	VERTEX_2D* pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックして、頂点情報へのポインタを取得
	g_pVtxBuffTimer->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntNumber = 0; nCntNumber < NUM_PLACE; nCntNumber++)
	{
		if (bRanking == false)
		{
			// 頂点座標の設定
			pVtx[0].pos = D3DXVECTOR3(Timer_POSX + (nCntNumber * Timer_SIZEX), Timer_POSY, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(Timer_POSX + (nCntNumber * Timer_SIZEX) + Timer_SIZEX, Timer_POSY, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(Timer_POSX + (nCntNumber * Timer_SIZEX), Timer_POSY + Timer_SIZEY, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(Timer_POSX + (nCntNumber * Timer_SIZEX) + Timer_SIZEX, Timer_POSY + Timer_SIZEY, 0.0f);
		}
		if (bRanking == true)
		{
			// 頂点座標の設定
			pVtx[0].pos = D3DXVECTOR3(370.0f + (nCntNumber * 170.0f), 350.0f, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(470.0f + (nCntNumber * 170.0f) + 100.0f, 350.0f, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(370.0f + (nCntNumber * 170.0f), 450.0f + 100.0f, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(470.0f + (nCntNumber * 170.0f) + 100.0f, 450.0f + 100.0f, 0.0f);
		}

		// rhwの設定
		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

		if (bRanking == false)
		{// ゲーム中は緑
			// 頂点カラー
			pVtx[0].col = D3DCOLOR_RGBA(0, 255, 0, 255);
			pVtx[1].col = D3DCOLOR_RGBA(0, 255, 0, 255);
			pVtx[2].col = D3DCOLOR_RGBA(0, 255, 0, 255);
			pVtx[3].col = D3DCOLOR_RGBA(0, 255, 0, 255);
		}
		else
		{// ランキング画面は白
			// 頂点カラー
			pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);
			pVtx[1].col = D3DCOLOR_RGBA(255, 255, 255, 255);
			pVtx[2].col = D3DCOLOR_RGBA(255, 255, 255, 255);
			pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);
		}


		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(0.1f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(0.1f, 1.0f);

		pVtx += 4;	// 頂点データ

	}
	// 頂点バッファをアンロックする
	g_pVtxBuffTimer->Unlock();

	if (g_bRanking == false)
	{
		g_nTimer = 121 * 60;
	}
}

//=================================
// タイマーの終了処理
//=================================
void UninitTimer(void)
{
	// テクスチャの破棄
	if (g_pTextureTimer != NULL)
	{
		g_pTextureTimer->Release();
		g_pTextureTimer = NULL;
	}

	// バッファの破棄
	if (g_pVtxBuffTimer != NULL)
	{
		g_pVtxBuffTimer->Release();
		g_pVtxBuffTimer = NULL;
	}
}

//=========================
// タイマー更新
//=========================
void UpdateTimer(void)
{
	if (g_bRanking == false)
	{

		int aTexU[NUM_PLACE]; // 各行の数字を格納

		g_nTimer--;

		VERTEX_2D* pVtx;

		// 頂点バッファをロックして、頂点情報へのポインタを取得
		g_pVtxBuffTimer->Lock(0, 0, (void**)&pVtx, 0);

		aTexU[0] = (g_nTimer / 60) % 1000 / 100;
		aTexU[1] = (g_nTimer / 60) % 100 / 10;
		aTexU[2] = (g_nTimer / 60) % 10 / 1;

		for (int nCntNumber = 0; nCntNumber < NUM_PLACE; nCntNumber++)
		{
			// テクスチャ座標の設定
			pVtx[0].tex = D3DXVECTOR2(aTexU[nCntNumber] * 0.1f, 0.0f);
			pVtx[1].tex = D3DXVECTOR2((aTexU[nCntNumber] + 1) * 0.1f, 0.0f);
			pVtx[2].tex = D3DXVECTOR2(aTexU[nCntNumber] * 0.1f, 1.0f);
			pVtx[3].tex = D3DXVECTOR2((aTexU[nCntNumber] + 1) * 0.1f, 1.0f);

			if (g_nTimer < 61 * 60)
			{
				// 頂点カラー
				pVtx[0].col = D3DCOLOR_RGBA(255, 255, 0, 255);
				pVtx[1].col = D3DCOLOR_RGBA(255, 255, 0, 255);
				pVtx[2].col = D3DCOLOR_RGBA(255, 255, 0, 255);
				pVtx[3].col = D3DCOLOR_RGBA(255, 255, 0, 255);
			}

			if (g_nTimer < 31 * 60)
			{
				// 頂点カラー
				pVtx[0].col = D3DCOLOR_RGBA(255, 0, 0, 255);
				pVtx[1].col = D3DCOLOR_RGBA(255, 0, 0, 255);
				pVtx[2].col = D3DCOLOR_RGBA(255, 0, 0, 255);
				pVtx[3].col = D3DCOLOR_RGBA(255, 0, 0, 255);
			}

			pVtx += 4;	// 頂点データ
		}

		if (g_nTimer < 0)
		{
			if (g_bTimeover == false)
			{
				g_bTimeover = true;
				//SetFade(MODE_GAMEOVER);
			}
		}

		// 頂点バッファをアンロックする
		g_pVtxBuffTimer->Unlock();
	}
}

//==========================
// タイマー描画
//==========================
void DrawTimer(void)
{
	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffTimer, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, g_pTextureTimer);

	for (int nCntNumber = 0; nCntNumber < NUM_PLACE; nCntNumber++)
	{
		// ポリゴン描写
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntNumber * 4, 2);
	}

}

//=========================
// タイマーの設定処理
//=========================
void SetTimer(int nTimer)
{
	int aTexU[NUM_PLACE]; // 各行の数字を格納
	g_nTimer = 121 * 60 - nTimer ;

	VERTEX_2D* pVtx;

	// 頂点バッファをロックして、頂点情報へのポインタを取得
	g_pVtxBuffTimer->Lock(0, 0, (void**)&pVtx, 0);

	aTexU[0] = (g_nTimer / 60) % 1000 / 100;
	aTexU[1] = (g_nTimer / 60) % 100 / 10;
	aTexU[2] = (g_nTimer / 60) % 10 / 1;

	for (int nCntNumber = 0; nCntNumber < NUM_PLACE; nCntNumber++)
	{
		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(aTexU[nCntNumber] * 0.1f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2((aTexU[nCntNumber] + 1) * 0.1f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(aTexU[nCntNumber] * 0.1f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2((aTexU[nCntNumber] + 1) * 0.1f, 1.0f);

		pVtx += 4;	// 頂点データ
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffTimer->Unlock();
}

//=============================
//  タイマー減算
//==============================
void AddTimer(int nValue)
{
	int aTexU[NUM_PLACE];
	g_nTimer += nValue;

	VERTEX_2D* pVtx;

	// 頂点バッファをロックして、頂点情報へのポインタを取得
	g_pVtxBuffTimer->Lock(0, 0, (void**)&pVtx, 0);

	aTexU[0] = g_nTimer % 1000 / 100;
	aTexU[1] = g_nTimer % 100 / 10;
	aTexU[2] = g_nTimer % 10 / 1;

	for (int nCntNumber = 0; nCntNumber < NUM_PLACE; nCntNumber++)
	{
		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(aTexU[nCntNumber] * 0.1f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2((aTexU[nCntNumber] + 1) * 0.1f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(aTexU[nCntNumber] * 0.1f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2((aTexU[nCntNumber] + 1) * 0.1f, 1.0f);

		pVtx += 4;	// 頂点データ
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffTimer->Unlock();
}

int GetTimer(void)
{
	return g_nTimer;
}