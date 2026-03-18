//==================================================================================
//
// DirectXのギミック処理 [gimmick.h]
// Author : TENMA
//
//==================================================================================
#ifndef _GIMMICK_H_		// IncludeGuard
#define _GIMMICK_H_

//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include "mathutil.h"
#include "player.h"

// 自作ユーティリティ使用宣言
USE_UTIL;

// パラメータ使用宣言
USE_PARAM;

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************

//**********************************************************************************
//*** ギミックタイプ ***
//**********************************************************************************
typedef enum
{
	GIMMICKTYPE_BIGBUTTON = 0,	// でかボタン
	GIMMICKTYPE_SMALLBUTTON,	// ちびボタン
	GIMMICKTYPE_FALLENTREE,		// 倒木
	GIMMICKTYPE_FALLENTREE2,	// 倒木
	GIMMICKTYPE_FALLENTREE3,	// 倒木
	GIMMICKTYPE_CLOSEDDOOR,		// 閉まっている扉
	GIMMICKTYPE_TUNNEL,			// トンネル
	GIMMICKTYPE_STATUE,			// 噴水前の銅像
	GIMMICKTYPE_MAX
}GIMMICKTYPE;

//**********************************************************************************
//*** ギミッククリア可能なプレイヤータイプ ***
//**********************************************************************************
typedef enum
{
	COULD_PLAYER_GIRL = 0,
	COULD_PLAYER_MOUSE,
	COULD_PLAYER_ALL,
	COULD_PLAYER_MAX
} COULD_PLAYER;

//**********************************************************************************
//*** ギミックシリンダーの種類 ***
//**********************************************************************************
typedef enum
{
	GC_STATUE = 0,		// 銅像のシリンダー
	GC_FALLENTREE,		// 倒木のシリンダー
	GC_BIGBUTTON,		// 駅前のボタンのシリンダー
	GC_MAX
}GIMMICKCYLINDER;

//**********************************************************************************
//*** ギミック構造体 ***
//**********************************************************************************
START_UNABLE
UNABLE_THISFILE(26495)
typedef struct
{
	// ギミックの要素
	D3DXVECTOR3 pos;		// 中心位置
	D3DXVECTOR3 rot;		// ギミック全体の向き
	D3DXMATRIX mtxWorld;	// ワールドマトリックス
	PARTS_INFO parts;		// ギミックパーツ
	COULD_PLAYER could;		// クリア可能なプレイヤータイプ
	GIMMICKTYPE myType;		// 自身のギミックの種類
	float fRadius;			// 検知半径
	int nCounter;			// 汎用カウンター
	int nIdxPrompt;			// プロンプトのインデックス
	bool bUse;				// 使用状況
	bool bClear;			// クリアしたか

	// モーション関連
	MOTION_INFO aMotionInfo[MOTIONTYPE_MAX];	// クリア時モーション
	int nNumMotion;			// 現在のモーションの総数
	bool bLoop;				// ループするかどうか
	int nNumKey;			// 現在のモーションのキーの総数
	int nKey;				// 現在のモーションの現在のキーNo.
	int nCounterMotion;		// 現在のモーションのカウンター
	bool bFinishMotion;		// 現在のモーションが終了したかどうか
	MOTIONTYPE motionType;	// モーションタイプ

	// モーションブレンド関連
	bool bBlendMotion;			// ブレンドモーションするかどうか
	MOTIONTYPE motionTypeBlend;	// ブレンドモーションの種類
	bool bLoopMotionBlend;		// ブレンドモーションがループするか
	int nNumKeyBlend;			// ブレンドモーションの総キー数
	int nKeyBlend;				// ブレンドモーションの現在のキーNo.
	int nCounterMotionBlend;	// ブレンドモーションのカウンター
	int nFrameBlend;			// ブレンドフレーム数
	int nCounterBlend;			// ブレンドカウンター
}Gimmick, * LPGIMMICK;

//**********************************************************************************
//*** ギミックシリンダー構造体 ***
//**********************************************************************************
typedef struct
{
	D3DXCOLOR col;				// 初期色
	float s;					// Lerp変換用変数
	float fAngle;				// sin波用変数
	GIMMICKTYPE type;			// 対象のギミック
	GIMMICKTYPE typeDetection;	// 範囲検知対象のギミック
	int nIdxMeshCylinder;		// メッシュシリンダーのインデックス
	bool bEnable;				// 有効判定
}GC_DATA;

END_UNABLE

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************
void InitGimmick(void);
void UninitGimmick(void);
void UpdateGimmick(void);
void DrawGimmick(void);

void SetGimmick(D3DXVECTOR3 pos, D3DXVECTOR3 rot, GIMMICKTYPE type);
void ClearGimmick(GIMMICKTYPE type);
bool IsClearGimmick(GIMMICKTYPE type);
bool CollisionGimmick(D3DXVECTOR3 *pPos, 
	D3DXVECTOR3 *pPosOld, 
	D3DXVECTOR3 *pMove, 
	Player *pPlayer,
	float fRadius,
	float fHeight);
Gimmick* GetGimmick(void);	// ギミック情報の譲渡
bool IsTutorialreveal(void);	// チュートリアルが表示されているかどうか

GC_DATA *GetGCData(void);
D3DXCOLOR GetGimmickColor(GIMMICKCYLINDER type);

#endif	// !_GIMMICK_H_