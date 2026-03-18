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
#define	MAX_PLAYER		(2)		// プレイヤーの最大数
#define PLAYER_POSDEF	D3DXVECTOR3(1400.0f, 100.0f, -1000.0f) // プレイヤーの初期位置
#define	PLAYER_MOVE		(0.6f)	// プレイヤーの移動速度
#define	PLAYER_ROTMOVE	(0.3f)	// プレイヤーの回転速度
#define	PLAYER_INI		(0.2f)	// プレイヤーの慣性
#define	PLAYER_LIM		(50.0f)	// プレイヤーの移動制限
#define	JUMP_FORCE		(2.4f)	// ジャンプ力

#define	GRAVITY			(-0.14f)			// 重力
#define	STICK_DEADAREA	(10000)			// ジョイスティックのデッドエリア
#define	MODEL_PLAYER	"data\\model.txt"	// モデルを読み込むファイル名

// ゲーム状態
#define TUTORIAL_NOW	(IsEndDialog() == false)		// チュートリアル中
#define GAME_NOW		(IsEndDialog() == true)			// ゲーム本編中
#define DIAROG_ON		(IsShowAnyDialog() == true)		// ダイアログ表示中
#define DIAROG_OFF		(IsShowAnyDialog() == false)	// ダイアログ非表示中
#define ITEM_ON			(IsEnableItemPut() == true)		// アイテム提出画面
#define ITEM_OFF		(IsEnableItemPut() == false)	// アイテム提出画面でない
#define ITEMPROMPT_ON	(IsDispPrompt(GetIdxShopPrompt()) == true)	// アイテム提出のプロンプトが表示されている
#define ITEMPROMPT_OFF	(IsDispPrompt(GetIdxShopPrompt()) == false)	// アイテム提出のプロンプトが表示されていない
#define UIMENU_ON		(GetEnableUImenu() == true)		// アイテムUI表示中
#define UIMENU_OFF		(GetEnableUImenu() == false)	// アイテムUI非表示中
#define MAP_ON			(GetEnableUImenu() == true)		// マップが拡大している
#define MAP_OFF			(GetEnableUImenu() == false)	// マップが拡大してない
#define TUTORIALTEX_ON	(IsTutorialreveal() == true)	// ギミックチュートリアルが表示されている
#define TUTORIALTEX_OFF	(IsTutorialreveal() == false)	// ギミックチュートリアルが表示されていない


// 色の設定
#define COL_RED			D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f)	// 赤色
#define COL_HALFRED		D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.5f)	// 赤色(半透明)
#define COL_GREEN		D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f)	// 緑色
#define COL_HALFGREEN	D3DXCOLOR(0.0f, 1.0f, 0.0f, 0.5f)	// 緑色(半透明)
#define COL_BLUE		D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)	// 青色
#define COL_HALFBLUE	D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.5f)	// 青色(半透明)
#define COL_YELLOW		D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f)	// 黄色
#define COL_HALFYELLOW	D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.5f)	// 黄色(半透明)
#define COL_PURPLE		D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f)	// 黄色
#define COL_HALFPURPLE	D3DXCOLOR(1.0f, 0.0f, 1.0f, 0.5f)	// 黄色(半透明)
#define COL_BLACK		D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)	// 黒
#define COL_HALFBLACK		D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.5f)	// 黒(半透明)

// ゲームの設定
#define MAX_ZMOVE1		(-1590)		// Z軸移動可能領域1
#define MAX_ZMOVE2		(990)		// Z軸移動可能領域2
#define MAX_XMOVE1		(315)		// X軸移動可能領域1
#define MAX_XMOVE2		(2290)		// X軸移動可能領域2

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
	ARMTYPE_CUT,		// 倒木などを切る腕
	ARMTYPE_MAX
}ArmType;

// =================================================
// プレイヤーの状態
typedef enum
{
	PLAYERSTATE_NEUTRAL = 0,	// 待機状態
	PLAYERSTATE_MOVE,			// 歩き状態
	PLAYERSTATE_THROWWAITING,	// 投げ待機状態
	PLAYERSTATE_CUTTING,		// カッティング状態
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
	bool bDisp;					// プレイヤーの描画可否
	bool bJump;					// ジャンプ中かどうか
	int nIdxCamera;				// 使用するカメラのインデックス
	int nNumModel;				// プレイヤーを構成するパーツ数
	PARTS_INFO PartsInfo;		// プレイヤーのパーツの情報
	Playerstate state;			// プレイヤーの状態
	ArmType Armtype;			// 腕の種類
	PlayerType playertype;		// 操作対象
	float fMove;				// プレイヤーの速度
	bool bDash;					// ダッシュ状態かどうか
	bool bArea;					// 切り替え不可能エリアにいるかどうか
	bool bChangeable;			// 切り替え事態が可能かどうか

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
void SetRotShop(D3DXVECTOR3 pos);

#endif