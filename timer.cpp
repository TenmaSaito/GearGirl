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
#define NUM_PLACE   (5)          // 5枚
#define Timer_POSX  (520.0f)
#define Timer_POSY  (25.0f)
#define Timer_SIZEX (60.0f)
#define Timer_SIZEY (60.0f)

// コロンのサイズ
#define COLON_SIZEX (60.0f)
#define COLON_SIZEY (60.0f)
#define COLON_SHIFT (-20.0f)

#define TIMER_LIMIT_MIN   (4)   // 4分(仮)
#define FPS               (60)
#define TIMER_LIMIT_FRAME (TIMER_LIMIT_MIN * 60 * FPS)

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
LPDIRECT3DTEXTURE9 g_pTextureTimer = NULL;        // 数字テクスチャ
LPDIRECT3DTEXTURE9 g_pTextureColon = NULL;        // コロンテクスチャ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffTimer = NULL;

int  g_nTimer;
bool g_bTimeover;
bool g_bRanking;

bool g_bColonVisible = true;   // コロン表示
int  g_nColonTimer = 0;        // 点滅用カウンタ

//===========================================================
//  タイマーの初期化処理
//===========================================================
void InitTimer(bool bRanking)
{
    LPDIRECT3DDEVICE9 pDevice;
    pDevice = GetDevice();

    // 数字テクスチャ
    D3DXCreateTextureFromFile(pDevice, "DATA\\TEXTURE\\number003.png", &g_pTextureTimer);

    // コロンテクスチャ
    D3DXCreateTextureFromFile(pDevice, "DATA\\TEXTURE\\colon.png", &g_pTextureColon);

    pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * NUM_PLACE,
        D3DUSAGE_WRITEONLY,
        FVF_VERTEX_2D,
        D3DPOOL_MANAGED,
        &g_pVtxBuffTimer,
        NULL);

    EndDevice();

    g_bTimeover = false;
    g_bRanking = bRanking;

    VERTEX_2D* pVtx;
    g_pVtxBuffTimer->Lock(0, 0, (void**)&pVtx, 0);

    for (int nCntNumber = 0; nCntNumber < NUM_PLACE; nCntNumber++)
    {
        if (bRanking == false)
        {
            float x = Timer_POSX + (nCntNumber * Timer_SIZEX);
            if (nCntNumber >= 3) x += COLON_SHIFT;

            if (nCntNumber == 2)
            {
                // コロン位置
                float cx = Timer_POSX + (2 * Timer_SIZEX) + (COLON_SHIFT * 0.5f);

                pVtx[0].pos = D3DXVECTOR3(cx, Timer_POSY, 0.0f);
                pVtx[1].pos = D3DXVECTOR3(cx + COLON_SIZEX, Timer_POSY, 0.0f);
                pVtx[2].pos = D3DXVECTOR3(cx, Timer_POSY + COLON_SIZEY, 0.0f);
                pVtx[3].pos = D3DXVECTOR3(cx + COLON_SIZEX, Timer_POSY + COLON_SIZEY, 0.0f);
            }
            else
            {
                // 数字位置
                pVtx[0].pos = D3DXVECTOR3(x, Timer_POSY, 0.0f);
                pVtx[1].pos = D3DXVECTOR3(x + Timer_SIZEX, Timer_POSY, 0.0f);
                pVtx[2].pos = D3DXVECTOR3(x, Timer_POSY + Timer_SIZEY, 0.0f);
                pVtx[3].pos = D3DXVECTOR3(x + Timer_SIZEX, Timer_POSY + Timer_SIZEY, 0.0f);
            }

        }
        else
        {
            // ランキング表示
            float x = 300.0f + (nCntNumber * 110.0f);
            if (nCntNumber >= 3) x += 20.0f;

            if (nCntNumber == 2)
            {
                float cx = 300.0f + (2 * 110.0f) + 10.0f;
                pVtx[0].pos = D3DXVECTOR3(cx, 350.0f, 0.0f);
                pVtx[1].pos = D3DXVECTOR3(cx + COLON_SIZEX, 350.0f, 0.0f);
                pVtx[2].pos = D3DXVECTOR3(cx, 450.0f, 0.0f);
                pVtx[3].pos = D3DXVECTOR3(cx + COLON_SIZEX, 450.0f, 0.0f);
            }
            else
            {
                pVtx[0].pos = D3DXVECTOR3(x, 350.0f, 0.0f);
                pVtx[1].pos = D3DXVECTOR3(x + 100.0f, 350.0f, 0.0f);
                pVtx[2].pos = D3DXVECTOR3(x, 450.0f, 0.0f);
                pVtx[3].pos = D3DXVECTOR3(x + 100.0f, 450.0f, 0.0f);
            }
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

        if (nCntNumber == 2)
        {
            pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
            pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
            pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
            pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);
        }
        else
        {
            pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
            pVtx[1].tex = D3DXVECTOR2(0.1f, 0.0f);
            pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
            pVtx[3].tex = D3DXVECTOR2(0.1f, 1.0f);
        }

        pVtx += 4;
    }

    g_pVtxBuffTimer->Unlock();

    if (g_bRanking == false)
    {
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

    // コロン破棄
    if (g_pTextureColon != NULL)
    {
        g_pTextureColon->Release();
        g_pTextureColon = NULL;
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

        //===========================
        // タイマー減算
        //===========================
        if (g_nTimer > 0)
        {
            g_nTimer--;
        }

        //===========================
        // 時間変換
        //===========================
        int totalSec = g_nTimer / FPS;
        int min = totalSec / 60;
        int sec = totalSec % 60;

        static int colonCnt = 0;

        // 残り30秒で点滅を高速
        int blinkSpeed = (totalSec <= 30) ? FPS / 4 : FPS;

        colonCnt++;

        if (colonCnt >= blinkSpeed)
        {
            colonCnt = 0;
            g_bColonVisible = !g_bColonVisible;
        }

        int aTexU[NUM_PLACE];
        aTexU[0] = (min / 10) % 10;
        aTexU[1] = (min % 10);
        aTexU[2] = 0; // コロン
        aTexU[3] = (sec / 10) % 10;
        aTexU[4] = (sec % 10);

        //===========================
        // 頂点更新
        //===========================
        VERTEX_2D* pVtx;
        g_pVtxBuffTimer->Lock(0, 0, (void**)&pVtx, 0);

        for (int nCntNumber = 0; nCntNumber < NUM_PLACE; nCntNumber++)
        {
            // ---------- 数字更新 ----------
            if (nCntNumber != 2)
            {
                pVtx[0].tex = D3DXVECTOR2(aTexU[nCntNumber] * 0.1f, 0.0f);
                pVtx[1].tex = D3DXVECTOR2((aTexU[nCntNumber] + 1) * 0.1f, 0.0f);
                pVtx[2].tex = D3DXVECTOR2(aTexU[nCntNumber] * 0.1f, 1.0f);
                pVtx[3].tex = D3DXVECTOR2((aTexU[nCntNumber] + 1) * 0.1f, 1.0f);
            }

            // ---------- 色制御 ----------
            D3DCOLOR col;

            // ===== コロンは常に固定色 =====
            if (nCntNumber == 2)
            {
                col = D3DCOLOR_RGBA(255, 255, 255, 255);   // 常に白
            }
            else
            {
                // ===== 数字だけ時間で変化 =====
                col = D3DCOLOR_RGBA(255, 255, 255, 255);

                if (totalSec < 60)
                    col = D3DCOLOR_RGBA(255, 255, 0, 255);

                if (totalSec < 30)
                    col = D3DCOLOR_RGBA(255, 0, 0, 255);

                if (totalSec <= 10 && (g_nTimer / 5) % 2 == 0)
                    col = D3DCOLOR_RGBA(255, 255, 255, 255);
            }


            // ===== 残り10秒でフラッシュ =====
            if (totalSec <= 10)
            {
                if ((g_nTimer / 5) % 2 == 0)   // 点滅
                {
                    col = D3DCOLOR_RGBA(255, 255, 255, 255); // 白く光る
                }
            }

            // ---------- コロン点滅 ----------
            if (nCntNumber == 2 && g_bColonVisible == false)
            {
                col = D3DCOLOR_RGBA(0, 0, 0, 0);           // 非表示
            }

            pVtx[0].col = col;
            pVtx[1].col = col;
            pVtx[2].col = col;
            pVtx[3].col = col;

            pVtx += 4;
        }

        g_pVtxBuffTimer->Unlock();

        //===========================
        // タイムオーバー判定
        //===========================
        if (g_nTimer <= 0 && g_bTimeover == false)
        {
            g_bTimeover = true;
            // SetFade(MODE_GAMEOVER);
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

    for (int nCntNumber = 0; nCntNumber < NUM_PLACE; nCntNumber++)
    {
        // コロンだけテクスチャを差し替える
        if (nCntNumber == 2)
        {
            pDevice->SetTexture(0, g_pTextureColon);
        }
        else
        {
            pDevice->SetTexture(0, g_pTextureTimer);
        }

        pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntNumber * 4, 2);
    }

     EndDevice();
}


//===========================================================
//  タイマーの設定処理
//===========================================================
void SetTimer(int nElapsedFrame)
{
    g_nTimer = TIMER_LIMIT_FRAME - nElapsedFrame;
    if (g_nTimer < 0) g_nTimer = 0;
    if (g_nTimer > TIMER_LIMIT_FRAME) g_nTimer = TIMER_LIMIT_FRAME;

    int totalSec = g_nTimer / FPS;
    int min = totalSec / 60;
    int sec = totalSec % 60;

    int aTexU[NUM_PLACE];
    aTexU[0] = (min / 10) % 10;
    aTexU[1] = (min % 10);
    aTexU[2] = 0; // コロン
    aTexU[3] = (sec / 10) % 10;
    aTexU[4] = (sec % 10);

    VERTEX_2D* pVtx;
    g_pVtxBuffTimer->Lock(0, 0, (void**)&pVtx, 0);

    for (int nCntNumber = 0; nCntNumber < NUM_PLACE; nCntNumber++)
    {
        if (nCntNumber != 2)
        {
            pVtx[0].tex = D3DXVECTOR2(aTexU[nCntNumber] * 0.1f, 0.0f);
            pVtx[1].tex = D3DXVECTOR2((aTexU[nCntNumber] + 1) * 0.1f, 0.0f);
            pVtx[2].tex = D3DXVECTOR2(aTexU[nCntNumber] * 0.1f, 1.0f);
            pVtx[3].tex = D3DXVECTOR2((aTexU[nCntNumber] + 1) * 0.1f, 1.0f);
        }

        pVtx += 4;
    }

    g_pVtxBuffTimer->Unlock();
}

//===========================================================
//  タイマー加算/減算
//===========================================================
void AddTimer(int nValue)
{
    g_nTimer += nValue;
    if (g_nTimer < 0) g_nTimer = 0;
    if (g_nTimer > TIMER_LIMIT_FRAME) g_nTimer = TIMER_LIMIT_FRAME;

    int totalSec = g_nTimer / FPS;
    int min = totalSec / 60;
    int sec = totalSec % 60;

    int aTexU[NUM_PLACE];
    aTexU[0] = (min / 10) % 10;
    aTexU[1] = (min % 10);
    aTexU[2] = 0; // コロン
    aTexU[3] = (sec / 10) % 10;
    aTexU[4] = (sec % 10);

    VERTEX_2D* pVtx;
    g_pVtxBuffTimer->Lock(0, 0, (void**)&pVtx, 0);

    for (int nCntNumber = 0; nCntNumber < NUM_PLACE; nCntNumber++)
    {
        if (nCntNumber != 2)
        {
            pVtx[0].tex = D3DXVECTOR2(aTexU[nCntNumber] * 0.1f, 0.0f);
            pVtx[1].tex = D3DXVECTOR2((aTexU[nCntNumber] + 1) * 0.1f, 0.0f);
            pVtx[2].tex = D3DXVECTOR2(aTexU[nCntNumber] * 0.1f, 1.0f);
            pVtx[3].tex = D3DXVECTOR2((aTexU[nCntNumber] + 1) * 0.1f, 1.0f);
        }

        pVtx += 4;
    }

    g_pVtxBuffTimer->Unlock();
}

int GetTimer(void)
{
    return g_nTimer;
}