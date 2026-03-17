// =================================================
// 
// タイトル用プレイヤー処理[tPlayer.h]
// Author : Tenma Saito
// 
// =================================================
#ifndef _TPLAYER_H_
#define _TPLAYER_H_

#include "main.h"
#include "motion.h"
#include "modeldata.h"

// =================================================
// マクロ定義
#define	MAX_TPART		(6)		// パーツの最大数
#define PLAYER_POSDEF	D3DXVECTOR3(1400.0f, 100.0f, -1000.0f) // プレイヤーの初期位置

// =================================================
// プレイヤーの構造体を定義
typedef struct
{
	// プレイヤー自身の要素
	D3DXVECTOR3 posOri;			// 何かの動作を行った位置
	D3DXVECTOR3 pos;			// 現在の位置
	D3DXVECTOR3 Offset[MAX_TPART];	// プレイヤーモデルのオフセット
	D3DXVECTOR3 posOld;			// 1フレーム前の位置
	D3DXVECTOR3 move;			// 移動量
	D3DXVECTOR3 rot;			// 向き
	D3DXVECTOR3 rotDest;		// 目標の向き
	D3DXVECTOR3 rotDiff;		// 角度の差分
	D3DXMATRIX mtxWorld;		// ワールドマトリックス
	bool bDisp;					// プレイヤーの描画可否
	bool bJump;					// ジャンプ中かどうか
	int nNumModel;				// プレイヤーを構成するパーツ数
	PARTS_INFO PartsInfo;		// プレイヤーのパーツの情報
	float fMove;				// プレイヤーの速度
	bool bDash;					// ダッシュ状態かどうか

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

	// 判別用など
	int nCntAllround;			// あらゆるカウントに使用
	bool bUseLandMotion;		// 着地モーションを再生したかどうか
}TPlayer;

// =================================================
// プロトタイプ宣言
void InitTPlayer(void);		// 初期化処理
void UninitTPlayer(void);	// 終了処理
void UpdateTPlayer(void);	// 更新処理
void DrawTPlayer(void);		// 描画処理
TPlayer *GetTPlayer(void);	// プレイヤー情報を渡す
void SetTPlayer(D3DXVECTOR3 pos, D3DXVECTOR3 rot);
#endif