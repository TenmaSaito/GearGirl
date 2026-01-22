//==================================================================================================================================
//
//			空の処理 [sky.h]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
#ifndef _SKY_H_
#define _SKY_H_	//2重インクルード防止

//==============================================================
// ヘッダーインクルード
#include "main.h"
#include "game.h"

//=========================================================================================
// マクロ定義
//=========================================================================================
#define MAX_TEX_SKY			(2)						// テクスチャ数
#define MAX_SKY				(16)					// 最大枚数
#define SKY_SIZE			(1000.0f)				// サイズ
#define SKY_SIZE_ADD		(1.0f)					// サイズ変更幅
#define REV_SKY				(0.01f)					// 回転速度
#define MOVE_SKY			(10.0f)					// 移動速度
#define SKY_REPET			(10.0f)					// 繰り返し数

//==============================================================
// メッシュスフィアの情報構造体
//==============================================================
typedef struct
{
	D3DXVECTOR3				pos;					// 位置
	D3DXVECTOR3				rot;					// 向き
	D3DXVECTOR3				angle;					// 一辺ごとの角度
	D3DXMATRIX				mtxWorld;				// ワールドマトリックス
	LPDIRECT3DVERTEXBUFFER9	pVtxBuff;				// 頂点バッファ
	LPDIRECT3DINDEXBUFFER9	pIdxBuffer;				// インデックスバッファ
	int						nHeightDivision;		// 縦分割数
	int						nCircleDivision;		// 円周分割数
	int						nVerti;					// 全頂点数
	int						nPrim;					// プリミティブ数
	int						nTex;					// テクスチャ番号
	bool					bInside;				// 内側を描画する
	bool					bOutside;				// 外側を描画する
	bool					bUse;					// 使ってる
}Sky;

//=========================================================================================
// プロトタイプ宣言
//=========================================================================================
void InitSky(void);
void UninitSky(void);
void UpdateSky(void);
void DrawSky(void);
void SetSky(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fSize, int nHeightDivision, int nCircleDivision,int nTex,bool bInside,bool bOutside);
Sky* GetSky(void);					// 情報の取得
#endif // !_SKY_H_
