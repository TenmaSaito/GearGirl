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

// =================================================
// プレイヤータイプ
typedef enum
{
	PLAYERTYPE_GIRL,
	PLAYERTYPE_MOUSE,
	PLAYERTYPE_MAX
}PlayerType;


// =================================================
// プレイヤーの状態
typedef enum
{
	PLAYERSTATE_NEUTRAL = 0,	// 待機状態
	PLAYERSTATE_WALK,			// 歩き状態
	PLAYERSTATE_ATTACK,			// 攻撃状態
	PLAYERSTATE_JUMP,			// ジャンプ状態
	PLAYERSTATE_LAND,			// 着地状態
	PLAYERSTATE_MAX
}Playerstate;

// =================================================
// プレイヤーの構造体を定義
typedef struct
{
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
	Model aModel[MAX_PART];		// モデル(パーツ)
	int nNumModel;				// モデルの総数(パーツ数)
	Playerstate state;			// プレイヤーの状態

	// モーションの要素
	MOTION_INFO aMotionInfo[MOTIONTYPE_MAX];	// モーション情報
	int nNumMotion;			// 現在のモーションの総数
	MOTIONTYPE motionType;	// 現在のモーションの種類
	bool bLoopMotion;		// ループするかどうか
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
}Player;

// =================================================
// プロトタイプ宣言
void InitPlayer(void);		// 初期化処理
void UninitPlayer(void);	// 終了処理
void UpdatePlayer(void);	// 更新処理
void DrawPlayer(void);		// 描画処理
Player *GetPlayer(void);	// プレイヤー情報を渡す
void MovePlayer(int nPlayer);	// プレイヤーの移動関数
void JumpPlayer(void);			// プレイヤーのジャンプ関数
void SetModel(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int NumIdx, int Parent, int nParts);	// モデルの設置処理
void UpdateMotion(void);	// モーションの更新
void SetKey(D3DXVECTOR3 pos, D3DXVECTOR3 rot);	// キー毎の位置、向きの設定
void SetKeyInfo(int nFrame);					// 一つ一つキーの設定
void SetMotionInfo(int nLoop, int NumKey);		// モーション情報の設定
void SetMotion(MOTIONTYPE motionType, bool bUseBrend, int nBlendFrame, PlayerType Playertype);	// モーションの呼び出し処理

#endif