//==================================================================================
//
// DirectXの計算関連ヘッダファイル [mathUtil.h]
// Author : TENMA
//
//==================================================================================
#ifndef _MATHUTIL_H_
#define _MATHUTIL_H_

//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include "param.h"
#include <time.h>

using namespace Constants;

// modeldata.hが存在する場合
#if __has_include("modeldata.h")
#include "modeldata.h"
#define MODELDATA_INCLUDED
#else
SetWarning("modeldata.hがインクルードされていません。modeldata関連関数は無効化されます。")
#endif

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define NOT_WARNING(C)			__pragma(warning(suppress : C))		// 指定した警告を一度だけ無効
#define UNABLE(C)				NOT_WARNING(C)						// 省略版
#define FLOATTOINT				4244								// floatからintへの変換警告
#define RECT_NULL				(RECT{0, 0, 0, 0})					// RECT初期化
#define INT_VECTOR3_NULL		(MyMathUtil::INT_VECTOR3{ 0, 0, 0 })	// INT_VECTOR3初期化
#define INT_VECTOR2_NULL		(MyMathUtil::INT_VECTOR2{ 0, 0 })		// INT_VECTOR2初期化
#define D3DCOLOR_NULL			(D3DCOLOR_RGBA(255, 255, 255, 255)) // D3DCOLOR_NULL
#define INT_INFINITY			(0x7FFFFFFF)						// int型の最大(signed intの最大値)
#define VECNULL					D3DXVECTOR3(0.0f,0.0f,0.0f)			// 省略版vectorNull
#define VEC_X(x)				D3DXVECTOR3(x, 0.0f, 0.0f)			// Xのみ変更
#define VEC_Y(y)				D3DXVECTOR3(0.0f, y, 0.0f)			// Yのみ変更
#define VEC_Z(z)				D3DXVECTOR3(0.0f, 0.0f, z)			// Zのみ変更
#define InitRot(x, y, z)		RepairedRot(D3DXVECTOR3(x, y, z))	// 修正済み角度
#define DEF_COL					D3DXCOLOR_NULL						// デフォルトカラー
#define foreach(type, var, lpArray)		for(type &var : lpArray)	// foreach構文
#define CMP_SUCCESS(cmp)		(cmp == 0)			// strcmp,strncmpでの成功判定
#define CMP_FAILED(cmp)			(cmp != 0)			// strcmp,strncmpでの失敗判定
#define STR_SUCCESS(str)		(str != NULL)		// strstrでの成功判定
#define STR_FAILED(str)			(str == NULL)		// strstrでの失敗判定
#define C_SUCCEED(a, b)			CMP_SUCCESS(strcmp(a, b))			// strcmp省略版
#define C_FAILED(a, b)			CMP_FAILED(strcmp(a, b))			// strcmp省略版
#define ST_SUCCEED(a, b)		STR_SUCCESS(strstr(a, b))			// strstr省略版
#define ST_FAILED(a, b)			STR_FAILED(strstr(a, b))			// strstr省略版
#ifndef STRUCT
#define STRUCT(...)				typedef struct __VA_ARGS__			// STRUCT宣言
#endif
#ifndef ENUM
#define ENUM(...)				typedef enum __VA_ARGS__			// ENUM宣言
#endif
#ifndef PARENT
#define PARENT(...)				: public __VA_ARGS__				// 継承宣言
#endif
#define USE_UTIL				using namespace MyMathUtil			// MyMathUtilの使用宣言
#define USE_LIB					using namespace MyLib				// MyLibの使用宣言

//----------------------------------------------------------------------------------
/*** フラグ関連 ***/
#define END_SHADER				(-1)						// シェーダーの終了コード
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
// 型宣言
//----------------------------------------------------------------------------------
using V2 = D3DXVECTOR2;
using V3 = D3DXVECTOR3;
using COL = D3DXCOLOR;
using IDX_TEX = int;
using IDX_2DPOLYGON = int;
using SHADER_PATH = char[MAX_PATH];
using TECHNIQUE_NAME = const char*;
using float2 = float[2];
using float3 = float[3];
using float4 = float[4];
using float4x4 = D3DXMATRIX;
typedef char STACKID[MAX_PATH];

namespace MyMathUtil
{
	//******************************************************************************
	//*** int型Vector3構造体 ***
	//******************************************************************************
	STRUCT()
	{
		int x;		// x
		int y;		// y
		int z;		// z
	}INT_VECTOR3;

	//******************************************************************************
	//*** int型Vector2構造体 ***
	//******************************************************************************
	STRUCT()
	{
		int x;		// x
		int y;		// y
	}INT_VECTOR2;

	//******************************************************************************
	//*** DualInputのフラグ列挙 ***
	//******************************************************************************
	ENUM()
	{
		DUAL_KEYBOARD	= 0x00000001,			// Keyboard入力
		DUAL_JOYPAD		= 0x00000002,			// ジョイパッド入力
		DUAL_OR			= 0x00000010,			// 片方
		DUAL_AND		= 0x00000020,			// 両方
		DUAL_PRESS		= 0x00000100,			// プレス
		DUAL_TRIGGER	= 0x00000200,			// トリガー
		DUAL_RELEASE	= 0x00000400,			// リリース
		DUAL_REPEAT		= 0x00000800,			// リピート
		DUAL_DUAL		= 0x00001000,			// コントローラー2番で判定

		DUAL_FORCE_DWORD = 0x7fffffff
	} DUAL;

#define DUAL_DUALID		3						// DUALをずらす距離

	//******************************************************************************
	//*** プロトタイプ宣言 ***
	//******************************************************************************
	
	//------------------------------------------------------------------------------
	/*** Input関連 ***/
	bool GetDualInput(int nKey, DWORD nFlag1, int nKey2 = -1, DWORD nFlag2 = -1);
	//------------------------------------------------------------------------------

	//------------------------------------------------------------------------------
	/*** 当たり判定、角度関連 ***/
	bool CollisionBox(RECT rect, D3DXVECTOR3 pos);
	bool CollisionBoxZ(D3DXVECTOR4 rect, D3DXVECTOR3 pos);
	bool IsDetection(D3DXVECTOR3 p1, D3DXVECTOR3 p2, float fRadius);
	void RepairFloat(float* fRepairTarget, int nCnt = 3);
	__forceinline float RepairRot(float fRot)
	{
		return (fRot > D3DX_PI) ? fRot -= D3DX_PI * 2.0f :
			((fRot <= -D3DX_PI) ? fRot += D3DX_PI * 2.0f :
				fRot);
	}
	void RepairRot(float* pOut, const float* fAngle);
	void RepairRot(D3DXVECTOR3* pOut, const D3DXVECTOR3* pIn);
	float InverseRot(float fRot);
	D3DXVECTOR3 InverseRot(D3DXVECTOR3 fRot);
	D3DXVECTOR3 RepairedRot(const D3DXVECTOR3 pIn);
	D3DXVECTOR3 DegreeToRadian(D3DXVECTOR3 degree);
	D3DXVECTOR3 RadianToDegree(D3DXVECTOR3 radian);
	//------------------------------------------------------------------------------

	//------------------------------------------------------------------------------
	/*** D3DXVECTOR3の位置関連 ***/
	D3DXVECTOR3 GetPTPLerp(D3DXVECTOR3 Start, D3DXVECTOR3 End, float s);
	void HomingPosToPos(D3DXVECTOR3 posTarget, D3DXVECTOR3* posMover, float fSpeed);
	float GetPosToPos(D3DXVECTOR3 posTarget, D3DXVECTOR3 posMover);
	D3DXVECTOR3 GetPosBetweenPos(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2);
	float GetPTPLength(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2);
	float GetPTPLength3D(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2);
	//------------------------------------------------------------------------------

	//------------------------------------------------------------------------------
	/*** D3DXCOLOR関連 ***/
	D3DXCOLOR GetColLerp(D3DXCOLOR Start, D3DXCOLOR End, float s);
	//------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------
	/*** ランダムな値取得関連 ***/
	D3DXVECTOR3 GetRandomVector3(int mx, int my, int mz);
	D3DXCOLOR GetRandomColor(bool bUseAlphaRand);
	//------------------------------------------------------------------------------

	//------------------------------------------------------------------------------
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
	//------------------------------------------------------------------------------

	//------------------------------------------------------------------------------
	/*** 変換関連 ***/
	D3DXVECTOR3 INTToFloat(INT_VECTOR3 nVector3);
	D3DXVECTOR2 INTToFloat(INT_VECTOR2 nVector2);
	//------------------------------------------------------------------------------

	//------------------------------------------------------------------------------
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
	//------------------------------------------------------------------------------

	//------------------------------------------------------------------------------
	/*** シェーダー関連 ***/
	_Check_return_ HRESULT LoadSheder(
		_In_ LPDIRECT3DDEVICE9 pDevice,
		_In_ SHADER_PATH pathName,
		_Out_ LPD3DXEFFECT* ppEffect
	);

	void SetSheder(
		_In_ LPD3DXEFFECT pEffect,
		_In_ TECHNIQUE_NAME TechniqueName,
		_In_ UINT Pass
	);

	void RemovePass(
		_In_ LPD3DXEFFECT pEffect,
		_In_ UINT NextPass = END_SHADER
	);
	//------------------------------------------------------------------------------

	//------------------------------------------------------------------------------
	/*** 単純計算関連 ***/
	// --- int --- //
	// 最大値
	__forceinline int Max(int x, int y)
	{
		return (x >= y) ? x : y;
	}

	// 最小値
	__forceinline int Min(int x, int y)
	{
		return (x >= y) ? y : x;
	}

	// クランプ
	__forceinline int Clamp(int x, int clampMin, int clampMax)
	{
		return (x > clampMax) ? clampMax 
			: ((x < clampMin) ? clampMin 
				: x);
	}

	// 二乗
	__forceinline int Square(int x)
	{
		return x * x;
	}

	// 線形補間
	__forceinline int Lerp(int start, int end, float s)
	{
		NOT_WARNING(4244)
		return start + ((end - start) * s);
	}

	// 上下判定
	__forceinline int Step(int y, int x)
	{
		return (x >= y) ? 1 : 0;
	}

	// 絶対値変換済み上下判定
	__forceinline int StepAbs(int y, int x)
	{
		return (x >= abs(y)) ? 1 : 0;
	}

	// 符号を返す
	__forceinline int Sign(int x)
	{
		return (x > 0) ? 1
			: ((x < 0) ? -1
				: 0);
	}

	// --- float --- //
	// 最大値
	__forceinline float Max(float x, float y)
	{
		return (x >= y) ? x : y;
	}

	// 最小値
	__forceinline float Min(float x, float y)
	{
		return (x >= y) ? y : x;
	}

	// クランプ
	__forceinline float Clamp(float x, float clampMin, float clampMax)
	{
		return (x > clampMax) ? clampMax
			: ((x < clampMin) ? clampMin
				: x);
	}

	// 二乗
	__forceinline float Square(float x)
	{
		return x * x;
	}

	// 線形補間
	__forceinline float Lerp(float start, float end, float s)
	{
		return start + ((end - start) * s);
	}

	// 上下判定
	__forceinline float Step(float y, float x)
	{
		return (x >= y) ? 1.0f : 0.0f;
	}

	// 絶対値変換済み上下判定
	__forceinline float StepAbs(float y, float x)
	{
		return (x >= fabsf(y)) ? 1.0f : 0.0f;
	}

	// 符号を返す
	__forceinline float Sign(float x)
	{
		return (x > 0.0f) ? 1.0f 
			: ((x < 0.0f) ? -1.0f 
				: 0.0f);
	}

	// --- VECTOR3 --- //
	// 円の弧の座標
	__forceinline D3DXVECTOR3 Arc(float radius, float radian, D3DXVECTOR3 offset = CParamVector::V3NULL)
	{
		return D3DXVECTOR3(offset.x + cosf(RepairRot(radian)) * radius,
			offset.y + sinf(RepairRot(radian)) * radius,
			offset.z + 0.0f);
	}

	// --- MATRIX --- //
	// 掛け算
	__forceinline D3DXMATRIX Multiply(D3DXMATRIX mtx1, D3DXMATRIX mtx2)
	{
		return mtx1 * mtx2;
	}

	// --- every ---//
	// 判定
	template <typename T>
	inline bool IsArray(T* pArray, int num, T Compare)
	{
		for(int nCntCom = 0; nCntCom < num; nCntCom++)
		{
			if (pArray[nCntCom] == Compare)
			{
				return true;
			}
		}

		return false;
	}
	//------------------------------------------------------------------------------

	//------------------------------------------------------------------------------
	/*** システム関連 ***/
	HRESULT CheckIndex(int TargetIndexMax, int Index, int TargetIndexMin = NULL);
	bool CheckPath(_In_ const char* pFileName);
	char *UniteChar(char* pOut, const char* fmt, ...);
	int GenerateMessageBox(_In_ UINT nType, _In_ const char* pCaption, _In_ const char* fmt, ...);
	char *GetErrorMessage(_In_ HRESULT hr, char *pOut, size_t size, bool bPopupMessageBox);
	__forceinline void SetRand(void)
	{
		srand((unsigned int)time(0));
	}

	bool SaveDataForHeap(const void *pData, size_t size, UINT ID);
	bool SaveDataForHeap(const void* pData, size_t size, STACKID SID);
	bool LoadDataFromHeap(void* pData, size_t size, UINT ID);
	bool LoadDataFromHeap(void* pData, size_t size, STACKID SID);
	void AllReleaseFromHeap(void);

	//==================================================================================
	// --- ヒープに新規でデータを保存 ---
	//==================================================================================
	template <size_t N>
	bool SaveDataForHeap(const void* pData, size_t size, const char(&SID)[N])
	{
		static_assert(N < MAX_PATH, "Too Long String...... Please input more short String");

		STACKID pSID = {};

		strcpy(pSID, SID);

		return MyMathUtil::SaveDataForHeap(pData, size, pSID);
	}

	//==================================================================================
	// --- ヒープ中データを取得及び解放 ---
	//==================================================================================
	template <size_t N>
	bool LoadDataFromHeap(void* pData, size_t size, const char(&SID)[N])
	{
		static_assert(N < MAX_PATH, "Too Long String...... Please input more short String");

		STACKID pSID = {};

		strcpy(pSID, SID);

		return MyMathUtil::LoadDataFromHeap(pData, size, pSID);
	}
	//------------------------------------------------------------------------------
}

#include <initializer_list>

//==================================================================================
// 
// --- 自作ベクター ---
// 
//==================================================================================
namespace MyLib
{
	//==================================================================================
	// --- キャパシティチェック関数 ---
	//==================================================================================
	bool IsCapacityOver(UINT num, UINT capasity);

	//==================================================================================
	// --- c言語自作vectorクラス ---
	//==================================================================================
	template<class T>
	class CVector
	{
		//------------------------------------------------------------
		// --- 型宣言 ---
		//------------------------------------------------------------
		using ElemType = T;

		//------------------------------------------------------------
		// --- 変数宣言 ---
		//------------------------------------------------------------
		ElemType *pHead;		// 確保したコンテナの先頭ポインタ
		UINT vectorNum;			// 使用中の配列数
		UINT vectorCapacity;	// メモリ予約済みの配列数

		//------------------------------------------------------------
		// --- メモリ容量の拡張 ---
		//------------------------------------------------------------
		bool GetCapacity(void)
		{
			// キャパシティを計算(新規なら1, 既にある場合は2倍確保)
			UINT newVectorCapacity = (vectorCapacity == 0) ? 1 : (vectorCapacity * 2);

			// メモリをキャパシティ分新規で確保(拡張)
			ElemType *pMem = (ElemType*)realloc(pHead, sizeof(ElemType) * newVectorCapacity);
			if (pMem == nullptr) return false;		// 確保失敗時はfalse

			/*** 成功時 ***/
			pHead = pMem;							// 確保したメモリポインタを割り当て
			vectorCapacity = newVectorCapacity;		// キャパシティを更新
			return true;
		}

	public:
		//------------------------------------------------------------
		// --- コンストラクタ --- 
		//------------------------------------------------------------
		// 通常コンストラクタ
		CVector(void) : pHead(nullptr), vectorNum(0), vectorCapacity(0) {}
		
		// 要素指定コンストラクタ
		CVector(const ElemType& Elem) : pHead(nullptr), vectorNum(0), vectorCapacity(0)
		{
			Add(Elem);
		}

		// 配列要素指定コンストラクタ
		template<size_t N>
		CVector(const ElemType(&Elem)[N]) : pHead(nullptr), vectorNum(0), vectorCapacity(0)
		{
			AddArray(Elem);
		}

		// 配列要素初期化コンストラクタ
		CVector(std::initializer_list<ElemType> init) : pHead(nullptr), vectorNum(0), vectorCapacity(0)
		{
			// サイズ0の場合終了
			if (init.size() == 0) return;

			// サイズを取得
			vectorCapacity = static_cast<UINT>(init.size());
			vectorNum = static_cast<UINT>(init.size());

			// メモリを新規で確保
			pHead = (ElemType*)malloc(sizeof(ElemType) * vectorCapacity);
			if (pHead == nullptr)
			{ // 失敗時は各サイズをリセット
				vectorCapacity = 0;
				vectorNum = 0;
				return;
			}

			// コンテナの値を格納
			memcpy(pHead, init.begin(), GetCapacitySize());
		}

		// コピーコンストラクタ
		CVector(const CVector& Copy) : pHead(nullptr), vectorNum(0), vectorCapacity(0)
		{
			if (Copy.vectorNum > 0 && Copy.vectorCapacity > 0)
			{ // 使用中の配列数が1以上なら
				// メモリを確保
				pHead = (ElemType*)malloc(sizeof(ElemType) * Copy.vectorCapacity);
				if (pHead != nullptr)
				{ // 確保成功時
					// 値を格納
					memcpy(pHead, Copy.pHead, sizeof(ElemType) * Copy.vectorNum);
					vectorCapacity = Copy.vectorCapacity;	// キャパシティをコピー
					vectorNum = Copy.vectorNum;				// 使用中の配列数をコピー
				}
			}
		}

		//------------------------------------------------------------
		// --- デストラクタ --- 
		//------------------------------------------------------------
		~CVector(void)
		{
			if (pHead)
			{ // NULLではない場合
				free(pHead);		// メモリを解放
				pHead = nullptr;	// ポインタをnullに
				vectorNum = 0;		// 使用中の配列数をリセット
				vectorCapacity = 0;	// キャパシティをリセット
			}
		}

		//------------------------------------------------------------
		// --- 先頭アドレス取得 --- 
		//------------------------------------------------------------
		ElemType *GetHead(void)
		{
			return pHead;
		}

		//------------------------------------------------------------
		// --- 最後尾に追加 --- 
		//------------------------------------------------------------
		bool Add(const ElemType& Elem)
		{
			// キャパシティチェック
			if (IsCapacityOver(vectorNum, vectorCapacity))
			{ // キャパシティオーバーの場合
				// メモリを拡張
				if (!GetCapacity()) return false;
			}

			/*** 成功時 ***/
			pHead[vectorNum] = Elem;	// 最後尾に値を格納
			vectorNum++;				// 使用中の配列数を増加
			return true;
		}

		//------------------------------------------------------------
		// --- 先端に追加 --- 
		//------------------------------------------------------------
		bool AddHead(const ElemType& Elem)
		{
			// キャパシティチェック
			if (IsCapacityOver(vectorNum, vectorCapacity))
			{ // キャパシティオーバーの場合
				// メモリを拡張
				if (!GetCapacity()) return false;
			}

			/*** 成功時 ***/
			memmove(&pHead[1], &pHead[0], GetVectorSize());		// 先頭から1ずらした場所にデータを移動
			pHead[0] = Elem;		// 先頭に値を格納
			vectorNum++;			// 使用中の配列数を増加
			return true;
		}

		//------------------------------------------------------------
		// --- 指定した番目に追加 --- 
		//------------------------------------------------------------
		bool AddMiddle(const ElemType& Elem, UINT nIdx)
		{
			// 指定位置が先頭もしくは最後尾だった場合、各関数呼び出し
			if (nIdx >= vectorNum) return Add(Elem);
			if (nIdx == 0) return AddHead(Elem);

			// キャパシティチェック
			if (IsCapacityOver(vectorNum, vectorCapacity))
			{ // キャパシティオーバーの場合
				// メモリを拡張
				if (!GetCapacity()) return false;
			}

			/*** 成功時 ***/
			memmove(&pHead[nIdx + 1], &pHead[nIdx], GetDesignationVectorSize(nIdx));	// メモリをずらす
			pHead[nIdx] = Elem;		// 指定位置に値を格納
			vectorNum++;			// 使用中の配列数を増加
			return true;
		}

		//------------------------------------------------------------
		// --- 配列を最後尾に追加 --- 
		//------------------------------------------------------------
		template<size_t N>
		bool AddArray(const ElemType(&ElemArray)[N])
		{
			// 配列数が0の場合失敗
			if (N <= 0) return false;

			// キャパシティチェック
			if (IsCapacityOver(vectorNum + N, vectorCapacity))
			{// キャパシティオーバーの場合
				while (1)
				{
					// メモリを拡張
					if (!GetCapacity()) return false;

					// 再度確認し、オーバーなら再度拡張
					if (!IsCapacityOver(vectorNum + N, vectorCapacity)) break;
				}
			}

			/*** 成功時 ***/
			memcpy(&pHead[vectorNum], ElemArray, sizeof(ElemType) * N);	// 配列サイズ分だけ最後尾に値を格納
			vectorNum += N;		// 使用中の配列数を配列サイズ分増加
			return true;
		}

		//------------------------------------------------------------
		// --- 配列を先端に追加 --- 
		//------------------------------------------------------------
		template<size_t N>
		bool AddArrayHead(const ElemType(&ElemArray)[N])
		{
			// 配列数が0の場合失敗
			if (N <= 0) return false;

			// キャパシティチェック
			if (IsCapacityOver(vectorNum + N, vectorCapacity))
			{// キャパシティオーバーの場合
				while (1)
				{
					// メモリを拡張
					if (!GetCapacity()) return false;

					// 再度確認し、オーバーなら再度拡張
					if (!IsCapacityOver(vectorNum + N, vectorCapacity)) break;
				}
			}

			/*** 成功時 ***/
			memmove(&pHead[N], &pHead[0], GetVectorSize());		// 先頭から配列サイズ分ずらす
			memcpy(&pHead[0], ElemArray, sizeof(ElemType) * N);		// 配列サイズ分だけ先頭に値を格納
			vectorNum += N;		// 使用中の配列数を配列サイズ分増加
			return true;
		}

		//------------------------------------------------------------
		// --- 配列を特定の場所に追加 --- 
		//------------------------------------------------------------
		template<size_t N>
		bool AddArrayMiddle(const ElemType(&ElemArray)[N], UINT nIdx)
		{
			// 配列数が0の場合失敗
			if (N <= 0) return false;

			// 指定位置が先頭もしくは最後尾だった場合、各関数呼び出し
			if (nIdx >= vectorNum) return AddArray(ElemArray);
			if (nIdx == 0) return AddArrayHead(ElemArray);

			// キャパシティチェック
			if (IsCapacityOver(vectorNum + N, vectorCapacity))
			{// キャパシティオーバーの場合
				while (1)
				{
					// メモリを拡張
					if (!GetCapacity()) return false;

					// 再度確認し、オーバーなら再度拡張
					if (!IsCapacityOver(vectorNum + N, vectorCapacity)) break;
				}
			}

			/*** 成功時 ***/
			// 指定位置から配列サイズ分ずらす
			memmove(&pHead[nIdx + N], &pHead[nIdx], GetDesignationVectorSize(nIdx));	
			memcpy(&pHead[nIdx], ElemArray, sizeof(ElemType) * N);		// 配列サイズ分だけ指定位置に値を格納
			vectorNum += N;		// 使用中の配列数を配列サイズ分増加
			return true;
		}

		//------------------------------------------------------------
		// --- 最後尾への文字列の追加(char型限定) --- 
		//------------------------------------------------------------
		template<size_t N>
		bool AddString(const char(&ElemArray)[N])
		{
			// 配列数が0の場合失敗
			if (N <= 0) return false;

			if (sizeof(ElemType) != sizeof(char)) return false;
			size_t len = strlen(ElemArray);

			// キャパシティチェック
			if (IsCapacityOver(vectorNum + len, vectorCapacity))
			{// キャパシティオーバーの場合
				while (1)
				{
					// メモリを拡張
					if (!GetCapacity()) return false;

					// 再度確認し、オーバーなら再度拡張
					if (!IsCapacityOver(vectorNum + len, vectorCapacity)) break;
				}
			}

			/*** 成功時 ***/
			memcpy(&pHead[vectorNum - 1], ElemArray, sizeof(ElemType) * N);		// 配列サイズ分だけ指定位置に値を格納
			vectorNum += len;		// 使用中の配列数を配列サイズ分増加
			return true;
		}

		//------------------------------------------------------------
		// --- 先頭への文字列の追加(char型限定) --- 
		//------------------------------------------------------------
		template<size_t N>
		bool AddStringHead(const char(&ElemArray)[N])
		{
			// 配列数が0の場合失敗
			if (N <= 0) return false;

			if (sizeof(ElemType) != sizeof(char)) return false;
			size_t len = strlen(ElemArray);

			// キャパシティチェック
			if (IsCapacityOver(vectorNum + len, vectorCapacity))
			{// キャパシティオーバーの場合
				while (1)
				{
					// メモリを拡張
					if (!GetCapacity()) return false;

					// 再度確認し、オーバーなら再度拡張
					if (!IsCapacityOver(vectorNum + len, vectorCapacity)) break;
				}
			}

			/*** 成功時 ***/
			memmove(&pHead[len], &pHead[0], GetVectorSize());			// 先頭から配列サイズ分ずらす
			memcpy(&pHead[0], ElemArray, sizeof(ElemType) * len);				// 配列サイズ分だけ先頭に値を格納
			vectorNum += len;		// 使用中の配列数を配列サイズ分増加
			return true;
		}

		//------------------------------------------------------------
		// --- 特定の配列への文字列の追加(char型限定) --- 
		//------------------------------------------------------------
		template<size_t N>
		bool AddStringMiddle(const char(&ElemArray)[N], UINT nIdx)
		{
			// 配列数が0の場合失敗
			if (N <= 0) return false;

			// 指定位置が先頭もしくは最後尾だった場合、各関数呼び出し
			if (nIdx >= vectorNum) return AddString(ElemArray);
			if (nIdx == 0) return AddStringHead(ElemArray);

			if (sizeof(ElemType) != sizeof(char)) return false;
			size_t len = strlen(ElemArray);

			// キャパシティチェック
			if (IsCapacityOver(vectorNum + len, vectorCapacity))
			{// キャパシティオーバーの場合
				while (1)
				{
					// メモリを拡張
					if (!GetCapacity()) return false;

					// 再度確認し、オーバーなら再度拡張
					if (!IsCapacityOver(vectorNum + len, vectorCapacity)) break;
				}
			}

			/*** 成功時 ***/
			// 指定位置から配列サイズ分ずらす
			memmove(&pHead[nIdx + len], &pHead[nIdx], GetVectorSize());
			memcpy(&pHead[nIdx], ElemArray, sizeof(ElemType) * len);		// 配列サイズ分だけ指定位置に値を格納
			vectorNum += len;		// 使用中の配列数を配列サイズ分増加
			return true;
		}

		//------------------------------------------------------------
		// --- サイズを変更 --- 
		//------------------------------------------------------------
		bool Reserve(UINT num)
		{
			// サイズチェック
			if (num == 0)
			{ // 0の場合
				if (pHead)
				{ // メモリを解放
					free(pHead);
					pHead = nullptr;
					vectorNum = 0;
					vectorCapacity = 0;
					return true;
				}
				else
				{ // 解放済み
					return false;
				}
			}
			else
			{ // 0以上の場合
				// メモリを指定サイズ分確保
				ElemType *pMem = (ElemType*)realloc(pHead, sizeof(ElemType) * num);
				if (pMem)
				{ // 成功時
					pHead = pMem;			// ポインタを保存
					vectorNum = num;		// 使用中配列数を保存
					vectorCapacity = num;	// キャパシティを保存
					return true;
				}
				else
				{ // resize失敗
					return false;
				}
			}
		}

		//------------------------------------------------------------
		// --- 要素数を変更 --- 
		//------------------------------------------------------------
		bool Resize(UINT num)
		{
			if (num == vectorNum) return true;

			// キャパシティチェック
			if (IsCapacityOver(num, vectorCapacity))
			{// キャパシティオーバーの場合
				while (1)
				{
					// メモリを拡張
					if (!GetCapacity()) return false;

					// 再度確認し、オーバーなら再度拡張
					if (!IsCapacityOver(num, vectorCapacity)) break;
				}
			}

			int vectorNumOld = vectorNum;		// 過去の配列数を保存
			vectorNum = num;					// 配列数を更新

			// 元の配列数と比較
			if (num > vectorNumOld)
			{ // 多くなっていた場合
				Fill(vectorNum, 0, num);		// 多くなった分を0で初期化
			}

			return true;
		}

		//------------------------------------------------------------
		// --- 最後尾の値を取得 --- 
		//------------------------------------------------------------
		ElemType *GetEndElem(void)
		{
			// 使用中の配列がない場合NULL
			if (vectorNum == 0U) return NULL;

			return &pHead[vectorNum - 1];
		}

		//------------------------------------------------------------
		// --- 最前列の値を取得 --- 
		//------------------------------------------------------------
		ElemType *GetHeadElem(void)
		{
			// 使用中の配列がない場合NULL
			if (vectorNum == 0U) return NULL;

			return &pHead[0];
		}

		//------------------------------------------------------------
		// --- 特定の番号の値を取得 --- 
		//------------------------------------------------------------
		ElemType *GetElem(UINT nIdx)
		{
			// 使用中の配列がない場合NULL
			if (vectorNum == 0U) return NULL;

			UINT IDX = nIdx;
			if (IDX >= vectorNum) IDX = vectorNum - 1;

			return &pHead[IDX];
		}

		//------------------------------------------------------------
		// --- 最後尾を消去 --- 
		//------------------------------------------------------------
		UINT DeleteEnd(void)
		{
			// ポインタがない場合NULL
			if (pHead == nullptr) return 0U;

			// 使用中の配列がない場合NULL
			if (vectorNum == 0) return 0U;

			pHead[vectorNum - 1] = {};		// 値を消去
			vectorNum--;					// 使用中の配列数を減少

			return vectorNum;				// 使用中の配列数を返す
		}

		//------------------------------------------------------------
		// --- 先端を消去 --- 
		//------------------------------------------------------------
		UINT DeleteHead(void)
		{
			// ポインタがない場合NULL
			if (pHead == nullptr) return 0U;

			// 使用中の配列がない場合NULL
			if (vectorNum == 0) return 0U;

			memmove(&pHead[0], &pHead[1], GetDesignationVectorSize(1));		// メモリをずらす

			pHead[vectorNum - 1] = {};		// 値を消去
			vectorNum--;					// 使用中の配列数を減少

			return vectorNum;				// 使用中の配列数を返す
		}

		//------------------------------------------------------------
		// --- 特定の配列の値を消去 --- 
		//------------------------------------------------------------
		UINT Delete(UINT nIdx)
		{
			// ポインタがない場合NULL
			if (pHead == nullptr) return 0U;

			// 使用中の配列がない場合NULL
			if (vectorNum == 0) return 0U;

			memmove(&pHead[nIdx], &pHead[nIdx + 1], GetDesignationVectorSize(nIdx));		// 値を消去

			vectorNum--;					// 使用中の配列数を減少

			return vectorNum;				// 使用中の配列数を返す
		}

		//------------------------------------------------------------
		// --- 特定の配列の範囲を消去 --- 
		//------------------------------------------------------------
		UINT Delete(UINT nIdx, UINT nIdxEnd)
		{
			// ポインタがない場合NULL
			if (pHead == nullptr) return 0U;

			// 使用中の配列がない場合NULL
			if (vectorNum == 0) return 0U;

			// 範囲指定が間違っている場合NULL
			if (nIdxEnd < nIdx) return 0U;

			// メモリを範囲で消去し、ずらす
			memmove(&pHead[nIdx], &pHead[nIdxEnd], GetDesignationVectorSize(nIdxEnd));

			vectorNum -= 1 + (nIdxEnd - nIdx);		// 使用中の配列数を減少

			return vectorNum;						// 使用中の配列数を返す
		}

		//------------------------------------------------------------
		// --- 全範囲を消去 --- 
		//------------------------------------------------------------
		void Clear(void)
		{
			// ポインタがない場合NULL
			if (pHead == nullptr) return;

			// 使用中の配列がない場合NULL
			if (vectorNum == 0) return;

			// 配列内を全て初期化
			memset(pHead, NULL, GetVectorSize());

			vectorNum = 0;		// 使用中の配列数を0に
		}

		//------------------------------------------------------------
		// --- 全範囲を特定の値で埋める --- 
		//------------------------------------------------------------
		void Fill(const ElemType& Elem)
		{
			// ポインタがない場合終了
			if (pHead == nullptr) return;

			// 使用中の配列がない場合終了
			if (vectorNum == 0) return;

			size_t size = GetVectorSize();

			// 配列内を全て特定の値で埋める
			for (UINT nCntFill = 0; nCntFill < vectorNum; nCntFill++)
			{
				pHead[nCntFill] = Elem;
			}
		}

		//------------------------------------------------------------
		// --- 特定の範囲を特定の値で埋める --- 
		//------------------------------------------------------------
		void Fill(UINT start, const ElemType& Elem, UINT end)
		{
			// ポインタがない場合終了
			if (pHead == nullptr) return;

			// 使用中の配列がない場合終了
			if (vectorNum == 0) return;

			// 開始位置が配列以上の場合終了
			if (start >= vectorNum) return;

			// 順番が逆転している場合終了
			if (start > end) return;

			UINT endIn = end;

			// 終点が配列以上の場合終了
			if (endIn > vectorNum) endIn = vectorNum;

			// 配列内を全て特定の値で埋める
			for (UINT nCntFill = start; nCntFill < endIn; nCntFill++)
			{
				pHead[nCntFill] = Elem;
			}
		}

		//------------------------------------------------------------
		// --- バッファを新規作成 --- 
		//------------------------------------------------------------
		void Start(const ElemType& Elem)
		{
			// 全要素のクリア
			Clear();

			// バッファの作成
			AddHead(Elem);
		}

		//------------------------------------------------------------
		// --- 配列バッファを新規作成 --- 
		//------------------------------------------------------------
		template<size_t N>
		void Start(const ElemType(&Elem)[N])
		{
			// 配列数が0の場合失敗
			if (N <= 0) return;

			// 全要素のクリア
			Clear();

			// バッファの作成
			AddArrayHead(Elem);
		}

		//------------------------------------------------------------
		// --- バッファを破棄 --- 
		//------------------------------------------------------------
		void End(void)
		{
			if (pHead != nullptr)
			{ // NULLではないなら
				free(pHead);		// メモリを解放
				pHead = nullptr;	// ポインタをnullに
				vectorNum = 0;		// 使用中の配列数をリセット
				vectorCapacity = 0;	// キャパシティをリセット
			}
		}

		//------------------------------------------------------------
		// --- 使用されている配列サイズの取得 --- 
		//------------------------------------------------------------
		size_t GetVectorSize(void)
		{
			return sizeof(ElemType) * vectorNum;
		}

		//-------------------------------------------------------------
		// --- 指定した配列番号を先頭とした使用中の配列サイズの取得 --- 
		//-------------------------------------------------------------
		size_t GetDesignationVectorSize(UINT nIdx)
		{
			UINT nGetIdx = nIdx;

			// idxが使用中の配列数よりも大きい場合、最大値に修正
			if (nGetIdx >= vectorNum) nGetIdx = vectorNum - 1;
			return GetVectorSize() - (sizeof(ElemType) * nGetIdx);
		}

		//------------------------------------------------------------
		// --- 予約済みの配列サイズの取得 --- 
		//------------------------------------------------------------
		size_t GetCapacitySize(void)
		{
			return sizeof(ElemType) * vectorCapacity;
		}

		//------------------------------------------------------------
		// --- 使用中の配列数の取得 --- 
		//------------------------------------------------------------
		UINT GetVectorNum(void)
		{
			return vectorNum;
		}

		//------------------------------------------------------------
		// --- 予約済みの配列数の取得 --- 
		//------------------------------------------------------------
		UINT GetCapacityNum(void)
		{
			return vectorCapacity;
		}

		//------------------------------------------------------------
		// --- 四則演算 --- 
		//------------------------------------------------------------
		CVector& operator += (const CVector& Elem)
		{
			if (Elem.vectorNum != vectorNum) return *this;

			for (UINT nCntElem = 0; nCntElem < vectorNum; nCntElem++)
			{
				pHead[nCntElem] += Elem.pHead[nCntElem];
			}

			return *this;
		}

		CVector& operator -= (const CVector& Elem)
		{
			if (Elem.vectorNum != vectorNum) return *this;

			for (UINT nCntElem = 0; nCntElem < vectorNum; nCntElem++)
			{
				pHead[nCntElem] -= Elem.pHead[nCntElem];
			}

			return *this;
		}

		CVector& operator *= (const float& Elem)
		{
			for (UINT nCntElem = 0; nCntElem < vectorNum; nCntElem++)
			{
				pHead[nCntElem] *= Elem;
			}

			return *this;
		}

		CVector& operator /= (const float& Elem)
		{
			for (UINT nCntElem = 0; nCntElem < vectorNum; nCntElem++)
			{
				pHead[nCntElem] /= Elem;
			}

			return *this;
		}

		//------------------------------------------------------------
		// --- 正判定 --- 
		//------------------------------------------------------------
		bool operator == (const CVector& Elem) const
		{
			if (Elem.vectorNum != vectorNum) return false;

			for (UINT nCntElem = 0; nCntElem < vectorNum; nCntElem++)
			{
				if (pHead[nCntElem] != Elem.pHead[nCntElem])
				{
					return false;
				}
			}

			return true;
		}

		//------------------------------------------------------------
		// --- 誤判定 --- 
		//------------------------------------------------------------
		bool operator != (const CVector& Elem) const
		{
			if (Elem.vectorNum != vectorNum) return true;

			for (UINT nCntElem = 0; nCntElem < vectorNum; nCntElem++)
			{
				if (pHead[nCntElem] != Elem.pHead[nCntElem])
				{
					return true;
				}
			}

			return false;
		}

		//------------------------------------------------------------
		// --- コピー演算子 --- 
		//------------------------------------------------------------
		CVector& operator = (const CVector& Copy)
		{
			// コピー元が自分自身の場合
			if (this == &Copy) return *this;

			// メモリ確保済みの場合
			if (pHead != nullptr)
			{ // メモリを解放
				End();
			}

			if (Copy.vectorNum > 0 && Copy.vectorCapacity > 0)
			{ // 使用中の配列数が1以上なら
				// メモリを確保
				pHead = (ElemType*)malloc(sizeof(ElemType) * Copy.vectorCapacity);
				if (pHead != nullptr)
				{ // 確保成功時
					// 値を格納
					memcpy(pHead, Copy.pHead, sizeof(ElemType) * Copy.vectorNum);
					vectorCapacity = Copy.vectorCapacity;	// キャパシティをコピー
					vectorNum = Copy.vectorNum;				// 使用中の配列数をコピー
				}
			}

			return *this;
		}
	};

	// 省略
	using String = CVector<char>;
	using WString = CVector<wchar_t>;
	using Integer = CVector<int>;
	using Floating = CVector<float>;
}
#endif
