//==================================================================================================================================
//
//			カメラ処理 [camera.cpp]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
//**************************************************************
// インクルード
#include "camera.h"
#include "input.h"
#include "debugproc.h"

#include "player.h"

//**************************************************************
// グローバル変数宣言
Camera g_aCamera[MAX_CAMERA];
int		g_nNumEnableCamera;				// 起動させるカメラの数
int		g_nActivePlayer;				// １P時の画面のキャラクター

//**************************************************************
// プロトタイプ宣言
void SetCameraOption(void);					// カメラ設定
void CameraFollow(void);					// プレイヤーに追従移動
void CameraRotation(P_CAMERA pCamera);		// プレイヤーと同じ向きに回転
void CameraOrbit(P_CAMERA pCamera);			// オービット処理
void CameraMove(P_CAMERA pCamera);			// カメラ移動（ポーズ中のみ

//=========================================================================================
// カメラ初期化
//=========================================================================================
void InitCamera(void)
{
	//**************************************************************
	// 変数宣言
	P_CAMERA pCamera = GetCamera();

	//**************************************************************
	// 各値の初期化
	g_nNumEnableCamera = GetNumPlayer();	// プレイヤー数
	g_nActivePlayer = 0;	// 画面のプレイヤー

	for (int nCntCamera = 0; nCntCamera < MAX_CAMERA; nCntCamera++, pCamera++)
	{
		// カメラ座標等
		pCamera->posV = D3DXVECTOR3 CAMERA_V_DEFPOS;					// 視点
		pCamera->posR = PLAYER_POSDEF;									// 注視点
		pCamera->posRDest = PLAYER_POSDEF;								// 目的の注視点
		pCamera->vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);					// 上方向ベクトル

		switch (nCntCamera)
		{
		case PLAYER_TWO:	// 2P用のカメラ設定
			pCamera->rot = CAMERA_2P_ROT;	// カメラの角度
			pCamera->fDist = CAMERA_2P_DISTANS;
			break;
		default:			// その他のカメラ設定
			pCamera->rot = CAMERA_1P_ROT;		// カメラの角度
			pCamera->fDist = CAMERA_1P_DISTANS;							// 視点と注視点の距離
			break;
		}

		// 画面設定等
		pCamera->viewport.X = SCREEN_WIDTH * 0.5f * nCntCamera;			// 画面左上 X 座標
		pCamera->viewport.Y = 0.0f;										// 画面左上 Y 座標
		pCamera->viewport.Width = SCREEN_WIDTH * 0.5f;					// 表示画面の横幅
		pCamera->viewport.Height = SCREEN_HEIGHT;						// 表示画面の高さ

		pCamera->viewport.MinZ = 0.0f;									
		pCamera->viewport.MaxZ = 1.0f;									
		pCamera->nCntAoutRot = 0;										// 自動で回り込み ONにするまでのカウンタ
		pCamera->bAoutRot = false;										//		〃		  OFF
		pCamera->bUse = true;
	}

	// 画面分割設定
	SetCameraOption();
}

//=========================================================================================
// カメラ設定
void SetCameraOption(void)
{
	//**************************************************************
	// 変数宣言
	P_CAMERA pCamera = GetCamera();

	g_nNumEnableCamera = GetNumPlayer();	// プレイヤー数

	for (int nCntCamera = 0; nCntCamera < MAX_CAMERA; nCntCamera++, pCamera++)
	{		
		if (g_nNumEnableCamera < 2)
		{// 1P
			pCamera->viewport.X = 0.0f;								// 画面左上 X 座標
			pCamera->viewport.Y = 0.0f;								// 画面左上 Y 座標
			pCamera->viewport.Width = SCREEN_WIDTH;					// 表示画面の横幅
			pCamera->viewport.Height = SCREEN_HEIGHT;				// 表示画面の高さ
		}
		else
		{// 2P
			pCamera->viewport.X = SCREEN_WIDTH * 0.5f * nCntCamera;	// 画面左上 X 座標
			pCamera->viewport.Y = 0.0f;								// 画面左上 Y 座標
			pCamera->viewport.Width = SCREEN_WIDTH * 0.5f;			// 表示画面の横幅
			pCamera->viewport.Height = SCREEN_HEIGHT;				// 表示画面の高さ
		}
	}
}

//=========================================================================================
// カメラ終了
//=========================================================================================
void UninitCamera(void)
{

}

//=========================================================================================
// カメラ更新
//=========================================================================================
void UpdateCamera(void)
{
	//**************************************************************
	// 変数宣言
	P_CAMERA pCamera = GetCamera();
	int			nActivePlayer = GetActivePlayer();

	// プレイヤー数が変わったら
	if (g_nNumEnableCamera != GetNumPlayer())
	{
		SetCameraOption();
	}

	// 操作キャラが変わったら
	if (g_nActivePlayer != nActivePlayer)
	{
		g_aCamera[nActivePlayer].posR = g_aCamera[g_nActivePlayer].posR;
		g_aCamera[nActivePlayer].posV = g_aCamera[g_nActivePlayer].posV;
		g_aCamera[nActivePlayer].rot = g_aCamera[g_nActivePlayer].rot;
		g_aCamera[nActivePlayer].fDist = g_aCamera[g_nActivePlayer].fDist;

		g_nActivePlayer = nActivePlayer;
	}

		if (GetKeyboardPress(CAM_2POPRAT))
			CameraOrbit(pCamera + 1);
		else
			CameraOrbit(pCamera);


	CameraFollow();

	//**************************************************************
	// 注視点から視点を求める
	pCamera = GetCamera();
	for (int nCntCamera = 0; nCntCamera < MAX_CAMERA; nCntCamera++, pCamera++)
	{
		if (pCamera->bUse)
		{
			pCamera->posV.x = pCamera->posR.x - sinf(D3DX_PI - pCamera->rot.y) * pCamera->fDist;
			pCamera->posV.y = pCamera->posR.y - cosf(D3DX_PI - pCamera->rot.x) * pCamera->fDist;
			pCamera->posV.z = pCamera->posR.z + cosf(D3DX_PI - pCamera->rot.y) * pCamera->fDist;

			PrintDebugProc("CAMERA %d\nposV : %~3f\nposR : %~3f\n", nCntCamera, pCamera->posV.x, pCamera->posV.y, pCamera->posV.z, pCamera->posR.x, pCamera->posR.y, pCamera->posR.z);
			PrintDebugProc("rot  : %~3f\n", pCamera->rot.x, pCamera->rot.y, pCamera->rot.z);
		}
	}
}

//==============================================================
//	プレイヤーに追従
void CameraFollow(void)
{
	//if (GetPause() == false)
	//{
		//**************************************************************
		// 変数宣言
		P_CAMERA pCamera = GetCamera();				// カメラ情報
		Player* pPlayer = GetPlayer();				// プレイヤー情報
		float fPlayerFront;							// プレイヤーより前
		static float fPlayerMoveRot;

		for (int nPlayer = 0; nPlayer < MAX_PLAYER; nPlayer++, pCamera++, pPlayer++)
		{
			//**************************************************************
			// プレイヤーに追従
			
			if (CAMERA_PLFR_DEADZONE < pPlayer->move.x * pPlayer->move.x + pPlayer->move.z * pPlayer->move.z)
			{// カメラを少し先へ
				fPlayerFront = pCamera->fDist * 0.25f;
				fPlayerMoveRot = atan2f(-pPlayer->move.x, -pPlayer->move.z);

				pCamera->posRDest.x = pPlayer->pos.x - fPlayerFront * sinf(fPlayerMoveRot);
				pCamera->posRDest.y = pPlayer->pos.y;
				pCamera->posRDest.z = pPlayer->pos.z - fPlayerFront * cosf(fPlayerMoveRot);
			}
#if 0
			}
			else
			{
				//**************************************************************
				// 変数宣言
				vec3 pos = GetAnchor();
				g_camera.posRDest = pos;
			}
#endif
			//**************************************************************
			// キャラクターに応じて角度を変更
			switch (nPlayer)
			{
			case PLAYER_ONE:
				pCamera->rot += (CAMERA_1P_ROT - pCamera->rot) * 0.25f;
				pCamera->fDist += (CAMERA_1P_DISTANS - pCamera->fDist) * 0.25f;
				break;

			case PLAYER_TWO:
				pCamera->rot += (CAMERA_2P_ROT - pCamera->rot) * 0.25f;
				pCamera->fDist += (CAMERA_2P_DISTANS - pCamera->fDist) * 0.25f;
				break;

			default:
				break;
			}


			//**************************************************************
			// カメラの位置を補正
			pCamera->posR.x += (pCamera->posRDest.x - pCamera->posR.x) * CAMERA_FOLLOW_FACTOR;
			pCamera->posR.y += (pCamera->posRDest.y - pCamera->posR.y) * CAMERA_FOLLOW_FACTOR;
			pCamera->posR.z += (pCamera->posRDest.z - pCamera->posR.z) * CAMERA_FOLLOW_FACTOR;
		}
}

//==============================================================
// カメラ移動（ポーズ中のみ
void CameraMove(P_CAMERA pCamera)
{
	//**************************************************************
	//移動
	if (GetKeyboardRepeat(CAM_MOVE_UP) == true)
	{// 前
		  pCamera->posR.z += -sinf(-pCamera->rot.y) * 5;
		  pCamera->posR.x += -cosf(-pCamera->rot.y) * 5;
	}
	if (GetKeyboardRepeat(CAM_MOVE_DW) == true)
	{// 手前
		pCamera->posR.z += sinf(-pCamera->rot.y) * 5;
		pCamera->posR.x += cosf(-pCamera->rot.y) * 5;
	}
	if (GetKeyboardRepeat(CAM_MOVE_L) == true)
	{// 左
		 pCamera->posR.z += -sinf(D3DX_PI * 0.5f - pCamera->rot.y) * 5;
		 pCamera->posR.x += -cosf(D3DX_PI * 0.5f - pCamera->rot.y) * 5;
	}
	if (GetKeyboardRepeat(CAM_MOVE_R) == true)
	{// 右
		 pCamera->posR.z += sinf(D3DX_PI * 0.5f - pCamera->rot.y) * 5;
		 pCamera->posR.x += cosf(D3DX_PI * 0.5f - pCamera->rot.y) * 5;
	}
}

//==============================================================
// 自動回転
void CameraRotation(P_CAMERA pCamera)
{
	//**************************************************************
	// 変数宣言
	bool bUse = false;
	Player* pPlayer = GetPlayer();
	float fRotMove =  pPlayer->rot.y - pCamera->rot.y - D3DX_PI * 0.5f;	// 変化させる角度(目的値と現在値の差)

	//**************************************************************
	// 回転方向正規化
	if (0 < fRotMove)
	{
		if (D3DX_PI < fRotMove)
		{
			fRotMove -= D3DX_PI * 2;
		}
	}
	else if (fRotMove < 0)
	{
		if (fRotMove < -D3DX_PI)
		{
			fRotMove += D3DX_PI * 2;
		}
	}
	// 補正しカメラを回転
	pCamera->rot.y += fRotMove * CAMERA_ROTET_FACTOR;

	//**************************************************************
	// 異常な角度(X_PIを超える)値を修正
	if (pCamera->rot.y < -D3DX_PI)
		pCamera->rot.y += D3DX_PI * 2;
	else if (pCamera->rot.y > D3DX_PI)
		pCamera->rot.y += -D3DX_PI * 2;
}

//==============================================================
// 注視点を中心に手動で回転
void CameraOrbit(P_CAMERA pCamera)
{
	//**************************************************************
	// 変数宣言
	bool bUse = false;
	vec3 rightStick;

	//**************************************************************
	// 注視点のまわりを回転
	// キーボード操作
	if (GetKeyboardPress(CAM_ORBIT_R) && GetKeyboardPress(DIK_M) != true)
	{
		pCamera->rot.y -= CAMERA_SPIN;
		bUse = true;
	}
	if (GetKeyboardPress(CAM_ORBIT_L))
	{
		pCamera->rot.y += CAMERA_SPIN;
		bUse = true;
	}
	if (GetKeyboardPress(CAM_ORBIT_UP))
	{
		pCamera->rot.x -= CAMERA_SPIN;
		pCamera->posV.x = pCamera->posR.x - cosf(D3DX_PI - pCamera->rot.y) * pCamera->fDist;
		pCamera->posV.y = pCamera->posR.y - cosf(D3DX_PI - pCamera->rot.x) * pCamera->fDist;
		pCamera->posV.z = pCamera->posR.z + sinf(D3DX_PI - pCamera->rot.x) * pCamera->fDist;

	}
	if (GetKeyboardPress(CAM_ORBIT_DW))
	{
		pCamera->rot.x += CAMERA_SPIN;
		pCamera->posV.x = pCamera->posR.x - cosf(D3DX_PI - pCamera->rot.y) * pCamera->fDist;
		pCamera->posV.y = pCamera->posR.y - cosf(D3DX_PI - pCamera->rot.x) * pCamera->fDist;
		pCamera->posV.z = pCamera->posR.z + sinf(D3DX_PI - pCamera->rot.x) * pCamera->fDist;
	}

	// コントローラー操作
	//if (GetJoypadRightStick(&rightStick))
	//{
	//	pCamera->rot.y += rightStick.x * REV_PLAYER;
	//	bUse = true;
	//}


	//**************************************************************
	// -πからπまでにする	
	if (pCamera->rot.x < -D3DX_PI)
		pCamera->rot.x = D3DX_PI;
	else if (D3DX_PI < pCamera->rot.x)
		pCamera->rot.x = -D3DX_PI;

	if (pCamera->rot.y < -D3DX_PI)
		pCamera->rot.y = D3DX_PI;
	else if (D3DX_PI < pCamera->rot.y)
		pCamera->rot.y = -D3DX_PI;

	if (pCamera->rot.z < -D3DX_PI)
		pCamera->rot.z = D3DX_PI;
	else if (D3DX_PI < pCamera->rot.z)
		pCamera->rot.z = -D3DX_PI;

	//**************************************************************
	// 視点から注視点を求める
	if (bUse)
	{
		pCamera->posV.x = pCamera->posR.x - cosf(D3DX_PI - pCamera->rot.y) * pCamera->fDist;
		pCamera->posV.y = pCamera->posR.y - cosf(D3DX_PI - pCamera->rot.x) * pCamera->fDist;
		pCamera->posV.z = pCamera->posR.z - sinf(D3DX_PI - pCamera->rot.y) * pCamera->fDist;
	}
}

//=========================================================================================
// カメラ設置
//=========================================================================================
void SetCamera(void)
{
	//**************************************************************
	// 変数宣言
	LPDIRECT3DDEVICE9	pDevice = GetDevice();		// デバイスへのポインタ
	P_CAMERA			pCam = GetCamera();
	static bool bCameraSwitch = false;				// 複数画面描画時カメラの切り替え

	for (int nCntCamera = 0; nCntCamera < MAX_CAMERA; nCntCamera++, pCam++)
	{
		if (g_nNumEnableCamera == 1)
		{
			if (g_nActivePlayer == PLAYERTYPE_MOUSE)
			{
				pCam++;
			}
		}
		else if (bCameraSwitch == true)
		{// 2P用カメラ設置
			pCam++;
			bCameraSwitch = false;
		}
		else
		{// 1P用カメラ設置
			bCameraSwitch = true;
		}

		if (pCam->bUse)
		{
			//**************************************************************
			// ビューポートの設定
			pDevice->SetViewport(&pCam->viewport);

			//**************************************************************
			// プロジェクションマトリックスの初期化
			D3DXMatrixIdentity(&pCam->mtxProjection);

			// プロジェクションマトリックスを作成
			D3DXMatrixPerspectiveFovLH(&pCam->mtxProjection,
				D3DXToRadian(VIEW_RADIAN),					// 視野角
				(float)pCam->viewport.Width / (float)pCam->viewport.Height,	// アスペクト比
				VIEW_MINDEPTH,								// 最短描画距離
				VIEW_MAXDEPTH);								// 最大描画距離

			// プロジェクションマトリックスを設定
			pDevice->SetTransform(D3DTS_PROJECTION, &pCam->mtxProjection);

			//**************************************************************
			// ビューマトリックスの初期化
			D3DXMatrixIdentity(&pCam->mtxView);

			// ビューマトリックスの作成
			D3DXMatrixLookAtLH(&pCam->mtxView, &pCam->posV, &pCam->posR, &pCam->vecU);

			// ビューマトリックスの設定
			pDevice->SetTransform(D3DTS_VIEW, &pCam->mtxView);

			EndDevice();// デバイス取得終了

			return;
		}
	}
	EndDevice();// デバイス取得終了
}

//=========================================================================================
// カメラの情報を取得
//=========================================================================================
P_CAMERA GetCamera(void)
{
	return &g_aCamera[0];
}

//=========================================================================================
// カメラの位置情報を取得
//=========================================================================================
void GetCameraPos(int nCamNum, vec3* pPosV, vec3* pPosR)
{
	//**************************************************************
	// 変数宣言
	P_CAMERA pCam;
	vec3 posV = D3DXVECTOR3_NULL, posR = D3DXVECTOR3_NULL;

	// カメラ数が範囲内なら
	if (0 <= nCamNum && nCamNum < MAX_CAMERA)
	{
		pCam = &g_aCamera[nCamNum];

		if (pCam->bUse)
		{
			posV = pCam->posV;
			posR = pCam->posR;
		}
	}
	
	*pPosV = posV;
	*pPosR = posR;
}

//=========================================================================================
// カメラの角度情報を取得
//=========================================================================================
void GetCameraRot(int nCamNum, vec3* pRot)
{
	//**************************************************************
	// 変数宣言
	P_CAMERA pCam;
	vec3 rot = D3DXVECTOR3_NULL;

	// カメラ数が範囲内なら
	if (0 <= nCamNum && nCamNum < MAX_CAMERA)
	{
		pCam = &g_aCamera[nCamNum];

		if (pCam->bUse)
		{
			rot = pCam->rot;
		}
	}

	*pRot = rot;
}

//=========================================================================================
// 使用中のカメラの数を取得
//=========================================================================================
int GetCameraNum(void)
{
	return g_nNumEnableCamera;
}

//=========================================================================================
// カメラリセット
//=========================================================================================
void CameraReset(P_CAMERA pCamera)
{
	//**************************************************************
	// 注視点
	pCamera->posR = GetPlayer()->pos;

	//**************************************************************
	// 注視点から視点を求める
	pCamera->fDist = CAMERA_1P_DISTANS;										// 視点と注視点の距離
	pCamera->rot = vec3(D3DX_PI * 0.2f, D3DX_PI * 0.5f, 0.0f);				// カメラの角度
	pCamera->posV.x = pCamera->posR.x - cosf(D3DX_PI - pCamera->rot.y) * pCamera->fDist;
	pCamera->posV.y = pCamera->posR.y - cosf(D3DX_PI - pCamera->rot.x) * pCamera->fDist;
	pCamera->posV.z = pCamera->posR.z - sinf(D3DX_PI - pCamera->rot.y) * pCamera->fDist;
}