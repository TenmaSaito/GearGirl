//==================================================================================================================================
//
//			カメラ処理 [camera.h]
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
#define VIEW_MINDEPTH			(10.0f)					// 最小描画距離
#define VIEW_MAXDEPTH			(10000.0f)				// 最大描画距離
#define CAMERA_V_DEFPOS			(0.0f, 350.0f, 500.0f)	// 視点のデフォ位置
#define CAMERA_R_DEFPOS			(0.0f, 0.0f, 0.0f)		// 注視点のデフォ位置
#define CAMERA_DISTANS			(700.0f)				// カメラと注視点の距離
#define CAMERA_MOVE				(10.0f)					// カメラの移動速度
#define CAMERA_SPIN				(0.01f)					// カメラの回転速度
#define CAMERA_FOLLOW_FACTOR	(0.15f)					// カメラが追従移動する時の補正
#define CAMERA_ROTET_FACTOR		(0.01f)					// カメラが追従回転する時の補正
#define CAMERA_PLAYER_FRONT		(50.0f)					// 注視点をプレイヤーより少し先にする

//**************************************************************
// カメラ操作
#define CAM_RESET		DIK_TAB					// カメラ位置角度リセット
#define CAM_MOVE_UP		DIK_W					// カメラ移動　前
#define CAM_MOVE_DW		DIK_S					// カメラ移動　後
#define CAM_MOVE_L		DIK_A					// カメラ移動　左
#define CAM_MOVE_R		DIK_D					// カメラ移動　右

#define CAM_ORBIT_UP	DIK_T					// カメラ移動回転　上
#define CAM_ORBIT_DW	DIK_B					// カメラ移動回転　下
#define CAM_ORBIT_L		DIK_Z					// カメラ移動回転　左
#define CAM_ORBIT_R		DIK_C					// カメラ移動回転　右

#define CAM_ZOOM		DIK_SPACE				// ズームアウト
#define CAM_ZOOM_IN		DIK_LSHIFT				// ズームイン (+スペース

#define MAX_CAMERA		2						// カメラの数

#define CAMERA_NULLCHECK(n)		if (-1 < n && n < MAX_CAMERA)


//==============================================================
// 列挙型
//==============================================================

//==============================================================
// カメラの構造体定義
//==============================================================
typedef struct Camera
{
	D3DXVECTOR3 posV;					// 視点	
	D3DXVECTOR3 posR;					// 注視点
	D3DXVECTOR3 posRDest;				// 目的の注視点
	D3DXVECTOR3 rot;					// 向き
	float fDist;						// 視点と注視点の距離
	D3DXVECTOR3 vecU;					// 上方向ベクトル
	D3DXMATRIX mtxProjection;			// プロジェクションマトリックス
	D3DXMATRIX mtxView;					// ビューマトリックス
	D3DVIEWPORT9 viewport;				// ビューポート

	bool bAoutRot;						// 自動で回り込み
	int nCntAoutRot;					// 自動で回り込むまでのカウンタ
	float fPlayerFront;					// プレイヤーより少し前
	float fPlayerMoveRot;				// プレイヤーの移動方向
	bool bUse;							// 配列を使っているか
	bool bEnable;						// 描画に使うか
}Camera;
POINTER(Camera, P_CAMERA);

//=========================================================================================
//プロトタイプ宣言
//=========================================================================================
void InitCamera(void);
void UninitCamera(void);
void UpdateCamera(void);
void SetCamera(void);										// カメラを設置（mainのDrawの最初にする）
void SetPotisionCamera(int nIdx, vec3 pos);					// カメラの位置更新
void CameraEnable(int nIdx);								// カメラを描画に使用すると宣言
int GetCamera(void);										// まだ未使用のカメラ番号を取得
P_CAMERA GetCameraArray(void);								// カメラ除法配列の先頭アドレスを取得
void GetCameraPos(int nCamNum, vec3* pPosV, vec3* pPosR);	// カメラの位置情報を取得
void GetCameraRot(int nCamNum, vec3* pRot);					// カメラの角度情報を取得
int GetCameraNum(void);										// 使用中のカメラの数を取得
void CameraReset(P_CAMERA pCamera);							// カメラリセット

#endif// !_CAMERA_H_