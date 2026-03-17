//==================================================================================================================================
//
//			リザルトの処理 [result.cpp]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
//**************************************************************
// インクルード
#include "ranking.h"
#include "input.h"
#include "fade.h"
#include "debugproc.h"
#include "game.h"
#include "sound.h"

//**************************************************************
// グローバル変数
LPDIRECT3DTEXTURE9		g_apTextureResult[MAX_RESULT_TEX] = {};		// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9	g_pVtxBuffResult = NULL;					// 頂点バッファへのポインタ
RESULT					g_aResult[MAX_RESULT];						// 今回の結果表示
RESULT					g_aRanking[MAX_RESULT];						// ランキング表示
RESULTMODE				g_resultMode;								// リザルト内のモード
RESULTMODE				g_resultNext;								// 更新先のモード
RESULTFADE				g_resultFade;
P_RESULT				g_pThisRanking;								// 今回のランキングを指すポインタ
int						g_nCounterEnding = 0;						// エンディングカウンター
int						g_nSelectResult;
int						g_nThisScore;								// 今回のスコア
int						g_nThisRank;								// 今回の順位
int						g_nScoreMoveCouter;							// 今回のスコアが動くフレームカウンター
int						g_aHighScore[MAX_NUMSCORE] =				// ランキング
{
	50000,30000,20000,15000,10000,
};
const char*				g_aResultTexFile[] =
{
	"data\\TEXTURE\\Result.png",					// RESULTTEX_RESULT		[0]
	"data\\TEXTURE\\Score.png",						// RESULTTEX_SCORE		[1]
	"data\\TEXTURE\\Ranking.png",					// RESULTTEX_RANK		[2]
	"data\\TEXTURE\\RankNum.png",					// RESULTTEX_RANKNUM	[3]
	"data\\TEXTURE\\Number002.png",					// RESULTTEX_NUM		[4]
};
const int				g_nMaxTexResult = sizeof g_aResultTexFile / sizeof(const char*);

//**********************************************************************************
//*** 定数変数 ***
//**********************************************************************************
constexpr int c_EndingTime = 300;		// エンディング持続時間

//**************************************************************
// プロトタイプ宣言
int SortRanking(int nThisScore);
void UpdateScore(void);
void UpdateRank(void);
void UpdateResultFade(void);
void UpdateVtxResult(P_RESULT pResult);

//=========================================================================================
// リザルト初期化
//=========================================================================================
void InitRanking(void)
{
	//**************************************************************
	// 変数宣言
	LPDIRECT3DDEVICE9	pDevice = GetDevice();			// デバイスへのポインタ
	P_RESULT			pResult = GetThisScore();		// リザルト情報ポインタ
	vec3				pos, size;
	int					nCntResult = 0;
	int					nCntRanking = 0;
	int					nThisScore = 40000;// GetScore();
	int					nThisRank;

	//**************************************************************
	// 変数初期化
	memset(&g_aResult[0], 0, sizeof(RESULT) * MAX_RESULT);
	memset(&g_aRanking[0], 0, sizeof(RESULT) * MAX_RESULT);
	g_resultMode = RESULT_NONE;
	g_resultNext = RESULT_NONE;
	g_resultFade = RESULTFADE_NONE;
	g_nScoreMoveCouter = SCOREMOVE;
	g_nSelectResult = 0;
	g_nThisScore = 40000;// GetScore();
	nThisRank = SortRanking(g_nThisScore);
	g_pThisRanking = NULL;

	//**************************************************************
	// テクスチャ読み込み
	for (int nCntTex = 0; nCntTex < g_nMaxTexResult; nCntTex++)
	{
		D3DXCreateTextureFromFile(pDevice, g_aResultTexFile[nCntTex], &g_apTextureResult[nCntTex]);
	}

	//**************************************************************
	// 頂点バッファの読み込み
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_RESULT,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffResult,
		NULL);

	//**************************************************************
	//  リザルト情報初期化
	// "RESULT"
	pos = vec3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.15f, 0.0f);
	size = vec3(SCREEN_WIDTH * 0.25f, SCREEN_HEIGHT * 0.1f, 0.0f);
	SetResult(pos, size, vec3_ZERO, colX_RED, RESULTTEX_RESULT, -1, GetThisScore());

	// "SCORE"
	pos = vec3(SCREEN_WIDTH * 0.4f, SCREEN_HEIGHT * 0.4f, 0.0f);
	size = vec3(SCREEN_WIDTH * 0.2f, SCREEN_HEIGHT * 0.06f, 0.0f);
	SetResult(pos, size, vec3_ZERO, colX_BLUE, RESULTTEX_SCORE, -1, GetThisScore());

	// 今回のスコア
	for (int nCntNum = MAX_SCOREDIGIT - 1; 0 <= nCntNum; nCntNum--)
	{
		pos = vec3(SCREEN_WIDTH * 0.65f + ((nCntNum - 1) * 40.0f),SCREEN_HEIGHT * 0.4f, 0.0f);
		size = vec3(SCREEN_WIDTH * 0.018f, SCREEN_HEIGHT * 0.06f, 0.0f);
		SetResult(pos, size, vec3_ZERO, colX_WHITE, RESULTTEX_NUM, nThisScore % 10, GetThisScore());
		nThisScore *= 0.1f;
	}

	//**************************************************************
	//  ランキング初期化
	// "RANKING"
	pos = vec3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.15f, 0.0f);
	size = vec3(SCREEN_WIDTH * 0.25f, SCREEN_HEIGHT * 0.1f, 0.0f);
	SetResult(pos, size, vec3_ZERO, colX_WHITE, RESULTTEX_RANK, -1, GetRanking());

	// 順位
	for (int nCntRank = 0; nCntRank < MAX_NUMSCORE; nCntRank++)
	{
		pos = vec3(SCREEN_WIDTH * 0.35f, SCREEN_HEIGHT * (nCntRank + 2) * 0.15f, 0.0f);
		size = vec3(SCREEN_WIDTH * 0.043f, SCREEN_HEIGHT * 0.08f, 0.0f);
		SetResult(pos, size, vec3_ZERO, colX_WHITE, RESULTTEX_RANKNUM, nCntRank, GetRanking());
	}

	// スコア
	for (int nCntRank = 0; nCntRank < MAX_NUMSCORE; nCntRank++)
	{
		nThisScore = g_aHighScore[nCntRank];
		for (int nCntNum = MAX_SCOREDIGIT - 1; 0 <= nCntNum; nCntNum--)
		{
			pos = vec3(SCREEN_WIDTH * 0.55f + ((nCntNum - 1) * 65.0f), SCREEN_HEIGHT * (nCntRank + 2) * 0.15f, 0.0f);
			size = vec3(SCREEN_WIDTH * 0.03f, SCREEN_HEIGHT * 0.06f, 0.0f);
			if (nCntNum == MAX_SCOREDIGIT - 1 && nThisRank == nCntRank)
			{// 今回のスコアの順位の一つ目のアドレス
				g_pThisRanking = SetResult(pos, size, vec3_ZERO, colX_WHITE, RESULTTEX_NUM, nThisScore % 10, GetRanking());
			}
			else
			{
				SetResult(pos, size, vec3_ZERO, colX_WHITE, RESULTTEX_NUM, nThisScore % 10, GetRanking());
			}
			nThisScore *= 0.1f;
		}
	}	
	g_nCounterEnding = 0;

	// リザルト内のモード
	SetResultMode(RESULT_SCORE);

	EndDevice();
}

//=========================================================================================
// ランキング並べ替え
int SortRanking(int nThisScore)
{
	int nTmp,nRank = MAX_NUMSCORE;

	for (int nCntI = 0; nCntI < MAX_NUMSCORE; nCntI++)
	{
		if (nThisScore <= g_aHighScore[nCntI])
		{
			continue;
		}
		else
		{
			nTmp = g_aHighScore[nCntI];
			g_aHighScore[nCntI] = nThisScore;
			nThisScore = nTmp;
			if (nRank == MAX_NUMSCORE)
			{
				nRank = nCntI;
			}
		}
	}
	return nRank;
}

//=========================================================================================
// リザルト終了
//=========================================================================================
void UninitRanking(void)
{
	//**************************************************************
	// テクスチャの破棄
	for (int nCntTex = 0; nCntTex < MAX_RESULT_TEX; nCntTex++)
	{
		if (g_apTextureResult[nCntTex] != NULL)
		{
			g_apTextureResult[nCntTex]->Release();
			g_apTextureResult[nCntTex] = NULL;
		}
	}

	//**************************************************************
	// 頂点バッファの破棄
	if (g_pVtxBuffResult != NULL)
	{
		g_pVtxBuffResult->Release();
		g_pVtxBuffResult = NULL;
	}

}

//=========================================================================================
// リザルト更新
//=========================================================================================
void UpdateRanking(void)
{
	if (g_resultFade == RESULTFADE_NONE)
	{
		switch (g_resultMode)
		{
			//**************************************************************
			// クリア画面更新
		case RESULT_SCORE:
			UpdateScore();
			break;

			//**************************************************************
			// ランキング更新
		case RESULT_RANGING:
			UpdateRank();
			break;

		default:
			break;
		}
	}
	else
	{
		UpdateResultFade();
	}

	g_nCounterEnding++;
}

//=========================================================================================
// スコア表示更新
void UpdateScore(void)
{
	//**************************************************************
	// 次へ
	if (GetKeyboardTrigger(DIK_RETURN)
		|| GetJoypadTrigger(0, JOYKEY_A)
		|| GetJoypadTrigger(0, JOYKEY_START)
		|| g_nCounterEnding >= c_EndingTime)
	{
		if (GetFade() == FADE_NONE)
		{
			SetResultMode(RESULT_RANGING);
			g_nCounterEnding = 0;
		}
	}

	P_RESULT pResult = GetThisScore();
	pResult += 2;

	//**************************************************************
	// スコアのアニメーション
	for (int nCntNum = 0; nCntNum < MAX_SCOREDIGIT; nCntNum++)
	{
		(pResult + nCntNum)->rot.x += 0.05f;
		(pResult + nCntNum)->pos.y += sinf((pResult + nCntNum)->rot.x + nCntNum) * 0.3f;

		(pResult + nCntNum)->col.r = 1 - sinf((pResult + nCntNum)->rot.x) * 0.3f;
		(pResult + nCntNum)->col.g = 1 - sinf((pResult + nCntNum)->rot.x) * 0.3f;
		(pResult + nCntNum)->col.b = 1 - sinf((pResult + nCntNum)->rot.x) * 0.3f;
	}

	//**************************************************************
	// 頂点の更新
	UpdateVtxResult(GetThisScore());
}

//=========================================================================================
// ランキング更新
void UpdateRank(void)
{
	//**************************************************************
	// 次へ
	if (GetKeyboardTrigger(DIK_RETURN)
		|| GetJoypadTrigger(0, JOYKEY_A)
		|| GetJoypadTrigger(0, JOYKEY_START)
		|| g_nCounterEnding >= c_EndingTime)
	{
		if (GetFade() == FADE_NONE)
		{
			SetFade(MODE_TEAMLOGO);
			StopSound();
		}
	}

	//**************************************************************
	// 表示を更新
	if (g_pThisRanking)
	{
		for (int nCntNum = 0; nCntNum < MAX_SCOREDIGIT; nCntNum++)
		{
			// 回転
			if (0 < g_nScoreMoveCouter)
			{
				(g_pThisRanking + nCntNum)->rot.z += 0.08f;
			}
			else
			{
				(g_pThisRanking + nCntNum)->rot.z = 0.0f;
			}

			// 色の変化
			(g_pThisRanking + nCntNum)->rot.x += 0.05f;
			(g_pThisRanking + nCntNum)->pos.y += sinf((g_pThisRanking + nCntNum)->rot.x + nCntNum) * 0.3f;

			(g_pThisRanking + nCntNum)->col.r = 1 - sinf((g_pThisRanking + nCntNum)->rot.x) * 0.3f;
			(g_pThisRanking + nCntNum)->col.g = 1 - sinf((g_pThisRanking + nCntNum)->rot.x) * 0.3f;
			(g_pThisRanking + nCntNum)->col.b = 1 - sinf((g_pThisRanking + nCntNum)->rot.x) * 0.3f;
		}
		g_nScoreMoveCouter--;
	}

	//**************************************************************
	// 頂点の更新
	UpdateVtxResult(GetRanking());
}

//=========================================================================================
// フェードの更新
void UpdateResultFade(void)
{
	P_RESULT pResult = GetThisModeRanking();
	
	if (g_resultFade == RESULTFADE_IN)
	{
		g_resultFade = RESULTFADE_NONE;
	}
	else if (g_resultFade == RESULTFADE_OUT)
	{
		if (g_resultMode == RESULT_NONE || RESULT_MAX < g_resultMode)
		{
			g_resultMode = g_resultNext;
			g_resultFade = RESULTFADE_IN;
		}
		else if(pResult)
		{
			g_resultMode = g_resultNext;
			g_resultFade = RESULTFADE_IN;
		}
	}

	UpdateVtxResult(GetThisModeRanking());
}

//=========================================================================================
// 頂点更新
void UpdateVtxResult(P_RESULT pResult)
{
	//**************************************************************
	// 変数宣言
	VERTEX_2D*	pVtx;				// 頂点情報へのポインタ
	int			nTexPosMax;

	//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	if (g_pVtxBuffResult && pResult)
	{
		g_pVtxBuffResult->Lock(0, 0, (void**)&pVtx, 0);
		for (int nCntResult = 0; nCntResult < MAX_RESULT; nCntResult++, pResult++)
		{
			if (pResult->bUse)
			{
				// 頂点座標を設定
				pVtx[nCntResult * 4 + 0].pos = VECTOR3_ANGLE(pResult->pos, pResult->fLength, pResult->rot.z + pResult->fAngle);
				pVtx[nCntResult * 4 + 1].pos = VECTOR3_ANGLE(pResult->pos, pResult->fLength, pResult->rot.z - pResult->fAngle);
				pVtx[nCntResult * 4 + 2].pos = VECTOR3_ANGLE(pResult->pos, pResult->fLength, pResult->rot.z + D3DX_PI - pResult->fAngle);
				pVtx[nCntResult * 4 + 3].pos = VECTOR3_ANGLE(pResult->pos, pResult->fLength, pResult->rot.z - D3DX_PI + pResult->fAngle);

				// rhwの設定
				pVtx[nCntResult * 4 + 0].rhw = 1.0f;
				pVtx[nCntResult * 4 + 1].rhw = 1.0f;
				pVtx[nCntResult * 4 + 2].rhw = 1.0f;
				pVtx[nCntResult * 4 + 3].rhw = 1.0f;

				// 頂点カラー設定
				pVtx[nCntResult * 4 + 0].col = pResult->col;
				pVtx[nCntResult * 4 + 1].col = pResult->col;
				pVtx[nCntResult * 4 + 2].col = pResult->col;
				pVtx[nCntResult * 4 + 3].col = pResult->col;

				// テクスチャの座標設定
				if (-1 < pResult->nTexPos)
				{// 数字のテスクチャなら
					nTexPosMax = (pResult->nTexPos + 1);
					pVtx[nCntResult * 4 + 0].tex = D3DXVECTOR2(0.1f * pResult->nTexPos, 0.0f);
					pVtx[nCntResult * 4 + 1].tex = D3DXVECTOR2(0.1f * nTexPosMax, 0.0f);
					pVtx[nCntResult * 4 + 2].tex = D3DXVECTOR2(0.1f * pResult->nTexPos, 1.0f);
					pVtx[nCntResult * 4 + 3].tex = D3DXVECTOR2(0.1f * nTexPosMax, 1.0f);
				}
				else
				{// それ以外は１枚絵
					pVtx[nCntResult * 4 + 0].tex = D3DXVECTOR2(0.0f, 0.0f);
					pVtx[nCntResult * 4 + 1].tex = D3DXVECTOR2(1.0f, 0.0f);
					pVtx[nCntResult * 4 + 2].tex = D3DXVECTOR2(0.0f, 1.0f);
					pVtx[nCntResult * 4 + 3].tex = D3DXVECTOR2(1.0f, 1.0f);
				}
			}
			else
			{
				// 頂点座標を設定
				pVtx[nCntResult * 4 + 0].pos = vec3_ZERO;
				pVtx[nCntResult * 4 + 1].pos = vec3_ZERO;
				pVtx[nCntResult * 4 + 2].pos = vec3_ZERO;
				pVtx[nCntResult * 4 + 3].pos = vec3_ZERO;

				// 頂点カラー設定
				pVtx[nCntResult * 4 + 0].col = colX_WHITE;
				pVtx[nCntResult * 4 + 1].col = colX_WHITE;
				pVtx[nCntResult * 4 + 2].col = colX_WHITE;
				pVtx[nCntResult * 4 + 3].col = colX_WHITE;

			}
		}
		// 頂点バッファのロック解除
		g_pVtxBuffResult->Unlock();
		//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	}
}

//=========================================================================================
// リザルト描画
//=========================================================================================
void DrawRanking(void)
{
	//**************************************************************
	// 変数宣言
	LPDIRECT3DDEVICE9	pDevice = GetDevice();		//デバイスへのポインタ
	P_RESULT			pResult;

	//頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0,
		g_pVtxBuffResult,
		0,
		sizeof(VERTEX_2D));

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	if (g_resultMode == RESULT_SCORE)
	{// スコア表示モード
		pResult = GetThisScore();
		for (int nCntResult = 0; nCntResult < MAX_RESULT; nCntResult++, pResult++)
		{
			if (pResult->bUse)
			{
				//テクスチャの設定
				pDevice->SetTexture(0, g_apTextureResult[pResult->nTex]);

				//ポリゴンの描画
				pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntResult * 4, 2);
			}
		}
	}
	else if (g_resultMode == RESULT_RANGING)
	{// ランキング表示モード
		pResult = GetRanking();
		for (int nCntResult = 0; nCntResult < MAX_RESULT; nCntResult++, pResult++)
		{
			if (pResult->bUse)
			{
				//テクスチャの設定
				pDevice->SetTexture(0, g_apTextureResult[pResult->nTex]);

				//ポリゴンの描画
				pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntResult * 4, 2);
			}
		}
	}
	EndDevice();
}

//=========================================================================================
// リザルト情報取得
//=========================================================================================
P_RESULT GetThisScore(void)
{
	return &g_aResult[0];
}

//=========================================================================================
// リザルト情報取得
//=========================================================================================
P_RESULT GetRanking(void)
{
	return &g_aRanking[0];
}

//=========================================================================================
// リザルト情報取得
//=========================================================================================
P_RESULT GetThisModeRanking(void)
{
	switch (g_resultMode)
	{
	case RESULT_SCORE:
		return GetThisScore();
		break;

	case  RESULT_RANGING:
		return GetRanking();
		break;

	default:
		return NULL;
		break;
	}
}

//=========================================================================================
// リザルトモードセット
//=========================================================================================
void SetResultMode(RESULTMODE next)
{
	g_resultNext = next;
	g_resultFade = RESULTFADE_OUT;
}

//=========================================================================================
// リザルトプリミティブセット
//=========================================================================================
P_RESULT SetResult(vec3 pos, vec3 size, vec3 rot, colX col, int nTex, int nTexPos, P_RESULT pResult)
{
	for (int nCntResult = 0; nCntResult < MAX_RESULT; nCntResult++, pResult++)
	{
		if (pResult->bUse == false)
		{
			pResult->pos = pos;
			pResult->size = size;
			pResult->rot = rot;
			pResult->col = col;
			pResult->fAngle = atan2f(-pResult->size.x, -pResult->size.y);
			pResult->fLength = sqrtf(pResult->size.x * pResult->size.x + pResult->size.y * pResult->size.y);
			pResult->nTex = nTex;
			pResult->nTexPos = nTexPos;
			pResult->bUse = true;

			return pResult;
		}
	}
	return NULL;
}