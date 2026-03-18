//==================================================================================
//
// DirectXのギミック処理 [gimmick.cpp]
// Author : TENMA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "effect.h"
#include "gimmick.h"
#include "motion.h"
#include "mesh.h"
#include "modeldata.h"
#include "param.h"
#include "prompt.h"
#include "particle.h"
#include "Texture.h"
#include "item.h"
#include "input.h"
#include "2Dpolygon.h"
#include "itemEffector.h"
#include "sound.h"

#include "Conditional_defs.h"
#include "Color_defs.h"
#include "MyInline.inl"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define POS_TUTORIAL	D3DXVECTOR3(SCREEN_WIDTH / 2, 400.0f, 0.0f)	// チュートリアル1枚絵を出す位置

//**********************************************************************************
//*** ギミックチュートリアルの列挙型 ***
//**********************************************************************************
typedef enum
{
	TUTORIALTYPE_CATAPALT = 0,
	TUTORIALTYPE_CHAINSAW,
	TUTORIALTYPE_VALVE,
	TUTORIALTYPE_MAX
}TUTORIALTYPE;

//**********************************************************************************
//*** ギミッククリア構造体 ***
//**********************************************************************************
typedef struct
{
	const char* pFileName;		// ファイル名
	COULD_PLAYER could;			// クリア可能なプレイヤータイプ
	D3DXVECTOR3 posDefault;		// デフォルト位置
	D3DXVECTOR3 rotDefault;		// デフォルト角度
	float fRadius;				// 当たり判定の半径
} GIMMICK_DATA;

//**********************************************************************************
//*** アイテム出現構造体 ***
//**********************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;	// 位置
	D3DXVECTOR3 move;	// 加速度
	IDX_ITEM nIdxItem;	// 出現するアイテム番号
	bool bSpawned;		// 出現済み判定
} ItemSpawn;

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************
void CaseMulti(LPGIMMICK pGimmick);
void CaseSolo(LPGIMMICK pGimmick);
void SetMotionType(MOTIONTYPE, bool bBlend, int nFrameBlend, GIMMICKTYPE);
void UpdateMotion(GIMMICKTYPE);
void SpawnItem(void);
void UpdateSpawnItem(void);
void UpdateGimmickCylinder(void);

//**********************************************************************************
//*** 定数変数 ***
//**********************************************************************************
const int g_nNumSpawnItem = 2;							// 噴水ギミッククリア時に出現するアイテム数
const ITEMTYPE g_aIdxItem[g_nNumSpawnItem] = { ITEMTYPE_GEARS_TRUE, ITEMTYPE_SPRING_FALSE };	// 出現するアイテム番号
const float g_fResistPow = 0.05f;						// 加速度の減速係数
const float g_fRadiusGCMin = 45.0f;						// 最小半径
const float g_fRadiusGCMax = 200.0f;					// 最大半径
const float g_fRadiusDetectionGC = 30.0f;				// 検知半径
const D3DXVECTOR3 g_aMoveSpawn[g_nNumSpawnItem] =		// 出現後のアイテムの加速度
{
	D3DXVECTOR3(-3.0f, 5.0f, -5.0f),
	D3DXVECTOR3(5.0f, 5.0f, 3.0f),
};

const D3DXVECTOR3 g_posSpawn = D3DXVECTOR3(1000, 105, 110);			// 出現開始位置
const D3DXVECTOR3 g_aVecParticle[2] =								// パーティクルの上下限ベクトル
{
	D3DXVECTOR3(500, 2000, 500),
	D3DXVECTOR3(-250, 0, -250),
};

const char* g_apGimmickPromptTexture[GIMMICKTYPE_MAX] =		// 各ギミックのプロンプト
{
	"data/TEXTURE/UIprompt/promptMouse.png",	// 人間用ボタン
	"UNUSED",									// UNUSED	
	"data/TEXTURE/UIprompt/promptWood.png",		// 倒木1
	"data/TEXTURE/UIprompt/promptWood.png",		// 倒木2
	"data/TEXTURE/UIprompt/promptWood.png",		// 倒木3
	"UNUSED",									// UNUSED
	"UNUSED",									// UNUSED
	"data/TEXTURE/UIprompt/promptBulb.png",		// バルブ
};

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
Gimmick g_aGimmick[GIMMICKTYPE_MAX];		// ギミック情報
GIMMICK_DATA g_aGimmickData[GIMMICKTYPE_MAX] =
{
	{"data/Scripts/bigbutton_g.txt", COULD_PLAYER_GIRL, D3DXVECTOR3(535, 100, -630), D3DXVECTOR3(0, 0, 0), 0.0f},
	{"data/Scripts/smallbutton_g.txt", COULD_PLAYER_MOUSE, D3DXVECTOR3(573, 100, -900), D3DXVECTOR3(0, 0, 0), 0.0f},
	{"data/Scripts/fallenTree.txt", COULD_PLAYER_GIRL, D3DXVECTOR3(1695, 100, 460), D3DXVECTOR3(0, CParamFloat::HALFPI, 0), 30.0f},
	{"data/Scripts/fallenTree.txt", COULD_PLAYER_GIRL, D3DXVECTOR3(1920, 100, 630), D3DXVECTOR3(0, CParamFloat::HALFPI, 0), 30.0f},
	{"data/Scripts/fallenTree.txt", COULD_PLAYER_GIRL, D3DXVECTOR3(1750, 100, 400), D3DXVECTOR3(0, 0, 0), 30.0f},
	{"data/Scripts/station_g.txt", COULD_PLAYER_ALL, D3DXVECTOR3(550, 100, -970), D3DXVECTOR3(0, D3DX_PI + CParamFloat::HALFPI, 0), 200.0f },
	{"data/Scripts/tunnel_g.txt", COULD_PLAYER_ALL, D3DXVECTOR3(1609, 10000, -760), D3DXVECTOR3(0, D3DX_PI + CParamFloat::HALFPI, 0), 200.0f },
	{"data/Scripts/statue_g.txt", COULD_PLAYER_GIRL, D3DXVECTOR3(1100, 100, 110), D3DXVECTOR3(0, CParamFloat::HALFPI, 0), 25.0f },
};

ItemSpawn g_aItemSpawn[g_nNumSpawnItem];	// 出現アイテム情報
IDX_TEXTURE g_nIdxTexTutorial[3];			// チュートリアル1枚絵用のインデックス
bool g_bAnyTex;								// いずれかのテクスチャが表示されているか
bool g_bDispTutorialChainsaw;				// 1度でもチェンソーチュートリアルテクスチャを表示したかどうか
bool g_bDispTutorialvalve;					// 1度でもバルブチュートリアルテクスチャを表示したかどうか
int g_aIdxGimmickCylinde[GC_MAX];			// 各主要ギミックのエリアシリンダーのインデックス
float g_aRadiusGimmickCylinder[GC_MAX];		// 各主要ギミックのシリンダー半径
GC_DATA g_aGCData[GC_MAX];		// シリンダー関連情報

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitGimmick(void)
{
	// 構造体を初期化
	memset(&g_aGimmick[0], NULL, sizeof(Gimmick) * GIMMICKTYPE_MAX);

	// ギミック数分だけ
	for (int nCntMotion = 0; nCntMotion < GIMMICKTYPE_MAX; nCntMotion++)
	{
		int nIdxMotion = -1;

		// ギミックの種類を保存
		g_aGimmick[nCntMotion].myType = (GIMMICKTYPE)nCntMotion;

		// モーションを読み込み
		if (LoadMotion(g_aGimmickData[nCntMotion].pFileName, &nIdxMotion) == true)
		{
			LPPARTS_INFO pPartsInfo = GetPartsInfo(nIdxMotion);	// パーツ情報のアドレスを取得
			if (pPartsInfo != nullptr)
			{ // NULLCHECK
				g_aGimmick[nCntMotion].parts = *pPartsInfo;		// アドレスの中身のみをコピー

				for (int nCntParts = 0; nCntParts < g_aGimmick[nCntMotion].parts.nNumParts; nCntParts++)
				{
					PARTS* pParts = &g_aGimmick[nCntMotion].parts.aParts[nCntParts];
					pParts->posLocal = pParts->pos;
					pParts->rotLocal = pParts->rot;
				}
			}

			LPMOTIONSCRIPT_INFO pScrInfo = nullptr;
			pScrInfo = GetMotionScriptInfo(nIdxMotion);
			if (pScrInfo != nullptr)
			{ // NULLCHECK
				for (int nCntRead = 0; nCntRead < pScrInfo->nNumMotion; nCntRead++)
				{ // モーション数分保存
					g_aGimmick[nCntMotion].aMotionInfo[nCntRead] = pScrInfo->aMotionInfo[nCntRead];
				}

				g_aGimmick[nCntMotion].bUse = true;

				g_aGimmick[nCntMotion].could = g_aGimmickData[nCntMotion].could;
				g_aGimmick[nCntMotion].pos = g_aGimmickData[nCntMotion].posDefault;
				g_aGimmick[nCntMotion].rot = g_aGimmickData[nCntMotion].rotDefault;
				g_aGimmick[nCntMotion].fRadius = g_aGimmickData[nCntMotion].fRadius;

				// テクスチャ読み込み判定
				if (C_FAILED(g_apGimmickPromptTexture[nCntMotion], "UNUSED"))
				{
					IDX_TEXTURE Tex;
					LoadTexture(g_apGimmickPromptTexture[nCntMotion], &Tex);
					g_aGimmick[nCntMotion].nIdxPrompt = SetPrompt(g_aGimmick[nCntMotion].pos, D3DXVECTOR2(15.0f, 8.0f), Tex, false);
					SetEnablePrompt(true, g_aGimmick[nCntMotion].nIdxPrompt);
				}
			}
		}
	}

	// 初期化
	AutoZeroMemory(g_aItemSpawn);

	g_bDispTutorialChainsaw = false;
	g_bDispTutorialvalve = false;

	// インデックスを保存
	for (int nCntSpawn = 0; nCntSpawn < g_nNumSpawnItem; nCntSpawn++)
	{
		g_aItemSpawn[nCntSpawn].nIdxItem = g_aIdxItem[nCntSpawn];
	}

	// === チュートリアルの2Dポリゴンのインデックスを取得 === //
	int TexTutorial;

	// 各ギミックのチュートリアルポリゴンの設置
	LoadTexture("data/TEXTURE/catapalttutorial.png", &TexTutorial);
	g_nIdxTexTutorial[0] = Set2DPolygon(POS_TUTORIAL, VECNULL, D3DXVECTOR2(900.0f, 610.0f), TexTutorial, DEF_COL);
	SetEnable2DPolygon(g_nIdxTexTutorial[TUTORIALTYPE_CATAPALT], false);

	LoadTexture("data/TEXTURE/chainsawtutorial.png", &TexTutorial);
	g_nIdxTexTutorial[1] = Set2DPolygon(POS_TUTORIAL, VECNULL, D3DXVECTOR2(900.0f, 610.0f), TexTutorial, DEF_COL);
	SetEnable2DPolygon(g_nIdxTexTutorial[TUTORIALTYPE_CHAINSAW], false);

	LoadTexture("data/TEXTURE/VALVEtutorial.png", &TexTutorial);
	g_nIdxTexTutorial[2] = Set2DPolygon(POS_TUTORIAL, VECNULL, D3DXVECTOR2(900.0f, 610.0f), TexTutorial, DEF_COL);
	SetEnable2DPolygon(g_nIdxTexTutorial[TUTORIALTYPE_VALVE], false);

	// 各ギミックのエリアシリンダーの設置
	g_aGCData[GC_STATUE].nIdxMeshCylinder = SetMeshCylinder(g_aGimmick[GIMMICKTYPE_STATUE].pos, VECNULL, COL_HALFBLUE, g_fRadiusGCMin, 1500.0f, 1, 8);
	SetEnableMeshCylinder(g_aGCData[GC_STATUE].nIdxMeshCylinder, true);
	g_aGCData[GC_STATUE].type = GIMMICKTYPE_STATUE;
	g_aGCData[GC_STATUE].typeDetection = GIMMICKTYPE_STATUE;
	g_aGCData[GC_STATUE].s = 0.0f;
	g_aGCData[GC_STATUE].fAngle = 0.0f;
	g_aGCData[GC_STATUE].col = COL_HALFBLUE;
	g_aGCData[GC_STATUE].bEnable = true;

	g_aGCData[GC_FALLENTREE].nIdxMeshCylinder = SetMeshCylinder(g_aGimmick[GIMMICKTYPE_FALLENTREE].pos, VECNULL, COL_HALFGREEN, g_fRadiusGCMin, 1500.0f, 1, 8);
	SetEnableMeshCylinder(g_aGCData[GC_FALLENTREE].nIdxMeshCylinder, true);
	g_aGCData[GC_FALLENTREE].type = GIMMICKTYPE_FALLENTREE;
	g_aGCData[GC_FALLENTREE].typeDetection = GIMMICKTYPE_FALLENTREE;
	g_aGCData[GC_FALLENTREE].s = 0.0f;
	g_aGCData[GC_FALLENTREE].fAngle = 0.0f;
	g_aGCData[GC_FALLENTREE].col = COL_HALFGREEN;
	g_aGCData[GC_FALLENTREE].bEnable = true;
	
	g_aGCData[GC_BIGBUTTON].nIdxMeshCylinder = SetMeshCylinder(g_aGimmick[GIMMICKTYPE_BIGBUTTON].pos, VECNULL, COL_HALFYELLOW, g_fRadiusGCMin, 1500.0f, 1, 8);
	SetEnableMeshCylinder(g_aGCData[GC_BIGBUTTON].nIdxMeshCylinder, true);
	g_aGCData[GC_BIGBUTTON].type = GIMMICKTYPE_CLOSEDDOOR;
	g_aGCData[GC_BIGBUTTON].typeDetection = GIMMICKTYPE_BIGBUTTON;
	g_aGCData[GC_BIGBUTTON].s = 0.0f;
	g_aGCData[GC_BIGBUTTON].fAngle = 0.0f;
	g_aGCData[GC_BIGBUTTON].col = COL_HALFYELLOW;
	g_aGCData[GC_BIGBUTTON].bEnable = true;
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitGimmick(void)
{

}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateGimmick(void)
{
	LPGIMMICK pGimmick = &g_aGimmick[0];
	ItemSpawn* pItemSpawn = &g_aItemSpawn[0];

	// もし更新すべき何かがあるならここで更新
	for (int nCntGimmick = 0; nCntGimmick < GIMMICKTYPE_MAX; nCntGimmick++, pGimmick++)
	{
		if (pGimmick->bUse == false) continue;

		if (GetNumPlayer() == 1)
		{ // ソロプレイ時
			CaseSolo(pGimmick);
		}
		else
		{ // マルチプレイ時
			CaseMulti(pGimmick);
		}

		LPGIMMICK pTarget = &g_aGimmick[GIMMICKTYPE_SMALLBUTTON];
		LPGIMMICK pTargetBig = &g_aGimmick[GIMMICKTYPE_BIGBUTTON];

		if (pTarget->bClear == true && pTargetBig->bClear == true && pGimmick->bClear == false && pGimmick->myType == GIMMICKTYPE_CLOSEDDOOR)
		{ // ネズミのボタンが押されたとき、扉ギミックをクリア判定
			pGimmick->bClear = true;
			SetMotionType(MOTIONTYPE_ACTION, false, 0, GIMMICKTYPE_CLOSEDDOOR);
		}

		if (pGimmick->bClear == true
			&& pGimmick->myType == GIMMICKTYPE_STATUE
			&& pGimmick->bFinishMotion == true
			&& pItemSpawn->bSpawned == false)
		{
			SpawnItem();
		}

		UpdateMotion(pGimmick->myType);

		pGimmick->nCounter++;		// カウンター増加
	}

	UpdateSpawnItem();

	// プレイヤーの情報を取得
	Player* pPlayer = GetPlayer();

	// === クリア判定になるまで、ギミックの場所にシリンダーを置いてわかりやすくする
	// バルブ
	if (g_aGimmick[GIMMICKTYPE_STATUE].bClear == true)
	{
		SetColorMeshCylinder(&GetMeshCylinder()[g_aGCData[GC_STATUE].nIdxMeshCylinder], COLOR_INV);
		ReleaseMesh(GetMeshCylinder(), g_aGCData[GC_STATUE].nIdxMeshCylinder, GetNumMeshCylinder());
		g_aGCData[GC_STATUE].bEnable = false;
	}

	// 倒木
	if (g_aGimmick[GIMMICKTYPE_FALLENTREE].bClear == true)
	{
		SetColorMeshCylinder(&GetMeshCylinder()[g_aGCData[GC_FALLENTREE].nIdxMeshCylinder], COLOR_INV);
		ReleaseMesh(GetMeshCylinder(), g_aGCData[GC_FALLENTREE].nIdxMeshCylinder, GetNumMeshCylinder());
		g_aGCData[GC_FALLENTREE].bEnable = false;
	}

	// 駅のガラスドア
	if (g_aGimmick[GIMMICKTYPE_CLOSEDDOOR].bClear == true)
	{
		SetColorMeshCylinder(&GetMeshCylinder()[g_aGCData[GC_BIGBUTTON].nIdxMeshCylinder], COLOR_INV);
		ReleaseMesh(GetMeshCylinder(), g_aGCData[GC_BIGBUTTON].nIdxMeshCylinder, GetNumMeshCylinder());
		g_aGCData[GC_BIGBUTTON].bEnable = false;
	}

	// === チュートリアルを非表示に === //
	if (g_bAnyTex == true)
	{
		if (GetKeyboardTrigger(DIK_RETURN) == true || GetJoypadTrigger(0, JOYKEY_A) == true)
		{
			PlaySound(SOUND_LABEL_SE_G_CANCEL);
			g_bAnyTex = false;
			for (int nCnt = 0; nCnt < TUTORIALTYPE_MAX; nCnt++)
			{// すべてのチュートリアルを非表示に
				SetEnable2DPolygon(g_nIdxTexTutorial[nCnt], false);
			}
		}
	}

	// シリンダーの更新
	UpdateGimmickCylinder();
}

//==================================================================================
// --- 描画 ---
//==================================================================================
void DrawGimmick(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	LPGIMMICK pGimmick = &g_aGimmick[0];
	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス
	D3DMATERIAL9 matDef;			// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;				// マテリアルデータへのポインタ

	/*** アルファテストを有効にする ***/
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// アルファテストを有効
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// 基準値よりも大きい場合にZバッファに書き込み
	pDevice->SetRenderState(D3DRS_ALPHAREF, 60);				// 基準値

	for (int nCntGimmick = 0; nCntGimmick < GIMMICKTYPE_MAX; nCntGimmick++, pGimmick++)
	{
		if (pGimmick->bUse == false) continue;

		if ((pGimmick->myType == GIMMICKTYPE_FALLENTREE)
			& (pGimmick->bClear == true))
		{
			continue;
		}
		if ((pGimmick->myType == GIMMICKTYPE_FALLENTREE2)
			& (pGimmick->bClear == true))
		{
			continue;
		}
		if ((pGimmick->myType == GIMMICKTYPE_FALLENTREE3)
			& (pGimmick->bClear == true))
		{
			continue;
		}

		// プレイヤーのワールドマトリックスの初期化
		D3DXMatrixIdentity(&pGimmick->mtxWorld);

		// プレイヤーの向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot,
			pGimmick->rot.y,
			pGimmick->rot.x,
			pGimmick->rot.z);

		D3DXMatrixMultiply(&pGimmick->mtxWorld, &pGimmick->mtxWorld, &mtxRot);	// かけ合わせる

		// プレイヤーの位置を反映
		D3DXMatrixTranslation(&mtxTrans,
			pGimmick->pos.x,
			pGimmick->pos.y,
			pGimmick->pos.z);

		D3DXMatrixMultiply(&pGimmick->mtxWorld, &pGimmick->mtxWorld, &mtxTrans);	// かけ合わせる

		// プレイヤーのワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &pGimmick->mtxWorld);

		// 現在のマテリアルを取得(保存)
		pDevice->GetMaterial(&matDef);

		// 全モデル(パーツ)の描画
		for (int nCntModel = 0; nCntModel < pGimmick->parts.nNumParts; nCntModel++)
		{
			D3DXMATRIX mtxRotModel, mtxTransModel;	// 計算用マトリックス
			D3DXMATRIX mtxParent;	// 親のマトリックス

			// パーツのワールドマトリックス初期化
			D3DXMatrixIdentity(&pGimmick->parts.aParts[nCntModel].mtxWorld);

			// パーツの向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRotModel,
				pGimmick->parts.aParts[nCntModel].rot.y,
				pGimmick->parts.aParts[nCntModel].rot.x,
				pGimmick->parts.aParts[nCntModel].rot.z);

			// かけ合わせる
			D3DXMatrixMultiply(&pGimmick->parts.aParts[nCntModel].mtxWorld,
				&pGimmick->parts.aParts[nCntModel].mtxWorld,
				&mtxRotModel);

			// パーツの位置(オフセット)を反映
			D3DXMatrixTranslation(&mtxTransModel,
				pGimmick->parts.aParts[nCntModel].pos.x,
				pGimmick->parts.aParts[nCntModel].pos.y,
				pGimmick->parts.aParts[nCntModel].pos.z);

			// かけ合わせる
			D3DXMatrixMultiply(&pGimmick->parts.aParts[nCntModel].mtxWorld,
				&pGimmick->parts.aParts[nCntModel].mtxWorld,
				&mtxTransModel);

			// パーツの「親マトリックス」の設定
			if (pGimmick->parts.aParts[nCntModel].nIdxModelParent != -1)
			{// 親モデルが存在する
				mtxParent = pGimmick->parts.aParts[pGimmick->parts.aParts[nCntModel].nIdxModelParent].mtxWorld;
			}
			else
			{// 親モデルが存在しない
				mtxParent = pGimmick->mtxWorld;
			}

			// 算出した「パーツのワールドマトリックス」と「親のマトリックス」をかけ合わせる
			D3DXMatrixMultiply(&pGimmick->parts.aParts[nCntModel].mtxWorld,
				&pGimmick->parts.aParts[nCntModel].mtxWorld,
				&mtxParent);

			// パーツのワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD,
				&pGimmick->parts.aParts[nCntModel].mtxWorld);

			LPMODELDATA pModelData = GetModelData(pGimmick->parts.aParts[nCntModel].nIdxModel);
			if (pModelData != NULL)
			{ // パーツの描画	
				// マテリアルデータへのポインタを取得
				pMat = (D3DXMATERIAL*)pModelData->pBuffMat->GetBufferPointer();

				for (int nCntMat = 0; nCntMat < (int)pModelData->dwNumMat; nCntMat++)
				{
					// マテリアルの設定
					pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

					// テクスチャの設定
					pDevice->SetTexture(0, pModelData->apTexture[nCntMat]);

					// モデル(パーツ)の描画
					pModelData->pMesh->DrawSubset(nCntMat);
				}
			}
		}

		// 保存していたマテリアルを戻す
		pDevice->SetMaterial(&matDef);
	}

	/*** アルファテストを無効にする ***/
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);		// アルファテストを無効化
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);	// 無条件にZバッファに書き込み
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);					// 基準値

	EndDevice();
}

//==================================================================================
// --- 設置 ---
//==================================================================================
void SetGimmick(D3DXVECTOR3 pos, D3DXVECTOR3 rot, GIMMICKTYPE type)
{
	if (FAILED(CheckIndex(GIMMICKTYPE_MAX, type)))
	{ // インデックス確認
		OutputDebugString(TEXT("ちょっと。ギミックの種類にそんなものないわよ。\n"));
		return;
	}

	// 値を代入
	g_aGimmick[type].pos = pos;
	g_aGimmick[type].rot = rot;
}

//==================================================================================
// --- クリア ---
//==================================================================================
void ClearGimmick(GIMMICKTYPE type)
{
	if (FAILED(CheckIndex(GIMMICKTYPE_MAX, type)))
	{ // インデックス確認
		OutputDebugString(TEXT("ちょっと。ギミックの種類にそんなものないってば。\n"));
		return;
	}

	// クリア状態に設定
	g_aGimmick[type].bClear = true;
}

//==================================================================================
// --- クリア状況取得 ---
//==================================================================================
bool IsClearGimmick(GIMMICKTYPE type)
{
	if (FAILED(CheckIndex(GIMMICKTYPE_MAX, type)))
	{ // インデックス確認
		OutputDebugString(TEXT("ちょっと。ギミックの種類にそんなものないってば。\n"));
		return false;
	}

	// クリア状態を取得
	return g_aGimmick[type].bClear;
}

//==================================================================================
// --- マルチプレイ時判定 ---
//==================================================================================
void CaseMulti(LPGIMMICK pGimmick)
{
	if (pGimmick->bClear == true) return;

	Player* pPlayer = GetPlayer();
	bool bDetection = false;

	if (IsDetection(pGimmick->pos, pPlayer[PLAYERTYPE_GIRL].pos, pGimmick->fRadius)
		&& (pGimmick->could == COULD_PLAYER_GIRL || pGimmick->could == COULD_PLAYER_ALL))
	{ // 少女の判定
		// === 倒木の判定処理 === //
		if (pGimmick->myType == GIMMICKTYPE_FALLENTREE && pPlayer->motionType == MOTIONTYPE_CUTTING && pPlayer->nKey == 3)
		{
			pGimmick->bClear = true;
			PlaySound(SOUND_LABEL_SE_G_FALLENTREE);
		}
		else if (pGimmick->myType == GIMMICKTYPE_FALLENTREE2 && pPlayer->motionType == MOTIONTYPE_CUTTING && pPlayer->nKey == 3)
		{
			pGimmick->bClear = true;
			PlaySound(SOUND_LABEL_SE_G_FALLENTREE);
		}
		else if (pGimmick->myType == GIMMICKTYPE_FALLENTREE3 && pPlayer->motionType == MOTIONTYPE_CUTTING && pPlayer->nKey == 3)
		{
			pGimmick->bClear = true;
			PlaySound(SOUND_LABEL_SE_G_FALLENTREE);
		}

		// === バルブの判定処理 === //
		if (pGimmick->myType == GIMMICKTYPE_STATUE && pPlayer->motionType == MOTIONTYPE_VALVE && pPlayer->Armtype == ARMTYPE_NORMAL)
		{
			PlaySound(SOUND_LABEL_SE_G_FOUNTAIN);
			pGimmick->bClear = true;
			SetMotionType(MOTIONTYPE_ACTION, false, 0, GIMMICKTYPE_STATUE);
		}

		// === チュートリアル表示処理 === //
		// チェンソーのチュートリアルをまだ表示していなければだす
		if ((pGimmick->myType == GIMMICKTYPE_FALLENTREE || pGimmick->myType == GIMMICKTYPE_FALLENTREE2 || pGimmick->myType == GIMMICKTYPE_FALLENTREE3) && g_bDispTutorialChainsaw == false)
		{
			PlaySound(SOUND_LABEL_SE_G_TUTORIAL);
			SetEnable2DPolygon(g_nIdxTexTutorial[1], true);
			g_bDispTutorialChainsaw = true;	// 表示済みに
			g_bAnyTex = true;	// チュートリアル表示中
		}
		// バルブのチュートリアルをまだ表示していなければだす
		if (pGimmick->myType == GIMMICKTYPE_STATUE && g_bDispTutorialvalve == false)
		{
			PlaySound(SOUND_LABEL_SE_G_TUTORIAL);
			SetEnable2DPolygon(g_nIdxTexTutorial[TUTORIALTYPE_VALVE], true);
			g_bDispTutorialvalve = true;	// 表示済みに
			g_bAnyTex = true;	// チュートリアル表示中
		}

		bDetection = true;
	}

	if (IsDetection(pGimmick->pos, pPlayer[PLAYERTYPE_MOUSE].pos, pGimmick->fRadius)
		&& (pGimmick->could == COULD_PLAYER_MOUSE || pGimmick->could == COULD_PLAYER_ALL))
	{ // ネズミの判定
		//bDetection = true;
	}

	if (bDetection == true && pGimmick->bClear == false)
	{ // どちらかが半径に入った場合
		SetEnablePrompt(true, pGimmick->nIdxPrompt);
	}
	else
	{
		SetEnablePrompt(false, pGimmick->nIdxPrompt);
	}
}

//==================================================================================
// --- ソロプレイ時判定 ---
//==================================================================================
void CaseSolo(LPGIMMICK pGimmick)
{
	if (pGimmick->bClear == true) return;

	Player* pPlayer = GetPlayer();
	PlayerType type = (PlayerType)GetActivePlayer();
	bool bDetection = false;

	if (IsDetection(pGimmick->pos, pPlayer[type].pos, pGimmick->fRadius)
		&& (pGimmick->could == type || pGimmick->could == COULD_PLAYER_ALL))
	{ // 操作中プレイヤーの判定
		// === 倒木の判定処理 === //
		if (pGimmick->motionType == MOTIONTYPE_ACTION) return;

		if (pGimmick->myType == GIMMICKTYPE_FALLENTREE && pPlayer->motionType == MOTIONTYPE_CUTTING && pPlayer->nKey == 3)
		{
			pGimmick->bClear = true;
			PlaySound(SOUND_LABEL_SE_G_FALLENTREE);
		}
		else if (pGimmick->myType == GIMMICKTYPE_FALLENTREE2 && pPlayer->motionType == MOTIONTYPE_CUTTING && pPlayer->nKey == 3)
		{
			pGimmick->bClear = true;
			PlaySound(SOUND_LABEL_SE_G_FALLENTREE);
		}
		else if (pGimmick->myType == GIMMICKTYPE_FALLENTREE3 && pPlayer->motionType == MOTIONTYPE_CUTTING && pPlayer->nKey == 3)
		{
			pGimmick->bClear = true;
			PlaySound(SOUND_LABEL_SE_G_FALLENTREE);
		}

		// === バルブの判定処理 === //
		if (pGimmick->myType == GIMMICKTYPE_STATUE && pPlayer->motionType == MOTIONTYPE_VALVE && pPlayer->Armtype == ARMTYPE_NORMAL)
		{
			PlaySound(SOUND_LABEL_SE_G_FOUNTAIN);
			pGimmick->bClear = true;
			SetMotionType(MOTIONTYPE_ACTION, false, 0, GIMMICKTYPE_STATUE);
		}

		// === チュートリアル表示処理 === //
		// チェンソーのチュートリアルをまだ表示していなければだす
		if ((pGimmick->myType == GIMMICKTYPE_FALLENTREE || pGimmick->myType == GIMMICKTYPE_FALLENTREE2 || pGimmick->myType == GIMMICKTYPE_FALLENTREE3) && g_bDispTutorialChainsaw == false)
		{
			PlaySound(SOUND_LABEL_SE_G_TUTORIAL);
			SetEnable2DPolygon(g_nIdxTexTutorial[1], true);
			g_bDispTutorialChainsaw = true;	// 表示済みに
			g_bAnyTex = true;	// チュートリアル表示中
		}
		// バルブのチュートリアルをまだ表示していなければだす
		if (pGimmick->myType == GIMMICKTYPE_STATUE && g_bDispTutorialvalve == false)
		{
			PlaySound(SOUND_LABEL_SE_G_TUTORIAL);
			SetEnable2DPolygon(g_nIdxTexTutorial[TUTORIALTYPE_VALVE], true);
			g_bDispTutorialvalve = true;	// 表示済みに
			g_bAnyTex = true;	// チュートリアル表示中
		}

		bDetection = true;
	}

	if (bDetection == true && pGimmick->bClear == false)
	{ // 半径に入った場合
		SetEnablePrompt(true, pGimmick->nIdxPrompt);
	}
	else
	{
		SetEnablePrompt(false, pGimmick->nIdxPrompt);
	}
}

//==================================================================================
// --- 当たり判定 ---
//==================================================================================
bool CollisionGimmick(
	D3DXVECTOR3* pPos,
	D3DXVECTOR3* pPosOld,
	D3DXVECTOR3* pMove,
	Player* pPlayer,
	float fRadius,
	float fHeight)
{
	// 当たっているかどうかをbool型で返す
	bool bLand = false;

	for (int nCntModel = 0; nCntModel < GIMMICKTYPE_MAX; nCntModel++)
	{
		if (g_aGimmick[nCntModel].bUse == true)
		{
			Gimmick* pObject = &g_aGimmick[nCntModel];

			for (int nCntParts = 0; nCntParts < pObject->parts.nNumParts; nCntParts++)
			{
				if ((nCntModel == GIMMICKTYPE_CLOSEDDOOR)
					& (g_aGimmick[nCntModel].bClear == true)
					& (nCntParts == pObject->parts.nNumParts - 1))
				{ // もしギミックがクリア済み且つ扉のギミックだった場合、透明壁を無視
					continue;
				}
				else if ((nCntModel == GIMMICKTYPE_FALLENTREE)
					& (g_aGimmick[nCntModel].bClear == true))
				{
					continue;
				}
				else if ((nCntModel == GIMMICKTYPE_FALLENTREE2)
					& (g_aGimmick[nCntModel].bClear == true))
				{
					continue;
				}
				else if ((nCntModel == GIMMICKTYPE_FALLENTREE3)
					& (g_aGimmick[nCntModel].bClear == true))
				{
					continue;
				}

				LPMODELDATA pObjInfo = GetModelData(pObject->parts.aParts[nCntParts].nIdxModel);
#if 1
				D3DXVECTOR3 offset = pObject->parts.aParts[nCntParts].pos;
				D3DXVECTOR3 posVtx[4] = {};		// オブジェクトの四辺の頂点
				D3DXVECTOR3 vecMove = D3DXVECTOR3_NULL;		// 移動ベクトル
				D3DXVECTOR3 vecLine = D3DXVECTOR3_NULL;		// 境界線ベクトル
				D3DXVECTOR3 vecLineA[4] = {};	// 境界線ベクトル
				D3DXVECTOR3 vecToPos = D3DXVECTOR3_NULL;	// 位置と境界線のはじめを結んだベクトル
				D3DXVECTOR3 vecToPosA[4] = {};	// 位置と境界線のはじめを結んだベクトル
				D3DXVECTOR3 vecToPosOld = D3DXVECTOR3_NULL;	// 過去位置と境界線のはじめを結んだベクトル 
				D3DXVECTOR3 vecLineNor = D3DXVECTOR3_NULL;	// 正規化した境界線ベクトル
				float fVecPos = 0.0f;
				float fVecPosA[4] = {};
				float fVecPosOld = 0.0f;
				float fPosToMove = 0.0f;					// vecToPosとの外積
				float fLineToMove = 0.0f;					// vecLineとの外積
				float fRate = 0.0f;							// 面積比率
				float fVecPosToNor = 0.0f;					// 逆法線との外積

				D3DXVECTOR3 Length;
				float fLength = 0.0f;
				float fAngle = 0.0f;
				bool bCollision[4] = {};

				posVtx[0].x = pObjInfo->mtxMin.x;
				posVtx[0].y = 0;
				posVtx[0].z = pObjInfo->mtxMin.z;

				D3DXVec3TransformCoord(&posVtx[0], &posVtx[0], &pObject->parts.aParts[nCntParts].mtxWorld);

				posVtx[1].x = pObjInfo->mtxMax.x;
				posVtx[1].y = 0;
				posVtx[1].z = pObjInfo->mtxMin.z;

				D3DXVec3TransformCoord(&posVtx[1], &posVtx[1], &pObject->parts.aParts[nCntParts].mtxWorld);

				posVtx[2].x = pObjInfo->mtxMax.x;
				posVtx[2].y = 0;
				posVtx[2].z = pObjInfo->mtxMax.z;

				D3DXVec3TransformCoord(&posVtx[2], &posVtx[2], &pObject->parts.aParts[nCntParts].mtxWorld);

				posVtx[3].x = pObjInfo->mtxMin.x;
				posVtx[3].y = 0;
				posVtx[3].z = pObjInfo->mtxMax.z;

				D3DXVec3TransformCoord(&posVtx[3], &posVtx[3], &pObject->parts.aParts[nCntParts].mtxWorld);

				/*** 移動ベクトルの取得 ***/
				vecMove = *pPos - *pPosOld;

				for (int nCntCollision = 0; nCntCollision < 4; nCntCollision++)
				{
					D3DXVECTOR3 vecNor = D3DXVECTOR3_NULL;		// 壁の逆法線ベクトル
					D3DXVECTOR3 vecCutLine = D3DXVECTOR3_NULL;	// 交点からの境界線ベクトル

					vecLine = posVtx[(nCntCollision + 1) % 4] - posVtx[nCntCollision];

					RepairFloat(&vecLine.x);
					RepairFloat(&vecLine.z);

					/*** 現在位置との関係を外積を使い求める ***/
					vecToPos = *pPos - posVtx[nCntCollision];

					fVecPos = (vecLine.z * vecToPos.x) - (vecLine.x * vecToPos.z);

					RepairFloat(&fVecPos);

					/*** 過去位置との関係を外積を使い求める ***/
					vecToPosOld = *pPosOld - posVtx[nCntCollision];

					fVecPosOld = (vecLine.z * vecToPosOld.x) - (vecLine.x * vecToPosOld.z);

					RepairFloat(&fVecPosOld);

					/*** 強制位置の判定 ***/

					/** 現在位置との外積 **/
					fPosToMove = (vecToPos.z * vecMove.x) - (vecToPos.x * vecMove.z);

					/** 最大値との外積 **/
					fLineToMove = (vecLine.z * vecMove.x) - (vecLine.x * vecMove.z);

					/** 面積比率の計算 **/
					fRate = fPosToMove / fLineToMove;

					vecNor.x = vecLine.z;
					vecNor.z = -vecLine.x;

					D3DXVec3Normalize(&vecNor, &vecNor);

					fVecPosToNor = (vecNor.z * vecMove.x) - (vecNor.x * vecMove.z);

					D3DXVECTOR3 vecF;
					vecCutLine = vecLine;

					if (fVecPosToNor == 0)
					{
						fVecPosToNor = fabsf(fVecPosToNor);
						D3DXVec3Normalize(&vecCutLine, &vecCutLine);

						vecF = vecCutLine * fVecPosToNor;
					}
					else if (fVecPosToNor > 0)
					{
						fVecPosToNor = fabsf(fVecPosToNor);
						D3DXVec3Normalize(&vecCutLine, &vecCutLine);

						vecF = -vecCutLine * fVecPosToNor;
					}
					else if (fVecPosToNor < 0)
					{
						fVecPosToNor = fabsf(fVecPosToNor);
						D3DXVec3Normalize(&vecCutLine, &vecCutLine);

						vecF = vecCutLine * fVecPosToNor;
					}

					/*** プレイヤーの壁のめり込み判定 ***/
					if (fVecPos <= 0 && fVecPosOld >= 0)
					{
						/*** もしも比率が範囲内であれば,衝突 ***/
						if (fRate >= 0.0f && fRate <= 1.0f)
						{
							if (pPos->y >= pObject->pos.y + offset.y + pObjInfo->mtxMin.y - 0.01f
								&& pPos->y <= pObject->pos.y + offset.y + pObjInfo->mtxMax.y)
							{
								if (g_aGimmick[nCntModel].myType != GIMMICKTYPE_BIGBUTTON && g_aGimmick[nCntModel].myType != GIMMICKTYPE_SMALLBUTTON)
								{// ボタン以外には側面の当たり判定をつける
									pPos->x = posVtx[nCntCollision].x + (vecLine.x * fRate) + vecF.x;
									pPos->z = posVtx[nCntCollision].z + (vecLine.z * fRate) + vecF.z;
								}
							}
						}
					}

					if (fVecPos < 0 && fVecPosOld < 0)
					{
						bCollision[nCntCollision] = true;
					}
				}

				if (bCollision[0] == true
					&& bCollision[1] == true
					&& bCollision[2] == true
					&& bCollision[3] == true)
				{// 上の当たり判定
					if (g_aGimmick[nCntModel].myType != GIMMICKTYPE_BIGBUTTON && g_aGimmick[nCntModel].myType != GIMMICKTYPE_SMALLBUTTON)
					{// ボタン以外の判定
						if (pPos->y + fHeight >= pObject->pos.y + offset.y + pObjInfo->mtxMin.y
							&& pPosOld->y + fHeight <= pObject->pos.y + offset.y + pObjInfo->mtxMin.y)
						{
							pPos->y = pObject->pos.y + offset.y + pObjInfo->mtxMin.y - fHeight;
						}
						else if (pPos->y <= pObject->pos.y + offset.y + pObjInfo->mtxMax.y
							&& pPosOld->y >= pObject->pos.y + offset.y + pObjInfo->mtxMax.y)
						{
							pPos->y = pObject->pos.y + offset.y + pObjInfo->mtxMax.y;
							bLand = true;
							pMove->y = 0.0f;
						}
					}
					else
					{// ボタンの判定
						if (pPos->y + fHeight >= pObject->pos.y + offset.y + pObjInfo->mtxMin.y
							&& pPosOld->y + fHeight <= pObject->pos.y + offset.y + pObjInfo->mtxMin.y)
						{
							pPos->y = pObject->pos.y + offset.y + pObjInfo->mtxMin.y - fHeight;
						}
						else if (pPos->y <= pObject->pos.y + offset.y + pObjInfo->mtxMax.y)
						{
							pPos->y = pObject->pos.y + offset.y + pObjInfo->mtxMax.y;
							bLand = true;
							pMove->y = 0.0f;
						}
					}

					if (g_aGimmick[nCntModel].bClear == false && g_aGimmick[nCntModel].myType == GIMMICKTYPE_BIGBUTTON && GetActivePlayer() == PLAYERTYPE_GIRL)
					{// でかボタンを押す
						SetMotionType(MOTIONTYPE_ACTION, false, 0, GIMMICKTYPE_BIGBUTTON);
						ClearGimmick(GIMMICKTYPE_BIGBUTTON);

						// カタパルトチュートリアルを表示
						SetEnable2DPolygon(g_nIdxTexTutorial[0], true);
						g_bAnyTex = true;
					}

					if (g_aGimmick[nCntModel].bClear == false && g_aGimmick[nCntModel].myType == GIMMICKTYPE_SMALLBUTTON && GetActivePlayer() == PLAYERTYPE_MOUSE)
					{// ちびボタンを押す
						SetMotionType(MOTIONTYPE_ACTION, false, 0, GIMMICKTYPE_SMALLBUTTON);
						ClearGimmick(GIMMICKTYPE_SMALLBUTTON);
						PlaySound(SOUND_LABEL_SE_G_OPENDOOR);
					}
				}
			}
		}
#endif
	}

	return bLand;
}

//==================================================================================
// --- モーションの変更！ ---
//==================================================================================
void SetMotionType(MOTIONTYPE motionTypeNext, bool bBlend, int nFrameBlend, GIMMICKTYPE Type)
{
	LPGIMMICK pGimmick = &g_aGimmick[Type];	// プレイヤー情報へのアドレス	

	if (motionTypeNext < 0 || motionTypeNext >= MOTIONTYPE_MAX)
	{ // モーションインデックスの上下限確認
		return;
	}

	// ブレンドモーションをするかどうか
	pGimmick->bBlendMotion = bBlend;

	if (bBlend == false)
	{
		/*** 各変数を初期化し、次のモーションを設定 ***/
		pGimmick->nCounterMotion = pGimmick->nCounterMotionBlend;
		pGimmick->nKey = pGimmick->nKeyBlend;
		pGimmick->motionType = motionTypeNext;
		pGimmick->bLoop = pGimmick->aMotionInfo[motionTypeNext].bLoop;
		pGimmick->nNumKey = pGimmick->aMotionInfo[motionTypeNext].nNumKey;
		pGimmick->bFinishMotion = false;

		/*** 現在のモーションのキー情報へのポインタ ***/
		KEY_INFO* pInfo = &pGimmick->aMotionInfo[pGimmick->motionType].aKeyInfo[pGimmick->nKey];

		/*** 全パーツの初期設定！ ***/
		for (int nCntModel = 0; nCntModel < pGimmick->parts.nNumParts; nCntModel++)
		{
			int nNext = (pGimmick->nKey + 1) % pGimmick->aMotionInfo[pGimmick->motionType].nNumKey;

			// 次のキーの値			
			float fRateKey = (float)pGimmick->nCounterMotion / (float)pInfo->nFrame;			// モーションカウンター / 再生フレーム数	

			D3DXVECTOR3 diffPos = {};	// 位置の差分
			D3DXVECTOR3 UpdatePos = {};	// 更新する位置
			D3DXVECTOR3 diffRot = {};	// 角度の差分
			D3DXVECTOR3 UpdateRot = {};	// 更新する角度
			KEY* pKey = &pInfo->aKey[nCntModel];	// 現在のキー
			PARTS* pModel = &pGimmick->parts.aParts[nCntModel];
			KEY* pKeyNext = &pGimmick->aMotionInfo[pGimmick->motionType].aKeyInfo[nNext].aKey[nCntModel];		// 次のキー			

			/** キー情報から位置と向きを算出！ **/
			// === POS ===			
			// --- X ---		
			diffPos.x = pKeyNext->pos.x - pKey->pos.x;
			UpdatePos.x = pKey->pos.x + diffPos.x * fRateKey;

			// --- Y ---		
			diffPos.y = pKeyNext->pos.y - pKey->pos.y;
			UpdatePos.y = pKey->pos.y + diffPos.y * fRateKey;

			// --- Z ---			
			diffPos.z = pKeyNext->pos.z - pKey->pos.z;
			UpdatePos.z = pKey->pos.z + diffPos.z * fRateKey;

			// === ROT ===			
			// --- X ---			
			diffRot.x = pKeyNext->rot.x - pKey->rot.x;
			RepairRot(&diffRot.x, &diffRot.x);

			UpdateRot.x = pKey->rot.x + diffRot.x * fRateKey;
			RepairRot(&UpdateRot.x, &UpdateRot.x);

			// --- Y ---			
			diffRot.y = pKeyNext->rot.y - pKey->rot.y;
			RepairRot(&diffRot.y, &diffRot.y);

			UpdateRot.y = pKey->rot.y + diffRot.y * fRateKey;
			RepairRot(&UpdateRot.y, &UpdateRot.y);

			// --- Z ---			
			diffRot.z = pKeyNext->rot.z - pKey->rot.z;
			RepairRot(&diffRot.z, &diffRot.z);

			UpdateRot.z = pKey->rot.z + diffRot.z * fRateKey;
			RepairRot(&UpdateRot.z, &UpdateRot.z);

			/** 位置、向きを更新！ **/
			pModel->pos = pModel->posLocal + UpdatePos;
			pModel->rot = pModel->rotLocal + UpdateRot;
		}
	}
	else
	{
		/*** 各変数を初期化し、ブレンドモーションを設定 ***/
		pGimmick->bFinishMotion = false;
		pGimmick->motionTypeBlend = motionTypeNext;
		pGimmick->nCounterBlend = 0;
		pGimmick->nCounterMotionBlend = 0;
		pGimmick->nFrameBlend = nFrameBlend;
		pGimmick->nNumKeyBlend = pGimmick->aMotionInfo[motionTypeNext].nNumKey;
		pGimmick->nKeyBlend = 0;
	}
}

//==================================================================================
// --- モーションアップデート！ ---
//==================================================================================
void UpdateMotion(GIMMICKTYPE type)
{
	Gimmick* pGimmick = &g_aGimmick[type];	// プレイヤー情報へのアドレス	

	/*** 現在のモーションのキー情報へのポインタ ***/
	KEY_INFO* pInfo = &pGimmick->aMotionInfo[pGimmick->motionType].aKeyInfo[pGimmick->nKey];
	KEY_INFO* pInfoBlend = &pGimmick->aMotionInfo[pGimmick->motionTypeBlend].aKeyInfo[pGimmick->nKeyBlend];

	/*** 全パーツの更新！ ***/
	for (int nCntModel = 0; nCntModel < pGimmick->parts.nNumParts; nCntModel++)
	{
		if ((pGimmick->nKey + 1) == pGimmick->aMotionInfo[pGimmick->motionType].nNumKey
			&& pGimmick->aMotionInfo[pGimmick->motionType].bLoop == false) continue;
		int nNext = (pGimmick->nKey + 1) % pGimmick->aMotionInfo[pGimmick->motionType].nNumKey;

		// 次のキーの値		
		float fRateKey = (float)pGimmick->nCounterMotion / (float)pInfo->nFrame;		// モーションカウンター / 再生フレーム数
		D3DXVECTOR3 diffPos = {};	// 位置の差分
		D3DXVECTOR3 UpdatePos = {};	// 更新する位置	
		D3DXVECTOR3 diffRot = {};	// 角度の差分		
		D3DXVECTOR3 UpdateRot = {};	// 更新する角度		
		KEY* pKey = &pInfo->aKey[nCntModel];	// 現在のキー		
		KEY* pKeyNext = &pGimmick->aMotionInfo[pGimmick->motionType].aKeyInfo[nNext].aKey[nCntModel];		// 次のキー		
		PARTS* pModel = &pGimmick->parts.aParts[nCntModel];

		if (pGimmick->bBlendMotion == true)
		{ // モーションブレンドあり	
			D3DXVECTOR3 diffKeyPosCurrent = {};	// 現在のモーションの位置の差分			
			D3DXVECTOR3 diffKeyRotCurrent = {};	// 現在のモーションの角度の差分		
			D3DXVECTOR3 diffKeyPosBlend = {};	// ブレンドモーションの位置の差分			
			D3DXVECTOR3 diffKeyRotBlend = {};	// ブレンドモーションの角度の差分	
			D3DXVECTOR3 diffPosBlend = {};		// 最終差分		
			D3DXVECTOR3 diffRotBlend = {};		// 最終差分			
			int nNextBlend = (pGimmick->nKeyBlend + 1) % pGimmick->aMotionInfo[pGimmick->motionTypeBlend].nNumKey;			// 次のキーの値		
			KEY* pKeyBlend = &pInfoBlend->aKey[nCntModel];																// 現在のキー	
			KEY* pKeyNextBlend = &pGimmick->aMotionInfo[pGimmick->motionTypeBlend].aKeyInfo[nNextBlend].aKey[nCntModel];	// 次のキー
			float fRateKeyBlend = (float)pGimmick->nCounterMotionBlend / (float)pInfoBlend->nFrame;						// モーションカウンター / 再生フレーム数
			float fRateBlend = (float)pGimmick->nCounterBlend / (float)pGimmick->nFrameBlend;							// ブレンドの相対量		

			// --- X ---			
			/* 現在のモーション */
			diffPos.x = pKeyNext->pos.x - pKey->pos.x;
			diffKeyPosCurrent.x = pKey->pos.x + diffPos.x * fRateKey;

			/* ブレンドモーション */
			diffPos.x = pKeyNextBlend->pos.x - pKeyBlend->pos.x;
			diffKeyPosBlend.x = pKeyBlend->pos.x + diffPos.x * fRateKeyBlend;

			/* 求める差分 */
			diffPosBlend.x = diffKeyPosBlend.x - diffKeyPosCurrent.x;
			UpdatePos.x = diffKeyPosCurrent.x + (diffPosBlend.x * fRateBlend);

			// --- Y ---		
			/* 現在のモーション */
			diffPos.y = pKeyNext->pos.y - pKey->pos.y;
			diffKeyPosCurrent.y = pKey->pos.y + diffPos.y * fRateKey;

			/* ブレンドモーション */
			diffPos.y = pKeyNextBlend->pos.y - pKeyBlend->pos.y;
			diffKeyPosBlend.y = pKeyBlend->pos.y + diffPos.y * fRateKeyBlend;

			/* 求める差分 */
			diffPosBlend.y = diffKeyPosBlend.y - diffKeyPosCurrent.y;
			UpdatePos.y = diffKeyPosCurrent.y + (diffPosBlend.y * fRateBlend);

			// --- Z ---			
			/* 現在のモーション */
			diffPos.z = pKeyNext->pos.z - pKey->pos.z;
			diffKeyPosCurrent.z = pKey->pos.z + diffPos.z * fRateKey;

			/* ブレンドモーション */
			diffPos.z = pKeyNextBlend->pos.z - pKeyBlend->pos.z;
			diffKeyPosBlend.z = pKeyBlend->pos.z + diffPos.z * fRateKeyBlend;

			/* 求める差分 */
			diffPosBlend.z = diffKeyPosBlend.z - diffKeyPosCurrent.z;
			UpdatePos.z = diffKeyPosCurrent.z + (diffPosBlend.z * fRateBlend);

			// === ROT ===			
			// --- X ---			
			/* 現在のモーション */
			diffRot.x = pKeyNext->rot.x - pKey->rot.x;
			RepairRot(&diffRot.x, &diffRot.x);
			diffKeyRotCurrent.x = pKey->rot.x + diffRot.x * fRateKey;
			RepairRot(&diffKeyRotCurrent.x, &diffKeyRotCurrent.x);

			/* ブレンドモーション */
			diffRot.x = pKeyNextBlend->rot.x - pKeyBlend->rot.x;
			RepairRot(&diffRot.x, &diffRot.x);
			diffKeyRotBlend.x = pKeyBlend->rot.x + diffRot.x * fRateKeyBlend;
			RepairRot(&diffKeyRotBlend.x, &diffKeyRotBlend.x);

			/* 求める差分 */
			diffRotBlend.x = diffKeyRotBlend.x - diffKeyRotCurrent.x;
			UpdateRot.x = diffKeyRotCurrent.x + (diffRotBlend.x * fRateBlend);

			// --- Y ---			
			diffRot.y = pKeyNext->rot.y - pKey->rot.y;
			RepairRot(&diffRot.y, &diffRot.y);
			diffKeyRotCurrent.y = pKey->rot.y + diffRot.y * fRateKey;
			RepairRot(&diffKeyRotCurrent.y, &diffKeyRotCurrent.y);

			/* ブレンドモーション */
			diffRot.y = pKeyNextBlend->rot.y - pKeyBlend->rot.y;
			RepairRot(&diffRot.y, &diffRot.y);
			diffKeyRotBlend.y = pKeyBlend->rot.y + diffRot.y * fRateKeyBlend;
			RepairRot(&diffKeyRotBlend.y, &diffKeyRotBlend.y);

			/* 求める差分 */
			diffRotBlend.y = diffKeyRotBlend.y - diffKeyRotCurrent.y;
			UpdateRot.y = diffKeyRotCurrent.y + (diffRotBlend.y * fRateBlend);

			// --- Z ---			
			diffRot.z = pKeyNext->rot.z - pKey->rot.z;
			RepairRot(&diffRot.z, &diffRot.z);
			diffKeyRotCurrent.z = pKey->rot.z + diffRot.z * fRateKey;
			RepairRot(&diffKeyRotCurrent.z, &diffKeyRotCurrent.z);

			/* ブレンドモーション */
			diffRot.z = pKeyNextBlend->rot.z - pKeyBlend->rot.z;
			RepairRot(&diffRot.z, &diffRot.z);
			diffKeyRotBlend.z = pKeyBlend->rot.z + diffRot.z * fRateKeyBlend;
			RepairRot(&diffKeyRotBlend.z, &diffKeyRotBlend.z);

			/* 求める差分 */
			diffRotBlend.z = diffKeyRotBlend.z - diffKeyRotCurrent.z;
			UpdateRot.z = diffKeyRotCurrent.z + (diffRotBlend.z * fRateBlend);
		}
		else
		{ // ブレンド無し	
		  /** キー情報から位置と向きを算出！ **/
		  // === POS ===			
		  // --- X ---			
			diffPos.x = pKeyNext->pos.x - pKey->pos.x;
			UpdatePos.x = pKey->pos.x + diffPos.x * fRateKey;

			// --- Y ---		
			diffPos.y = pKeyNext->pos.y - pKey->pos.y;
			UpdatePos.y = pKey->pos.y + diffPos.y * fRateKey;

			// --- Z ---			
			diffPos.z = pKeyNext->pos.z - pKey->pos.z;
			UpdatePos.z = pKey->pos.z + diffPos.z * fRateKey;

			// === ROT ===			
			// --- X ---		
			diffRot.x = pKeyNext->rot.x - pKey->rot.x;
			RepairRot(&diffRot.x, &diffRot.x);
			UpdateRot.x = pKey->rot.x + diffRot.x * fRateKey;
			RepairRot(&UpdateRot.x, &UpdateRot.x);

			// --- Y ---		
			diffRot.y = pKeyNext->rot.y - pKey->rot.y;
			RepairRot(&diffRot.y, &diffRot.y);
			UpdateRot.y = pKey->rot.y + diffRot.y * fRateKey;
			RepairRot(&UpdateRot.y, &UpdateRot.y);

			// --- Z ---			
			diffRot.z = pKeyNext->rot.z - pKey->rot.z;
			RepairRot(&diffRot.z, &diffRot.z);
			UpdateRot.z = pKey->rot.z + diffRot.z * fRateKey;
			RepairRot(&UpdateRot.z, &UpdateRot.z);
		}

		/** 位置、向きを更新！ **/
		pModel->pos = pModel->posLocal + UpdatePos;
		pModel->rot = pModel->rotLocal + UpdateRot;
	}

	if (pGimmick->bBlendMotion == false)
	{// ブレンドなし
		pGimmick->nCounterMotion++;

		if (pGimmick->nCounterMotion >= pInfo->nFrame && pGimmick->bFinishMotion == false)
		{ // モーションカウンターが現在のキー情報のフレーム数を超えた場合
			if (pGimmick->motionType < 0 || pGimmick->motionType >= MOTIONTYPE_MAX)
			{ // モーションインデックスの上下限確認		
				return;
			}

			/** キーを一つ進める **/
			pGimmick->nKey = ((pGimmick->nKey + 1) % pGimmick->aMotionInfo[pGimmick->motionType].nNumKey);

			if (pGimmick->nKey == pGimmick->aMotionInfo[pGimmick->motionType].nNumKey - 1 && pGimmick->bLoop == false)
			{
				pGimmick->bFinishMotion = true;
			}

			pGimmick->nCounterMotion = 0;
		}
	}
	else
	{// ブレンドあり
		pGimmick->nCounterMotionBlend++;

		if (pGimmick->nCounterMotionBlend >= pInfoBlend->nFrame)
		{ // モーションカウンターがブレンドモーションの現在のキーのフレーム数を超えた場合	
			/** ブレンドモーションのキーを一つ進める **/
			pGimmick->nKeyBlend = ((pGimmick->nKeyBlend + 1) % pGimmick->aMotionInfo[pGimmick->motionTypeBlend].nNumKey);
			pGimmick->nCounterMotionBlend = 0;
		}

		pGimmick->nCounterBlend++;

		if (pGimmick->nCounterBlend >= pGimmick->nFrameBlend)
		{ // ブレンドカウンターがブレンドフレーム数を超えた場合	
			pGimmick->bFinishMotion = true;
			SetMotionType(pGimmick->motionTypeBlend, false, 20, type);
		}
	}
}

//==================================================================================
// --- ギミック情報の譲渡 ---
//==================================================================================
Gimmick* GetGimmick(void)
{
	return &g_aGimmick[0];
}

//==================================================================================
// --- アイテムの出現処理 ---
//==================================================================================
void SpawnItem(void)
{
	ItemSpawn* pItemSpawn = &g_aItemSpawn[0];

	for (int nCntUpdate = 0; nCntUpdate < g_nNumSpawnItem; nCntUpdate++, pItemSpawn++)
	{
		pItemSpawn->bSpawned = true;	// 出現状態に変更
		pItemSpawn->pos = g_posSpawn;	// 出現位置を設定

		// 各加速度を設定
		pItemSpawn->move = g_aMoveSpawn[nCntUpdate];
		pItemSpawn->pos += CONVERSION_Y(pItemSpawn->move, 140.0f);	// 初速を設定

		// アイテムを設置
		pItemSpawn->nIdxItem = SetItem(VECNULL, VECNULL, g_aIdxItem[nCntUpdate]);

		P_ITEM pItem = GetItem();
		pItem[pItemSpawn->nIdxItem].bUse = true;
		pItem[pItemSpawn->nIdxItem].bGirl = pItem[pItemSpawn->nIdxItem].bMouse = true;
	}

	// 噴水パーティクルを設置
	SetParticle(CONVERSION_Y(g_posSpawn, g_posSpawn.y + 30.0f), CParamColor::BLUE, g_aVecParticle[1], g_aVecParticle[0],
		1, 3.0f, 1000000, 15, true);
}

//==================================================================================
// --- 出現後のアイテム更新処理 ---
//==================================================================================
void UpdateSpawnItem(void)
{
	ItemSpawn* pItemSpawn = &g_aItemSpawn[0];

	for (int nCntUpdate = 0; nCntUpdate < g_nNumSpawnItem; nCntUpdate++, pItemSpawn++)
	{
		if (!pItemSpawn->bSpawned || pItemSpawn->pos.y <= 120.0f) continue;

		P_ITEM pItem = &GetItem()[pItemSpawn->nIdxItem];

		pItemSpawn->pos += pItemSpawn->move;		// 位置を更新
		pItem->pos = pItemSpawn->pos;
		SetPositionItemEffector(pItem->nIdxEffector, pItem->pos);

		// 加速度を減速
		pItemSpawn->move.x += (0.0f - pItemSpawn->move.x) * g_fResistPow;
		pItemSpawn->move.z += (0.0f - pItemSpawn->move.z) * g_fResistPow;

		// 重力を適用
		pItemSpawn->move.y += GRAVITY;

		// 地面判定
		if (pItemSpawn->pos.y <= 120.0f)
		{
			pItemSpawn->pos.y = 120.0f;
			pItemSpawn->move = VECNULL;
		}
	}
}

//==================================================================================
// --- ギミックシリンダーの更新処理 ---
//==================================================================================
void UpdateGimmickCylinder(void)
{
	Player *pPlayer = GetPlayer();
	int nStartCount = (GetNumPlayer() == 2) ? 0 : GetActivePlayer();
	int nMaxCount = (GetNumPlayer() == 2) ? PLAYERTYPE_MAX : PLAYERTYPE_MAX - (1 - GetActivePlayer());
	bool aAlreadyDetection[GC_MAX] = {};		// 判定変数(falseの場合、どのプレイヤーにも近くない)

	for (int nCntPlayer = nStartCount; nCntPlayer < nMaxCount; nCntPlayer++, pPlayer++)
	{
		for (int nCntGimmick = 0; nCntGimmick < GC_MAX; nCntGimmick++)
		{
			if (g_aGCData[nCntGimmick].bEnable == false) continue;

			LPGIMMICK pGimmick = &GetGimmick()[g_aGCData[nCntGimmick].typeDetection];

			// 半径を確認
			if (IsDetection(pPlayer->pos, pGimmick->pos, g_fRadiusDetectionGC))
			{ // 半径以下なら
				aAlreadyDetection[nCntGimmick] = true;		// 判定済み
				
			}
		}
	}

	for (int nCntGimmick = 0; nCntGimmick < GC_MAX; nCntGimmick++)
	{
		LPGIMMICK pGimmick = &GetGimmick()[g_aGCData[nCntGimmick].typeDetection];

		if (g_aGCData[nCntGimmick].bEnable == false && g_aGCData[nCntGimmick].s == 0.0f) continue;

		if (aAlreadyDetection[nCntGimmick])
		{ // プレイヤーが近いなら、半径増加
			if (g_aGCData[nCntGimmick].s < 1.0f)
			{
				g_aGCData[nCntGimmick].s += 0.01f;
				g_aGCData[nCntGimmick].s = (g_aGCData[nCntGimmick].s >= 1.0f) ? 1.0f : g_aGCData[nCntGimmick].s;
			}

			// シリンダーの半径を適用
			SetRadiusMeshCylinder(&GetMeshCylinder()[g_aGCData[nCntGimmick].nIdxMeshCylinder],
				Lerp(g_fRadiusGCMin, g_fRadiusGCMax, g_aGCData[nCntGimmick].s));

			// シリンダーの色を変換
			g_aGCData[nCntGimmick].fAngle = RepairRot(g_aGCData[nCntGimmick].fAngle + 0.07f);
			SetColorMeshCylinder(&GetMeshCylinder()[g_aGCData[nCntGimmick].nIdxMeshCylinder],
				GetColLerp(g_aGCData[nCntGimmick].col, CONVERSION_A(g_aGCData[nCntGimmick].col, 1.0f),
					sinf(g_aGCData[nCntGimmick].fAngle)));
		}
		else
		{ // プレイヤーが離れていれば、半径減少
			if (g_aGCData[nCntGimmick].s > 0.0f)
			{
				g_aGCData[nCntGimmick].s -= 0.01f;
				g_aGCData[nCntGimmick].s = (g_aGCData[nCntGimmick].s <= 0.0f) ? 0.0f : g_aGCData[nCntGimmick].s;
			}

			// シリンダーの半径を適用
			SetRadiusMeshCylinder(&GetMeshCylinder()[g_aGCData[nCntGimmick].nIdxMeshCylinder],
				Lerp(g_fRadiusGCMin, g_fRadiusGCMax, g_aGCData[nCntGimmick].s));

			// シリンダーの色を元に戻す
			SetColorMeshCylinder(&GetMeshCylinder()[g_aGCData[nCntGimmick].nIdxMeshCylinder], 
				g_aGCData[nCntGimmick].col);
		}
	}
}

//==================================================================================
// --- ギミックエリアを取得 ---
//==================================================================================
GC_DATA *GetGCData(void)
{
	return &g_aGCData[0];
}

//==================================================================================
// --- ギミックエリアの色を取得 ---
//==================================================================================
D3DXCOLOR GetGimmickColor(GIMMICKCYLINDER type)
{
	return g_aGCData[type].col;
}

//==================================================================================
// --- チュートリアルが表示されているかどうか ---
//==================================================================================
bool IsTutorialreveal(void)
{
	return g_bAnyTex;
}