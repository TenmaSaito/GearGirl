//==================================================================================
//
// DirectXのタイマー処理 [timer.cpp]
// Author : KOMATSU SHOTA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include"main.h"
#include"player.h"
#include"timer.h"
#include"fade.h"

//*************************************************************************************************
//*** マクロ定義 ***
//*************************************************************************************************
#define NUM_PLACE   (4)          // 4桁
#define Timer_POSX  (520.0f)     // タイマー表記　横
#define Timer_POSY  (25.0f)      // タイマー表記　縦
#define Timer_SIZEX (60.0f)      // タイマー伸ばし　横
#define Timer_SIZEY (60.0f)      // タイマー伸ばし　縦

#define TIMER_LIMIT_MIN   (4)   // 制限時間：4分
#define FPS               (60)
#define TIMER_LIMIT_FRAME (TIMER_LIMIT_MIN * 60 * FPS)

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
LPDIRECT3DTEXTURE9 g_pTextureTimer = NULL;        // テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffTimer = NULL;   // 頂点バッファへのポインタ
int  g_nTimer;        // 残りフレーム
bool g_bTimeover;     // タイムオーバー
bool g_bRanking;

//===========================================================
//  タイマーの初期化処理
//===========================================================
void InitTimer(bool bRanking)
{
    LPDIRECT3DDEVICE9 pDevice;
    pDevice = GetDevice();

    D3DXCreateTextureFromFile(pDevice, "DATA\\TEXTURE\\number000.png", &g_pTextureTimer);

    pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * NUM_PLACE,
        D3DUSAGE_WRITEONLY,
        FVF_VERTEX_2D,
        D3DPOOL_MANAGED,
        &g_pVtxBuffTimer,
        NULL);

    g_bTimeover = false;
    g_bRanking = bRanking;

    VERTEX_2D* pVtx;
    g_pVtxBuffTimer->Lock(0, 0, (void**)&pVtx, 0);

    for (int nCntNumber = 0; nCntNumber < NUM_PLACE; nCntNumber++)
    {
        if (bRanking == false)
        {
            // 秒の2桁
            float x = Timer_POSX + (nCntNumber * Timer_SIZEX);
            if (nCntNumber >= 2) x += 20.0f;

            pVtx[0].pos = D3DXVECTOR3(x, Timer_POSY, 0.0f);
            pVtx[1].pos = D3DXVECTOR3(x + Timer_SIZEX, Timer_POSY, 0.0f);
            pVtx[2].pos = D3DXVECTOR3(x, Timer_POSY + Timer_SIZEY, 0.0f);
            pVtx[3].pos = D3DXVECTOR3(x + Timer_SIZEX, Timer_POSY + Timer_SIZEY, 0.0f);
        }
        else
        {
            // スコア表示（残り時間）
            float x = 330.0f + (nCntNumber * 120.0f);
            if (nCntNumber >= 2) x += 20.0f;

            pVtx[0].pos = D3DXVECTOR3(x, 350.0f, 0.0f);
            pVtx[1].pos = D3DXVECTOR3(x + 100.0f, 350.0f, 0.0f);
            pVtx[2].pos = D3DXVECTOR3(x, 450.0f, 0.0f);
            pVtx[3].pos = D3DXVECTOR3(x + 100.0f, 450.0f, 0.0f);
        }

        pVtx[0].rhw = 1.0f;
        pVtx[1].rhw = 1.0f;
        pVtx[2].rhw = 1.0f;
        pVtx[3].rhw = 1.0f;

        if (bRanking == false)
        {
            pVtx[0].col = D3DCOLOR_RGBA(0, 255, 0, 255);
            pVtx[1].col = D3DCOLOR_RGBA(0, 255, 0, 255);
            pVtx[2].col = D3DCOLOR_RGBA(0, 255, 0, 255);
            pVtx[3].col = D3DCOLOR_RGBA(0, 255, 0, 255);
        }
        else
        {
            pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);
            pVtx[1].col = D3DCOLOR_RGBA(255, 255, 255, 255);
            pVtx[2].col = D3DCOLOR_RGBA(255, 255, 255, 255);
            pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);
        }

        // 初期テクスチャ座標
        pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
        pVtx[1].tex = D3DXVECTOR2(0.1f, 0.0f);
        pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
        pVtx[3].tex = D3DXVECTOR2(0.1f, 1.0f);

        pVtx += 4;
    }

    g_pVtxBuffTimer->Unlock();

    if (g_bRanking == false)
    {
        // 4分スタート
        g_nTimer = TIMER_LIMIT_FRAME;
    }
}

//===========================================================
//  タイマーの終了処理
//===========================================================
void UninitTimer(void)
{
    if (g_pTextureTimer != NULL)
    {
        g_pTextureTimer->Release();
        g_pTextureTimer = NULL;
    }

    if (g_pVtxBuffTimer != NULL)
    {
        g_pVtxBuffTimer->Release();
        g_pVtxBuffTimer = NULL;
    }
}

//===========================================================
//  タイマーの更新処理
//===========================================================
void UpdateTimer(void)
{
    if (g_bRanking == false)
    {
        // 0未満に落とさない
        if (g_nTimer > 0)
        {
            g_nTimer--;
        }

        int totalSec = g_nTimer / FPS;     // 残り秒
        int min = totalSec / 60;           // 残り分
        int sec = totalSec % 60;           // 残り秒

        // 4桁
        int aTexU[NUM_PLACE];
        aTexU[0] = (min / 10) % 10;        // 分 十の位
        aTexU[1] = (min % 10);             // 分 一の位
        aTexU[2] = (sec / 10) % 10;        // 秒 十の位
        aTexU[3] = (sec % 10);             // 秒 一の位

        VERTEX_2D* pVtx;
        g_pVtxBuffTimer->Lock(0, 0, (void**)&pVtx, 0);

        for (int nCntNumber = 0; nCntNumber < NUM_PLACE; nCntNumber++)
        {
            pVtx[0].tex = D3DXVECTOR2(aTexU[nCntNumber] * 0.1f, 0.0f);
            pVtx[1].tex = D3DXVECTOR2((aTexU[nCntNumber] + 1) * 0.1f, 0.0f);
            pVtx[2].tex = D3DXVECTOR2(aTexU[nCntNumber] * 0.1f, 1.0f);
            pVtx[3].tex = D3DXVECTOR2((aTexU[nCntNumber] + 1) * 0.1f, 1.0f);

            // 残り秒 色変化
            if (totalSec < 2 * 59 + 3)
            {
                // 残り1分で黄色に変化
                pVtx[0].col = D3DCOLOR_RGBA(255, 255, 0, 255);
                pVtx[1].col = D3DCOLOR_RGBA(255, 255, 0, 255);
                pVtx[2].col = D3DCOLOR_RGBA(255, 255, 0, 255);
                pVtx[3].col = D3DCOLOR_RGBA(255, 255, 0, 255);
            }
            if (totalSec < 61)
            {
                // 残り30秒で赤色に変化
                pVtx[0].col = D3DCOLOR_RGBA(255, 0, 0, 255);
                pVtx[1].col = D3DCOLOR_RGBA(255, 0, 0, 255);
                pVtx[2].col = D3DCOLOR_RGBA(255, 0, 0, 255);
                pVtx[3].col = D3DCOLOR_RGBA(255, 0, 0, 255);
            }

            pVtx += 4;
        }

        g_pVtxBuffTimer->Unlock();

        if (g_nTimer <= 0)
        {
            if (g_bTimeover == false)
            {
                g_bTimeover = true;
                // SetFade(MODE_GAMEOVER);  // タイムオーバーでゲームオーバー
            }
        }
    }
}

//===========================================================
//  タイマーの描画処理
//===========================================================
void DrawTimer(void)
{
    LPDIRECT3DDEVICE9 pDevice;
    pDevice = GetDevice();

    pDevice->SetStreamSource(0, g_pVtxBuffTimer, 0, sizeof(VERTEX_2D));
    pDevice->SetFVF(FVF_VERTEX_2D);
    pDevice->SetTexture(0, g_pTextureTimer);

    for (int nCntNumber = 0; nCntNumber < NUM_PLACE; nCntNumber++)
    {
        pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntNumber * 4, 2);
    }
}

//===========================================================
//  タイマーの設定処理
//===========================================================
void SetTimer(int nElapsedFrame)
{
    int aTexU[NUM_PLACE];

    g_nTimer = TIMER_LIMIT_FRAME - nElapsedFrame;
    if (g_nTimer < 0) g_nTimer = 0;
    if (g_nTimer > TIMER_LIMIT_FRAME) g_nTimer = TIMER_LIMIT_FRAME;

    int totalSec = g_nTimer / FPS;
    int min = totalSec / 60;
    int sec = totalSec % 60;

    aTexU[0] = (min / 10) % 10;
    aTexU[1] = (min % 10);
    aTexU[2] = (sec / 10) % 10;
    aTexU[3] = (sec % 10);

    VERTEX_2D* pVtx;
    g_pVtxBuffTimer->Lock(0, 0, (void**)&pVtx, 0);

    for (int nCntNumber = 0; nCntNumber < NUM_PLACE; nCntNumber++)
    {
        pVtx[0].tex = D3DXVECTOR2(aTexU[nCntNumber] * 0.1f, 0.0f);
        pVtx[1].tex = D3DXVECTOR2((aTexU[nCntNumber] + 1) * 0.1f, 0.0f);
        pVtx[2].tex = D3DXVECTOR2(aTexU[nCntNumber] * 0.1f, 1.0f);
        pVtx[3].tex = D3DXVECTOR2((aTexU[nCntNumber] + 1) * 0.1f, 1.0f);

        pVtx += 4;
    }

    g_pVtxBuffTimer->Unlock();
}

//===========================================================
//  タイマー加算/減算
//===========================================================
void AddTimer(int nValue)
{
    int aTexU[NUM_PLACE];

    g_nTimer += nValue;
    if (g_nTimer < 0) g_nTimer = 0;
    if (g_nTimer > TIMER_LIMIT_FRAME) g_nTimer = TIMER_LIMIT_FRAME;

    int totalSec = g_nTimer / FPS;
    int min = totalSec / 60;
    int sec = totalSec % 60;

    aTexU[0] = (min / 10) % 10;
    aTexU[1] = (min % 10);
    aTexU[2] = (sec / 10) % 10;
    aTexU[3] = (sec % 10);

    VERTEX_2D* pVtx;
    g_pVtxBuffTimer->Lock(0, 0, (void**)&pVtx, 0);

    for (int nCntNumber = 0; nCntNumber < NUM_PLACE; nCntNumber++)
    {
        pVtx[0].tex = D3DXVECTOR2(aTexU[nCntNumber] * 0.1f, 0.0f);
        pVtx[1].tex = D3DXVECTOR2((aTexU[nCntNumber] + 1) * 0.1f, 0.0f);
        pVtx[2].tex = D3DXVECTOR2(aTexU[nCntNumber] * 0.1f, 1.0f);
        pVtx[3].tex = D3DXVECTOR2((aTexU[nCntNumber] + 1) * 0.1f, 1.0f);

        pVtx += 4;
    }

    g_pVtxBuffTimer->Unlock();
}

int GetTimer(void)
{
    return g_nTimer;
}
