// =================================================
// 
// モデル処理[player.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _PLAYER_H_
#define _PLAYER_H_

#include"model.h"
#include"motion.h"

// =================================================
// マクロ定義
#define	MAX_PART		(6)		// パーツの最大数

#define	MAX_PLAYER		(2)				// プレイヤーの最大数
#define PLAYER_POSDEF	D3DXVECTOR3(0.0f, 100.0f, 0.0f) // プレイヤーの初期位置
#define	PLAYER_MOVE		(0.5f)			// プレイヤーの移動速度
#define	PLAYER_ROTMOVE	(0.3f)			// プレイヤーの回転速度
#define	PLAYER_INI		(0.2f)			// プレイヤーの慣性
#define	PLAYER_LIM		(50.0f)			// プレイヤーの移動制限
#define	ATTACK_MOVE		(1.0f)			// 突進の移動速度
#define	JUMP_FORCE		(3.8f)			// ジャンプ力

#define	GRAVITY			(-0.18f)			// 重力
#define	STICK_DEADAREA	(10000)			// ジョイスティックのデッドエリア
#define	MODEL_PLAYER	"data\\model.txt"	// モデルを読み込むファイル名

// =================================================
// プレイヤータイプ
typedef enum
{
	PLAYERTYPE_GIRL = 0,	// 少女
	PLAYERTYPE_MOUSE,		// ネズミ
	PLAYERTYPE_MAX		
}PlayerType;

// =================================================
// アームの種類
typedef enum
{
	ARMTYPE_NORMAL = 0,	// 通常の腕
	ARMTYPE_CATAPULT,	// カタパルトの腕
	ARMTYPE_MAX
}ArmType;

// =================================================
// プレイヤーの状態
typedef enum
{
	PLAYERSTATE_NEUTRAL = 0,	// 待機状態
	PLAYERSTATE_MOVE,			// 歩き状態
	PLAYERSTATE_ATTACK,			// 攻撃状態
	PLAYERSTATE_JUMP,			// ジャンプ状態
	PLAYERSTATE_LAND,			// 着地状態
	PLAYERSTATE_MAX
}Playerstate;

// =================================================
// プレイヤーの構造体を定義
typedef struct
{
	// プレイヤー自身の要素
	D3DXVECTOR3 posOri;			// 何かの動作を行った位置
	D3DXVECTOR3 pos;			// 現在の位置
	D3DXVECTOR3 Offset[MAX_PART];	// プレイヤーモデルのオフセット
	D3DXVECTOR3 posOld;			// 1フレーム前の位置
	D3DXVECTOR3 move;			// 移動量
	D3DXVECTOR3 rot;			// 向き
	D3DXVECTOR3 rotDest;		// 目標の向き
	D3DXVECTOR3 rotDiff;		// 角度の差分
	D3DXMATRIX mtxWorld;		// ワールドマトリックス
	D3DXVECTOR3 vtxMin;			// プレイヤーの最小値
	D3DXVECTOR3 vtxMax;			// プレイヤーの最大値
	D3DXVECTOR3 VecMove;		// プレイヤーの移動方向のベクトル
	D3DXVECTOR3 VectoPos;		// 境界線ベクトル - 移動ベクトル
	bool bDisp;					// プレイヤーの描画可否
	bool bJump;					// ジャンプ中かどうか
	int nIdxCamera;				// 使用するカメラのインデックス
	int nNumModel;				// プレイヤーを構成するパーツ数
	PARTS_INFO PartsInfo;		// プレイヤーのパーツの情報
	Playerstate state;			// プレイヤーの状態
	ArmType Armtype;			// 腕の種類
	PlayerType playertype;		// 操作対象

	// モーションの要素
	MOTION_INFO aMotionInfo[MOTIONTYPE_MAX];	// モーション情報
	int nNumMotion;			// 現在のモーションの総数
	MOTIONTYPE motionType;	// 現在のモーションの種類
	bool bLoop;				// ループするかどうか
	int nNumKey;			// 現在のモーションのキーの総数
	int nKey;				// 現在のモーションの現在のキーNo.
	int nCounterMotion;		// 現在のモーションのカウンター
	bool bFinishMotion;		// 現在のモーションが終了したかどうか
	int nFrame[16];			// キーの中のフレーム数

	// モーションブレンドの要素
	bool bBlendMotion;			// ブレンドモーションするかどうか
	MOTIONTYPE motionTypeBlend;	// ブレンドモーションの種類
	bool bLoopMotionBlend;		// ブレンドモーションがループするか
	int nNumKeyBlend;			// ブレンドモーションの総キー数
	int nKeyBlend;				// ブレンドモーションの現在のキーNo.
	int nCounterMotionBlend;	// ブレンドモーションのカウンター
	int nFrameBlend;			// ブレンドフレーム数
	int nCounterBlend;			// ブレンドカウンター

	int nCntAllround;			// あらゆるカウントに使用
	bool bUseLandMotion;		// 着地モーションを再生したかどうか
}Player;

// =================================================
// プロトタイプ宣言
void InitPlayer(void);		// 初期化処理
void UninitPlayer(void);	// 終了処理
void UpdatePlayer(void);	// 更新処理
void DrawPlayer(void);		// 描画処理
Player *GetPlayer(void);	// プレイヤー情報を渡す
int GetNumPlayer(void);			// プレイ人数情報を渡す
int GetActivePlayer(void);		// 操作しているプレイヤー情報を渡す
void SetPlayer(D3DXVECTOR3* pPosGirl, D3DXVECTOR3* pPosMouse);	// プレイヤーの位置情報を渡す
void ChangeNumPlayer(void);		// プレイ人数を切り替える(デバッグ用)

#endif