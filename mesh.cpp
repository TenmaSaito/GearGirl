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

//**************************************************************
// グローバル変数
bool		g_bMeshDebug[MESHTYPE];			// メッシュのデバッグ表示　ON OFF

//**************************************************************
// プロトタイプ宣言
void MeshDebug(void);						// メッシュのデバッグ表示処理

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
	DrawMeshSphere();

	// Dome
	DrawMeshDome();

	// Cylinder
	DrawMeshCylinder();

	// Ring
	DrawMeshRing();
}
