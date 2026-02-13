//==================================================================================
//
// DirectXのMenuのUI表示処理 [UImenu.cpp]
// Author : TENMA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "UImenu.h"
#include "Texture.h"
#include "mathUtil.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define PATH_GEAR		"data/TEXTURE/menugear.png"			// テクスチャパス
#define DEF_MENU_POS	D3DXVECTOR3(0, SCREEN_HEIGHT, 0)	// 初期位置
#define DEF_MENU_SIZE	D3DXVECTOR2(100, 100)				// デフォルトサイズ
#define OPEN_SIZE		(DEF_MENU_SIZE * 5)					// 拡大後のサイズ

//**********************************************************************************
//*** 頂点バッファ構造体 ***
//**********************************************************************************
typedef struct
{
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff;	// 頂点バッファ
	HRESULT hr;		// 直前のエラーコード
	bool bSafe;		// 格納状態
}SAFE_VERTEX;

//**********************************************************************************
//*** プロンプト構造体 ***
//**********************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;					// 位置
	D3DXVECTOR3 rot;					// 角度
	D3DXVECTOR2 size;					// サイズ
	SAFE_VERTEX vtxSafe;				// 頂点バッファ
	float s;							// Lerp変換用
	float fAngle;						// 回転用
	int nIdxTexture;					// テクスチャインデックス
	bool bUse;							// 使用状況
	bool bEnable;						// メニューとして描画するか
} UImenu, *LPUIMENU;

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************
void CreateSafeVtx(SAFE_VERTEX *pSafeVtx, bool bErrorOutput);

D3DXVECTOR3 GetPTPLerp(D3DXVECTOR3 Start, D3DXVECTOR3 End, float s);
D3DXVECTOR2 GetPTPLerp(D3DXVECTOR2 Start, D3DXVECTOR2 End, float s);
D3DXCOLOR GetColLerp(D3DXCOLOR Start, D3DXCOLOR End, float s);
void DrawPolygon(LPDIRECT3DDEVICE9 pDevice, 
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff,
	LPDIRECT3DTEXTURE9 pTexture, 
	UINT VertexFormatSize,
	DWORD FVF,
	int nNumPolygon,
	UINT OffSet = 0);

void DrawPolygonTextureArray(LPDIRECT3DDEVICE9 pDevice,
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff,
	LPDIRECT3DTEXTURE9 *pTexture,
	UINT nNumTexture,
	int *pArrayTexNo,
	UINT VertexFormatSize,
	DWORD FVF,
	int nNumPolygon,
	UINT OffSet = 0);

void DrawPolygonTextureFromIndex(LPDIRECT3DDEVICE9 pDevice,
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff,
	int nIdxTexture,
	UINT VertexFormatSize,
	DWORD FVF,
	int nNumPolygon,
	UINT OffSet = 0);

void CalcWorldMatrix(LPDIRECT3DDEVICE9 pDevice,
	D3DXMATRIX *mtxWorld,
	D3DXVECTOR3 pos,
	D3DXVECTOR3 rot,
	bool bSetTransform);

void CalcWorldMatrixFromParent(LPDIRECT3DDEVICE9 pDevice,
	D3DXMATRIX *mtxWorld,
	D3DXMATRIX *mtxParent,
	D3DXVECTOR3 pos,
	D3DXVECTOR3 rot,
	bool bSetTransform);

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
UImenu g_menu;

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitUImenu(void)
{
	LPUIMENU pMenu = &g_menu;

	ZeroMemory(&g_menu, sizeof(g_menu));

	// 初期設定
	pMenu->pos = DEF_MENU_POS;
	pMenu->size = DEF_MENU_SIZE;
	pMenu->s = 0.001f;

	// 頂点生成
	CreateSafeVtx(&pMenu->vtxSafe, true);

	if (pMenu->vtxSafe.bSafe == false)
	{
		OutputDebugString(TEXT("頂点バッファの作成に失敗しちゃいました......"));
		return;
	}

	// テクスチャの読み込み
	if (FAILED(LoadTexture(PATH_GEAR, &g_menu.nIdxTexture)))
	{
		OutputDebugString(TEXT("テクスチャの読み込みに失敗しました......"));
		return;
	}

	VERTEX_2D *pVtx = nullptr;

	// 頂点設定
	pMenu->vtxSafe.pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);
	if (pVtx == nullptr)
	{
		OutputDebugString(TEXT("頂点バッファへのアクセスに失敗しました？！"));
		return;
	}

	// 頂点位置を設定
	MyMathUtil::SetPolygonPos(pVtx, pMenu->pos, pMenu->size);

	// 座標変換用値を設定
	MyMathUtil::SetPolygonRHW(pVtx);

	// ポリゴンカラーを設定
	MyMathUtil::SetDefaultColor(pVtx);

	// ポリゴンテクスチャを設定
	MyMathUtil::SetDefaultTexture(pVtx);

	pMenu->vtxSafe.pVtxBuff->Unlock();

	pMenu->bUse = true;
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitUImenu(void)
{
	LPUIMENU pMenu = &g_menu;

	RELEASE(pMenu->vtxSafe.pVtxBuff);
}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateUImenu(void)
{
	LPUIMENU pMenu = &g_menu;

	if (pMenu->vtxSafe.bSafe == false) return;

	if (pMenu->bEnable != false)
	{
		if (pMenu->s < 1.0f)
		{
			pMenu->s += 0.02f;
			if (pMenu->s >= 1.0f)
			{
				pMenu->s = 1.0f;
			}
		}
	}
	else
	{
		if (pMenu->s > 0.001f)
		{
			pMenu->s *= 0.93f;
			if (pMenu->s <= 0.001f)
			{
				pMenu->s = 0.001f;
			}
		}
	}

	pMenu->fAngle += 0.01f;

	VERTEX_2D* pVtx = nullptr;
	pMenu->vtxSafe.pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);
	if (pVtx == nullptr) return;

	// 描画座標をLerp変換で求める
	// (※ Lerp変換めっちゃ便利だよおおおお！！！！！！
	//	   元居た位置を覚えていなくても良いし！)

	MyMathUtil::RollPolygon(pVtx, 
		GetPTPLerp(pMenu->pos, WINDOW_MID, pMenu->s), 
		GetPTPLerp(pMenu->size, OPEN_SIZE, pMenu->s).x,
		GetPTPLerp(pMenu->size, OPEN_SIZE, pMenu->s).y,
		pMenu->fAngle,
		0);

	pMenu->vtxSafe.pVtxBuff->Unlock();
}

//==================================================================================
// --- 描画 ---
//==================================================================================
void DrawUImenu(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	LPUIMENU pMenu = &g_menu;

	if (pMenu->bUse == true)
	{
		DrawPolygonTextureFromIndex(pDevice,
			pMenu->vtxSafe.pVtxBuff,
			pMenu->nIdxTexture,
			sizeof(VERTEX_2D),
			FVF_VERTEX_2D,
			1);
	}

	// デバイスの終了
	EndDevice();
}

//==================================================================================
// --- メニュー画面のオンオフ ---
//==================================================================================
void SetEnableUImenu(bool bDisp, int nIdxUImenu)
{
	LPUIMENU pMenu = &g_menu;

	pMenu->bEnable = bDisp;
}

//==================================================================================
// --- メニュー画面のオンオフ取得 ---
//==================================================================================
bool GetEnableUImenu(void)
{
	LPUIMENU pMenu = &g_menu;

	return pMenu->bEnable;
}

//==================================================================================
// --- 頂点バッファの作成 ---
//==================================================================================
void CreateSafeVtx(SAFE_VERTEX *pSafeVtx, bool bErrorOutput)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	HRESULT hr;

	// 頂点バッファの作成
	hr = pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&pSafeVtx->pVtxBuff,
		NULL);

	pSafeVtx->hr = hr;

	if (FAILED(hr))
	{
		pSafeVtx->bSafe = false;
		if (bErrorOutput) OutputDebugString(TEXT("Failed CreateVertexBuffer"));

		// デバイスの取得終了
		EndDevice();
		return;
	}

	pSafeVtx->bSafe = true;

	// デバイスの取得終了
	EndDevice();
}

//==================================================================================
// --- 二点間のLerp変換 ---
//==================================================================================
D3DXVECTOR3 GetPTPLerp(D3DXVECTOR3 Start, D3DXVECTOR3 End, float s)
{
	D3DXVECTOR3 returnLerp = D3DXVECTOR3(0, 0, 0);

	// ベクトルの差分を求める
	returnLerp = End - Start;

	// 差分をsを使用して割合計算
	returnLerp *= s;

	// 開始位置から加算して位置を取得
	returnLerp += Start;

	return returnLerp;
}

//==================================================================================
// --- 二点間のLerp変換 Vector2 ---
//==================================================================================
D3DXVECTOR2 GetPTPLerp(D3DXVECTOR2 Start, D3DXVECTOR2 End, float s)
{
	D3DXVECTOR2 returnLerp = D3DXVECTOR2(0, 0);

	// ベクトルの差分を求める
	returnLerp = End - Start;

	// 差分をsを使用して割合計算
	returnLerp *= s;

	// 開始位置から加算して位置を取得
	returnLerp += Start;

	return returnLerp;
}

//==================================================================================
// --- 二色間のLerp変換 ---
//==================================================================================
D3DXCOLOR GetColLerp(D3DXCOLOR Start, D3DXCOLOR End, float s)
{
	D3DXCOLOR returnCol = D3DXCOLOR(1, 1, 1, 1);

	returnCol = End - Start;
	returnCol *= s;
	returnCol += Start;

	return returnCol;
}

//==================================================================================
// --- ポリゴン描画(テクスチャバッファ使用) ---
//==================================================================================
void DrawPolygon(LPDIRECT3DDEVICE9 pDevice,
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff,
	LPDIRECT3DTEXTURE9 pTexture,
	UINT VertexFormatSize,
	DWORD FVF,
	int nNumPolygon,
	UINT OffSet)
{
	// NULLCHECK
	if (pDevice == nullptr)
	{
		OutputDebugString(TEXT("pDeviceが設定されていません！"));
		return;
	}

	// NULLCHECK
	if (pVtxBuff == nullptr)
	{
		OutputDebugString(TEXT("pVtxBuffが設定されていません！"));
		return;
	}

	for (int nCntPoly = OffSet; nCntPoly < nNumPolygon; nCntPoly++)
	{
		/*** 頂点バッファをデータストリームに設定 ***/
		pDevice->SetStreamSource(0, pVtxBuff, 0, VertexFormatSize);

		/*** 頂点フォーマットの設定 ***/
		pDevice->SetFVF(FVF);

		/*** テクスチャの設定 ***/
		pDevice->SetTexture(0, pTexture);

		/*** ポリゴンの描画 ***/
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,		// プリミティブの種類
			4 * nCntPoly,								// 描画する最初の頂点インデックス
			2);											// 描画するプリミティブの数
	}
}

//==================================================================================
// --- ポリゴン描画(配列テクスチャバッファ使用) ---
//==================================================================================
void DrawPolygonTextureArray(LPDIRECT3DDEVICE9 pDevice,
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff,
	LPDIRECT3DTEXTURE9 *pTexture,
	UINT nNumTexture,
	int* pArrayTexNo,
	UINT VertexFormatSize,
	DWORD FVF,
	int nNumPolygon,
	UINT OffSet)
{
	// NULLCHECK
	if (pDevice == nullptr)
	{
		OutputDebugString(TEXT("pDeviceが設定されていません！"));
		return;
	}

	// NULLCHECK
	if (pVtxBuff == nullptr)
	{
		OutputDebugString(TEXT("pVtxBuffが設定されていません！"));
		return;
	}

	// NULLCHECK
	if (pArrayTexNo == nullptr)
	{
		OutputDebugString(TEXT("pArrayTexNoが設定されていません！"));
		return;
	}

	for (int nCntPoly = OffSet; nCntPoly < nNumPolygon; nCntPoly++)
	{
		/*** 頂点バッファをデータストリームに設定 ***/
		pDevice->SetStreamSource(0, pVtxBuff, 0, VertexFormatSize);

		/*** 頂点フォーマットの設定 ***/
		pDevice->SetFVF(FVF);

		/*** テクスチャの設定 ***/
		pDevice->SetTexture(0, pTexture[pArrayTexNo[nCntPoly]]);

		/*** ポリゴンの描画 ***/
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,		// プリミティブの種類
			4 * nCntPoly,								// 描画する最初の頂点インデックス
			2);											// 描画するプリミティブの数
	}
}

//==================================================================================
// --- ポリゴン描画(GetTexture使用) ---
//==================================================================================
void DrawPolygonTextureFromIndex(LPDIRECT3DDEVICE9 pDevice,
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff,
	int nIdxTexture,
	UINT VertexFormatSize,
	DWORD FVF,
	int nNumPolygon,
	UINT OffSet)
{
	// NULLCHECK
	if (pDevice == nullptr)
	{
		OutputDebugString(TEXT("pDeviceが設定されていません！"));
		return;
	}

	// NULLCHECK
	if (pVtxBuff == nullptr)
	{
		OutputDebugString(TEXT("pVtxBuffが設定されていません！"));
		return;
	}

	for (int nCntPoly = OffSet; nCntPoly < nNumPolygon; nCntPoly++)
	{
		/*** 頂点バッファをデータストリームに設定 ***/
		pDevice->SetStreamSource(0, pVtxBuff, 0, VertexFormatSize);

		/*** 頂点フォーマットの設定 ***/
		pDevice->SetFVF(FVF);

		/*** テクスチャの設定 ***/
		pDevice->SetTexture(0, GetTexture(nIdxTexture));

		/*** ポリゴンの描画 ***/
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,		// プリミティブの種類
			4 * nCntPoly,								// 描画する最初の頂点インデックス
			2);											// 描画するプリミティブの数
	}
}

//==================================================================================
// --- マトリックス計算 ---
//==================================================================================
void CalcWorldMatrix(LPDIRECT3DDEVICE9 pDevice,
	D3DXMATRIX *mtxWorld,
	D3DXVECTOR3 pos,
	D3DXVECTOR3 rot,
	bool bSetTransform)
{
	D3DXMATRIX mtxRot, mtxTrans;		// 計算用マトリックス

	/*** ワールドマトリックスの初期化 ***/
	D3DXMatrixIdentity(mtxWorld);

	/*** 向きを反映 (※ 位置を反映する前に必ず行うこと！) ***/
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
		rot.y,			// Y軸回転
		rot.x,			// X軸回転
		rot.z);			// Z軸回転

	D3DXMatrixMultiply(mtxWorld, mtxWorld, &mtxRot);

	/*** 位置を反映 (※ 向きを反映したのちに行うこと！) ***/
	D3DXMatrixTranslation(&mtxTrans,
		pos.x,
		pos.y,
		pos.z);

	D3DXMatrixMultiply(mtxWorld, mtxWorld, &mtxTrans);

	// ワールドマトリックスの適用をする場合
	if (bSetTransform)
	{
		/*** ワールドマトリックスの設定 ***/
		pDevice->SetTransform(D3DTS_WORLD, mtxWorld);
	}
}

//==================================================================================
// --- マトリックス計算 (親マトリックス使用) ---
//==================================================================================
void CalcWorldMatrixFromParent(LPDIRECT3DDEVICE9 pDevice,
	D3DXMATRIX *mtxWorld,
	D3DXMATRIX *mtxParent,
	D3DXVECTOR3 pos,
	D3DXVECTOR3 rot,
	bool bSetTransform)
{
	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス

	// パーツのワールドマトリックス初期化
	D3DXMatrixIdentity(mtxWorld);

	// パーツの向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
		rot.y,
		rot.x,
		rot.z);

	// かけ合わせる
	D3DXMatrixMultiply(mtxWorld,
		mtxWorld,
		&mtxRot);

	// パーツの位置(オフセット)を反映
	D3DXMatrixTranslation(&mtxTrans,
		pos.x,
		pos.y,
		pos.z);

	// かけ合わせる
	D3DXMatrixMultiply(mtxWorld,
		mtxWorld,
		&mtxTrans);

	// 算出した「パーツのワールドマトリックス」と「親のマトリックス」をかけ合わせる
	D3DXMatrixMultiply(mtxWorld,
		mtxWorld,
		mtxParent);

	// ワールドマトリックスの適用をする場合
	if (bSetTransform)
	{
		/*** ワールドマトリックスの設定 ***/
		pDevice->SetTransform(D3DTS_WORLD, mtxWorld);
	}
}