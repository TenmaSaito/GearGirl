// =================================================
// 
// モデル処理[model.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _MODEL_H_
#define _MODEL_H_

#include"main.h"

// =================================================
// マクロ定義
#define	MAX_TEX		(16)	// テクスチャの最大数
#define	MAX_MODEL	(512)	// モデルの最大数
#define	PLAYER_XFIX	(30)	// プレイヤーのXサイズの補正
#define	PLAYER_YFIX	(0)		// プレイヤーのYサイズの補正
#define	PLAYER_ZFIX	(30)	// プレイヤーのZサイズの補正

// =================================================
// モデルの構造体を定義
typedef struct
{
	LPDIRECT3DTEXTURE9 apTexture[MAX_TEX];	// テクスチャのポインタ
	LPD3DXMESH pMesh;			// メッシュ(頂点情報)へのポインタ
	LPD3DXBUFFER pBuffMat;		// マテリアルのポインタ
	DWORD dwNumMat;				// マテリアルの数
	D3DXVECTOR3 vtxMin, vtxMax;	// モデルの最小値、最大値
	D3DXVECTOR3 VecLine;		// 境界線ベクトル
	D3DXVECTOR3 VecToPos;		// 境界線ベクトルから移動方向のベクトルに向けたベクトル
}Model;

// =================================================
// モデル個々の構造体を定義
typedef struct
{
	D3DXVECTOR3 pos;		// 位置
	D3DXVECTOR3 rot;		// 向き
	D3DXVECTOR3 move;		// 移動量
	D3DXMATRIX mtxWorld;	// ワールドマトリックス
	int nType;				// 種類
	int nShadow;			// 影
	float sinMove;			// sin関数による上下移動
	bool bUse;				// モデル自体を使用中かどうか
	bool bUseShadow;		// 影を使用中かどうか
	bool bDisp;				// 表示するかどうか
}ModelInfo;

// =================================================
// モデルの種類を列挙型で定義
typedef enum
{
	
	MODELTYPE_MAX
}ModelType;

// =================================================
// プロトタイプ宣言
void InitModel(void);	// 初期化処理
void UninitModel(void);	// 終了処理
void UpdateModel(void);	// 更新処理
void DrawModel(void);	// 描画処理
void SetModel(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nType, int nUseShadow);		// モデルの設定処理
Model* GetModel(void);			// モデル情報の譲渡(種類毎)
ModelInfo* GetModelInfo(void);	// モデル情報の譲渡(個別)
bool LoadModel(void);			// モデルの読み込み処理
bool JudgeComent(char* pStr);	// スクリプト読み込みでのコメントアウトの処理
bool CollisionModel(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove);	// モデルとの当たり判定

#endif