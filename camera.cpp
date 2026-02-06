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
int		g_nSetCameraPosCounter;			// カメラ切り替え後位置角度を修正するカウンター
bool	g_bCameraDebug;					// カメラの情報
CameraType	g_readyCamera;				// 直近でセットしたカメラ

//**************************************************************
// プロトタイプ宣言
void SetCameraOption(void);					// カメラ設定
void NearCameraIntegration(void);			// 2P時近ければカメラ統合
void CameraChange(void);					// カメラを変更
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
	g_nActivePlayer = 0;					// 画面のプレイヤー
	g_bCameraDebug = false;
	g_nSetCameraPosCounter = SETCAMERAPOS_COUNTER;

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
	PrintDebugProc("\nCAMERAデバッグ表示切り替え：[F2]");
	if (GetKeyboardTrigger(DIK_F2))
	{
		g_bCameraDebug = g_bCameraDebug ^ 1;
	}

	//**************************************************************
	// 変数宣言
	P_CAMERA	pCamera;
	int			nActivePlayer = GetActivePlayer();

#ifdef _DEBUG
	// プレイヤー数が変わったら
	if (g_nNumEnableCamera != GetNumPlayer())
	{
		SetCameraOption();
	}
#endif

	//**************************************************************
	// 画面
	// ２人プレイ時
	if (GetNumPlayer() == 2)
	{
		NearCameraIntegration();
	}
	// 操作キャラが変わったら
	else if (g_nActivePlayer != nActivePlayer)
	{
		CameraChange();
	}

	//**************************************************************
	// カメラ
	pCamera = GetCamera();
	if (GetActivePlayer() == PLAYER_ONE && GetKeyboardPress(CAM_2POPRAT) == false)
	{
		CameraOrbit(pCamera);		// 回転
		CameraMove(pCamera);		// カメラ距離の変更
	}
	else
	{
		CameraOrbit(pCamera + 1);
		CameraMove(pCamera + 1);
	}

	// 追従処理
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

			if (g_bCameraDebug)
			{
				PrintDebugProc("CAMERA %d\nposV : %~3f\nposR : %~3f\n", nCntCamera, pCamera->posV.x, pCamera->posV.y, pCamera->posV.z, pCamera->posR.x, pCamera->posR.y, pCamera->posR.z);
				PrintDebugProc("rot  : %~3f\ndist ： %f\n", pCamera->rot.x, pCamera->rot.y, pCamera->rot.z,pCamera->fDist);
			}
		}
	}		
}

//==============================================================
// 2Pプレイ時プレイヤーが近ければカメラを統合
void NearCameraIntegration(void)
{
	//**************************************************************
	// 変数宣言
	P_CAMERA	pCamera = GetCamera();

	Player*		pGirl = GetPlayer();
	Player*		pMouse = pGirl + 1;
	vec3		playerDist = pGirl->pos - pMouse->pos;

	// 一定距離近づいたら
	if (SQUARE(playerDist.x) + SQUARE(playerDist.y) + SQUARE(playerDist.z) < 1000)
	{
		// 画面を統合
		pCamera->viewport.X = 0.0f;								// 画面左上 X 座標
		pCamera->viewport.Y = 0.0f;								// 画面左上 Y 座標
		pCamera->viewport.Width = SCREEN_WIDTH;					// 表示画面の横幅
		pCamera->viewport.Height = SCREEN_HEIGHT;				// 表示画面の高さ
		g_nNumEnableCamera = 1;
	}
	else // 普段は分離
	{
		g_nNumEnableCamera = GetNumPlayer();

		for (int nCntCamera = 0; nCntCamera < MAX_CAMERA; nCntCamera++, pCamera++)
		{
			pCamera->viewport.X = SCREEN_WIDTH * 0.5f * nCntCamera;	// 画面左上 X 座標
			pCamera->viewport.Y = 0.0f;								// 画面左上 Y 座標
			pCamera->viewport.Width = SCREEN_WIDTH * 0.5f;			// 表示画面の横幅
			pCamera->viewport.Height = SCREEN_HEIGHT;				// 表示画面の高さ
		}
	}

}

//==============================================================
// カメラ変更
void CameraChange(void)
{
	//**************************************************************
	// 変数宣言
	int			nActivePlayer = GetActivePlayer();

	g_aCamera[nActivePlayer].posR = g_aCamera[g_nActivePlayer].posR;
	g_aCamera[nActivePlayer].posV = g_aCamera[g_nActivePlayer].posV;
	g_aCamera[nActivePlayer].rot = g_aCamera[g_nActivePlayer].rot;
	g_aCamera[nActivePlayer].fDist = g_aCamera[g_nActivePlayer].fDist;

	g_nActivePlayer = nActivePlayer;
	g_nSetCameraPosCounter = SETCAMERAPOS_COUNTER;			// このフレーム数以内にカメラが戻る
}

//==============================================================
//	プレイヤーに追従
void CameraFollow(void)
{
	//**************************************************************
	// 変数宣言
	P_CAMERA pCamera = GetCamera();				// カメラ情報
	Player* pPlayer = GetPlayer();				// プレイヤー情報
	float fPlayerFront;							// プレイヤーより前
	float fPlayerMoveRot;						// プレイヤーが移動している方向
	float fCameraFactor = CAMERA_FOLLOW_FACTOR;	// カメラ追従速度倍率

	for (int nPlayer = 0; nPlayer < MAX_PLAYER; nPlayer++, pCamera++, pPlayer++)
	{
		//**************************************************************
		// プレイヤーに追従
		if (CAMERA_PLFR_DEADZONE < SQUARE(pPlayer->move.x) + SQUARE(pPlayer->move.z))
		{// カメラを少し先へ
			fPlayerFront = pCamera->fDist * 0.25f;
			fPlayerMoveRot = atan2f(-pPlayer->move.x, -pPlayer->move.z);

			pCamera->posRDest.x = pPlayer->pos.x - fPlayerFront * sinf(fPlayerMoveRot);
			pCamera->posRDest.z = pPlayer->pos.z - fPlayerFront * cosf(fPlayerMoveRot);
		}

		//**************************************************************
		// キャラクターに応じて調整
		switch (nPlayer)
		{
		case PLAYER_ONE:
			pCamera->posRDest.y = pPlayer->pos.y + 10.0f; // 頭の高さに追従

			// カメラ切り替え時の処理
			if (0 < g_nSetCameraPosCounter)
			{
				pCamera->rot.x += (CAMERA_1P_ROT.x - pCamera->rot.x) * g_nSetCameraPosCounter / SETCAMERAPOS_COUNTER;	// 向きを戻す
				pCamera->fDist += (CAMERA_1P_DISTANS - pCamera->fDist) * g_nSetCameraPosCounter / SETCAMERAPOS_COUNTER;	// 距離を戻す
				g_nSetCameraPosCounter--;
			}
			break;

		case PLAYER_TWO:
			pCamera->posRDest.y = pPlayer->pos.y + 3.0f;	// 頭の高さに追従
			fCameraFactor += 0.1f;							// ネズミは追従速度を上げる

			// カメラ切り替え時の処理
			if (0 < g_nSetCameraPosCounter)
			{
				pCamera->rot.x += (CAMERA_2P_ROT.x - pCamera->rot.x) * g_nSetCameraPosCounter / SETCAMERAPOS_COUNTER;	 // 向きを戻す
				pCamera->fDist += (CAMERA_2P_DISTANS - pCamera->fDist) * g_nSetCameraPosCounter / SETCAMERAPOS_COUNTER;	 // 距離を戻す
				g_nSetCameraPosCounter--;
			}
			break;

		default:
			break;
		}

		//**************************************************************
		// カメラの位置を補正
		pCamera->posR.x += (pCamera->posRDest.x - pCamera->posR.x) * fCameraFactor;
		pCamera->posR.y += (pCamera->posRDest.y - pCamera->posR.y) * fCameraFactor;
		pCamera->posR.z += (pCamera->posRDest.z - pCamera->posR.z) * fCameraFactor;
	}
}

//==============================================================
// カメラ移動（ポーズ中のみ
void CameraMove(P_CAMERA pCamera)
{
#if 0
	//**************************************************************
	// 移動
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
#endif

	//**************************************************************
	// 距離の変更
	if (GetKeyboardPress(CAM_ZOOM_OUT))
	{
		pCamera->fDist += 1;
	}
	if (GetKeyboardPress(CAM_ZOOM_IN))
	{
		pCamera->fDist -= 1;
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
	static bool			bCameraSwitch = false;		// 複数カメラ設置時、1Pを描画したらtrue
	static float		fViewRadian = VIEW_RADIAN;	// 視野角

	for (int nCntCamera = 0; nCntCamera < MAX_CAMERA; nCntCamera++, pCam++)
	{
		// 設置するカメラが一つなら
		if (g_nNumEnableCamera == 1)
		{
			// アクティブプレイヤーがネズミなら
			if (g_nActivePlayer == PLAYERTYPE_MOUSE && nCntCamera != PLAYERTYPE_MOUSE)
			{				
				fViewRadian = VIEW_RADIAN_MOUSE;
				continue;
			}
		}
		// 二つ設置する場合・
		else if (bCameraSwitch == true && nCntCamera != PLAYERTYPE_MOUSE)
		{// 2P用カメラ設置
			bCameraSwitch = false;
			fViewRadian = VIEW_RADIAN_MOUSE;
			continue;
		}
		else if(nCntCamera == PLAYERTYPE_GIRL)
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
				D3DXToRadian(fViewRadian),					// 視野角
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
			g_readyCamera = CameraType(nCntCamera);
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
// 描画中（直近でセットした）カメラの番号
//=========================================================================================
CameraType GetReadyCamera(void)
{
	return g_readyCamera;
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