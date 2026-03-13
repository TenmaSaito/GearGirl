//==================================================================================================================================
//
//			プレイヤーカメラ処理 [camera.cpp]
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
Camera	g_aCamera[MAX_CAMERA];			// カメラ情報
int		g_nNumEnableCamera;				// 起動させるカメラの数
int		g_nActivePlayer;				// １P時の画面のキャラクター
int		g_nSetCameraPosCounter;			// カメラ切り替え後位置角度を修正するカウンター
bool	g_bCameraDebug;					// カメラの情報
CameraType	g_readyCamera;				// 直近でセットしたカメラ

//**************************************************************
// プロトタイプ宣言
void SetCameraOption(void);				// カメラ設定
void NearCameraIntegration(void);		// 2P時近ければカメラ統合
void CameraChange(void);				// カメラを変更
void CameraFollow(void);				// プレイヤーに追従移動
void CameraRotation(P_CAMERA pCamera);	// プレイヤーと同じ向きに回転
void CameraOrbit(void);					// カメラ回転処理
void CameraMove(void);					// カメラ移動（ポーズ中のみ

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
		pCamera->bUse = true;
		pCamera->posV = vec3 CAMERA_V_DEFPOS;					// 視点
		pCamera->posR = PLAYER_POSDEF;							// 注視点
		pCamera->posRDest = PLAYER_POSDEF;						// 目的の注視点
		pCamera->vecU = vec3(0.0f, 1.0f, 0.0f);					// 上方向ベクトル

		switch (nCntCamera)
		{
		case CAMERATYPE_PLAYER_ONE:	// 1P用のカメラ設定
			pCamera->rot = CAMERA_1P_ROT;						// カメラの角度
			pCamera->fDist = CAMERA_1P_DISTANS;					// 視点と注視点の距離
			pCamera->fViewMin = VIEW_1P_MINDEPTH;				// 最小描画距離
			pCamera->fViewMax = VIEW_1P_MAXDEPTH;				// 最大描画距離
			pCamera->type = CAMERATYPE_PLAYER_ONE;
			break;

		case CAMERATYPE_PLAYER_TWO:	// 2P用のカメラ設定
			pCamera->rot = CAMERA_2P_ROT;
			pCamera->fDist = CAMERA_2P_DISTANS;
			pCamera->fViewMin = VIEW_2P_MINDEPTH;				// 最小描画距離
			pCamera->fViewMax = VIEW_2P_MAXDEPTH;				// 最大描画距離
			pCamera->type = CAMERATYPE_PLAYER_TWO;
			break;
		default:// その他のカメラ設定
			pCamera->rot = CAMERA_1P_ROT;						// カメラの角度
			pCamera->fDist = CAMERA_1P_DISTANS;					// 視点と注視点の距離
			pCamera->bUse = false;
			break;
		}

		pCamera->viewport.X = 0.0f;								// 画面左上 X 座標
		pCamera->viewport.Y = 0.0f;								// 画面左上 Y 座標
		pCamera->viewport.Width = SCREEN_WIDTH;					// 表示画面の横幅
		pCamera->viewport.Height = SCREEN_HEIGHT;				// 表示画面の高さ
		pCamera->fViewRadian = VIEW_RADIAN;						// 視野角

		// 画面設定等
		pCamera->viewport.MinZ = 0.0f;									
		pCamera->viewport.MaxZ = 1.0f;

		pCamera->nCntAoutRot = 0;								// 自動で回り込み ONにするまでのカウンタ
		pCamera->bAoutRot = false;								//		〃		  OFF
		pCamera->bFocusMode = false;							// フォーカス	　OFF
	}

	// 画面分割設定
	SetCameraOption();

	FogEnable(CAMERATYPE_PLAYER_TWO, true, colX_ZERO, 0.5f, 3000.0f);
}

//=========================================================================================
// カメラ設定
void SetCameraOption(void)
{
	//**************************************************************
	// 変数宣言
	P_CAMERA pCamera = GetCamera();

	g_nNumEnableCamera = GetNumPlayer();	// プレイヤー数

	for (int nCntCamera = 0; nCntCamera <= CAMERATYPE_PLAYER_TWO; nCntCamera++, pCamera++)
	{
		if (g_nNumEnableCamera < 2)
		{// 1P プレイ時
			pCamera->viewport.X = 0.0f;								// 画面左上 X 座標
			pCamera->viewport.Y = 0.0f;								// 画面左上 Y 座標
			pCamera->viewport.Width = SCREEN_WIDTH;					// 表示画面の横幅
			pCamera->viewport.Height = SCREEN_HEIGHT;				// 表示画面の高さ
			pCamera->fViewRadian = VIEW_RADIAN;						// 視野角
		}
		else
		{// 2P プレイ時
			pCamera->viewport.X = SCREEN_WIDTH * 0.5f * nCntCamera;	// 画面左上 X 座標
			pCamera->viewport.Y = 0.0f;								// 画面左上 Y 座標
			pCamera->viewport.Width = SCREEN_WIDTH * 0.5f;			// 表示画面の横幅
			pCamera->viewport.Height = SCREEN_HEIGHT;				// 表示画面の高さ
			pCamera->fViewRadian = VIEW_RADIAN;						// 視野角
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
	PrintDebugProc("\nCAMERAデバッグ表示切り替え：[F2] : ");
	if (GetKeyboardTrigger(DIK_F2))
	{
		g_bCameraDebug = g_bCameraDebug ^ 1;
	}

	if(g_bCameraDebug)
		PrintDebugProc("ON\n");
	else
		PrintDebugProc("OFF\n");
	
	//**************************************************************
	// 変数宣言
	P_CAMERA	pCamera;
	int			nActivePlayer = GetActivePlayer();

	// プレイヤー数が変わったら
	if (g_nNumEnableCamera != GetNumPlayer())
	{
		SetCameraOption();				// 各カメラの設定
	}

	//**************************************************************
	// 画面	
	if (GetNumPlayer() == 2)
	{// ２人プレイ時
		NearCameraIntegration();		// 近ければ結合
	}
	else if (g_nActivePlayer != nActivePlayer)
	{// 操作キャラが変わったら
		CameraChange();		// 切り替え
	}

	//**************************************************************
	// カメラ動かす処理
	pCamera = GetCamera();

	// シングルプレイ && 少女アクティブ
	if (GetNumPlayer() == 1 && GetActivePlayer() == PLAYERTYPE_GIRL)
	{
		// ダッシュ中
		if (GetPlayer()->bDash)
		{
			if (pCamera->fViewRadian <= VIEW_RADIAN * 1.5f)
				pCamera->fViewRadian += 1.0f;		// 視野角広げ
		}
		else if (VIEW_RADIAN < pCamera->fViewRadian)
		{// ダッシュ解除後
			pCamera->fViewRadian -= 1.0f;			// 視野角狭め
		}
	}
	
	// 共有部
	CameraOrbit();					// カメラ回転
	CameraFollow();					// 追従
	CameraReset();					// カメラリセット

	//**************************************************************
	// 注視点から視点を求める
	pCamera = GetCamera();
	Player* pPl = GetPlayer();
	for (int nCntCamera = 0; nCntCamera < MAX_CAMERA; nCntCamera++, pCamera++)
	{
		if (pCamera->bUse)
		{
			pCamera->posV.x = pCamera->posR.x - sinf(pCamera->rot.y) * sinf(pCamera->rot.x) * pCamera->fDist;
			pCamera->posV.y = pCamera->posR.y - cosf(pCamera->rot.x) * pCamera->fDist;
			pCamera->posV.z = pCamera->posR.z - cosf(pCamera->rot.y) * sinf(pCamera->rot.x) * pCamera->fDist;

			if (g_bCameraDebug)
			{
				PrintDebugProc("\nCAMERA %d\nposV : %d %d %d\nposR : %d %d %d\n", nCntCamera, (int)pCamera->posV.x, (int)pCamera->posV.y, (int)pCamera->posV.z, (int)pCamera->posR.x, (int)pCamera->posR.y, (int)pCamera->posR.z);
				PrintDebugProc("rot  : %~3f\ndist ： %d\n", pCamera->rot.x, pCamera->rot.y, pCamera->rot.z, (int)pCamera->fDist);
				PrintDebugProc("視野角: %d\n", (int)pCamera->fViewRadian);

				PrintDebugProc("Player%d pos   :%~3f\n", nCntCamera, pPl->pos.x, pPl->pos.y, pPl->pos.z);
				PrintDebugProc("Player%d posOld:%~3f\n", nCntCamera, pPl->posOld.x, pPl->posOld.y, pPl->posOld.z);
				pPl++;
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
		for (int nCntCamera = 0; nCntCamera < MAX_CAMERA; nCntCamera++, pCamera++)
		{
			// 画面を統合
			pCamera->viewport.X = 0.0f;								// 画面左上 X 座標
			pCamera->viewport.Y = 0.0f;								// 画面左上 Y 座標
			pCamera->viewport.Width = SCREEN_WIDTH;					// 表示画面の横幅
			pCamera->viewport.Height = SCREEN_HEIGHT;				// 表示画面の高さ
		}
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
	vec3  stick = vec3_ZERO;

	for (int nPlayer = 0; nPlayer < MAX_PLAYER; nPlayer++, pCamera++, pPlayer++)
	{
		//**************************************************************
		// プレイヤーに追従
		if (pCamera->bFocusMode == false)
		{
			if (CAMERA_PLFR_DEADZONE < SQUARE(pPlayer->move.x) + SQUARE(pPlayer->move.z)
				|| (CAMERA_PLFR_DEADZONE < SQUARE(pPlayer->pos.x - pPlayer->posOld.x) + SQUARE(pPlayer->pos.z - pPlayer->posOld.z)))
			{// カメラを少し先へ
				GetJoypadLeftStick(nPlayer, &stick);
				fPlayerFront = pCamera->fDist * 0.2f;
				fPlayerMoveRot = atan2f(-pPlayer->move.x, -pPlayer->move.z);

				pCamera->posRDest.x = pPlayer->pos.x - fPlayerFront * sinf(fPlayerMoveRot);
				pCamera->posRDest.z = pPlayer->pos.z - fPlayerFront * cosf(fPlayerMoveRot);
			}

			//**************************************************************
			// キャラクターに応じて調整
			switch (nPlayer)
			{
			case CAMERATYPE_PLAYER_ONE:
				pCamera->posRDest.y = pPlayer->pos.y + 15.0f; // 頭の高さに追従

				// カメラ切り替え時の処理
				if (0 < g_nSetCameraPosCounter)
				{
					pCamera->rot.x += (CAMERA_1P_ROT.x - pCamera->rot.x) * g_nSetCameraPosCounter / SETCAMERAPOS_COUNTER;	// 向きを戻す
					pCamera->fDist += (CAMERA_1P_DISTANS - pCamera->fDist) * g_nSetCameraPosCounter / SETCAMERAPOS_COUNTER;	// 距離を戻す
					g_nSetCameraPosCounter--;
				}
				break;

			case CAMERATYPE_PLAYER_TWO:
				pCamera->posRDest.y = pPlayer->pos.y + 2.0f;			// 頭の高さに追従
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
}

//==============================================================
// カメラ移動（ポーズ中のみ
void CameraMove(void)
{
#if 0
	//**************************************************************
	// 変数宣言
	P_CAMERA pCamera = GetCamera();				// カメラ情報

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
#endif
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
// 注視点を中心に回転
void CameraOrbit(void)
{
	//**************************************************************
	// 変数宣言
	P_CAMERA pCamera = GetCamera();				// カメラ情報
	vec3 rightStick;

	for (int nCntPlayer = 0; nCntPlayer < GetNumPlayer(); nCntPlayer++, pCamera++)
	{
		// 1人プレイ時
		if (GetNumPlayer() == 1)
		{
			if (GetActivePlayer() == PLAYERTYPE_MOUSE)
			{
				pCamera++;
			}
		}

		//**************************************************************
		// 注視点のまわりを回転
		// キーボード操作
		if (GetKeyboardPress(CAM_ORBIT_R(nCntPlayer)))
		{
			pCamera->rot.y -= CAMERA_SPIN_X;
		}
		if (GetKeyboardPress(CAM_ORBIT_L(nCntPlayer)))
		{
			pCamera->rot.y += CAMERA_SPIN_X;
		}

		if (GetKeyboardPress(CAM_ORBIT_UP(nCntPlayer)))
		{
			pCamera->rot.x -= CAMERA_SPIN_Y;
		}
		if (GetKeyboardPress(CAM_ORBIT_DW(nCntPlayer)))
		{
			pCamera->rot.x += CAMERA_SPIN_Y;
		}

		// コントローラー操作
		if (GetJoypadRightStick(GetNumPlayer() == 1 ? 0 : (int)pCamera->type, &rightStick))
		{// 1人プレイなら常に一つ目のコントローラー情報を取得。そうじゃなければカメラのタイプ（対応プレイヤーナンバー）のコントローラー情報を取得
			pCamera->rot.y += rightStick.x * CAMERA_SPIN_X;
			pCamera->rot.x += rightStick.y * CAMERA_SPIN_Y;
		}

		//**************************************************************
		// 角度正規化
		if (pCamera->rot.x < CAMERA_LOWLIM)
			pCamera->rot.x = CAMERA_LOWLIM;
		else if (CAMERA_UPLIM < pCamera->rot.x)
			pCamera->rot.x = CAMERA_UPLIM;

		if (pCamera->rot.y < -D3DX_PI)
			pCamera->rot.y += D3DX_PI * 2;
		else if (D3DX_PI < pCamera->rot.y)
			pCamera->rot.y -= D3DX_PI * 2;

		if (pCamera->rot.z < -D3DX_PI)
			pCamera->rot.z += D3DX_PI * 2;
		else if (D3DX_PI < pCamera->rot.z)
			pCamera->rot.z -= D3DX_PI * 2;
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

	for (int nCntCamera = 0; nCntCamera < MAX_CAMERA; nCntCamera++, pCam++)
	{
		// 設置するカメラが一つなら
		if (g_nNumEnableCamera == 1)
		{
			if (GetNumPlayer() == 1)
			{
				// アクティブプレイヤーがネズミなら
				if (g_nActivePlayer == PLAYERTYPE_MOUSE && nCntCamera != PLAYERTYPE_MOUSE)
				{
					continue;
				}
			}
		}
		// 二つ設置する場合
		else if (bCameraSwitch == true && nCntCamera != PLAYERTYPE_MOUSE)
		{// 2P用カメラ設置
			bCameraSwitch = false;
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
				D3DXToRadian(pCam->fViewRadian),					// 視野角
				(float)pCam->viewport.Width / (float)pCam->viewport.Height,	// アスペクト比
				pCam->fViewMin,								// 最短描画距離
				pCam->fViewMax);							// 最大描画距離
			// プロジェクションマトリックスを設定
			pDevice->SetTransform(D3DTS_PROJECTION, &pCam->mtxProjection);

			//**************************************************************
			// ビューマトリックスの初期化
			D3DXMatrixIdentity(&pCam->mtxView);

			// ビューマトリックスの作成
			D3DXMatrixLookAtLH(&pCam->mtxView, &pCam->posV, &pCam->posR, &pCam->vecU);

			// ビューマトリックスの設定
			pDevice->SetTransform(D3DTS_VIEW, &pCam->mtxView);

			//**************************************************************
			// フォグの処理
			if (pCam->bFog)
			{
				pDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
				pDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
				pDevice->SetRenderState(D3DRS_FOGCOLOR, pCam->FogColor);
				pDevice->SetRenderState(D3DRS_FOGSTART,*(DWORD *)(&pCam->fStart));
				pDevice->SetRenderState(D3DRS_FOGEND,*(DWORD *)(&pCam->fEnd));
			}

			g_readyCamera = (CameraType)nCntCamera;	// セットしたカメラの番号を保存
			break;
		}
	}

	EndDevice();// デバイス取得終了
}

//=========================================================================================
// UIカメラ設置
//=========================================================================================
void SetUICamera(vec3 viewTopLeft, vec2 size)
{
	LPDIRECT3DDEVICE9	pDevice = GetDevice();		// デバイスへのポインタ
	D3DXMATRIX mtxProjection,mtxView;
	D3DVIEWPORT9 viewport;
		viewport.X = viewTopLeft.x;
		viewport.Y = viewTopLeft.y;
		viewport.Width = size.x;
		viewport.Height = size.y;
		viewport.MinZ = 0.0f;
		viewport.MaxZ = 1.0f;

	vec3 vecU = vec3(0.0f, 1.0f, 0.0f),
		posV = UICAMERA_POSV,
		posR = UICAMERA_POSR;

	float fViewRadian = 45.0f,
		fViewMin = 10.0f,
		fViewMax = 1000.0f;

	//**************************************************************
	// ビューポートの設定
	pDevice->SetViewport(&viewport);

	//**************************************************************
	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&mtxProjection);

	// プロジェクションマトリックスを作成
	D3DXMatrixPerspectiveFovLH(&mtxProjection,
		D3DXToRadian(fViewRadian),						// 視野角
		(float)viewport.Width / (float)viewport.Height,	// アスペクト比
		fViewMin,										// 最短描画距離
		fViewMax);										// 最大描画距離

	// プロジェクションマトリックスを設定
	pDevice->SetTransform(D3DTS_PROJECTION, &mtxProjection);

	//**************************************************************
	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&mtxView);

	// ビューマトリックスの作成
	D3DXMatrixLookAtLH(&mtxView, &posV, &posR, &vecU);

	// ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &mtxView);

	//**************************************************************
	// 画面クリア(バックバッファとZバッファのクリア)
	pDevice->Clear(0, NULL,
		(D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

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
void CameraReset(void)
{
	//**************************************************************
	// 変数宣言
	P_CAMERA pCamera = GetCamera();				// カメラ情報
	Player* pPlayer = GetPlayer();
	float	fPlayerRot;

	for (int nCntCamera = 0; nCntCamera < PLAYERTYPE_MAX; nCntCamera++, pCamera++,pPlayer++)
	{
		if (GetJoypadTrigger(nCntCamera, CAM_RESETJOY) || GetKeyboardTrigger(CAM_RESETKEY(nCntCamera)))
		{
			if (GetNumPlayer() == 1 && GetActivePlayer() == PLAYERTYPE_MOUSE)
			{// 1人プレイでネズミがアクティブなら
				pCamera++;
				pPlayer++;
			}

			fPlayerRot = pPlayer->rot.y - D3DX_PI;
			pCamera->rot.y = fPlayerRot;
		}
	}
}

//=========================================================================================
// 霧を有効化
//=========================================================================================
void FogEnable(CameraType type, bool bEnable, D3DXCOLOR col, float fStart, float fEnd)
{
	//**************************************************************
	// 変数宣言
	P_CAMERA pCamera = GetCamera();

	//**************************************************************
	// 値の保存
	switch (type)
	{
	case CAMERATYPE_PLAYER_ONE:
		pCamera->bFog = bEnable;
		pCamera->FogColor = col;
		pCamera->fStart = fStart;
		pCamera->fEnd = fEnd;
		break;

	case CAMERATYPE_PLAYER_TWO:
		pCamera++;
		pCamera->bFog = bEnable;
		pCamera->FogColor = col;
		pCamera->fStart = fStart;
		pCamera->fEnd = fEnd;
		break;

	default:
		break;
	}
}

//=========================================================================================
// 霧を削除
//=========================================================================================
void CleanFog(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// Disable fog blending.
	pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	
	EndDevice();
}

//=========================================================================================
// フォーカスモードのオンオフ
//=========================================================================================
void Focus(CameraType type, bool bEnable)
{
	g_aCamera[type].bFocusMode = bEnable;
}

//=========================================================================================
// フォーカス位置設定
//=========================================================================================
void Focus(CameraType type, vec3 pos)
{
	P_CAMERA pCam = &g_aCamera[type];

	if (pCam->bFocusMode)
	{
		pCam->posR.x = pos.x;
		pCam->posR.y = pos.y;
		pCam->posR.z = pos.z;
	}
}