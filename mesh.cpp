//==================================================================================================================================
// 
//			メッシュの処理 [mesh.cpp]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
//**************************************************************
// インクルード
#include "input.h"
#include "debugproc.h"
#include "mesh.h"
#include "Texture.h"

//**************************************************************
// グローバル変数
bool		g_bMeshDebug[MESHTYPE];			// メッシュのデバッグ表示　ON OFF

//**************************************************************
// プロトタイプ宣言
void MeshDebug(void);						// メッシュのデバッグ表示処理
void DrawMeshOne(P_MESH pMesh, int nMaxMesh); // drawを簡易化するための関数分け

//=========================================================================================
// メッシュの初期化処理
//=========================================================================================
void InitMesh(void)
{
	// Sphere
	InitMeshSphere();

	// Dome
	InitMeshDome();

	// Cylinder
	InitMeshCylinder();

	// Ring
	InitMeshRing();

	//SetMeshCylinder(vec3(0.0f, 120.0f, 0.0f), vec3_ZERO, 100.0f, 100.0f, 1, 16);
	//SetMeshSphere(vec3(200.0f, 120.0f, 0.0f), vec3_ZERO, 100.0f, 16, 32);
	//SetMeshRing(vec3(0.0f, 120.0f, 100.0f), vec3_ZERO, 100.0f, 120.0f, 3, 16);
	//SetMeshDome(vec3(200.0f, 120.0f, 100.0f), vec3_ZERO, 100.0f, 8, 16);
}

//=========================================================================================
// メッシュの終了処理
//=========================================================================================
void UninitMesh(void)
{
	// Sphere
	UninitMeshSphere();

	// Dome
	UninitMeshDome();

	// Cylinder
	UninitMeshCylinder();

	// Ring
	UninitMeshRing();
}

//=========================================================================================
// メッシュの更新処理
//=========================================================================================
void UpdateMesh(void)
{
	// Sphere
	UpdateMeshSphere();

	// Dome
	UpdateMeshDome();

	// Cylinder
	UpdateMeshCylinder();

	// Ring
	UpdateMeshRing();

#ifdef _DEBUG
	MeshDebug();
#endif
}

//=========================================================================================
// メッシュのデバッグ表示処理
void MeshDebug(void)
{
	//**************************************************************
	// 変数宣言
	P_MESH	pMesh[MESHTYPE];

	//**************************************************************
	// ガイド表示
	if (GetKeyboardPress(DEBUG_MESH_KEY))
	{
		PrintDebugProc("\nM + S => MESH_SPHERE   [ %d / %d ]\n", GetNumMeshSphere(), MAX_MESHSPHERE);
		PrintDebugProc("\nM + D => MESH_DOME     [ %d / %d ]\n", GetNumMeshDome(), MAX_MESHDOME);
		PrintDebugProc("\nM + C => MESH_CYLINDER [ %d / %d ]\n", GetNumMeshCylinder(), MAX_MESHCYLINDER);
		PrintDebugProc("\nM + R => MESH_RING     [ %d / %d ]\n", GetNumMeshRing(), MAX_MESHRING);

		//**************************************************************
		// 各メッシュタイプごとの表示切り替え
		if (GetKeyboardTrigger(DEBUG_SPHERE_KEY))
		{
			g_bMeshDebug[0] = g_bMeshDebug[0] ^ 1;
		}
		if (GetKeyboardTrigger(DEBUG_DOME_KEY))
		{
			g_bMeshDebug[1] = g_bMeshDebug[1] ^ 1;
		}
		if (GetKeyboardTrigger(DEBUG_CYLINDER_KEY))
		{
			g_bMeshDebug[2] = g_bMeshDebug[2] ^ 1;
		}
		if (GetKeyboardTrigger(DEBUG_RING_KEY))
		{
			g_bMeshDebug[3] = g_bMeshDebug[3] ^ 1;
		}
	}

	//**************************************************************
	// 各メッシュタイプごとに情報取得
	if (g_bMeshDebug[0])
	{
		pMesh[0] = GetMeshSphere();
	}

	if (g_bMeshDebug[1])
	{
		pMesh[1] = GetMeshDome();
	}

	if (g_bMeshDebug[2])
	{
		pMesh[2] = GetMeshCylinder();
	}

	if (g_bMeshDebug[3])
	{
		pMesh[3] = GetMeshRing();
	}



}

//=========================================================================================
// メッシュの描画処理
//=========================================================================================
void DrawMesh(void)
{
	// Sphere
	// DrawMeshSphere();
	DrawMeshOne(GetMeshSphere(), MAX_MESHSPHERE);

	// Dome
	// DrawMeshDome();
	DrawMeshOne(GetMeshDome(), MAX_MESHDOME);

	// Cylinder
	// DrawMeshCylinder();
	DrawMeshOne(GetMeshCylinder(), MAX_MESHCYLINDER);

	// Ring
	// DrawMeshRing();
	DrawMeshOne(GetMeshRing(), MAX_MESHRING);
}

//=========================================================================================
// メッシュの描画処理　簡易化のための関数わけ
//=========================================================================================
void DrawMeshOne(P_MESH pMesh, int nMax)
{
	//**************************************************************
	// 変数宣言
	LPDIRECT3DDEVICE9	pDevice = GetDevice();		// デバイスへのポインタ
	D3DXMATRIX			mtxRot, mtxTrans;			// マトリックス計算用

	for (int nCntMesh = 0; nCntMesh < nMax; nCntMesh++, pMesh++)
	{
		if (pMesh->bUse)
		{
			//**************************************************************
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&pMesh->mtxWorld);

			//**************************************************************
			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, pMesh->rot.y, pMesh->rot.x, pMesh->rot.z);
			D3DXMatrixMultiply(&pMesh->mtxWorld, &pMesh->mtxWorld, &mtxRot);

			//**************************************************************
			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, pMesh->pos.x, pMesh->pos.y, pMesh->pos.z);
			D3DXMatrixMultiply(&pMesh->mtxWorld, &pMesh->mtxWorld, &mtxTrans);

			//**************************************************************
			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &pMesh->mtxWorld);

			//**************************************************************
			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0,
				pMesh->pVtxBuff,
				0,
				sizeof(VERTEX_3D));

			// インデックスバッファを設定
			pDevice->SetIndices(pMesh->pIdxBuffer);

			//**************************************************************
			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			//**************************************************************
			// テクスチャの設定
			pDevice->SetTexture(0, GetTexture(pMesh->nIdxTexture));

			//**************************************************************
			// カリングモードの設定
			pDevice->SetRenderState(D3DRS_CULLMODE, pMesh->culling);

			//**************************************************************
			// スフィアの描画
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, pMesh->nVerti, 0, pMesh->nPrim);
		}
	}

	EndDevice();	// デバイス取得終了

}