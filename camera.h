//==================================================================================================================================
//
//			プレイヤーカメラ処理 [camera.h]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
#ifndef _CAMERA_H_
#define _CAMERA_H_

//==============================================================
// ヘッダーインクルード
#include "main.h"
#include "endomacro.h"

//=========================================================================================
// マクロ定義
//=========================================================================================
#define VIEW_RADIAN				(45.0f)					// 視野角
#define VIEW_RADIAN_MOUSE		(90.0f)					// ネズミの視野角
#define VIEW_1P_MINDEPTH		(5.0f)					// 最小描画距離
#define VIEW_1P_MAXDEPTH		(10000.0f)				// 最大描画距離
#define VIEW_2P_MINDEPTH		(5.0f)					// 最小描画距離
#define VIEW_2P_MAXDEPTH		(1000.0f)				// 最大描画距離
#define CAMERA_V_DEFPOS			(0.0f, 350.0f, 500.0f)	// 視点のデフォ位置
#define CAMERA_R_DEFPOS			(0.0f, 0.0f, 0.0f)		// 注視点のデフォ位置
#define CAMERA_1P_DISTANS		(40.0f)					// 少女の,カメラと注視点の距離
#define CAMERA_2P_DISTANS		(10.0f)					// ネズミの,カメラと注視点の距離
#define CAMERA_1P_ROT			vec3(1.3f, 0.0f, 0.0f)	// 少女のカメラの角度
#define CAMERA_2P_ROT			vec3(1.12f, 0.0f, 0.0f)	// ネズミのカメラの角度
#define CAMERA_MOVE				(10.0f)					// カメラの移動速度
#define CAMERA_SPIN				(0.01f)					// カメラの回転速度
#define CAMERA_FOLLOW_FACTOR	(0.15f)					// カメラが追従移動する時の補正
#define CAMERA_ROTET_FACTOR		(0.01f)					// カメラが追従回転する時の補正
#define CAMERA_PLAYER_FRONT		(50.0f)					// 注視点をプレイヤーより少し先にする
#define CAMERA_PLFR_DEADZONE	(0.01f)					// これ以上速度があればカメラを動かす
#define SETCAMERAPOS_COUNTER	(30)					// カメラ切り替え時このフレーム以内に元の位置に戻る

//**************************************************************
// カメラ操作
#define CAM_2POPRAT		DIK_LCONTROL			// ２Pのカメラを操作

#define CAM_RESET		DIK_TAB					// カメラ位置角度リセット
#define CAM_MOVE_UP		DIK_W					// カメラ移動　上
#define CAM_MOVE_DW		DIK_S					// カメラ移動　下
#define CAM_MOVE_L		DIK_A					// カメラ移動　左
#define CAM_MOVE_R		DIK_D					// カメラ移動　右

#define CAM_ORBIT_UP	DIK_T					// カメラ移動回転　上
#define CAM_ORBIT_DW	DIK_B					// カメラ移動回転　下
#define CAM_ORBIT_L		DIK_C					// カメラ移動回転　左
#define CAM_ORBIT_R		DIK_Z					// カメラ移動回転　右

#define CAM_ZOOM_OUT	DIK_R					// ズームアウト
#define CAM_ZOOM_IN		DIK_V					// ズームイン (+スペース

//==============================================================
// 列挙型
//==============================================================
typedef enum
{
	CAMERATYPE_PLAYER_ONE = 0,			// 少女に追従カメラ
	CAMERATYPE_PLAYER_TWO,				// ネズミに追従カメラ
	CAMERATYPE_FOCUS,					// 集中して見せる
	MAX_CAMERA
}CameraType;

//==============================================================
// カメラの構造体定義
//==============================================================
typedef struct Camera
{
	D3DXVECTOR3 posV;					// 視点	
	D3DXVECTOR3 posR;					// 注視点
	D3DXVECTOR3 posRDest;				// 目的の注視点
	D3DXVECTOR3 rot;					// 向き
	float		fDist;					// 視点と注視点の距離
	float		fViewMin;				// 最小描画距離
	float		fViewMax;				// 最大描画距離
	D3DXVECTOR3 vecU;					// 上方向ベクトル
	D3DXMATRIX	mtxProjection;			// プロジェクションマトリックス
	D3DXMATRIX	mtxView;				// ビューマトリックス
	D3DVIEWPORT9 viewport;				// ビューポート
	float		fViewRadian;			// 視野角

	bool		bFog;					// 霧を出すか
	D3DXCOLOR	FogColor;				// 霧の色
	float		fStart;					// 霧の開始距離
	float		fEnd;					// 霧の終着距離

	bool		bAoutRot;				// 自動で回り込み
	int			nCntAoutRot;			// 自動で回り込むまでのカウンタ
	float		fPlayerFront;			// プレイヤーより少し前
	float		fPlayerMoveRot;			// プレイヤーの移動方向
	CameraType	type;					// カメラタイプ
	bool		bUse;					// 配列を使っているか
}Camera;
POINTER(Camera, P_CAMERA);

//=========================================================================================
//プロトタイプ宣言
//=========================================================================================
void InitCamera(void);
void UninitCamera(void);
void UpdateCamera(void);
void SetCamera(void);													// カメラを設置（mainのDrawの最初にする）
void ReleaseCamera(int nIdx);											// カメラを開放
P_CAMERA GetCamera(void);												// カメラ除法配列の先頭アドレスを取得
void GetCameraPos(int nCamNum, vec3* pPosV, vec3* pPosR);				// カメラの位置情報を取得
void GetCameraRot(int nCamNum, vec3* pRot);								// カメラの角度情報を取得
int GetCameraNum(void);													// 使用中のカメラの数を取得
CameraType GetReadyCamera(void);										// 描画中のカメラ番号を取得
void CameraReset(P_CAMERA pCamera);										// カメラリセット
void FogEnable(CameraType type, bool bEnable = true, D3DXCOLOR col = colX_ZERO, float fStart = 0.0f, float fEnd = 0.0f);
																		// 霧を有効化
void CleanFog(void);													// 霧を削除
void Focus(vec3 pos); // フォーカス

#endif// !_CAMERA_H_