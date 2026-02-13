//================================================================================================================
//
// DirectXの計算関連ヘッダファイル [mathUtil.h]
// Author : TENMA
//
//================================================================================================================
#ifndef _MATHUTIL_H_
#define _MATHUTIL_H_

//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "main.h"

// modeldata.hが存在する場合
#if __has_include("modeldata.h")
#include "modeldata.h"
#define MODELDATA_INCLUDED
#endif

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define RECT_NULL				(RECT{0, 0, 0, 0})				// RECT初期化
#define INT_VECTOR3_NULL		(INT_VECTOR3{ 0, 0, 0 })		// INT_VECTOR3初期化
#define INT_VECTOR2_NULL		(INT_VECTOR2{ 0, 0 })			// INT_VECTOR2初期化
#define D3DCOLOR_NULL			(D3DCOLOR_RGBA(255, 255, 255, 255)) // D3DCOLOR_NULL
#define INT_INFINITY			(0x7FFFFFFF)					// int型の最大(signed intの最大値)
#define VECNULL					D3DXVECTOR3(0.0f,0.0f,0.0f)		// 省略版vectorNull
#define VEC_X(x)				D3DXVECTOR3(x, 0.0f, 0.0f)		// Xのみ変更
#define VEC_Y(y)				D3DXVECTOR3(0.0f, y, 0.0f)		// Yのみ変更
#define VEC_Z(z)				D3DXVECTOR3(0.0f, 0.0f, z)		// Zのみ変更
#define InitRot(x, y, z)		RepairedRot(D3DXVECTOR3(x, y, z))	// 修正済み角度
#define DEF_COL					D3DXCOLOR_NULL					// デフォルトカラー

//----------------------------------------------------------------------------------
/*** フラグ関連 ***/
#define END_SHADER				(0xffffffff)						// シェーダーの終了コード
//----------------------------------------------------------------------------------

namespace MyMathUtil
{
	//**********************************************************************************
	//*** int型Vector3構造体 ***
	//**********************************************************************************
	typedef struct
	{
		int x;		// x
		int y;		// y
		int z;		// z
	}INT_VECTOR3;

	//**********************************************************************************
	//*** int型Vector2構造体 ***
	//**********************************************************************************
	typedef struct
	{
		int x;		// x
		int y;		// y
	}INT_VECTOR2;

	//**********************************************************************************
	//*** プロトタイプ宣言 ***
	//**********************************************************************************

	//----------------------------------------------------------------------------------
	/*** 当たり判定、角度関連 ***/
	bool CollisionBox(RECT rect, D3DXVECTOR3 pos);
	bool CollisionBoxZ(D3DXVECTOR4 rect, D3DXVECTOR3 pos);
	void RepairFloat(float* fRepairTarget, int nCnt = 3);
	float RepairRot(float fRot);
	void RepairRot(float* pOut, const float* fAngle);
	void RepairRot(D3DXVECTOR3* pOut, const D3DXVECTOR3* pIn);
	float InverseRot(float fRot);
	D3DXVECTOR3 InverseRot(D3DXVECTOR3 fRot);
	D3DXVECTOR3 RepairedRot(const D3DXVECTOR3 pIn);
	D3DXVECTOR3 DegreeToRadian(D3DXVECTOR3 degree);
	D3DXVECTOR3 RadianToDegree(D3DXVECTOR3 radian);
	//----------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------
	/*** D3DXVECTOR3の位置関連 ***/
	D3DXVECTOR3 GetPTPLerp(D3DXVECTOR3 Start, D3DXVECTOR3 End, float s);
	void HomingPosToPos(D3DXVECTOR3 posTarget, D3DXVECTOR3* posMover, float fSpeed);
	float GetPosToPos(D3DXVECTOR3 posTarget, D3DXVECTOR3 posMover);
	D3DXVECTOR3 GetPosBetweenPos(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2);
	float GetPTPLength(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2);
	float GetPTPLength3D(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2);
	//----------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------
	/*** D3DXCOLOR関連 ***/
	D3DXCOLOR GetColLerp(D3DXCOLOR Start, D3DXCOLOR End, float s);
	//----------------------------------------------------------------------------------
	
	//----------------------------------------------------------------------------------
	/*** ランダムな値取得関連 ***/
	D3DXVECTOR3 GetRandomVector3(int mx, int my, int mz);
	D3DXCOLOR GetRandomColor(bool bUseAlphaRand);
	//----------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------
	/*** 頂点情報関連 ***/
	void RollPolygon(VERTEX_2D* pVtx, D3DXVECTOR3 pos, float fWidth, float fHeight, float fRot, int nSpeed);
	void SetFullScreenPolygon(VERTEX_2D* pVtx);
	void SetPolygonSize(VERTEX_3D* pVtx, D3DXVECTOR2 size, bool bY);
	void SetPolygonPos(VERTEX_2D* pVtx, D3DXVECTOR3 pos, D3DXVECTOR2 size);

	// テクスチャ設定
	template<typename VERTEX>
	void SetDefaultTexture(VERTEX* pVtx)
	{
		if (pVtx == nullptr) return;

		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);
	}

	// 色設定
	template<typename VERTEX>
	void SetDefaultColor(VERTEX* pVtx, D3DXCOLOR col = D3DXCOLOR_NULL)
	{
		if (pVtx == nullptr) return;

		pVtx[0].col = col;
		pVtx[1].col = col;
		pVtx[2].col = col;
		pVtx[3].col = col;
	}

	void SetPolygonNormal(VERTEX_3D* pVtx, D3DXVECTOR3 nor);
	void SetPolygonRHW(VERTEX_2D* pVtx);
	//----------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------
	/*** 変換関連 ***/
	D3DXVECTOR3 INTToFloat(INT_VECTOR3 nVector3);
	D3DXVECTOR2 INTToFloat(INT_VECTOR2 nVector2);
	//----------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------
	/*** 描画関連 ***/
	void DrawPolygon(_In_ LPDIRECT3DDEVICE9 pDevice,
		_In_ LPDIRECT3DVERTEXBUFFER9 pVtxBuff,
		_In_opt_ LPDIRECT3DTEXTURE9 pTexture,
		_In_ UINT VertexFormatSize,
		_In_ DWORD FVF,
		_In_ int nNumPolygon,
		_In_opt_ UINT OffSet = NULL);

	void DrawPolygonTextureArray(_In_ LPDIRECT3DDEVICE9 pDevice,
		_In_ LPDIRECT3DVERTEXBUFFER9 pVtxBuff,
		_In_ LPDIRECT3DTEXTURE9 *pTexture,
		_In_ UINT nNumTexture,
		_In_ const int *pArrayTexNo,
		_In_ UINT VertexFormatSize,
		_In_ DWORD FVF,
		_In_ int nNumPolygon,
		_In_opt_ UINT OffSet = NULL);

	void DrawPolygonTextureFromIndex(_In_ LPDIRECT3DDEVICE9 pDevice,
		_In_ LPDIRECT3DVERTEXBUFFER9 pVtxBuff,
		_In_ int nIdxTexture,
		_In_ UINT VertexFormatSize,
		_In_ DWORD FVF,
		_In_ int nNumPolygon,
		_In_opt_ UINT OffSet = NULL);

#pragma push_macro("NULL")
#undef NULL
#define NULL nullptr

	void Draw3DModelFromXFile(_In_ LPDIRECT3DDEVICE9 pDevice,
		_In_ const D3DXMATERIAL *pMat,
		_In_ DWORD dwNumMat,
		_In_ LPDIRECT3DTEXTURE9 *ppTexture,
		_In_ LPD3DXMESH pMesh,
		_In_ const D3DXMATRIX *pMtxWorld,
		_In_opt_ const D3DXMATRIX *pMtxShadow = NULL);

	void Draw3DModelByCustomColorFromXFile(_In_ LPDIRECT3DDEVICE9 pDevice,
		_In_ const D3DXMATERIAL* pMat,
		_In_ DWORD dwNumMat,
		_In_ LPDIRECT3DTEXTURE9* ppTexture,
		_In_ LPD3DXMESH pMesh,
		_In_ const D3DXMATRIX* pMtxWorld,
		_In_ D3DCOLORVALUE CustomColor);

#ifdef MODELDATA_INCLUDED
	void Draw3DModelFromModelData(_In_ LPDIRECT3DDEVICE9 pDevice,
		_In_ const MODELDATA *pModelData,
		_In_ const D3DXMATRIX *pMtxWorld,
		_In_opt_ const D3DXMATRIX *pMtxShadow = NULL);

	void Draw3DModelByCustomColorFromModelData(_In_ LPDIRECT3DDEVICE9 pDevice,
		_In_ const MODELDATA* pModelData,
		_In_ const D3DXMATRIX* pMtxWorld,
		_In_ D3DCOLORVALUE CustomColor);
#endif
#pragma pop_macro("NULL")

	D3DXMATRIX *CalcWorldMatrix(_Inout_ D3DXMATRIX *pMtxWorld,
		_In_ D3DXVECTOR3 pos,
		_In_ D3DXVECTOR3 rot);

	D3DXMATRIX *CalcWorldMatrixFromParent(_Inout_ D3DXMATRIX *pMtxWorld,
		_In_ const D3DXMATRIX *pMtxParent,
		_In_ D3DXVECTOR3 pos,
		_In_ D3DXVECTOR3 rot);

	D3DXMATRIX *CreateShadowMatrix(_In_ LPDIRECT3DDEVICE9 pDevice,
		_In_ const D3DXMATRIX *pMtxWorld,
		_In_ D3DXVECTOR3 pos,
		_In_ D3DXVECTOR3 nor,
		_In_ UINT nIdxLight,
		_Out_ D3DXMATRIX *pOut);

	void SetEnableZFunction(_In_ LPDIRECT3DDEVICE9 pDevice,
		_In_ bool bEnable);

	void SetUpPixelFog(_In_ D3DXCOLOR Col,
		_In_ float fStart,
		_In_ float fEnd);

	void CleanUpPixelFog(void);
	//----------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------
	/*** シェーダー関連 ***/
	void SetSheder(_In_ LPD3DXEFFECT pEffect,
		_In_ const char *TechniqueName,
		_In_ UINT Pass);

	void RemovePass(_In_ LPD3DXEFFECT pEffect,
		_In_ UINT NextPass = END_SHADER);
	//----------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------
	/*** システム関連 ***/
	HRESULT CheckIndex(int TargetIndexMax, int Index, int TargetIndexMin = NULL);
	bool CheckPath(_In_ const char* pFileName);
	char *UniteChar(char* pOut, const char* fmt, ...);
	int GenerateMessageBox(_In_ UINT nType, _In_ const char* pCaption, _In_ const char* fmt, ...);
	char *GetErrorMessage(_In_ HRESULT hr, char *pOut, size_t size, bool bPopupMessageBox);
	//----------------------------------------------------------------------------------
}

#endif
