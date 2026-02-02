//=========================================================
// 
// "ポーズ処理"	[pause.cpp]
// Author : KikuchiMina
// 
//=========================================================
#include "input.h"
#include "fade.h"
#include "main.h"
#include "game.h"
#include "pause.h"
#include "time.h"

//=========================================================
//マクロ定義
//=========================================================
#define MAX_POUSE	(PAUSE_MENU_MAX)		//ポーズの種類

//=========================================================
//グローバル変数
//=========================================================
LPDIRECT3DTEXTURE9		g_apTexturePause[MAX_POUSE]	 = {};		//テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9	g_pVtxBuffPause				 = NULL;	//頂点バッファへのポインタ
PAUSE_MENU				g_PauseMenu;							//ポーズメニュー
D3DXVECTOR3 g_posPause[MAX_POUSE];								//位置
bool nUpDisp = false;
int nUpCounter = 0;
int nPauseData;

//=========================================================
//テクスチャファイル名
//=========================================================
const char* c_apFilenamePause[PAUSE_MENU_MAX] =
{
	"data\\TEXTURE\\Pouse Restart.png",
	"data\\TEXTURE\\Pouse Retitle.png",
	"data\\TEXTURE\\Pouse Close.png",
};

//=========================================================
//ポーズの初期化処理
//=========================================================
void InitPause(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();		//デバイスへのポインタの取得
	VERTEX_2D* pVtx;								//頂点情報へのポインタ

	//-------------------------------------
	//テクスチャの読み込み
	for (int nCnrTex = 0; nCnrTex < PAUSE_MENU_MAX; nCnrTex++)
	{
		D3DXCreateTextureFromFile(pDevice, c_apFilenamePause[nCnrTex], &g_apTexturePause[nCnrTex]);
	}

	//-------------------------------------
	//初期化
	nPauseData = 0;
	g_PauseMenu = PAUSE_MENU_RESTART;
	for (int nCntPouse = 0; nCntPouse < MAX_POUSE; nCntPouse++)
	{
		g_posPause[nCntPouse] = D3DXVECTOR3(640.0f, 0.0f, 0.0f);
	}

	//-------------------------------------
	//頂点バッファの作成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_POUSE,
								D3DUSAGE_WRITEONLY,
								FVF_VERTEX_2D,
								D3DPOOL_MANAGED,
								&g_pVtxBuffPause,
								NULL);

	//-------------------------------------
	//頂点バッファをロックし、頂点情報へのポインタを獲得
	g_pVtxBuffPause->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntPause = 0; nCntPause < MAX_POUSE; nCntPause++)
	{
		//位置の設定
		pVtx[0].pos = D3DXVECTOR3(g_posPause[nCntPause].x - 300.0f, nCntPause * 120.0f + 175.0f, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(g_posPause[nCntPause].x + 300.0f, nCntPause * 120.0f + 175.0f, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(g_posPause[nCntPause].x - 300.0f, nCntPause * 120.0f + 275.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(g_posPause[nCntPause].x + 300.0f, nCntPause * 120.0f + 275.0f, 0.0f);

		//rhwの設定
		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

		//頂点カラーの設定
		pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 155);
		pVtx[1].col = D3DCOLOR_RGBA(255, 255, 255, 155);
		pVtx[2].col = D3DCOLOR_RGBA(255, 255, 255, 155);
		pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 155);

		//テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);
		pVtx += 4;
	}

	//-------------------------------------
	//頂点バッファをアンロック
	g_pVtxBuffPause->Unlock();
}

//=========================================================
//ポーズの終了処理
//=========================================================
void UninitPause(void)
{
	//-------------------------------------
	//テクスチャの破棄
	//-------------------------------------
	for (int nCount = 0; nCount < MAX_POUSE; nCount++)
	{
		if (g_apTexturePause[nCount] != NULL)
		{
			g_apTexturePause[nCount]->Release();
			g_apTexturePause[nCount] = NULL;
		}
	}

	//-------------------------------------
	//頂点バッファの破棄
	//-------------------------------------
	if (g_pVtxBuffPause != NULL)
	{
		g_pVtxBuffPause->Release();
		g_pVtxBuffPause = NULL;
	}
}

//=========================================================
//ポーズの更新処理
//=========================================================
void UpdatePause(void)
{
	//VERTEX_2D* pVtx;										//頂点情報へのポインタ
	//FADE pFade = GetFade();									//フェード情報の取得

	////-------------------------------------
	////頂点バッファをロックし、頂点情報へのポインタを獲得
	//g_pVtxBuffPause->Lock(0, 0, (void**)&pVtx, 0);

	////頂点座標を進める
	//pVtx += nPauseData * 4;
	//pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 155);
	//pVtx[1].col = D3DCOLOR_RGBA(255, 255, 255, 155);
	//pVtx[2].col = D3DCOLOR_RGBA(255, 255, 255, 155);
	//pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 155);

	////-------------------------------------
	////頂点バッファをアンロック
	//g_pVtxBuffPause->Unlock();

	////頂点バッファをロックし、頂点情報へのポインタを獲得
	//g_pVtxBuffPause->Lock(0, 0, (void**)&pVtx, 0);

	////-------------------------------------
	////W・Sキーが押された
	//if (nPauseData >= -1 || nPauseData <= MAX_POUSE)
	//{
	//	if (GetJoypadRepeat(0, JOYKEY_UP) == true || GetJoypadRepeat(1, JOYKEY_UP) == true ||
	//		GetKeyboardRepeat(DIK_W) == true || GetKeyboardRepeat(DIK_UP) == true)
	//	{//Wキーが押された
	//		switch (nPauseData)
	//		{
	//		case PAUSE_MENU_RESTART:
	//			if (--nPauseData <= 0)
	//			{
	//				nPauseData = PAUSE_MENU_CLOSE;
	//			}
	//			break;
	//		case PAUSE_MENU_RETITLE:
	//			if (--nPauseData <= 0)
	//			{
	//				nPauseData = PAUSE_MENU_RESTART;
	//			}
	//			break;
	//		case PAUSE_MENU_CLOSE:
	//			if (--nPauseData == 1)
	//			{
	//				nPauseData = PAUSE_MENU_RETITLE;
	//			}
	//			break;
	//		}
	//	}
	//	if (GetJoypadRepeat(0, JOYKEY_DOWN) == true || GetJoypadRepeat(1, JOYKEY_DOWN) == true ||
	//		GetKeyboardRepeat(DIK_S) == true || GetKeyboardRepeat(DIK_DOWN) == true)
	//	{//Sキーが押された
	//		switch (nPauseData)
	//		{
	//		case PAUSE_MENU_RESTART:
	//			if (++nPauseData == 1)
	//			{
	//				nPauseData = PAUSE_MENU_RETITLE;
	//			}
	//			break;
	//		case PAUSE_MENU_RETITLE:
	//			if (++nPauseData >= 2)
	//			{
	//				nPauseData = PAUSE_MENU_CLOSE;
	//			}
	//			break;
	//		case PAUSE_MENU_CLOSE:
	//			if (++nPauseData >= 2)
	//			{
	//				nPauseData = PAUSE_MENU_RESTART;
	//			}
	//			break;
	//		}
	//	}

	//	//頂点座標を進める
	//	pVtx += nPauseData * 4;
	//	pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	//	pVtx[1].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	//	pVtx[2].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	//	pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	//}

	////-------------------------------------
	////Pキーが押された
	//if (GetJoypadTrigger(0, JOYKEY_BACK) == true ||
	//	GetJoypadTrigger(1, JOYKEY_BACK) == true ||
	//	GetKeyboardTrigger(DIK_P) == true)
	//{
	//	for (int nCntPause = 0; nCntPause < MAX_POUSE; nCntPause++)
	//	{
	//		pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 155);
	//		pVtx[1].col = D3DCOLOR_RGBA(255, 255, 255, 155);
	//		pVtx[2].col = D3DCOLOR_RGBA(255, 255, 255, 155);
	//		pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 155);
	//	}
	//	nPauseData = 0;
	//}

	////-------------------------------------
	////ENTERキーが押された
	//if (GetJoypadPress(0, JOYKEY_A) == true || GetJoypadPress(1, JOYKEY_A) == true || GetKeyboardPress(DIK_RETURN) == true)
	//{
	//	switch (nPauseData)
	//	{
	//	case PAUSE_MENU_RESTART:
	//		SetFade(MODE_GAME);
	//		break;
	//	case PAUSE_MENU_RETITLE:
	//		SetFade(MODE_TITLE);
	//		break;
	//	case PAUSE_MENU_CLOSE:
	//		SetEnablePause(false);
	//		break;
	//	}
	//}

	////頂点バッファをアンロック
	//g_pVtxBuffPause->Unlock();
}

//=========================================================
//ポーズの描画処理
//=========================================================
void DrawPause(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			//デバイスへのポインタ・取得

	//-------------------------------------
	//頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0,
							g_pVtxBuffPause,			//頂点バッファへのポインタ
							0,
							sizeof(VERTEX_2D));			//頂点情報構造体のサイズ

	//-------------------------------------
	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntPouse = 0; nCntPouse < MAX_POUSE; nCntPouse++)
	{
		//テクスチャの設定
		pDevice->SetTexture(0, g_apTexturePause[nCntPouse]);

		//ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,		//プリミティブの種類
								nCntPouse * 4,			//描画する最初の頂点インデックス
								2);						//プリミティブ(ポリゴン)数
	}
}