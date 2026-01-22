//==================================================================================
//
// DirectXのモーション読み込み用ヘッダファイル [motion.h]
// Author : TENMA
//
//==================================================================================
#ifndef _MOTION_H_
#define _MOTION_H_

//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "main.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define MAX_MOTIONDATA		(256)			// 読み込めるモーション数
#define ERROR_MOTION		(-1)			// モーション読み込み失敗時の値
#define MAX_MOTION			(20)			// モーションの最大数
#define MAX_KEY_INFO		(30)			// キー情報の最大数
#define MAX_KEY				(20)			// キーの最大数

//**********************************************************************************
//*** モーションの種類 ***
//**********************************************************************************
typedef enum
{
	MOTIONTYPE_NEUTRAL = 0,			// 待機
	MOTIONTYPE_MOVE,				// 移動
	MOTIONTYPE_ACTION,				// 行動
	MOTIONTYPE_JUMP,				// ジャンプ
	MOTIONTYPE_LANDING,				// 着地
	MOTIONTYPE_RESERVED_0,			// モーション0
	MOTIONTYPE_RESERVED_1,			// モーション1
	MOTIONTYPE_RESERVED_2,			// モーション2
	MOTIONTYPE_RESERVED_3,			// モーション3
	MOTIONTYPE_RESERVED_4,			// モーション4
	MOTIONTYPE_MAX
}MOTIONTYPE;

//**********************************************************************************
//*** キー要素構造体の定義 ***
//**********************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;	// 位置
	D3DXVECTOR3 rot;	// 向き
} KEY;

//**********************************************************************************
//*** キー情報構造体の定義 ***
//**********************************************************************************
typedef struct
{
	int nFrame;					// 再生フレーム
	KEY aKey[MAX_KEY];			// 各パーツのキー要素(パーツ数分だけ用意)
} KEY_INFO;

//**********************************************************************************
//*** モ―ション情報構造体の定義 ***
//**********************************************************************************
typedef struct MOTION_INFO
{
	KEY_INFO aKeyInfo[MAX_KEY_INFO];	// キー情報
	int nNumKey;						// キーの総数
	bool bLoop;							// ループするかどうか
	bool bUse;							// 使用しているか
	char aFileName[MAX_PATH];			// モーションファイル名
} MOTION_INFO;

typedef MOTION_INFO *LPMOTION_INFO;

//**********************************************************************************
//*** モ―ションスクリプト情報構造体の定義 ***
//**********************************************************************************
typedef struct MOTIONSCRIPT_INFO
{
	MOTION_INFO aMotionInfo[MOTIONTYPE_MAX];	// モーション情報
	int nNumMotion;								// モーションの総数
	bool bUse;									// 使用しているか
	char aFileName[MAX_PATH];					// モーションファイル名
} MOTIONSCRIPT_INFO;

typedef MOTIONSCRIPT_INFO *LPMOTIONSCRIPT_INFO;

//**********************************************************************************
//*** パーツ構造体の定義 ***
//**********************************************************************************
typedef struct PARTS
{
	D3DXVECTOR3 pos;			// 位置(オフセット)
	D3DXVECTOR3 posLocal;		// ローカル座標(オフセット)
	D3DXVECTOR3 rot;			// 向き
	D3DXVECTOR3 rotLocal;		// ローカル角度
	D3DXMATRIX mtxWorld;		// ワールドマトリックス
	int nIdxModel;				// モデルインデックス
	int nIdxModelParent;		// 親モデルのインデックス
	bool bUse;					// 使用状況
} PARTS;

//**********************************************************************************
//*** パーツ情報構造体の定義 ***
//**********************************************************************************
typedef struct PARTS_INFO
{
	PARTS aParts[MAX_KEY];		// パーツ情報
	int nNumParts;				// パーツ数
	bool bUse;					// 使用状況
} PARTS_INFO;

typedef PARTS_INFO *LPPARTS_INFO;

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************
void InitMotion(void);
void UninitMotion(void);

bool LoadMotion(_In_ const char* pMotionFileName, int* pOutIdx);
LPMOTIONSCRIPT_INFO GetMotionScriptInfo(int nType);
LPPARTS_INFO GetPartsInfo(int nType);
#endif
