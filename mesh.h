//==================================================================================================================================
//
//			メッシュの処理 [mesh.h]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
#ifndef _MESH_H_
#define _MESH_H_	//2重インクルード防止

//==============================================================
// ヘッダーインクルード
#include "main.h"
#include "endomacro.h"

//=========================================================================================
// マクロ定義
//=========================================================================================
#define MAX_TEX_MESH			(2)						// テクスチャ数
#define MESHTYPE				(4)						// 種類
#define MAX_MESHSPHERE			(16)					// スフィア
#define MAX_MESHDOME			(16)					// ドーム
#define MAX_MESHCYLINDER		(16)					// シリンダー
#define MAX_MESHRING			(64)					// リング
#define MAX_MESH				(MAX_MESHSPHERE + MAX_MESHDOME + MAX_MESHCYLINDER + MAX_MESHRING)

//**************************************************************
// デバッグ表示用キー
#define DEBUG_MESH_KEY			DIK_M					// メッシュのデバッグ情報を表示
#define DEBUG_SPHERE_KEY		DIK_S					// スフィアの詳細情報
#define DEBUG_DOME_KEY			DIK_D					// ドームの詳細情報
#define DEBUG_CYLINDER_KEY		DIK_C					// シリンダーの詳細情報
#define DEBUG_RING_KEY			DIK_R					// リングの詳細情報

//==============================================================
// メッシュ基本情報の構造体
//==============================================================
typedef struct MeshInfo
{
	vec3					pos;				// 位置
	vec3					rot;				// 向き
	vec3					size;				// 大きさ
	int						nCircleDivision;	// U (円)分割数
	int						nHeightDivision;	// V (高さ)分割数

	int						nVerti;				// 全頂点数
	int						nPrim;				// プリミティブ数
	D3DXMATRIX				mtxWorld;			// ワールドマトリックス
	LPDIRECT3DVERTEXBUFFER9	pVtxBuff;			// 頂点バッファ
	LPDIRECT3DINDEXBUFFER9	pIdxBuffer;			// インデックスバッファ
	int						nIdxTexture;		// テスクチャ番号

	bool					bInner;				// 内面カリング
	bool					bOuter;				// 外面カリング
	bool					bUse;				// 使ってるか
}MeshInfo;
POINTER(MeshInfo, P_MESH);

//=========================================================================================
// プロトタイプ宣言
//=========================================================================================
void InitMesh(void);
void InitMeshSphere(void);
void InitMeshDome(void);
void InitMeshCylinder(void);
void InitMeshRing(void);

void UninitMesh(void);
void UninitMeshSphere(void);
void UninitMeshDome(void);
void UninitMeshCylinder(void);
void UninitMeshRing(void);

void UpdateMesh(void);
void UpdateMeshSphere(void);
void UpdateMeshDome(void);
void UpdateMeshCylinder(void);
void UpdateMeshRing(void);

void DrawMesh(void);
void DrawMeshSphere(void);
void DrawMeshDome(void);
void DrawMeshCylinder(void);
void DrawMeshRing(void);

void SetMeshSphere	(vec3 pos, vec3 rot, float fRadius, int nHeightDivision, int nCircleDivision, bool bInner = false, bool bOuter = true, int nTex = -1);
void SetMeshDome	(vec3 pos, vec3 rot, float fRadius, int nHeightDivision, int nCircleDivision, bool bInner = false, bool bOuter = true, int nTex = -1);
void SetMeshCylinder(vec3 pos, vec3 rot, float fRadius,float fHeight, int nHeightDivision, int nCircleDivision, bool bInner = false, bool bOuter = true, int nTex = -1);
void SetMeshRing	(vec3 pos, vec3 rot, float fInner, float fOuter, int nHeightDivision, int nCircleDivision, bool bInner = false, bool bOuter = true, int nTex = -1);

P_MESH GetMeshSphere(void);
P_MESH GetMeshDome(void);
P_MESH GetMeshCylinder(void);
P_MESH GetMeshRing(void);

int GetNumMeshSphere(void);
int GetNumMeshDome(void);
int GetNumMeshCylinder(void);
int GetNumMeshRing(void);

#endif // !_MESH_H_
