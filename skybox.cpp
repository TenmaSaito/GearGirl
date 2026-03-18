//==================================================================================
//
// DirectXのスカイボックスのcppファイル [skybox.cpp]
// Author : TENMA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "skybox.h"
#include "tCamera.h"
#include "mathUtil.h"
#include "mode.h"

using namespace MyMathUtil;

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define SKYBOX_SHADER_FILENAME	"data\\FX\\skybox.fx"			// スカイボックスシェーダーパス
#define SKYBOX_TEX_FILENAME		"data\\TEXTURE\\Space2.dds"		// スカイボックステクスチャ名
#define TECHNIQUE_NAME			"SkyTec"						// テクニック名

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
LPDIRECT3DCUBETEXTURE9 g_pCubeTex = nullptr;	// キューブマップバッファ
LPD3DXEFFECT g_pEffectSkybox = nullptr;			// シェーダーバッファ
LPD3DXMESH g_pMeshSphere = nullptr;				// スフィアバッファ
bool g_bUseSkybox = false;						// スカイボックスの使用情報

//**********************************************************************************
//*** パス指定 ***
//**********************************************************************************
SKYBOX_TYPE SKYBOX::CLOUD = "data\\TEXTURE\\CLOUD.dds";
SKYBOX_TYPE SKYBOX::NORMAL = "data\\TEXTURE\\Skymap5.dds";
SKYBOX_TYPE SKYBOX::SPACE = "data\\TEXTURE\\Space2.dds";
SKYBOX_TYPE SKYBOX::REDMOON = "data\\TEXTURE\\RedMoon.dds";

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitSkybox(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	HRESULT hr = S_OK;					// 読み込み判定
	SHADER_PATH path = SKYBOX_SHADER_FILENAME;

	hr = LoadSheder(pDevice, path, &g_pEffectSkybox);

	if (FAILED(hr))
	{ // 読み込み失敗時
		GenerateMessageBox(MB_ICONERROR,
			"Error",
			"シェーダーの読み込みに失敗しちゃいました！\nこの子が悪いです！\n対象パス : %s",
			SKYBOX_SHADER_FILENAME);
	}

	// スカイボックス用メッシュ作成
	hr = D3DXCreateSphere(pDevice,		// デバイス
		10000,							// 半径(カメラの最遠距離より遠いとよし)
		128,							// 分割数
		128,							// 分割数
		&g_pMeshSphere,					// メッシュへのポインタ
		NULL);							// NULL

	if (FAILED(hr))
	{ // エラー取得後、メッセージ描画
		char aErrorStr[256];
		GetErrorMessage(hr, &aErrorStr[0], sizeof(aErrorStr), true);
	}

	EndDevice();

	g_bUseSkybox = false;
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitSkybox(void)
{
	RELEASE(g_pCubeTex);

	RELEASE(g_pEffectSkybox);

	RELEASE(g_pMeshSphere);
}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateSkybox(void)
{
	
}

//==================================================================================
// --- 描画 ---
//==================================================================================
void DrawSkybox(void)
{
	if (g_bUseSkybox == false) return;

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXMATRIX mtxWorldViewProjection;
	D3DXMATRIX mtxView, mtxProj, mtxWorld;

	// Zテスト無効
	SetEnableZFunction(pDevice, false);

	// カリングタイプ変更
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	// シェーダーの開始
	SetSheder(g_pEffectSkybox,
		TECHNIQUE_NAME,
		0);

	// キューブマップ適用
	g_pEffectSkybox->SetTexture("Skybox", g_pCubeTex);

	// ビュー行列とプロジェクション行列を取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxProj);

	// カメラの情報を取得
	LPTCAMERA pCam = GetTCamera();

	if (GetMode() == MODE_TITLE)
	{
		D3DXMatrixIdentity(&mtxView);

		// 角度
		static float fAng = 0.0f;
		fAng += 0.0005f;

		// ビューマトリックスの作成
		V3 rot = Arc(100.0f, fAng);
		rot = V3(rot.x, pCam->posR.y - 42.0f, rot.y);

		D3DXMatrixLookAtLH(&mtxView, &CParamVector::V3NULL, &rot, &pCam->vecU);

	}

	// カメラのワールドマトリックスを作成
	CalcWorldMatrix(&mtxWorld, pCam->posV, VECNULL);

	// 行列の掛け算で、座標変換行列の生成
	D3DXMatrixIdentity(&mtxWorldViewProjection);
	mtxWorldViewProjection = mtxWorldViewProjection * mtxWorld * mtxView * mtxProj;

	// ワールドビューマトリックスを設定
	g_pEffectSkybox->SetMatrix("WorldViewProj", &mtxWorldViewProjection);

	// 描画
	g_pMeshSphere->DrawSubset(0);

	// シェーダーの終了
	RemovePass(g_pEffectSkybox);

	// カリングタイプ変更
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// Zテスト有効
	SetEnableZFunction(pDevice, true);

	// デバイスの終了
	EndDevice();
}

//==================================================================================
// --- スカイボックスの設置 ---
//==================================================================================
void SetSkybox(SKYBOX_TYPE type)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	HRESULT hr = S_OK;					// 読み込み判定

	// 今あるテクスチャバッファを解放
	RELEASE(g_pCubeTex);

	// スカイボックステクスチャの読み込み処理
	hr = D3DXCreateCubeTextureFromFile(pDevice,
		type,
		&g_pCubeTex);

	if (FAILED(hr))
	{ // エラー取得後、メッセージ描画
		char aErrorStr[256];
		GetErrorMessage(hr, &aErrorStr[0], sizeof(aErrorStr), true);
	}

	g_bUseSkybox = true;

	// デバイスの終了
	EndDevice();
}